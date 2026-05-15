# Vulnerability 17: Server-Side Vulnerabilities Exposed Through Client Code

## Summary
Analysis of Max messenger decompiled source reveals multiple server-side vulnerability patterns exposed through client-side API usage.

---

## 1. API Endpoints Discovered

- **Base API**: `https://api.ok.ru` (hardcoded in `AbstractC1161tp.java`)
- **Calls endpoint**: configurable via PmsKey `calls-endpoint`
- **Batch API**: `batch.execute/` prefix (e.g., `batch.execute/vchat.getExternalIdsByOkIds`, `batch.execute/vchat.getOkIdsByExternalIds`)
- **WebSocket/WT endpoints**: dynamic signaling endpoints for calls
- **API methods found**: `JoinConversation`, `StartConversation`, `CreateJoinLink`, `RemoveJoinLink`, `HangupConversation`, `GetOkIdByExternalId`, `GetOkIdsByExternalIds`, `ClientSupportedCodecs`

---

## 2. IDOR Patterns (Insecure Direct Object References)

**HIGH RISK**: Multiple API calls use user-controllable IDs with no client-side ownership validation:

- `chatId` (long) passed directly to poll results, message context menus, file downloads, location sharing, admin permissions
- `messageId` (long) used in video viewer, file download, poll operations
- `contactId` (long) in `ProfileEditAdminPermissionsWidget` — could grant admin to arbitrary contacts
- `senderId` in `ShowLocationScreen` — access other users' location data
- `fileId`/`attachId` in `FileDownloadWarningBottomSheet` — download arbitrary attachments
- `pollId` in poll finish/result screens — manipulate polls in other chats

**Key finding**: Parameters like `chatId`, `messageId`, `contactId` are passed as plain longs with no cryptographic binding or ownership token.

---

## 3. Mass Assignment Vulnerabilities

- **Admin permissions**: `ProfileEditAdminPermissionsWidget` takes `chatId + contactId + type` — client specifies the permission type to grant
- **Call features**: `enableFeatureForAll`/`enableFeatureForRoles` — client sends feature name and role set directly via signaling JSON commands
- **Media mute**: `updateMediaOptionsForParticipant`/`updateMediaOptionsForAll` — client sends media state maps for arbitrary participants
- **Role grants**: `grantRoles(participantId, revoke, roles[])` — client specifies roles array directly in `StereoRoomManagerImpl`
- **PmsKey settings**: 300+ server-controlled parameters (max-msg-length, max-participants, file-upload-max-size, etc.) delivered to client — if any are trusted client-side without server re-validation, limits can be bypassed

---

## 4. Rate Limiting (Client-Side Only)

Evidence of client-side throttling suggesting server may lack enforcement:

- `RetryKt` implements `retryWithFastBackoff` and `retryWithSlowBackoff` — client-side retry logic
- `retryApiCallForFastWorkRequired`, `retryApiCallForBackgroundWork`, `retryApiCallForJoining`, `retryApiCallForOutgoing` — different retry strategies per call type
- `send-queue-size` PmsKey — client-enforced send queue limit
- `subscription-timeout-seconds` — client-side subscription timeout
- `wm-backlog-worker-backoff-delay-sec` — client-side backoff for workers
- No evidence of server-returned `429` or `Retry-After` header handling

**Risk**: Removing client-side throttling could allow API flooding.

---

## 5. Admin/Moderator API Calls Accessible to Regular Users

- **`promoteParticipant`** / **`revokePromotion`** in `StereoRoomManagerImpl` — sends promotion commands via signaling
- **`enableFeatureForAll`** / **`enableFeatureForRoles`** — sends JSON `{"command": "enable-feature-for-roles", "roles": [...]}` via signaling
- **`muteAll()`** in `Conversation` interface — no visible client-side admin check before call
- **`setAnonJoinForbidden`** / **`setWaitingRoomEnabled`** — call-level settings modifiable
- **`addParticipantsExtIds` with `isUnban` parameter** — client controls unban flag
- **`AddChatAdminsScreen`** / **`AdminsFromContactsScreen`** — admin addition screens pass only `chatId` + `localAccountId`

**Risk**: If server doesn't re-validate caller's admin status, regular users could execute admin commands.

---

## 6. Information Disclosure

- **`BatchInternalIdResponse`** maps `external_user_id` ↔ `ok_user_id` — leaks internal platform IDs
- **`ConversationParams`** response includes: `endpoint`, `token`, `wsIps`, `wtEndpoint`, STUN/TURN servers, `p2p_forbidden` flag
- **`CallInfo`** contains full endpoint URLs and connection parameters
- **`JoinByLinkResponse`** exposes `wt_endpoint`, `endpoint`, device index
- **Participant data**: `isAdmin()`, `isCreator()` checks on `ConversationParticipant` — full role info sent to all participants
- **`debug-profile-info`** PmsKey suggests debug data may be exposed in production

---

## 7. Broken Access Control

- **Join by link**: `JoinConversationByLink` only requires a link string + optional `anonymToken` — no additional auth
- **File access**: `fileUrl` passed directly in `FileDownloadWarningBottomSheet` — URLs may be predictable/unprotected
- **Poll manipulation**: `PollFinishBottomSheet` takes `chatId + messageId + pollId` — could finish polls in other chats
- **Chat history**: `queryChatHistory(offset, count, callback)` — pagination with no visible per-message access control
- **Location access**: `ShowLocationScreen` accepts `senderId + messageId` — could view location of messages in other chats

---

## 8. Batch/GraphQL Endpoints

- **Batch API confirmed**: `OkApiServiceInternal` uses `BATCH_PREFIX = "batch.execute/"` 
- **`BatchRequestKt.toBatchRequest()`** — combines multiple requests into single batch call
- **`parseBatchResponse()`** — generic batch response parser
- **Methods batched**: `vchat.getExternalIdsByOkIds`, `vchat.getOkIdsByExternalIds`

**Risk**: Batch endpoint could allow:
- Enumeration of user IDs by batching ID resolution requests
- Bypassing per-request rate limits by combining many operations
- Query amplification attacks

---

## Severity Assessment

| Finding | Severity | Exploitability |
|---------|----------|----------------|
| IDOR on chatId/messageId | HIGH | Easy - just change numeric IDs |
| Admin commands without validation | HIGH | Medium - requires understanding signaling protocol |
| Batch ID enumeration | MEDIUM | Easy - batch API is straightforward |
| Client-side rate limiting only | MEDIUM | Easy - patch client or use custom client |
| Mass assignment on roles/features | HIGH | Medium - requires crafted signaling messages |
| Information disclosure (internal IDs) | MEDIUM | Easy - observe API responses |
| File URL access control | MEDIUM | Needs URL pattern analysis |

---

## Recommendations for Testing

1. Intercept API calls and swap `chatId`/`messageId` values to test IDOR
2. Send `promoteParticipant`/`enableFeatureForAll` signaling commands from non-admin participant
3. Batch large numbers of `getOkIdsByExternalIds` requests to test enumeration limits
4. Remove client-side retry/backoff and flood endpoints
5. Modify `PollFinishBottomSheet` parameters to finish polls in unowned chats
6. Test `addParticipantByLink` with expired/revoked links for race conditions
