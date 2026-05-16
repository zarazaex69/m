---
tags: [conversation-factory, calls-sdk, sdk-version, animoji, webrtc, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ConversationFactory.java
related:
  - "[[413-conversation-interface]]"
  - "[[389-vchat-api-requests]]"
  - "[[384-externcalls-sdk-config]]"
---

# ConversationFactory — фабрика звонков

`ConversationFactory` — фабрика для создания звонков. SDK версия `0.1.13`.

## Методы создания звонков

| Метод | Что |
|---|---|
| `call(params)` | исходящий звонок |
| `answer(params)` | ответить на входящий |
| `join(params)` | войти в конференцию |
| `joinByLink(params)` | войти по ссылке |
| `joinAnonByLink(params)` | войти анонимно по ссылке |
| `createConfRoom(params)` | создать конференц-комнату |
| `hangup(reason, cid, anonToken)` | завершить звонок |

## Конфигурация

| Метод | Что |
|---|---|
| `setIsWebTransportEnabled(bool)` | WebTransport |
| `setLoadKwsBySdkEnabled(bool)` | загрузка KWS через SDK |
| `setIceTransportType(type)` | тип ICE транспорта |
| `setVpnPreference(pref)` | предпочтение VPN |
| `setLogConfiguration(config)` | конфигурация логирования |
| `setDomainId(str)` | ID домена |
| `setBackendRenderVmoji(bool, str)` | серверный рендеринг анимоджи |
| `setAnimojiDataSupplier(supplier)` | поставщик данных анимоджи |
| `setInCallAnalyticsUploadEnabled(bool)` | аналитика во время звонка |
| `setCallAnalyticsUploadParams(d1, d2)` | параметры загрузки аналитики |
| `setEmulatedNegotiationErrorType(type)` | **эмуляция ошибки negotiation** |
| `setCamera2ApiEnabled(bool)` | Camera2 API |
| `setDynamicScreenShareSizeUpdateEnabled(bool)` | динамическое обновление размера screen share |
| `setFilterCallMuteStateInitForAdmins(bool)` | фильтр mute для администраторов |
| `setNonOpusRemovalEnabled(bool)` | удаление не-Opus кодеков |
| `setSkipRequestReallocEnabled(bool)` | пропуск realloc запросов |
| `setEnqueuedCommandMergeEnabled(bool)` | слияние команд в очереди |

## Что важно

1. **`SDK_VERSION = "0.1.13"`** — версия SDK звонков.

2. **`setEmulatedNegotiationErrorType`** — эмуляция ошибки negotiation. Это тестовый механизм в production.

3. **`setBackendRenderVmoji`** — серверный рендеринг анимоджи. Данные анимоджи передаются на сервер.

4. **`joinAnonByLink`** — анонимный вход по ссылке. Поддерживается анонимное участие в звонках.

5. **`bitrate_dump_config`** — конфиг дампа битрейта хранится в SharedPreferences.

## Сводка

`ConversationFactory`: SDK v0.1.13. 6 методов создания звонков. `setEmulatedNegotiationErrorType`/`setBackendRenderVmoji`/`setLoadKwsBySdkEnabled`. `joinAnonByLink` — анонимный вход.
