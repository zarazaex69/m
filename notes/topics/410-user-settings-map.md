---
tags: [user-settings, transcription, audio, server-control, surveillance, push, notifications]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ma4.java
related:
  - "[[409-audio-video-transcription]]"
  - "[[03-pms-server-flags]]"
  - "[[245-login-ws-response]]"
---

# UserSettings — серверные настройки пользователя

`ma4` — объект UserSettings, синхронизируемый с сервером. Содержит настройки пользователя.

## Поля UserSettings (map)

| Ключ | Что |
|---|---|
| `pushNewContacts` | push для новых контактов |
| `dontDustirbUntil` | не беспокоить до (timestamp) |
| `hiddenOnline` | скрытый онлайн |
| `led` | LED уведомления |
| `dialogsLed` | LED для диалогов |
| `chatsLed` | LED для чатов |
| `vibration` | вибрация |
| `dialogsVibration` | вибрация для диалогов |
| `chatsVibration` | вибрация для чатов |
| `inactiveTTL` | TTL неактивности |
| `audioTranscriptionEnabled` | **транскрипция аудио включена** |
| `unsafeFiles` | небезопасные файлы |
| `safeMode` | безопасный режим |

## Что важно

1. **`audioTranscriptionEnabled`** — транскрипция аудиосообщений управляется через UserSettings. Это серверная настройка, синхронизируемая с сервером.

2. **`hiddenOnline`** — скрытый онлайн-статус. Сервер знает реальный статус.

3. **`dontDustirbUntil`** — режим "не беспокоить" с timestamp. Сервер знает, когда пользователь недоступен.

4. **`safeMode`** — безопасный режим. Неизвестно, что именно ограничивает.

5. **`inactiveTTL`** — TTL неактивности. Сервер знает, когда пользователь неактивен.

## Сводка

`UserSettings`: audioTranscriptionEnabled/hiddenOnline/dontDustirbUntil/safeMode/inactiveTTL/pushNewContacts/led/vibration. Синхронизируется с сервером.
