# VULN-98: Final Deep Dive — Max Messenger Remaining Vulnerabilities

**Date:** 2025-05-15  
**Scope:** Clipboard URL preview, Pin/passcode bypass, Message forwarding metadata, Disappearing messages, Group invite link security  
**Source:** ~/max/decompiled/jadx/base/sources

---

## 1. Clipboard URL Preview / Link Preview — SSRF Potential

### Findings

No dedicated client-side URL preview fetching logic (OGP/OpenGraph parser, `fetchPreview`, `urlPreview`) was found in the decompiled source. The only "preview" references are:
- `CallJoinLinkPreviewWidget` — UI widget for call join link previews (not URL content fetching)
- `profile_media_link_preview` — layout resource ID
- `ChatPreviewView` — theme preview widget

### Assessment: LOW (client-side) / MEDIUM (server-side)

The app does **not** perform client-side URL preview fetching. Link previews are generated **server-side** — the client sends the URL to the server, which fetches the OGP metadata and returns it. This means:

- **SSRF risk exists on the server**, not the client. An attacker can send a message containing `http://169.254.169.254/latest/meta-data/` or internal network URLs, and the server will attempt to fetch them to generate a preview.
- The client has no URL validation or filtering before sending URLs to the server.
- No evidence of SSRF protections (IP blocklist, private range filtering) visible from client code.

**Impact:** Server-side SSRF via crafted URLs in messages. Could expose cloud metadata endpoints, internal services.

---

## 2. Pin/Passcode Bypass via Notification Direct Reply

### Findings

**Pin Lock Implementation:**
- `app.pinLock.screenshotEnabled` — SharedPreferences boolean controlling screenshot protection
- Pin lock UI resources: `pin_lock_step_enter`, `pin_lock_step_confirm`, `pin_lock_wrong_code`, `pin_lock_wrong_code_alert`
- Biometric integration via `BiometricPrompt` in `WebAppRootScreen`
- Auto-lock setting: `setting_pin_lock_auto_lock`

**Critical Finding — Notification Direct Reply Bypasses Pin Lock:**

In `NotificationTamService.java` (line ~1078):
```java
case "ru.ok.tamtam.action.DIRECT_REPLY":
    // Directly processes reply text and sends message
    // NO pin lock state check before sending
    Bundle resultsFromIntent = RemoteInput.getResultsFromIntent(intent);
    CharSequence text = resultsFromIntent.getCharSequence("ru.ok.tamtam.extra.TEXT_REPLY");
    // ... sends message directly via m20533a() or n3c
```

The `NotificationTamService` handles `DIRECT_REPLY` action **without checking if the app is pin-locked**. When the device is locked and the app has pin lock enabled:
1. Notification arrives with reply action
2. User can reply directly from notification shade
3. Message is sent without any pin/biometric verification

### Assessment: MEDIUM

**Impact:** Bypasses app lock for sending messages. An attacker with physical access to a locked device can:
- Read message previews in notifications
- Send replies to any conversation that has a notification visible
- No pin/biometric check is performed for the `DIRECT_REPLY` intent

**Additional concern:** The notification also exposes `CHAT_SERVER_ID` and `MESSAGE_SERVER_ID` as intent extras, leaking conversation identifiers.

---

## 3. Message Forwarding Metadata Leak

### Findings

**Database Schema — Forwarding metadata stored:**
```sql
msg_link_type, msg_link_id, msg_link_chat_id, msg_link_chat_name, 
msg_link_chat_link, msg_link_out_chat_id, msg_link_out_msg_id
```

**Forward Data Model (`od7.java`):**
```
ForwardMessagesSendData(messageIds, attachId, isForwardAttach, text, shouldHideAuthor, delayedAttributes)
```

**Key observations:**
- `shouldHideAuthor` is a boolean field in `ForwardMessagesSendData` — sent to server
- `isAuthorVisibilityAvailable` in `ForwardQuoteData` — determines if the option is shown
- `GetAuthorVisibilityAvailableUseCase` — checks if hiding is available
- `isOnboardedAuthorVisibilityOnForward` — tracks if user has seen the onboarding

**Critical Issue:** The database stores `msg_link_chat_id`, `msg_link_chat_name`, and `msg_link_chat_link` for ALL forwarded messages regardless of `shouldHideAuthor`. These fields persist the **original source chat information** in the local database of the recipient.

### Assessment: MEDIUM-HIGH

**Impact:** Even when `shouldHideAuthor=true`:
- The `msg_link_chat_id` (original chat ID) is stored in the recipient's local database
- `msg_link_out_chat_id` and `msg_link_out_msg_id` reference the original message
- A user with root access or database extraction can recover the original sender/chat info
- The server may also retain this metadata regardless of the hide flag

The "hide author" feature appears to be a **UI-level concealment** rather than true metadata stripping.

---

## 4. Disappearing Messages — TTL Implementation

### Findings

**Message TTL fields in database:**
- `ttl` INTEGER NOT NULL — the time-to-live value
- `live_until` INTEGER NOT NULL — absolute timestamp when message expires
- `view_time` INTEGER NOT NULL — when the message was viewed

**Server payload parsing (`lj7.java`):**
```java
if (strM11185U.equals("liveUntil")) {
    lValueOf6 = Long.valueOf(j8h.m11182R(kxaVar, j2));
}
```

**Critical Findings:**
1. **No client-side deletion scheduler found.** No `ttlTimer`, `ttlCleanup`, `ttlExpiry`, `checkTtl`, or similar background task exists in the decompiled code.
2. **Messages are stored with full content** (`text`, `attaches` BLOB) alongside the `ttl` and `live_until` fields.
3. **No `DELETE FROM messages WHERE live_until < ?`** query was found — there is no evidence of automatic purging based on TTL expiry.
4. The `live_until` field is read from the database during message display, suggesting the client **hides** expired messages in the UI but does **not delete them from the database**.

### Assessment: HIGH

**Impact:** "Disappearing" messages are **not actually deleted** from the local SQLite database. They are merely hidden in the UI when `live_until` timestamp is exceeded. An attacker with:
- Root access to the device
- Physical access + ADB
- Backup extraction

...can recover ALL "disappeared" messages in full, including text and attachments (stored as BLOBs).

The TTL enforcement appears to be purely a **UI filter**, not a data destruction mechanism.

---

## 5. Group/Call Invite Link Security

### Findings

**Invite link infrastructure:**
- PMS keys: `invite-link`, `invite-short`, `invite-long`, `invite-header` — server-configured URL templates
- `inviteLink` / `inviteShort` / `inviteLong` — different link formats
- API methods: `vchat.getAnonymTokenByLink`, `vchat.joinConversationByLink`, `vchat.removeJoinLink`

**Anonymous Join Flow (`GetAnonymTokenByLinkRequest.java`):**
```java
public void populateParams(C0282dp c0282dp) {
    c0282dp.m5683b(ApiProtocol.PARAM_JOIN_LINK, this.joinLink);
    c0282dp.m5683b("anonymName", this.name);
}
```

- Anonymous users can join calls with just a link + arbitrary name
- Error code 457 = auth required (some links require authentication)
- `JoinByLinkResponse` returns: ICE servers, conversation ID, peer info — **before full authentication**

**Key Issues:**
1. **No rate limiting visible client-side** — the `GetAnonymTokenByLinkRequest` has no retry/backoff logic
2. **Link format leaks info:** The `invite-short` and `invite-long` PMS keys suggest predictable URL patterns
3. **Anonymous join without auth:** `GetAnonymTokenByLinkRequest` allows joining with just a link string and arbitrary `anonymName` — no CAPTCHA or verification
4. **Pre-join info disclosure:** `CallJoinLinkPreviewWidget` shows call participants (via `OneMeStackAvatarView`) before joining, leaking group member info to anyone with the link

### Assessment: MEDIUM

**Impact:**
- Invite links may be brute-forceable if the token space is small (depends on server-side `invite-short` format)
- Anyone with a link can see group/call participant info before joining
- Anonymous join allows impersonation (arbitrary `anonymName`)
- No visible rate limiting on join attempts

---

## Summary Table

| # | Vulnerability | Severity | Type |
|---|---|---|---|
| 1 | Server-side SSRF via link preview | MEDIUM | Server-side |
| 2 | Pin lock bypass via notification reply | MEDIUM | Authentication bypass |
| 3 | Forwarded message metadata leak despite "hide author" | MEDIUM-HIGH | Privacy violation |
| 4 | Disappearing messages not deleted from DB | HIGH | Data persistence |
| 5 | Invite link info disclosure + anonymous join | MEDIUM | Access control |

---

## Recommendations

1. **Link Preview SSRF:** Server should validate URLs against private IP ranges, implement timeout, and restrict redirect following
2. **Pin Lock Bypass:** Check `pinLock` state in `NotificationTamService` before processing `DIRECT_REPLY`; disable reply action when app is locked
3. **Forward Metadata:** Strip `msg_link_chat_id`, `msg_link_chat_name`, `msg_link_chat_link` from forwarded messages when `shouldHideAuthor=true` — both server-side and in recipient's local DB
4. **Disappearing Messages:** Implement a background `WorkManager` task that periodically `DELETE`s messages where `live_until < currentTime`; also securely wipe attachment BLOBs
5. **Invite Links:** Add rate limiting, CAPTCHA for anonymous joins, increase token entropy, don't show participant info before authentication
