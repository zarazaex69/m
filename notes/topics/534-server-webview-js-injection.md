---
tags: [critical, webapp, jsbridge, rce, server-control, javascript, injection, surveillance]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/ghk.java
  - work/jadx_base/sources/defpackage/upk.java
  - work/jadx_base/sources/defpackage/i6e.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[07-nfc-hce-webapp]]"
  - "[[419-webapp-js-events]]"
  - "[[421-webapp-request-phone]]"
  - "[[37-webapp-privilege-configuration]]"
---

# 534. Server→WebView произвольная инъекция JavaScript

## Суть

Сервер MAX может выполнить **произвольный JavaScript** в контексте любой мини-аппы через `evaluateJavascript()`. В сочетании с привилегированным JS-bridge, который даёт доступ к NFC-эмуляции, номеру телефона, биометрии и файловой системе — это **полноценный remote code execution** через WebView.

## Механизм

`ghk.java` (строка 99):
```java
webView.evaluateJavascript(String.format(
    isPrivate 
        ? "(() => { PrivateWebApp.sendEvent(%s, %s); })();"
        : "(() => { WebApp.sendEvent(%s, %s); })();",
    eventName, eventData));
```

- `eventName` и `eventData` приходят **с сервера** (через объект `bik`)
- Код выполняется в контексте загруженной мини-аппы
- Для привилегированных мини-апп (`PrivateWebApp`) — доступ к расширенному bridge

## Привилегированный JS-bridge (PrivateWebViewHandler)

`i6e.java` — инжектится в WebView для ботов из серверного whitelist (`availableBotForPrBridge` PmsKey, тип `LongSet`).

Через `postEvent(name, data)` привилегированная мини-аппа может:
- `WebAppVerifyMobileId` — получить MSISDN абонента
- `WebAppNfcEmulateNfcTag` — эмулировать NFC-карту
- `WebAppBiometryRequestAuth` — запросить биометрию
- `WebAppRequestPhone` — получить номер телефона с хешем
- `WebAppSecureStorageSaveKey` — записать в secure storage
- `WebAppDownloadFile` — скачать файл
- `WebAppOpenLink` / `WebAppOpenMaxLink` — открыть URL

## Цепочка атаки

```
СЕРВЕР MAX
    │
    │  Отправляет event через WS/push
    ▼
ghk.java: evaluateJavascript("PrivateWebApp.sendEvent('verify_mobile_id', '{...}')")
    │
    ▼
WebView выполняет JS → вызывает PrivateWebViewHandler.postEvent()
    │
    ▼
Native код: возвращает MSISDN / эмулирует NFC / скачивает файл
```

## Что делает это критическим

1. **Сервер контролирует whitelist** привилегированных ботов — может добавить любого
2. **URL мини-аппы** загружается с сервера — может быть подменён
3. **evaluateJavascript** выполняет произвольный код — не только предопределённые события
4. **Нет consent UI** — пользователь не видит, что мини-аппа получает его номер или эмулирует NFC
5. В сочетании с QUIC TLS bypass — MITM может инжектировать JS в любую мини-аппу

## 14 категорий действий через JS-bridge

biometry, brightness, codereader, download, haptic, link, nfc, phone, private (verify_mobile_id), share, storage, system, unsupported, viewport

## Статус в 26.16.0

Без изменений. `PrivateWebViewHandler` и `evaluateJavascript` на месте.
