# VULN-52: Notification Direct Reply & Data Backup/Export Analysis

## Executive Summary

Max messenger (package: `ru.oneme.app`) implements notification direct reply via `NotificationTamService`. The service lacks authentication checks before sending replies. Backup is properly disabled (`allowBackup="false"`), and no user-facing chat export feature was found.

---

## 1. Notification Direct Reply

### 1.1 Implementation

- **Service**: `ru.p027ok.tamtam.android.services.NotificationTamService`
- **Action**: `ru.ok.tamtam.action.DIRECT_REPLY`
- **Intent builder**: `p000/soc.java` (line 432) creates the PendingIntent targeting the service
- **RemoteInput key**: `ru.ok.tamtam.extra.TEXT_REPLY`

Flow:
1. `soc.m21386g()` builds a `PendingIntent.getService()` with action `DIRECT_REPLY`
2. `NotificationTamService.onStartCommand()` receives the intent
3. Extracts `CHAT_SERVER_ID`, `PUSH_ID`, `EVENT_KEY`, `MESSAGE_SERVER_ID` from extras
4. Gets reply text via `RemoteInput.getResultsFromIntent(intent)`
5. Sends message directly via `crg` (message sending component) without any auth check

### 1.2 Authentication: NONE

**Finding: No authentication before sending reply**

- No biometric, PIN, or keyguard check in `NotificationTamService`
- No call to `KeyguardManager`, `BiometricPrompt`, or any passcode verification
- The app has a `pinLock` feature (`app.pinLock.screenshotEnabled` in settings) but it is NOT enforced for notification replies
- Reply is sent immediately upon receiving the intent, even from lock screen

**Risk**: Anyone with physical access to a locked device can reply to messages via the notification shade without unlocking the app.

### 1.3 External Triggering via NotificationListenerService

**Finding: Low risk but possible**

- `NotificationTamService` is declared **without** `android:exported` attribute in manifest
- On `targetSdkVersion="35"` (API 35), services without explicit `exported` default to `false`
- No intent-filter declared on the service
- The PendingIntent uses `FLAG_IMMUTABLE` (134217728) — confirmed in `ij9.m10405p()` which ORs `33554432` (FLAG_MUTABLE) only if SDK < 31 and FLAG_MUTABLE already set
- **However**: A `NotificationListenerService` on the same device CAN read notification content and programmatically trigger the reply RemoteInput action via `NotificationCompat.Action` — this is an Android platform behavior, not specific to Max

**Risk**: A malicious app with `BIND_NOTIFICATION_LISTENER_SERVICE` permission (user-granted) could intercept and trigger replies on behalf of the user.

### 1.4 Reply Content Logging/Tracking

**Finding: Analytics events logged for replies**

- Successful reply logs: `"onNotificationQuickReplied: chatServerId=<id>, lastMessage=<eventKey>"` (line 1104)
- Empty reply logs: `"onNotificationQuickRepliedWithEmptyText: pushId=<id>, eventKey=<key>"` (line 1088)
- Analytics events sent via `ok9.m16044h()` with parameters:
  - `"trid"` (tracking ID / push ID)
  - `"eKey"` (event key)
  - `"p_op"` = `"n_q_rep"` (successful reply) or `"n_q_rep_empty"` (empty reply)
- **Reply text content itself is NOT logged** in analytics — only metadata (chat ID, push ID, event key)
- Messages go through normal send path (`crg` / `arg` message object)

---

## 2. Data Backup & Export

### 2.1 Android Backup Configuration

**Finding: Backup properly disabled**

```xml
<!-- AndroidManifest.xml line 168 -->
android:allowBackup="false"
```

- `allowBackup="false"` is set on the `<application>` element
- No `fullBackupContent` or `dataExtractionRules` XML files found
- No custom `BackupAgent` implementation detected
- Some data explicitly stored in `getNoBackupFilesDir()` (GMS app ID, potentially crypto keys)

**Risk**: LOW — Android Auto Backup will not include app data.

### 2.2 Chat History Export

**Finding: No user-facing chat export feature found**

- No `chatExport`, `exportChat`, `export_chat`, `exportHistory` references in codebase
- No GDPR data export/download feature detected
- No `requestPersonalData` or `downloadData` API endpoints found
- The `forwardChat` functionality exists but is for message forwarding between chats, not export

### 2.3 Data Export (GDPR Compliance)

**Finding: No client-side GDPR data export implementation**

- No evidence of a "Download My Data" or "Request Data Export" feature in the app
- Privacy settings exist (`one.p011me.settings.privacy`) but focus on:
  - Safe Mode onboarding
  - App lock (PIN lock)
  - Screenshot control
- If GDPR export exists, it may be server-side only (web portal) — not implemented in the Android client

### 2.4 noBackupFilesDir Usage

Files stored in no-backup directory:
- `com.google.android.gms.appid-no-backup` (GMS instance ID)
- Potentially crypto/session keys (via `C0669o2.java` conditional path)

---

## 3. Risk Summary

| Issue | Severity | Description |
|-------|----------|-------------|
| No auth on notification reply | **Medium** | Physical access allows sending messages without app unlock |
| NotificationListener abuse | **Low** | Malicious listener app can trigger replies (requires user permission) |
| Reply metadata analytics | **Info** | Push ID and event key logged, but not message content |
| Backup disabled | **Good** | `allowBackup="false"` prevents data extraction via ADB backup |
| No chat export | **Info** | No client-side export reduces data exfiltration surface |
| No GDPR export in client | **Info** | May be a compliance gap if not available server-side |

## 4. Recommendations

1. **Add authentication gate for notification replies** — Check if app lock (PIN) is enabled and require device unlock before processing `DIRECT_REPLY` action
2. **Consider hiding notification content on lock screen** — Use `setVisibility(VISIBILITY_SECRET)` or `VISIBILITY_PRIVATE` when app lock is active
3. **Add FLAG_IMMUTABLE explicitly** — While the helper method handles it, explicit declaration improves clarity
4. **Implement client-side data export** — For GDPR/privacy compliance, consider adding a data portability feature

---

*Analysis date: 2026-05-15*
*Source: ~/max/decompiled/jadx/base/sources*
*Key files: NotificationTamService.java, soc.java, ij9.java, AndroidManifest.xml*
