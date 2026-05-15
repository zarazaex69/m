# VULN-48: Scheduled Messages & Draft Sync — Privacy and Security Analysis

**Date:** 2025-05-15  
**Target:** Max Messenger (Android, decompiled)  
**Source:** ~/max/decompiled/jadx/base/sources  
**PmsKeys:** `scheduled-messages-enabled`, `scheduled-posts-enabled`, `drafts-sync-enabled`, `scheduled-faves-enabled`

---

## 1. Grep Results (Key Hits)

```
PmsKey.java:164:  PmsKey f88678draftssyncenabled = new PmsKey("drafts-sync-enabled", 46);
PmsKey.java:170:  PmsKey f88839scheduledmessagesenabled = new PmsKey("scheduled-messages-enabled", 48);
PmsKey.java:173:  PmsKey f88840scheduledpostsenabled = new PmsKey("scheduled-posts-enabled", 49);
PmsKey.java:176:  PmsKey f88838scheduledfavesenabled = new PmsKey("scheduled-faves-enabled", 50);

j5f.java:284:    tt_worker_draft_sync = 2131955570  (WorkManager worker ID for draft sync)
bv5.java:93-97:  Tasks.DraftDiscard { requestId, chatId, serverTime }
gv5.java:127-130: Tasks.DraftSave { requestId, chatId, draft (bytes) }
Tasks.java:7251:  UpdateFireTimeProtoTask { requestId, chatId, messageId, fireTime, notifySender }
wy9.java:50:      SendMedia { caption, media, sendAsFile, sliceData, fireTime }
```

Key classes:
- `gv5` — DraftSave task (sends draft content to server)
- `bv5` — DraftDiscard task (tells server to clear draft)
- `yv3` — dropServerDraft logic (checks `drafts-sync-enabled`)
- `ov5` — triggers DraftSave via WorkManager
- `xaj` — UpdateFireTimeProtoTask (reschedule scheduled messages)
- `ScheduledSendPickerBottomSheet` — UI for scheduling

---

## 2. Draft Sync: Real-Time Server Upload (PRIVACY CONCERN — HIGH)

**Finding:** Drafts ARE synced to the server in near-real-time when `drafts-sync-enabled` is true.

**Evidence:**
- `gv5.java` (DraftSave task) serializes draft content as protobuf bytes and sends to server:
  ```java
  Tasks.DraftSave draftSave = new Tasks.DraftSave();
  draftSave.chatId = this.f22711d;
  draftSave.draft = ev5.m6768b(this.f22712e);  // full draft content serialized
  ```
- `ov5.java:85` triggers DraftSave immediately when draft changes:
  ```java
  ucc.m22785r(uccVar, new gv5(uccVar.m22809s().f17673a.m25807k(), j2, tjcVar2));
  ```
- `ChatScreen.java:998` logs: `"save draft, textLength:"` — confirms draft is saved on every text change
- `yag.java` tracks `draftsLastSync` timestamp — periodic full sync exists
- `ri9.java` has `isDraftsChanged` flag — triggers sync on reconnect

**Privacy Impact:** The server sees the full text of what users are typing BEFORE they press send. This includes:
- Partially typed messages
- Messages the user decides not to send
- Sensitive content being composed

**Controlled by:** `PmsKey.f88678draftssyncenabled` — server-side feature flag (user has NO opt-out)

---

## 3. Scheduled Messages: Server Reads Content Before Delivery (CONFIRMED)

**Finding:** Scheduled messages are sent to the server immediately at scheduling time, not at delivery time.

**Evidence:**
- `wy9.java` (SendMedia) includes `fireTime` as a parameter alongside the full message content:
  ```java
  "SendMedia(caption=..., media=..., sendAsFile=..., sliceData=..., fireTime=...)"
  ```
- The message content (caption, media, attachments) is uploaded to the server at the moment of scheduling
- `UpdateFireTimeProtoTask` (`xaj.java`) only updates the delivery time — the content is already on the server:
  ```java
  updateFireTimeProtoTask.chatId = this.f79497d;
  updateFireTimeProtoTask.messageId = this.f79498e;  // message already exists server-side
  updateFireTimeProtoTask.fireTime = this.f79499f;   // just changes when to deliver
  ```
- `ScheduledChatScreen` is a separate view showing messages already stored server-side

**Security Impact:** Server operators can read scheduled message content at any time before the intended delivery. There is no client-side encryption of scheduled content. Messages scheduled far in the future (days/weeks) sit in plaintext on the server.

---

## 4. Scheduled Message Spoofing (Sender Identity)

**Finding:** No client-side sender spoofing vector found, but the `userId` field in DraftSave requests is derived from chat metadata, not hardcoded from auth.

**Evidence:**
- `gv5.java:249`: `fx2Var.m18551h(jLongValue, "userId")` — userId is extracted from `fr2VarM17437N.m7810q().m5498r()` (channel/group context)
- `bv5.java` (DraftDiscard) similarly uses userId from chat metadata
- The `chatId` in both DraftSave and scheduled send tasks is a client-provided long value

**Risk Assessment:** LOW-MEDIUM. The userId appears to come from the local chat object (which is server-provided), but:
- A modified client could potentially supply arbitrary `chatId` values in DraftSave/DraftDiscard
- The `UpdateFireTimeProtoTask` takes `chatId` + `messageId` — if server doesn't validate ownership, a modified client could attempt to reschedule other users' messages
- No evidence of cryptographic binding between sender identity and the scheduled message

---

## 5. Draft Sync Includes Deleted/Unsent Messages (PRIVACY CONCERN — MEDIUM)

**Finding:** Draft content is sent to server BEFORE the user decides to send or delete. Discarded drafts require an explicit DraftDiscard call.

**Evidence:**
- `bv5.java` implements `Tasks.DraftDiscard` — a separate network request to tell the server to delete the draft
- `yv3.java:57-65`: Logic shows that discard only happens if `drafts-sync-enabled` is true AND there was a previous server draft:
  ```
  "Drafts sync NOT enabled. Not discard to server"
  "Drafts sync enabled. No old draft. Not discard to server"
  "Drafts sync enabled. Discard to server"
  ```
- `gv5.java:45`: `"onSuccess: draft was discarded"` — race condition: if DraftSave succeeds but DraftDiscard fails (network error), the server retains the unsent draft
- `bv5.java:50`: Server compares timestamps: `"chat has server draft older than current DRAFT_DISCARD server time"` — time-based reconciliation means stale drafts may persist

**Privacy Impact:**
- If user types a message and deletes it, the content was already uploaded
- DraftDiscard is a best-effort operation (max 1 retry per `mo461f()`)
- Network failures during discard leave "deleted" content on the server
- The `draftsLastSync` mechanism means ALL drafts are bulk-synced periodically

---

## 6. Scheduled Faves (scheduled-faves-enabled) — Reminders Feature

**Finding:** `scheduled-faves-enabled` controls the "Scheduled Reminders" feature — the ability to schedule messages to yourself in Favorites/Saved Messages.

**Evidence:**
- `p5g.java` (ScheduledSendPickerMode enum):
  ```java
  REMINDER = 0   // "scheduled-faves-enabled" — send to self/favorites
  CHANNEL = 1    // "scheduled-posts-enabled" — channel posts
  DEFAULT = 2    // "scheduled-messages-enabled" — regular chat messages
  ```
- `ScheduledSendPickerBottomSheet.java:162`:
  ```java
  textView.setText(iOrdinal != 0 ? iOrdinal != 1 ? 
      k5f.scheduled_send_message_title :    // DEFAULT mode
      k5f.scheduled_send_post_title :       // CHANNEL mode
      k5f.scheduled_remind_title);          // REMINDER mode (faves)
  ```
- Resource strings confirm: `scheduled_remind_title`, `scheduled_remind_button_text`, `scheduled_reminders_send_later`, `scheduled_send_favs_onboarding_tooltip`
- `wol.java:41`: Different labels based on chat type: `scheduled_reminders_send_later` for favorites
- `v3f.java:164`: `scheduled_chat_screen_empty_reminders` — empty state for reminders view
- `qp6.java:733`: Feature flag usage: `new ro6(this, PmsKey.f88838scheduledfavesenabled, i20)`

**Summary:** "Scheduled Faves" = personal reminders. Users can schedule a message to their Saved Messages/Favorites that fires at a chosen time. Same server-side storage concerns apply — reminder content sits on server in plaintext until fire time.

---

## Summary of Vulnerabilities

| # | Issue | Severity | Type |
|---|-------|----------|------|
| 1 | Draft sync uploads typing in real-time to server | HIGH | Privacy |
| 2 | Scheduled messages stored in plaintext on server before delivery | MEDIUM | Privacy |
| 3 | No user opt-out for draft sync (server-controlled flag) | MEDIUM | Privacy/Control |
| 4 | Failed DraftDiscard leaves "deleted" content on server | MEDIUM | Data Retention |
| 5 | Potential chatId manipulation in DraftSave/UpdateFireTime | LOW | Integrity |
| 6 | Bulk draftsLastSync exposes all draft history periodically | MEDIUM | Privacy |

---

## Recommendations

1. **Draft sync should be opt-in** with clear user disclosure that typing is transmitted before sending
2. **Scheduled messages** should use client-side encryption if stored server-side before delivery
3. **DraftDiscard** should have guaranteed delivery (persistent retry) or drafts should expire server-side
4. **Server should validate** chatId/messageId ownership in UpdateFireTime to prevent rescheduling attacks
5. **Audit trail** needed for when server accesses scheduled message content before fire time
