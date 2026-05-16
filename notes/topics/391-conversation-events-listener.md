---
tags: [calls-events, conversation-events, surveillance, fingerprint, deanon, mute]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/events/ConversationEventsListener.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/events/SharedAnalyticsEvent.java
related:
  - "[[101-calls-sdk-managers]]"
  - "[[376-asr-online-manager]]"
  - "[[382-conversation-stats]]"
---

# ConversationEventsListener — события звонка

`ConversationEventsListener` — полный список событий звонка. 40+ callbacks.

## Ключевые события наблюдения

| Callback | Что |
|---|---|
| `onParticipantsDeAnonymized(list, map)` | **деанонимизация участников** (старый ID → новый ID) |
| `onMicrophoneForciblyMuted()` | **принудительное отключение микрофона** |
| `onMigratedToServerTopology()` | переход на серверную топологию (→ ASR включается) |
| `onOpponentFingerprintChanged(fingerprint)` | **изменение fingerprint оппонента** |
| `onCustomData(JSONObject)` | **произвольные данные с сервера** |
| `onCallParticipantsNetworkStatusChanged(list)` | статус сети участников |
| `onParticipantsAdded/Removed/Updated/Changed` | изменения состава участников |
| `onRateCall(RateCallData)` | запрос оценки звонка |
| `onAnonJoinForbiddenChanged(bool)` | анонимный вход запрещён |
| `onAdminInCallChanged(bool)` | администратор в звонке |
| `onWaitingRoomParticipantsChanged(update)` | изменение зала ожидания |
| `onConversationIdChanged(prev, new)` | изменение ID звонка |
| `onJoinLinkUpdated(link)` | обновление ссылки входа |
| `onCallStartResolutionFailed(list)` | ошибка разрешения ID участников |
| `onExternalByInternalResolution(participant)` | разрешение external ID по internal |

## CallEndInfo

| Поле | Что |
|---|---|
| `reason` | причина завершения |
| `hints` | `Set<HangupHint>` — подсказки |
| `explanationHtml` | **HTML-объяснение** причины завершения |

## SharedAnalyticsEvent

Обёртка над `SdkMetricStatEvent` для `AnalyticsEventListener`. Поля: `name`, `value`, `stringValue`, `timestamp`, `data`.

## Что важно

1. **`onOpponentFingerprintChanged(fingerprint)`** — сервер отслеживает fingerprint каждого участника.

2. **`onCustomData(JSONObject)`** — сервер может отправить произвольные данные клиенту во время звонка.

3. **`onParticipantsDeAnonymized`** — деанонимизация с маппингом старый→новый ID.

4. **`explanationHtml`** в `CallEndInfo` — причина завершения может содержать HTML.

5. **`onMigratedToServerTopology`** — при этом событии `AsrOnlineManager` автоматически включает серверный ASR.

## Сводка

40+ callbacks. Ключевые: `onParticipantsDeAnonymized`/`onMicrophoneForciblyMuted`/`onMigratedToServerTopology`/`onOpponentFingerprintChanged`/`onCustomData(JSONObject)`.
