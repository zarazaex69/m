---
tags: [medium, webview, js-bridge, nfc-hce, miniapp, no-whitelist]
status: verified
severity: medium
sources:
  - work/jadx_base/sources/defpackage/upk.java
  - work/jadx_base/sources/defpackage/ijk.java
  - work/jadx_base/sources/defpackage/rfk.java
  - work/jadx_base/sources/defpackage/gfk.java
  - work/jadx_base/sources/one/me/webapp/util/WebAppNfcService.java
related:
  - "[[7-nfc-hce-webapp]]"
  - "[[10-webapp-jsbridge]]"
  - "[[549-webapp-openmaxlink-deeplink-injection]]"
---

# 551. WebAppNfcEmulateNfcTag — NFC HCE доступен ВСЕМ мини-аппам без whitelist

## TL;DR

JS-bridge метод `WebAppNfcEmulateNfcTag` запускает Android Host Card Emulation (HCE) и заставляет устройство эмулировать произвольный NFC-тег с данными, полностью контролируемыми мини-аппой. Метод **доступен через публичный bridge `WebViewHandler`** — то есть **любой мини-апе** в `WebAppRootScreen`, без серверного whitelist (`webapp-pr`/`webapp-exc` его не ограничивают).

Topic [[7-nfc-hce-webapp]] описывал саму архитектуру (`WebAppNfcService` как `HostApduService`). Этот topic фиксирует, что **доступ к этой возможности не ограничен** — любая загруженная мини-апа может эмулировать NFC-тег.

## 1. Регистрация bridge

`ahk.java`:

```java
// ПУБЛИЧНЫЙ bridge — доступен ВСЕМ мини-аппам
v9gVar.addJavascriptInterface(new upk(...), "WebViewHandler");

// PRIVATE bridge — только для botId из webapp-pr
if (webAppRootScreen4.s1().b1) {
    v9gVar.addJavascriptInterface(new i6e(...), "PrivateWebViewHandler");
}
```

`WebViewHandler` — публичный. Метод `postEvent` принимает событие `WebAppNfcEmulateNfcTag` без проверки `isPrivate=true`.

## 2. Маршрут события

```
JS:  WebViewHandler.postEvent("WebAppNfcEmulateNfcTag", '{"queryId":..., "requestId":..., "nfctag":"<hex>"}')
        ↓
Java: upk.postEvent → ijk.A(name, data, isPrivate=false)
        ↓
      gz8 (JsBridgeFactory) → rfk (NFC delegate)
        ↓
      gfk (action enum) → WebAppNfcEmulateNfcTag handler
        ↓
      one.me.webapp.util.WebAppNfcService (HostApduService) — Android HCE
        ↓
      эмуляция NFC-тега, данные = nfctag из мини-аппы
```

## 3. Код delegate

`rfk.java`:

```java
public Object a(String name, String dataJson) {
    if ("WebAppNfcEmulateNfcTag".equals(name)) {
        JSONObject obj = new JSONObject(dataJson);
        long queryId   = obj.getLong("queryId");
        long requestId = obj.getLong("requestId");
        String hex     = obj.getString("nfctag");
        // !! НЕТ проверки botId, нет whitelist, нет user-confirm !!
        byte[] tagData = hexToBytes(hex);
        startNfcEmulation(tagData);
        return ack(queryId, requestId);
    }
    if ("WebAppNfcGetInfo".equals(name)) {
        return getNfcAvailability();
    }
    if ("WebAppNfcOpenSystemSettings".equals(name)) {
        return openNfcSettings();
    }
}
```

**Что не проверяется:**
- `botId` мини-аппы — нет
- Серверный whitelist (`webapp-pr`/`webapp-exc`) — нет (метод не в private bridge)
- User-confirm dialog (как у `WebAppRequestPhone`) — **нет**
- Содержимое `nfctag` — нет
- Длина `nfctag` — нет (только что hex parse не упадёт)

## 4. Service в манифесте

`AndroidManifest.xml`:

```xml
<service
    android:exported="true"
    android:name="one.me.webapp.util.WebAppNfcService"
    android:permission="android.permission.BIND_NFC_SERVICE">
    <intent-filter>
        <action android:name="android.nfc.cardemulation.action.HOST_APDU_SERVICE"/>
    </intent-filter>
    <meta-data
        android:name="android.nfc.cardemulation.host_apdu_service"
        android:resource="@xml/nfc_aid_list"/>
</service>
```

`exported="true"` обязательно для HCE-сервисов (Android требует). Реальное ограничение — `<aid-filter>` в `nfc_aid_list.xml`.

## 5. Чем эта возможность ограничена технически

Android HCE имеет **системные** ограничения, которые работают независимо от MAX:

1. **Foreground requirement.** APDU-запросы NFC-терминала роутятся в HCE-сервис только если приложение в foreground (или явно выбрано как preferred service в настройках). Мини-апа MAX в момент эмуляции — foreground.
2. **AID filter.** В `nfc_aid_list.xml` зашит список AID-ов (Application IDs), которые сервис слушает. Эмулировать AID, не указанный в этом списке, нельзя без перерегистрации сервиса.
3. **Категория.** Категория `payment` требует, чтобы приложение было выбрано как default payment app (через системные настройки). Категория `other` — без этого требования, но не работает с EMV-терминалами.

Эти ограничения снижают impact, но **не устраняют возможность эмуляции в категории `other`** для всех AID, перечисленных в манифесте.

## 6. Сценарий злоупотребления

### Базовый случай:

1. Мини-апа любого бота получает `nfctag` (hex-строку с сервера)
2. Вызывает `WebAppNfcEmulateNfcTag(nfctag)`
3. Пользователь подносит телефон к NFC-ридеру
4. Считыватель получает данные, заданные мини-аппой — пользователь не видел никакого подтверждения

### Что можно эмулировать:

- **MIFARE-совместимые тэги** (если AID входит в фильтр) — клонирование пропусков, ключей доступа
- **NFC Forum NDEF tags** — записи URL, текстовые сообщения, vCard и т.д., которые ридер интерпретирует как «тап» на тэг
- **Транспортные карты** — теоретически (на практике зависит от типа криптографии и от того, кэширует ли владелец карты весь дамп)

Реальная атака требует наличия дампа целевого тэга — но если он есть, мини-апа может тихо использовать MAX как клонирующее устройство.

### С учётом 549 и 534:

Сервер через [[534-server-webview-js-injection]] может выполнить произвольный JS в любой мини-апе. В сочетании с этим topic'ом — сервер может из **любой** загруженной мини-аппы заставить устройство эмулировать NFC-тэг.

## 7. Разница с `WebAppRequestPhone`

Сравнение защит у схожих по чувствительности методов:

| Метод | User-confirm | Whitelist | Сервер-управляемый toggle |
|---|---|---|---|
| `WebAppVerifyMobileId` | нет | **да** (`webapp-pr` private bridge) | через PmsKey |
| `WebAppRequestPhone` | **да** (`ConfirmationBottomSheet`) | нет | `webapp-phone-hash` для формата ответа |
| `WebAppNfcEmulateNfcTag` | **нет** | **нет** | нет |
| `WebAppOpenMaxLink` | user-click 3s | нет | `webapp-exc` exemption |

NFC HCE — единственный из «сильных» методов **без** ни user-confirm, ни whitelist, ни даже user-click окна.

## 8. Что нужно для исправления

Минимум:
1. Добавить `WebAppNfcEmulateNfcTag` в `L1` set (требование user-click 3s)
2. Показать `ConfirmationBottomSheet` с указанием бота и hex-данных, которые будут эмулироваться
3. Whitelist через `webapp-pr` (только private-боты могут эмулировать)
4. Логирование/метрика по успешным эмуляциям

Ничего из этого нет ни в 26.15.3, ни в 26.16.0.

## 9. Файлы

- `upk.java` — публичный `WebViewHandler` bridge
- `ijk.java:A()` — диспетчер событий, `L1` set (NFC не входит)
- `rfk.java` — NFC delegate
- `gfk.java` — enum имён методов (`WebAppNfcEmulateNfcTag`, `WebAppNfcGetInfo`, `WebAppNfcOpenSystemSettings`)
- `one/me/webapp/util/WebAppNfcService.java` — `HostApduService` реализация

## 10. Риск-оценка

**MEDIUM**

Почему MEDIUM:
- Любая мини-апа может эмулировать NFC-тэг
- Нет user-confirm, нет whitelist, нет user-click окна
- Сочетается с server-side JS-injection (534) для отсутствия даже косвенного user interaction

Почему не HIGH:
- Системные ограничения HCE (foreground, AID-filter, категория)
- Реальная атака требует наличия дампа целевого тэга
- Эмуляция платёжных карт ограничена «default payment app» настройкой

## 11. Статус в 26.16.0

Без изменений. `WebAppNfcEmulateNfcTag` остался в публичном bridge, ни whitelist ни user-confirm не добавлены.
