---
tags: [calls, factory, builder, initialization, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ConversationFactory.java
related:
  - "[[179-conversation-interface]]"
  - "[[134-webrtc-field-trials]]"
  - "[[194-start-conversation-delegate]]"
---

# ConversationFactory — фабрика звонков

`ConversationFactory` — создаёт `ConversationImpl` для разных типов звонков.

## Методы создания звонков

| Метод | Что |
|---|---|
| `startCall(StartCallParams)` | исходящий P2P звонок |
| `createConf(CreateConfParams)` | создать конференцию |
| `answerCall(AnswerCallParams)` | ответить на звонок |
| `joinCall(JoinCallParams)` | присоединиться к звонку |
| `joinAnonByLink(JoinAnonByLinkParams)` | анонимный вход по ссылке |
| `joinByLink(JoinByLinkParams)` | вход по ссылке |

## ConversationBuilder — параметры

| Параметр | Что |
|---|---|
| `setHasVideo(bool)` | видеозвонок |
| `setIsCaller(bool)` | инициатор |
| `setIsAnswer(bool)` | ответ |
| `setIsJoined(bool)` | присоединение |
| `setCid(str)` | ID разговора |
| `setMyId(id)` | ID текущего пользователя |
| `setOpponentId(id)` | ID собеседника |
| `setChatId(id)` | ID чата |
| `setFieldTrials(trials)` | WebRTC field trials |
| `setWaitForAdminEnabled(bool)` | ожидание администратора |
| `setWatchTogetherEnabledForAll(bool)` | watch together |
| `setAnonToken(token)` | анонимный токен |
| `setJoinLink(link)` | ссылка для входа |
| `setClientType(type)` | тип клиента |
| `setDomainId(id)` | ID домена |
| `setLocale(locale)` | локаль |
| `setExperiments(config)` | конфигурация экспериментов |
| `setRemoteSettings(settings)` | серверные настройки |

## Что важно

1. **`setRemoteSettings(getRemoteSettings())`** — каждый звонок инициализируется с серверными настройками.

2. **`setExperiments(applyBitrateDumpGatheringConfig(...))`** — эксперименты применяются при создании каждого звонка.

3. **`setFieldTrials(trials)`** — WebRTC field trials (см. [[134-webrtc-field-trials]]) передаются в каждый звонок.

4. **`p2pStartConversationDelegate` vs `serverStartConversationDelegate`** — разные делегаты для P2P и серверных звонков.

## Сводка

`ConversationFactory` — 6 методов создания звонков. `ConversationBuilder` — 18+ параметров. Каждый звонок инициализируется с `RemoteSettings`, `experiments`, `fieldTrials`.
