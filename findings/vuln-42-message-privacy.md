# VULN-42: Message Privacy Analysis

## 1. Message Deletion

**Key files:** `ru/p027ok/tamtam/nano/Tasks.java` (MsgDelete proto), `p000/njb.java` (delete task impl)

### Findings:

- **MsgDelete protobuf** has fields: `forMe` (bool), `notDeleteMessageFromDb` (bool), `complaint` (string)
- The `forMe` flag controls delete-for-self vs delete-for-all behavior
- **Critical:** `notDeleteMessageFromDb` flag exists — when `true`, the message is NOT deleted from local DB even after "deletion"
- In `njb.java` line 304: `msgDelete.notDeleteMessageFromDb = this.f44026k` — this flag is passed to the server task
- The server API request (line 337) sends `forMe` parameter: `dk9Var.m18546c("forMe", z)` — when `forMe=false`, it's a delete-for-all request sent to server

### Privacy Issues:

1. **`notDeleteMessageFromDb=true`** means messages can be "deleted" in UI but retained in local SQLite DB (`messages` table)
2. Delete-for-all sends server request but local DB deletion is handled separately via `rsa.m20120c()` which runs `DELETE FROM messages WHERE chat_id = ? AND id in (...)`
3. **Recovery possible:** If `notDeleteMessageFromDb=true` or if the delete task fails (see `onMaxFailCount` → `returnToActiveMessages`), messages revert to ACTIVE status
4. PmsKey `delete-msg-fys-large-chat-disabled` can disable delete-for-yourself in large chats

## 2. Message Forwarding

**Key files:** `p000/od7.java` (ForwardMessagesSendData), `one/p011me/chats/forward/ForwardPickerScreen.java`

### Findings:

- `ForwardMessagesSendData` contains field `shouldHideAuthor` (boolean, field `f46816e`)
- Forward URL pattern: `:chats/forward?messages_ids=`
- String resources: `picker_chats_forward_from_self`, `picker_chats_forward_from_somebody`, `picker_chats_forward_from_channel` — UI shows original sender info

### Privacy Issues:

1. **`shouldHideAuthor` is client-side only** — no corresponding field found in the protobuf `Tasks.MsgSend` structure sent to server. The MsgSend proto only has: requestId, messageId, chatId, chatServerId, userId, notify, lastKnownDraftTime, traceId
2. **No `hideAuthor` in wire protocol** — the server likely still stores the original sender in message metadata regardless of the hide-author UI toggle
3. The `forwardedMessageId` field in message models (`uua.java` line 64) retains the original message reference
4. Forward click handler exposes `MessageLink$ForwardModel` with original sender data

## 3. Read Receipts

**Key files:** `p000/sbf.java` (ReadMarkSender), `p000/q4c.java`, `p000/haj.java`

### Findings:

- Read marks are sent via `sbf.m21144c()` which calls server with chatServerId and mark timestamp
- `ReadMarkSender` (class `sbf`) sends read position to server unconditionally
- PmsKey `max-readmarks` controls maximum tracked read marks (server-side limit)
- `MemberListItem` in `gla.java` contains `lastReadMark` per member — **server tracks per-user read position**
- `NotificationsReadMarksDao` persists read marks locally
- `onSelfReadMarkChanged` events propagate read state changes

### Privacy Issues:

1. **No disable option found** — no client-side toggle to disable read receipt sending. The `ReadMarkSender` sends unconditionally when a chat is opened
2. **Server tracks per-member reads** — `lastReadMark` per `MemberListItem` proves server maintains granular per-user read tracking
3. Even if UI hides receipts, server stores the data (evidenced by `lastOpenReadMark` in `Protos.java` Chat structure)
4. Read marks are sent as soon as messages become visible — no user consent gate

## 4. Typing Indicators

**Key files:** `p000/qp6.java` (PMS config), `p000/lza.java` (typing sender)

### Findings:

- PmsKey `typing-send-enabled` (field `f88867typingsendenabled`) is a **server-controlled** feature flag
- `qp6.java` exposes `isTypingSendEnabled()` method
- Typing events sent via `sendTypingJob` in `lza.java` and `uf3.java`
- `UploadFileAttachWorker` also sends typing during file uploads via `n4d.m14693f()`

### Privacy Issues:

1. **`typing-send-enabled` is a server PMS key, not a user setting** — the server decides whether typing is sent, not the user
2. When enabled, typing events are sent to the server which relays them — the server always knows you're typing
3. Setting it to `false` would stop the client from sending typing events entirely (server wouldn't know), BUT this is not user-controllable — it's a remote config flag
4. `notif-typing-presence` PmsKey links typing to presence system — typing reveals online status

## 5. Online Status / Presence

**Key files:** `ru/p027ok/tamtam/android/prefs/PmsKey.java` (presence keys)

### Findings:

- Presence system controlled by multiple PmsKeys: `presence-ttl`, `presence-view-port`, `presence-external`, `presence-seen-eq`, `presence-stat`, `presence-keep-bg-cache`, `presence-offline-move-timeout`
- `phone-privacy-config` PmsKey exists with `phonePrivacySettingEnabled` accessor
- `isOnline` field tracked in contact/member models
- `presence-offline-move-timeout` controls when user is marked offline

### Privacy Issues:

1. **No user-facing hide-online toggle found** — presence is managed entirely by server-controlled PMS keys
2. `presence-keep-bg-cache` suggests server caches presence even when app is backgrounded
3. `presence-stat` enables presence statistics collection on server
4. `presence-external` controls whether presence is shared externally — implies server always tracks internally
5. Online status is derived server-side; client cannot opt out of being tracked as online

## Summary of Critical Privacy Gaps

| Feature | User Control | Server Tracking | Privacy Risk |
|---------|-------------|-----------------|--------------|
| Delete for all | Yes (UI) | Sends request | Messages recoverable from local DB via `notDeleteMessageFromDb` flag |
| Hide forward author | Yes (UI) | Not in wire proto | Server likely retains original sender metadata |
| Read receipts | **None** | Always tracked | Per-user read marks stored server-side, no opt-out |
| Typing indicators | **None** (server flag) | When enabled | Server-controlled, not user preference |
| Online status | **None** | Always tracked | No hide option; presence-stat collects analytics |

**Severity:** Medium-High — Users have no meaningful control over read receipts, typing, or online status privacy. The "hide author" forward feature may be cosmetic only.
