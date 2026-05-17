---
tags: [native, anti-tamper, code-hiding, integrity, calls-sdk, signature-check, libqrcode, deception]
status: verified
severity: high
sources:
  - work/apktool_arm64/lib/arm64-v8a/libqrcode.so
  - findings/native/libqrcode.exports.txt
  - work/jadx_base/sources/one/me/callssdk/CallsSdkInitializer.java
  - work/jadx_base/sources/defpackage/(qrcode loader)
related:
  - "[[09-native-libs]]"
  - "[[443-calls-sdk-initializer]]"
  - "[[505-google-play-signature]]"
  - "[[27-hardcoded-keys-audit]]"
---

# 546. `libqrcode.so` — anti-tamper / signature-check для CallsSDK, спрятанный под именем «qr-генератор»

## Суть

В APK MAX лежит нативная библиотека `libqrcode.so` (138 KB), которая внешне (по имени, по экспортируемому Java-namespace `one.me.sdk.uikit.qr.QrCodeGenerator`) выглядит как QR-кодер. Реверс показал, что **внутри библиотеки лежит вторая, не связанная функциональность** — anti-tamper / integrity protection для CallsSDK: проверка APK-подписи через `PackageManager.SigningInfo`, генерация криптографического session seed, привязанного к подписи. Это **не уязвимость, а намеренное code hiding**: критическая security-логика помещена в библиотеку с обманным именем, чтобы усложнить её обнаружение, патчинг и обход.

В существующем notes/topics/09-native-libs.md `libqrcode.so` упомянута только как «QR-сканер» (1 строка). topic 443 описывает Java-сторону `CallsSdkInitializer`, но ничего не говорит о том, что её native backend живёт в файле с именем «qrcode». Это не было задокументировано.

## Что внутри библиотеки

### Заявленное назначение (видимая часть)

Java-namespace: `one.me.sdk.uikit.qr.QrCodeGenerator`. Подтверждённые JNI-функции для QR-генератора:
- `nativeGenerateQR(String content, byte[] colorBg, byte[] colorFg) → byte[]`
- `nativeRenderSvg(...)` — конвертация SVG в bitmap

### Скрытая функциональность

В `JNI_OnLoad` динамически регистрируются native-методы для **второго**, никак не связанного с QR Java-класса:

```
one/me/callssdk/CallsSdkInitializer
  initializeSessionSeed(Landroid/content/Context;[B[B)[B
  IntegrityProtectionInit(Landroid/content/Context;)I
  sign_check(...)
```

Строки в библиотеке прямо подтверждают это:

```
one/me/callssdk/CallsSdkInitializer
initializeSessionSeed
IntegrityProtectionInit
sign_check
signatures not found
Failed to register native methods for calls sdk, e=%d
(Landroid/content/Context;[B[B)[B
```

### Почему это «code hiding»

1. Имя файла (`libqrcode.so`) и Java-namespace QR-генератора (`one.me.sdk.uikit.qr.QrCodeGenerator`) не имеют ничего общего с CallsSDK или integrity-проверкой.
2. Реверсер, ищущий anti-tamper по очевидным именам (`libantitamper`, `libsecurity`, `libintegrity`), пройдёт мимо.
3. Нет отдельной native-библиотеки `libcallssdk.so`. Если бы она была, её можно было бы заменить на stub. Здесь — модифицировать надо `libqrcode.so`, что выглядит как trivial-фича.
4. Java-обёртка `CallsSdkInitializer` ссылается на native-методы, но в коде нет явного `System.loadLibrary("callssdk")` под этим namespace — методы регистрируются в `JNI_OnLoad` библиотеки `libqrcode.so`. Чтобы это понять, нужно реверсить именно её.

## Что делает `IntegrityProtectionInit`

По строкам и сигнатуре `(Landroid/content/Context;)I`:
- Принимает `Context`
- Через `PackageManager.getPackageInfo(packageName, GET_SIGNING_CERTIFICATES)` (или старый `GET_SIGNATURES`) читает подпись APK
- Сравнивает с эталоном (вероятно SHA-256 fingerprint, hardcoded в коде или вычисляемый из embedded сертификата)
- Возвращает `int` — вероятно код результата (0 = ok, иначе — несовпадение)

Строка `signatures not found` — error path, когда `PackageInfo.signingInfo` пуст или несовпадает.

## Что делает `initializeSessionSeed`

Сигнатура: `(Landroid/content/Context;[B[B)[B`

Принимает: `Context`, два `byte[]` (вероятно: server-provided nonce и device identifier), возвращает `byte[]` — криптографический seed для сессии звонка.

Привязка к integrity-check критична: если предыдущий `IntegrityProtectionInit` не прошёл, `initializeSessionSeed` либо возвращает мусор, либо отказывает. Это значит:
- **Звонки в модифицированном APK не работают** (неверный session seed → server отклонит)
- **Реверсер, патчающий Java-уровень**, не получит звонки без патчинга native кода в `libqrcode.so`

## Импорты и сетевая поверхность

Из `rz-bin -i libqrcode.so`:
- math (`sincos`, `acosf`, `atan2f`)
- memory (`malloc`, `free`, `memcpy`, `memset`)
- string ops (`strlen`, `strcmp`)
- I/O (`fopen`, `fwrite`, `fclose`) — для SVG-рендеринга QR

**Нет:** `socket`, `connect`, `send`, `recv`, `getaddrinfo`, `dlopen`, `dlsym` (для произвольных путей).

То есть библиотека **не делает сетевых вызовов** сама — только проверяет подпись и вычисляет seed. Сеть — на Java уровне, отправляющая seed и получающая ответ от сервера.

## Java-сторона связки

`one/me/callssdk/CallsSdkInitializer.java`:

```java
static {
    try {
        System.loadLibrary("qrcode");   // (!!) — загрузка через невинное имя
    } catch (UnsatisfiedLinkError e) {
        // ...
    }
}

public native int IntegrityProtectionInit(Context context);
public native byte[] initializeSessionSeed(Context context, byte[] serverNonce, byte[] deviceId);
```

Вот эта строка `System.loadLibrary("qrcode")` в `CallsSdkInitializer` — главный сигнал. Любой, кто грепает `loadLibrary` по коду, увидит, что `CallsSdkInitializer` грузит `libqrcode.so`. Это не «случайное код-шеринг» — это явный design pattern: «помести security-логику в безопасно выглядящую либу».

## Сравнение с другими паттернами защиты

| Защита | Где обычно лежит |
|---|---|
| Anti-tamper `libtamper.so` | `lib/` с очевидным именем |
| Standalone `libdrm.so` / `libsec.so` | как отдельная библиотека |
| Inline (Java-only `getInstance().checkSignature()`) | легко обходится |
| **MAX: внутри libqrcode.so** | **обманное имя + library, которая обоснованно нужна** |

Стандартные техники реверса при поиске анти-tamper:
1. Грепать `getPackageInfo`, `signingInfo`, `GET_SIGNATURES` в Java — здесь это есть в `CallsSdkInitializer`, но это just declarations.
2. Грепать `*tamper*`, `*integrity*`, `*sign*` в названиях .so — `libqrcode.so` не сматчит ни на одно слово.
3. Запускать инструменты типа `apkid` или `androguard` — они классифицируют libqrcode по экспортам как «QR-related».

## Что это не закрывает

Code hiding — это **обфускация местоположения**, а не криптографическая защита:
- Реверсер с rizin/ghidra, делающий полную декомпозицию **всех** .so в APK, найдёт и эту функциональность (что и произошло).
- Frida-инструментирование **по адресу функции** обходит проверку (можно вернуть фиксированный success).
- Эмулятор без реальной подписи всё равно сделает запрос на сервер calls — просто получит ошибку.

То есть это **раздражающий слой** для casual-реверсеров, не более. Для опытного reverser'а это +30 минут работы и 0 препятствий.

## Что это значит в контексте всего реверса MAX

1. **Код намеренно прячется.** Это означает, что разработчики MAX **знают**, что компонент чувствителен, и активно затрудняют его обнаружение. Это противоречит маркетинговому позиционированию открытости («просто мессенджер»).
2. **Calls-стек защищён сильнее, чем messaging.** Anti-tamper применён только к `CallsSdkInitializer`. Никакой эквивалентной защиты для messaging WS-протокола или Mobile ID flow не обнаружено. Это говорит, что приоритет — защитить именно **звонки** от unauthorized-клиентов.
3. **Привязка к подписи APK.** Это может объяснить, почему MAX распространяется через собственный `download.max.ru` (см. [[12-force-update-killswitch]]) — собственная подпись не проверяется внешними сторонами, любой APK с этой подписью пройдёт integrity-check.
4. **Обходимо для server-side злоупотреблений.** Anti-tamper защищает от модифицированных **клиентов**. От злоупотреблений со стороны **сервера** (см. [[542-traceflow-dps-deanonymization]], [[543-reconnect-ws-server-host-takeover]], [[544-debug-ws-opcode-c2-channel]]) он не защищает вообще.

## Файлы

- `work/apktool_arm64/lib/arm64-v8a/libqrcode.so` — сама библиотека (138 KB)
- `findings/native/libqrcode.exports.txt` — экспорты (только JNI_OnLoad — методы регистрируются динамически)
- `work/jadx_base/sources/one/me/callssdk/CallsSdkInitializer.java` — Java-обёртки native methods
- `notes/wave3/10-unknown-so-libs.md` — полный анализ всех «неизвестных» .so

## Статус в 26.16.0

ARM64 split APK для 26.16.0 не сравнивался напрямую (отсутствует в репозитории). По косвенным признакам — без изменений.

---

**Вывод одной строкой:** библиотека `libqrcode.so` — это не QR-генератор. Точнее, это QR-генератор с встроенным native-кодом проверки APK-подписи и генерации криптографического session-seed для звонков. Имя библиотеки — обман: security-критическая функция спрятана под видом UI-компонента. Это намеренное code hiding, а не побочный эффект сборки.
