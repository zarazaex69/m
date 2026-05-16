---
tags: [protocol, tasks, protobuf, background-tasks, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[203-live-location]]"
  - "[[08-background-wake]]"
---

# Tasks.proto — фоновые задачи (protobuf nano)

`ru.ok.tamtam.nano.Tasks` — protobuf nano-схема фоновых задач клиента. 60+ типов задач.

## Полный список задач

| Задача | Что |
|---|---|
| `AssetsAdd`/`AssetsMove`/`AssetsRemove`/`AssetsListModify` | управление медиа-ассетами |
| `ChangeChatPhoto`/`ChangeProfileOrChatPhoto` | смена фото |
| `ChannelLeave` | выход из канала |
| `ChatClear`/`ChatDelete`/`ChatHide` | управление чатами |
| `ChatComplain`/`Complain` | жалобы |
| `ChatCreate`/`ChatUpdate`/`ChatMembersUpdate` | создание/обновление чатов |
| `ChatGroupMark`/`ChatMark`/`ChatMarkBatch` | отметки чатов |
| `ChatPersonalConfig`/`ChatPinSetVisibility` | персональные настройки чата |
| `ChatSubscribe`/`ChatsList` | подписки и список чатов |
| `Config` | конфигурация |
| `ConfirmPresent`/`CongratsStatus` | подтверждения/поздравления |
| `ContactUpdate`/`ContactVerify` | обновление/верификация контактов |
| `CritLog` | **критические логи** |
| `DeleteChatsBatch` | пакетное удаление чатов |
| `DraftDiscard`/`DraftSave` | черновики |
| `ExternalVideoSend` | отправка внешнего видео |
| `FileDownload`/`FileDownloadCmd`/`FileUpload`/`FileUploadCmd`/`PrepareFileUpload` | файловые операции |
| `LocationRequest`/`LocationStop` | **геолокация** (liveLocation/messageId/requestId) |
| `MsgCancelReaction`/`MsgReact` | реакции |
| `MsgDelete`/`MsgDeleteRange` | удаление сообщений |
| `MsgEdit` | редактирование |
| `MsgSend`/`MsgSendCallback` | отправка сообщений |
| `MsgSharePreview`/`MsgView` | просмотр/превью |
| `PhotoUpload`/`VideoUpload`/`VideoConvert`/`VideoPlay` | медиа |
| `Profile`/`RemoveContactPhoto` | профиль |
| `SuspendBot` | приостановка бота |
| `SyncChatHistory`/`WarmChatHistory` | синхронизация истории |
| `UpdateFireTimeProtoTask` | обновление времени уведомлений |
| `Quality` | качество |

## LocationRequest (proto)

| Поле | Что |
|---|---|
| `requestId` | ID запроса |
| `messageId` | ID сообщения |
| `liveLocation` | живая геолокация |

## Что важно

1. **`CritLog`** — задача отправки критических логов. Отдельный тип задачи для логов.

2. **`LocationRequest`/`LocationStop`** — геолокация как фоновая задача. `liveLocation=true` — живая геолокация.

3. **`SuspendBot`** — приостановка бота. Сервер может управлять ботами через задачи.

4. **`UpdateFireTimeProtoTask`** — обновление времени уведомлений. Сервер управляет расписанием уведомлений.

## Сводка

60+ типов фоновых задач в protobuf nano. Ключевые: `LocationRequest`(liveLocation), `CritLog`, `SuspendBot`, `UpdateFireTimeProtoTask`, `SyncChatHistory`/`WarmChatHistory`.
