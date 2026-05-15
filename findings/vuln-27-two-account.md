# VULN-27: Two-Account System Session Isolation Analysis

## Overview

Max messenger implements a multi-account ("two-account-mvp") feature controlled by `PmsKey.f88866twoaccountmvp`. The system uses `lg9` (LocalAccountId) with values 0 (primary) and 1 (secondary) to distinguish accounts.

## Architecture

### Account Identity: `lg9` class (`p000/lg9.java`)
- `f36929b` = LocalAccountId(raw=0) — primary account
- `f36930c` = LocalAccountId(raw=1) — secondary account
- `m13329a(prefix, suffix)` generates account-scoped names: primary gets `""` suffix, secondary gets `"1_"` suffix

### Account Initialization
- `AccountInitializer` (`one/me/android/initialization/AccountInitializer.java`) holds `lg9 f47893b` field
- Each account gets its own DI scope via `C0835q7.m18686b(lg9Var)` returning a `t6g` scope container
- Account swap triggered via long-press in `hr9.java` on MainScreen

## Findings

### 1. SharedPreferences — PROPERLY ISOLATED ✓
Account-scoped preferences use `lg9.m13329a()` for naming:
- `ri9.java`: `lg9Var.m13329a("user", "prefs")` → "user_prefs" / "user1_prefs"
- `ugj.java`: `lg9Var.m13329a("app", "prefs")` → "app_prefs" / "app1_prefs"
- `e3e.java`: `lg9Var.m13329a("auth", "prefs")` → "auth_prefs" / "auth1_prefs"

**ISSUE**: `experiments_prefs` (in `tg6`) is created WITHOUT lg9 scoping — shared between accounts. Experiment flags from account A are visible to account B.

### 2. Database/Cache — PROPERLY ISOLATED ✓
- `xqc.java`: `lg9Var.m13329a("cache", "db")` → separate cache DBs per account
- `C0483j6.java`: `m13329a("chats_v2", null)` and `m13329a("folders_v1", null)` — separate chat storage

### 3. WebSocket Connections — PARTIALLY ISOLATED ⚠️
- `tch.java` (line 586-588): WebSocket URL includes `token` and `userId` parameters
- Connection restart (`m21750a`) appends token and userId to URL
- **ISSUE**: WebSocket is a singleton transport (`tch` is abstract, single instance pattern). During account switch, the old WebSocket may still be connected briefly. No evidence of dual-WebSocket support — only one connection active at a time. Messages arriving during switch window could be delivered to wrong account context.

### 4. Push Notifications — CRITICAL ISOLATION BUG 🔴
- `FcmMessagingService.java`: `m16972f()` hardcodes `lg9.f36929b` (account 0 only!)
- Push token (`onNewToken`) is registered only for the primary account
- `cei.java` (line ~92): There IS a userId check — if push contains a `compressed` userId field that doesn't match current account's userId, the push is dropped. But this check is gated behind a PmsKey flag (`qp6.f57357S2[160]`).
- **VULNERABILITY**: If the flag at index 160 is disabled, pushes for account B will be processed by account A's handler, potentially showing account B's messages in account A's notification context.
- Single FCM token shared between both accounts — server must route correctly.

### 5. File Storage — NOT ISOLATED 🔴
- `lv6.java`: All file paths (`audioCache`, `imageCache`, `upload`, downloads) use a SINGLE shared directory
- `m13702i()` downloads to `Downloads/MAX/` — no account subdirectory
- `m13704k()` image cache: shared `imageCache` folder
- `m13698d()` audio cache: shared `audioCache` folder
- **VULNERABILITY**: Account B can access cached media files (images, audio, video) downloaded by account A. No per-account file isolation exists.

### 6. Session Token Leakage — LOW RISK ⚠️
- Auth tokens stored in account-scoped `auth_prefs` / `auth1_prefs` (via `e3e.java`)
- `lg0.java`: session_data includes auth_token and device_id per request
- Token is passed to WebSocket URL during connection
- **ISSUE**: During account switch (observed in `hr9.java`), the swap is immediate with no explicit token invalidation/cleanup of in-memory references. Race condition window exists where network requests initiated before swap completes could carry wrong token.

### 7. Contacts Isolation — PROPERLY ISOLATED ✓
- `kl0.java`: `ContactsPickerScreen` receives `lg9` as constructor parameter
- Contacts database (`ru.ok.tamtam.contacts.ContactsDatabase`) is instantiated per-account scope via DI
- Each account scope gets its own contacts DAO

### 8. Notification System — NOT PROPERLY SEPARATED 🔴
- `m1c.java`: Notification channels use FIXED IDs without account suffix:
  - `"ru.oneme.app.new.activeCalls"`
  - `"ru.oneme.app.new.incomingCalls."`
  - `"ru.oneme.app.chats"`
  - `"ru.oneme.app.dialogs"`
- **VULNERABILITY**: Both accounts share the same notification channels. A user cannot configure different notification settings per account. Notifications from both accounts appear in the same channel, making it impossible to distinguish which account received a message from the notification shade alone.

## Summary of Vulnerabilities

| Area | Severity | Issue |
|------|----------|-------|
| File Storage | HIGH | Shared cache/download directories — media leaks between accounts |
| Push Notifications | HIGH | FCM handler hardcoded to account 0; userId check is flag-gated |
| Notification Channels | MEDIUM | Shared channels — no per-account notification separation |
| WebSocket | MEDIUM | Single connection, race during switch |
| Experiments Prefs | LOW | Shared experiment flags between accounts |
| Session Tokens | LOW | Brief race window during account swap |

## Recommendations

1. Prefix all file storage paths with account ID subdirectory
2. Create per-account notification channels (append account suffix)
3. Always enable the userId check in push handler (remove flag gate)
4. Register separate push tokens or implement server-side routing with mandatory userId field
5. Ensure WebSocket is fully disconnected before account swap completes
6. Scope `experiments_prefs` per account
