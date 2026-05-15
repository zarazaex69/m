# API Endpoints - Max Messenger (One.Me / TamTam)

## Base URL
```
https://api.ok.ru
```
Source: `p000/AbstractC1161tp.java` — `Uri.parse("https://api.ok.ru")`

---

## 1. Authentication Endpoints

| Method | Source |
|--------|--------|
| `auth.anonymLogin` | `p000/lg0.java`, `p000/pqi.java`, `sdk/di/ApiModuleImpl` |
| `vchat.getAnonymTokenByLink` | `sdk/api/GetAnonymTokenByLinkRequest.java` |

- Login flow: `one.me.login.inputphone.InputPhoneScreen` → `ConfirmPhoneScreen` → `InputNameScreen`
- Token stored as `auth.token` in SharedPreferences
- Error handling: `auth.banned` check in `ConversationImpl.java`

---

## 2. Batch Execute (RPC Multiplexer)

| Method | Source |
|--------|--------|
| `batch.execute` | `OkApiServiceInternal.java` (BATCH_PREFIX) |
| `batch.executeV2` | `p000/agl.java`, `p000/av0.java` |

Batch wraps multiple API calls. Observed patterns:
- `batch.execute/vchat.getExternalIdsByOkIds`
- `batch.execute/vchat.getOkIdsByExternalIds`

---

## 3. Video Chat (vchat.*) Endpoints

| Method | Purpose | Source |
|--------|---------|--------|
| `vchat.startConversation` | Start a call | `request/StartConversation.java` |
| `vchat.joinConversation` | Join existing call | `request/JoinConversation.java` |
| `vchat.joinConversationByLink` | Join via invite link | `request/JoinConversationByLink.java` |
| `vchat.hangupConversation` | End call | `request/HangupConversation.java` |
| `vchat.getConversationParams` | Get call params | `request/GetConversationParams.java` |
| `vchat.createJoinLink` | Create invite link | `request/CreateJoinLink.java` |
| `vchat.removeJoinLink` | Remove invite link | `request/RemoveJoinLink.java` |
| `vchat.getOkIdByExternalId` | Resolve external→internal ID | `request/GetOkIdByExternalId.java` |
| `vchat.getOkIdsByExternalIds` | Batch resolve ext→int IDs | `request/GetOkIdsByExternalIds.java` |
| `vchat.getExternalIdsByOkIds` | Batch resolve int→ext IDs | `request/GetExternalIdsByOkIds.java` |
| `vchat.clientSupportedCodecs` | Report codec support | `request/ClientSupportedCodecs.java` |
| `vchat.clientStats` | Client statistics | `SdkIntervalStatEvent.java`, `SdkMetricStatEvent.java` |
| `vchat.getLogUploadUrl` | Get URL for log upload | `p000/uq7.java` |

---

## 4. Settings & Configuration

| Method | Source |
|--------|--------|
| `settings.get` | `request/GetSettings.java` |

---

## 5. Logging

| Method | Source |
|--------|--------|
| `log.externalLog` | `OneLogApiRequest.java`, `p000/mcc.java` |

---

## 6. Binary Protocol Commands (TamTam Nano Tasks)

These are protobuf-serialized commands sent over the persistent socket connection. Source: `ru/p027ok/tamtam/nano/Tasks.java`

### Chat Management
- `ChatCreate` — Create new chat
- `ChatDelete` — Delete chat
- `ChatClear` — Clear chat history
- `ChatHide` — Hide chat from list
- `ChatUpdate` — Update chat properties
- `ChatSubscribe` — Subscribe to channel
- `ChatsList` — List chats
- `ChatMark` — Mark chat read/unread
- `ChatMarkBatch` — Batch mark chats
- `ChatGroupMark` — Mark group
- `ChatMembersUpdate` — Add/remove members
- `ChatPersonalConfig` — Per-chat settings
- `ChatPinSetVisibility` — Pin/unpin messages
- `ChatComplain` — Report chat
- `ChannelLeave` — Leave channel
- `DeleteChatsBatch` — Batch delete chats
- `SyncChatHistory` — Sync history
- `WarmChatHistory` — Preload history

### Messaging
- `MsgSend` — Send message
- `MsgEdit` — Edit message
- `MsgDelete` — Delete message
- `MsgDeleteRange` — Delete message range
- `MsgReact` — Add reaction
- `MsgCancelReaction` — Remove reaction
- `MsgView` — Mark message viewed
- `MsgSendCallback` — Bot callback button press
- `MsgSharePreview` — Share preview

### Media & Files
- `FileUpload` — Upload file
- `FileUploadCmd` — File upload command
- `FileDownload` — Download file
- `FileDownloadCmd` — File download command
- `PhotoUpload` — Upload photo
- `PrepareFileUpload` — Prepare upload slot
- `VideoUpload` — Upload video
- `VideoConvert` — Transcode video
- `VideoPlay` — Stream video
- `ExternalVideoSend` — Send external video link

### Profile & Contacts
- `Profile` — Get/update profile
- `ContactUpdate` — Update contact
- `ContactVerify` — Verify contact
- `RemoveContactPhoto` — Remove contact photo
- `ChangeChatPhoto` — Change chat avatar
- `ChangeProfileOrChatPhoto` — Change profile/chat photo

### Assets & Stickers
- `AssetsAdd` — Add asset
- `AssetsListModify` — Modify asset list
- `AssetsMove` — Move asset
- `AssetsRemove` — Remove asset

### Drafts & Scheduling
- `DraftSave` — Save draft
- `DraftDiscard` — Discard draft
- `UpdateFireTimeProtoTask` — Update scheduled message time

### Moderation & Admin
- `Complain` — Report content/user
- `ChatComplain` — Report chat
- `SuspendBot` — Suspend bot

### Location
- `LocationRequest` — Request location
- `LocationStop` — Stop location sharing

### Presence
- `ConfirmPresent` — Confirm online presence
- `CongratsStatus` — Birthday/congrats status

### Logging
- `CritLog` — Send critical log
- `Config` — Get server config

---

## 7. Media/Upload Endpoints

- Upload URL retrieved dynamically via `vchat.getLogUploadUrl` and `requestUploadUrl` flow
- Upload status tracked in: `UPLOAD_URL_RETRIEVE` enum (`p000/qdj.java`)
- Stream URLs: `streamurl` field parsed in `p000/ba8.java`, `p000/aa8.java`
- File upload max size configured via PMS key: `file-upload-max-size`
- Video upload config: `upload-video-config` PMS key

---

## 8. WebSocket/Signaling Protocol

The app uses a **binary protobuf protocol** over persistent TCP/TLS connection (not standard WebSocket frames).

### Stat Event Types (jm5 enum)
| Name | ID | Key |
|------|-----|-----|
| STARTUP_REPORT | 0 | `startup_report` |
| AB_EVENT | 1 | `ab_event` |
| OPCODE | 2 | `opcode` |
| CHAT_HISTORY_WARM | 3 | `ch_history` |
| CHAT_LIST | 4 | `open_chats_to_render` |
| WEB_APP | 5 | `web_app` |
| UPLOAD_HANG | 6 | `upload_hang` |
| UPLOAD_ERROR | 7 | `upload_error` |
| MEMORY | 8 | `memory` |
| BATTERY | 9 | `battery` |
| TRANSCODE | 10 | `transcode` |
| BAD_PUSHES | 11 | `bad_pushes` |

### Call Signaling
- WebSocket IPs provided in `CallInfo.wsIps`
- WebTransport endpoint: `CallInfo.wtEndpoint` / `wtIps`
- TURN/STUN servers parsed by `TurnStunParser`
- Signaling commands sent as JSON over WebSocket to call endpoint

---

## 9. API Protocol Parameters (ApiProtocol.java)

Key parameters used across endpoints:
```
PARAM_ANONYM_TOKEN, PARAM_CAPABILITIES, PARAM_CHAT_ID,
PARAM_CLIENT_APP_KEY, PARAM_CONVERSATION_ID, PARAM_CREATE_JOIN_LINK,
PARAM_DEVICE_ID, PARAM_DOMAIN_ID, PARAM_EXTERNAL_IDS,
PARAM_IS_VIDEO, PARAM_JOIN_LINK, PARAM_KEYS,
PARAM_ONLY_ADMIN_CAN_RECORD, PARAM_ONLY_ADMIN_CAN_SHARE_MOVIE,
PARAM_PAYLOAD, PARAM_PEER_ID, PARAM_PLATFORM,
PARAM_PROTOCOL_VERSION, PARAM_REASON, PARAM_SDK_VERSION,
PARAM_TURN_SERVERS, PARAM_TYPE, PARAM_UIDS,
PARAM_VERSION, PARAM_WAIT_FOR_ADMIN, PARAM_WEB_RTC_PLATFORM
```

---

## 10. Error Types (API Exceptions)

| Exception | Meaning |
|-----------|---------|
| `ApiErrorTooManyUsers` | Participant limit |
| `ApiErrorParticipantLimitExceeded` | Hard participant cap |
| `ApiErrorUserBlocked` | User is blocked |
| `ApiErrorUserPrivate` | User has private profile |
| `ApiErrorUserBanned` | User is banned |
| `ApiLoginException` | Auth failure |
| `ApiCaptchaException` | Captcha required |
| `ApiSessionChangedException` | Session invalidated |
| `UnknownOpcodeException` | Unknown binary protocol opcode |

---

## Security Notes

1. **batch.executeV2** allows multiplexing multiple API calls — potential for SSRF or privilege escalation if method validation is weak
2. **auth.anonymLogin** provides anonymous tokens for call joining — token lifecycle and scope should be audited
3. Binary protocol uses short opcodes — unknown opcodes throw `UnknownOpcodeException` suggesting server-side validation exists
4. Upload URLs are dynamically generated — potential for URL manipulation
5. `PARAM_ONLY_ADMIN_CAN_RECORD` / `PARAM_ONLY_ADMIN_CAN_SHARE_MOVIE` suggest admin-level controls that could be bypassed
