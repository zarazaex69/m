---
tags: [critical, webview, ssl-bypass, pmskey, server-control, mixed-content, popup, wav2lip, deepfake, tflite, hardcoded-key]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/xyc.java
  - work/jadx_base/sources/defpackage/syc.java
  - work/jadx_base/sources/defpackage/ri9.java
  - work/jadx_base/sources/defpackage/ppk.java
  - work/jadx_base/sources/defpackage/esd.java
  - work/jadx_base/sources/one/me/chatmedia/viewer/VideoWebViewScreen.java
  - work/jadx_base/sources/one/me/webview/FaqWebViewWidget.java
  - work/apktool_arm64/lib/arm64-v8a/libEnhancementLibShared.so
related:
  - "[[534-server-webview-js-injection]]"
  - "[[541-miniapp-disable-flag-secure]]"
  - "[[03-pms-server-flags]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[28-vk-enh-decrypt-key]]"
---

# 547. Server-controlled SSL bypass + WebView popup hardening gaps + Wav2Lip deepfake + hardcoded TFLite key

Группа из четырёх не задокументированных ранее находок, объединённых темой клиентских настроек, которыми удалённо управляет сервер либо которые несут архитектурный риск независимо от пользователя.

## 1. PmsKey `web_app:ssl_check` — server-controlled WebView SSL bypass

### Что это

В `xyc.java` (общий WebViewClient для мини-аппов и видео в чате):

```java
// xyc.java:97-119
public void onReceivedSslError(WebView webView, SslErrorHandler sslErrorHandler, SslError sslError) {
    if (!((opk) this.b).i()) {        // i() → ri9.S() → PmsKey "web_app:ssl_check"
        sslErrorHandler.cancel();      // безопасный путь
        // ...
    } else {
        sslErrorHandler.proceed();     // ПРИНЯТЬ ЛЮБОЙ сертификат
    }
}
```

Где `i()` — это `ri9.S()`:

```java
// ri9.java:81 — определение
this.B0 = new bbi(i5, "web_app:ssl_check", bool, this.e, zjf.a(Boolean.class));

// ri9.java:162 — getter
public final boolean S() {
    return ((Boolean) this.B0.x(this, f1[15])).booleanValue();
}
```

Имя `"web_app:ssl_check"` — это **PmsKey-флаг** из 334 серверно-управляемых ([[03-pms-server-flags]]). Сервер через NOTIF_CONFIG (опкод 134) может выставить его в `true` для конкретного `userId`.

### Отличие от ранее задокументированного

В `notes/wave1/06-jsbridge-full.md` упоминался `isDisableWebAppSsl` как доступный «через DevMenu в production». Это другое: тот флаг — DevMenu-toggle, требует физического доступа к устройству. Здесь — **серверный PmsKey**, сервер может включить его удалённо для любого пользователя без участия владельца устройства.

### Что это даёт серверу

Когда `web_app:ssl_check = true`:
1. Любой `SslError` (несовпадение CN, неверная цепочка, expired cert, self-signed) — игнорируется
2. WebView принимает **любой** TLS-сертификат
3. Сервер на пути MITM (с валидным TLS-сертом любого CA, или с self-signed) подменяет содержимое мини-аппы
4. Через [[534-server-webview-js-injection]] сервер инжектит JS в WebView — но теперь даже без необходимости компрометировать `*.oneme.ru`-хост

### Связь с другими находками

- **[[543-reconnect-ws-server-host-takeover]]**: после redirect WS на свой сервер атакующий контролирует и WS, и (через `web_app:ssl_check=true`) WebView
- **[[537-quic-tls-bypass-mitm]]**: для QUIC уже есть пустой TrustManager; теперь и для WebView сервер может отключить TLS-проверки
- **[[541-miniapp-disable-flag-secure]]**: после MITM можно изменить мини-аппу так, чтобы она вызвала `setup_screen_capture_behavior` и отключила FLAG_SECURE

### Scope

Все WebView, использующие `xyc` как `WebViewClient`:
- Все WebApp-мини-аппы (через `ahk.java` / `ppk.java`)
- `VideoWebViewScreen` (через `esd.java`)

## 2. `onCreateWindow` popup WebView — без security hardening

### Что найдено

В `syc.java:67-73` (WebChromeClient для мини-аппы) при попытке `window.open()` создаётся новый WebView:

```java
public final boolean onCreateWindow(WebView webView, boolean z, boolean z2, Message message) {
    vyc vycVar = new vyc(webView.getContext(), 14);
    vycVar.setWebViewClient(new xyc(this, vycVar));
    ((WebView.WebViewTransport) message.obj).setWebView(vycVar);
    message.sendToTarget();
    return true;
}
```

**Что НЕ установлено** на новом WebView:
- `setAllowFileAccess(false)` — не вызывается → default `true` до Android 11, `false` от Android 11+
- `setAllowContentAccess(false)` — не вызывается → **default `true`**
- `setJavaScriptEnabled(false)` — не вызывается (наследуется? нет — это новый инстанс, defaults применяются)
- `setMixedContentMode(...)` — не вызывается
- `setWebContentsDebuggingEnabled` — не задано

**Что есть:** только `WebViewClient` (для SSL-handler) — но WebViewClient не контролирует `WebSettings`.

### Почему это риск

Default `allowContentAccess = true` означает, что popup-WebView может загружать `content://` URI:
- `content://media/external/...` — доступ к фото/видео пользователя (если есть permission у MAX, а он есть)
- `content://com.android.contacts/...` — доступ к контактам
- любые другие content provider'ы, к которым у MAX есть `grantUriPermission`

Скрипт в основной мини-аппе:

```javascript
// в основной мини-аппе:
const popup = window.open("javascript:void(0)");
popup.document.body.innerHTML = '<img src="content://media/external/images/media/N">';
// или
popup.document.body.innerHTML = '<iframe src="content://contacts/people"></iframe>';
```

Если у мини-аппы есть права на эти content URIs (через `grantUriPermissions=true` у [[09-attack-surface-manifest]] FileProvider/NotificationsImagesProvider), popup может прочитать данные.

## 3. `VideoWebViewScreen` — `MIXED_CONTENT_ALWAYS_ALLOW`

### Что найдено

`one/me/chatmedia/viewer/VideoWebViewScreen.java:241-244`:

```java
vycVar.getSettings().setJavaScriptEnabled(true);
vycVar.getSettings().setAllowContentAccess(true);
vycVar.getSettings().setAllowFileAccess(false);
vycVar.getSettings().setMixedContentMode(0);   // MIXED_CONTENT_ALWAYS_ALLOW
```

### Почему это риск

`MIXED_CONTENT_ALWAYS_ALLOW` (значение 0) — самая небезопасная опция: HTTPS-страница может подгружать **HTTP-ресурсы** без предупреждений и без блокировки. В сочетании с:
- `setJavaScriptEnabled(true)` — JS включён
- `setAllowContentAccess(true)` — content:// доступ открыт

Если HTTPS-страница `https://video.example.com/player.html` подгружает `<script src="http://cdn.example.com/player.js">`, и атакующий контролирует HTTP-канал (открытый Wi-Fi, роутер) — он инжектит произвольный JS, который выполняется в контексте `https://video.example.com` и может через content:// читать данные пользователя.

В сочетании с PmsKey `web_app:ssl_check` (находка #1 в этом topic), даже HTTPS-канал не защищает.

## 4. FAQ WebView пропускает `max://` deeplinks

### Что найдено

`one/me/webview/FaqWebViewWidget.java` использует `nk6.java` как WebViewClient:

```java
// nk6.java:20 (упрощённо)
public final boolean shouldOverrideUrlLoading(WebView wv, WebResourceRequest req) {
    Uri uri = req.getUrl();
    String scheme = uri.getScheme();
    if ("mailto".equals(scheme) || "tel".equals(scheme)) { ... }
    if ("max".equals(scheme)) {
        c1(uri);   // → routing через DeepLinkProcessor
        return true;
    }
    if ("https".equals(scheme) || "http".equals(scheme)) { ... }
    return false;
}
```

### Почему это риск

FAQ-страница (загружается с **внешнего** сервера `help.max.ru` или подобного) может содержать `<a href="max://action/...">` или `window.location = "max://action/..."`. WebViewClient безусловно пропускает любой `max://`-URI в `DeepLinkProcessor`.

Что может сделать `max://` deeplink:
- Открыть произвольный экран приложения (включая deeplink на startwebapp)
- Передать параметры в auth-flow (включая `:auth?externalCallback=1`, см. [[13-deeplinks-idp]])
- Триггернуть `LinkInterceptorActivity` со side-effect через MyTracker (см. [[wave1/09-attack-surface-manifest]])

Атакующий, контролирующий `help.max.ru` (или содержимое FAQ-страницы — XSS-injection, серверное редактирование), может через одно посещение FAQ запустить произвольное действие в приложении.

## 5. Wav2Lip V1/V3/V4 — production deepfake engine в видеозвонках

### Что найдено

В `libEnhancementLibShared.so` (декомпиляция через rizin) обнаружены namespace'ы:

```
vk::enh::Wav2LipV1
vk::enh::Wav2LipV3
vk::enh::Wav2LipV4
vk::enh::Animoji
vk::enh::AnimojiFactory
vk::enh::animoji_messages::AnimojiMessage
vk::enh::animoji_messages::SetInternalBufferMessage
```

Плюс:
- TFLite Detection PostProcess (face detection)
- `cache_landmarks` (face landmarks caching между фреймами)
- `vk::enh::CPUImage::save(const char*)` (сохранение фреймов на диск)
- `vk::enh::Image::fromPtr(ptr, w, h, channels, Format)` (raw pixel buffer)

### Что такое Wav2Lip

[Wav2Lip](https://github.com/Rudrabha/Wav2Lip) — известная open-source нейросеть, которая по аудио + видео-кадру с лицом генерирует **синхронизированное движение губ**. Это deepfake-технология.

В MAX обнаружены **три версии** (V1, V3, V4) — production-ready, не «эксперимент».

### Что это означает

В production-сборке мессенджера (без флага «лабораторная фича») присутствует full pipeline: face detection → landmark extraction → Wav2Lip lip-sync. Возможные применения:
1. **Доброкачественные**: cosmetic-фильтры, аватары, dubbing для людей с речевыми проблемами
2. **Сомнительные**: автоматическая подмена лица другого человека на лицо пользователя в реальном времени
3. **Эксплуатируемые**: если сервер может управлять Wav2Lip-pipeline через [[540-webrtc-encryption-disable-stun-marking-vpn-bypass]] field trials — потенциальная подмена лица одного из участников звонка для другого

В коде MAX **не найдено UI-toggle** для отключения Wav2Lip. Активируется ли он в обычных звонках — определяется серверной конфигурацией. Сам факт наличия full deepfake engine в production-мессенджере без публичного описания фичи — это аномалия.

## 6. Hardcoded AES key для расшифровки .tflite моделей

### Что найдено

В `libEnhancementLibShared.so` функции `vk::enh::encrypt` (@ 0x001302dc) и `vk::enh::decrypt` (@ 0x00130360) — обе ~132 байта. Загружают **hardcoded 16-байтный ключ** из секции `.rodata` по адресу `0x83513`:

```
6f 8c c4 b7 19 d4 0d 16 d1 fc b9 ba bb c4 7d 7e
```

### Зачем

`.tflite` модели для ASR/Speaker Recognition/Diarization/Wav2Lip и других модулей **скачиваются с сервера** (см. [[16-server-pushed-ml-models-in-calls]]) и хранятся на диске в зашифрованном виде. При загрузке `vk::enh::decrypt` расшифровывает их в памяти.

### Почему это бесполезная защита

1. Ключ зашит в сам бинарник — любой реверсер с rizin/r2/ghidra извлекает его за 30 секунд.
2. Алгоритм определяется по близлежащему коду (вероятно AES-128 или ChaCha20 — определить точно требует декомпиляции `fcn.0055c2c0`).
3. **Любой может расшифровать все скачанные с сервера модели** и проанализировать:
   - Архитектуру нейросетей
   - Предобученные веса
   - Списки ключевых слов в KWS-моделях (до их удаления в 26.16.0)
   - Лейблы в AudioClassifier (что конкретно ловит)
   - Голосовые embeddings reference в Speaker Recognition

### Контекст

В topic [[28-vk-enh-decrypt-key]] был описан другой decrypt-ключ (для другого модуля). Этот — **новый**, ранее не задокументированный. Связан с pipeline encrypt/decrypt в native слое для всех `.tflite` моделей.

## Сравнительная таблица 4 находок

| # | Что | Уровень контроля | Trigger | Видимость пользователю |
|---|---|---|---|---|
| 1 | PmsKey `web_app:ssl_check` | **Сервер** | NOTIF_CONFIG | Нет (нет UI-индикации SSL-bypass) |
| 2 | Popup WebView без hardening | Атакующий через мини-аппу | `window.open()` | Нет (popup в фоне может быть невидим) |
| 3 | `MIXED_CONTENT_ALWAYS_ALLOW` в Video WebView | Атакующий на канале (HTTP-инъекция) | Открытие видео в чате | Нет |
| 4 | FAQ WebView пропускает `max://` | Атакующий, контролирующий FAQ-сервер | Любая ссылка в FAQ | Нет |

И:
- 5 (Wav2Lip): архитектурный риск, не активный эксплоит
- 6 (Hardcoded TFLite key): архитектурная слабость защиты ML-моделей

## Что объединяет эти находки

Все — это **или удалённо включаемые опции**, или **архитектурные дефолты, переключенные не в безопасную сторону**. Каждая по отдельности — средний риск. Вместе с уже задокументированными [[534-server-webview-js-injection]] (server→evaluateJavascript), [[541-miniapp-disable-flag-secure]] (server-toggle FLAG_SECURE), [[543-reconnect-ws-server-host-takeover]] — образуют **стек**, в котором сервер контролирует поведение клиентского WebView вплоть до полной возможности MITM с инъекцией JS, обходом FLAG_SECURE и чтением content:// данных без user-consent.

## Файлы

- `defpackage/xyc.java:97-119` — onReceivedSslError ветвление по PmsKey
- `defpackage/ri9.java:81,162` — определение PmsKey `web_app:ssl_check` и `S()` getter
- `defpackage/ppk.java:258`, `defpackage/esd.java:263` — реализации `opk.i()`
- `defpackage/syc.java:67-73` — onCreateWindow без hardening
- `one/me/chatmedia/viewer/VideoWebViewScreen.java:241-244` — MIXED_CONTENT_ALWAYS_ALLOW
- `defpackage/nk6.java:20` — FAQ WebViewClient пропускает max://
- `libEnhancementLibShared.so` — Wav2Lip namespaces, encrypt/decrypt + hardcoded key
- `notes/wave3/04-webview-unsafe-settings.md` — полная таблица WebView
- `notes/wave3/06-libenh-decomp.md` — реверс libEnhancement

## Статус в 26.16.0

- PmsKey `web_app:ssl_check` — без изменений
- onCreateWindow popup — без изменений
- VideoWebViewScreen — без изменений
- FAQ WebView — без изменений
- Wav2Lip V1/V3/V4 — без изменений (KWS удалили, lip-sync deepfake оставили)
- Hardcoded encryption key — без изменений (key reuse через релизы)

---

**Вывод одной строкой:** server-controlled MITM на WebView (PmsKey `web_app:ssl_check`) + дыры в popup/video/FAQ WebView-настройках + production deepfake engine + бесполезная защита ML-моделей одним hardcoded key — каждая мелочь сама по себе, но вместе с уже найденным они закрывают каждый из путей пользователя в WebView.
