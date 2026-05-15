# Vulnerability 10: Push Notification Handling

## Overview

Analysis of FCM push notification handling in Max messenger for code execution and injection vulnerabilities.

## FirebaseMessagingService Implementation

**File:** `one/p011me/sdk/vendor/push/FcmMessagingService.java`

The app extends `FirebaseMessagingService` and processes push data in `mo4173d()` (onMessageReceived). Data is extracted from the Bundle into a `Map<String, String>` (`dmf` object) and dispatched to `cei` coroutine via `eei` class.

## Push Message Types & Data Flow

The `cei` class (push handler coroutine) routes messages by `type` field:

| Type | Handler | Risk |
|------|---------|------|
| `ConversationReadOnOtherDevice` | Marks messages read | Low |
| Messages (has `mc`/`suid` + `msgid`) | `tn6.m22053c()` → `eqe.m6662d()` | Medium |
| `MessageRemoved` / `ChatMessageRemoved` | `eei.m6390a()` | Low |
| `InboundCall` | `fei.m7420c()` → call service | **HIGH** |
| `TamtamSpam` (deeplink) | `fei.m7419b(uri, msg, title, imageUrl)` | **CRITICAL** |
| `LocationRequest` | Triggers location sync | Medium |

## Critical Findings

### 1. Push-Triggered Deeplink Injection (CRITICAL)

**Location:** `cei.java:245`, `fei.java:m7419b()`

When push type is `"TamtamSpam"`, the handler directly extracts:
```java
eeiVar4.m6393d().m7419b(
    (String) map.get("uri"),      // attacker-controlled deeplink
    (String) map.get("msg"),
    (String) map.get("title"),
    (String) map.get("imageUrl")  // attacker-controlled image URL
);
```

The `uri` field is passed to `ucc.m22811u(str)` which creates a navigation task with the raw URI string — **no validation or allowlist check**. This chains directly into the deeplink injection vulnerabilities (see vuln-03).

**Impact:** Server-side attacker (or MITM on FCM) can trigger arbitrary deeplink navigation without user interaction.

### 2. VoIP/Call Push Bypasses User Interaction (HIGH)

**Location:** `cei.java:169-230`

`InboundCall` push type triggers:
- Foreground service start (`CallServiceImpl`)
- Full-screen intent display
- WakeLock acquisition
- Direct call connection setup

Push data fields used without validation:
- `trid` - transaction ID
- `eKey` - encryption key
- `suid` - sender user ID  
- `vcId` - video conference ID
- `vcp` - video call parameters
- `userName` - displayed caller name

The call is initiated **without user interaction** — the push alone triggers `startForeground()` and displays the call UI. A malicious push could spoof caller identity.

### 3. Notification Image Loading - SSRF Mitigated

**Location:** `NotificationsImagesProvider.java`, `goc.java:m8726f()`

Image URLs from push (`largeImageUrl`, `imageUrl`) are loaded via a ContentProvider:
```java
new Uri.Builder().scheme("content").authority("ru.oneme.app.notifications")
    .appendPath("message_image").appendPath(str).appendPath(String.valueOf(z)).build()
```

**Mitigation found:** `NotificationsImagesProvider.openFile()` checks:
- `jel.m11375k()` - rejects internal URIs (checks if path resolves to app's internal directories)
- Read-only mode enforced: `if (!"r".equals(str)) throw SecurityException`

However, the URL is still fetched from network (`loadFromNetwork` flag), meaning **server-side SSRF is possible** if the server fetches the image on behalf of the client.

### 4. PendingIntent Security

**Location:** `soc.java:437-560`, `CallServiceImpl.java:949-953`

PendingIntent flags used:
- `201326592` = `FLAG_IMMUTABLE | FLAG_UPDATE_CURRENT` ✓
- `67108864` = `FLAG_IMMUTABLE` ✓  
- `134217728` = `FLAG_UPDATE_CURRENT` (without IMMUTABLE — **potentially mutable on API < 31**)

The direct reply PendingIntent at `soc.java:437`:
```java
PendingIntent.getService(context, i, intent, ij9.m10394U(intent, ij9.m10405p(134217728)));
```
Uses `FLAG_UPDATE_CURRENT` without `FLAG_IMMUTABLE`. On Android < 12, this PendingIntent is mutable and could be hijacked by a malicious app to intercept reply text.

### 5. Notification Tap Navigates via Deeplink

**Location:** `vq9.java:62-66`, `kab.java:1732`

Notification tap intent is constructed as:
```java
intent.setAction("CUSTOM_DEEP_LINK");
intent.setData(Uri.parse(str + "://" + str2 + "/" + k95Var.f33220b));
```

The path component (`k95Var.f33220b`) is built from push data including `push_link` field:
```java
sb.append("&push_link=".concat(str11));  // kab.java
```

This means push payload data flows into the deeplink URI used for navigation.

### 6. Background Execution from Push

Push notifications trigger:
- **WakeLock acquisition** (`fqe` class) - keeps device awake
- **Foreground service start** for calls
- **Database writes** (fcm_notifications table stores push data)
- **Network requests** for image loading

No evidence of arbitrary code download/execution, but the deeplink trigger effectively allows arbitrary in-app navigation and WebView loading.

### 7. Direct Reply Data Flow

**Location:** `NotificationTamService.java:1078-1100`

Reply text goes to `m20533a(longExtra, notificationTamService, charSequence2, ...)` which sends to the app's own message sending infrastructure. The `CHAT_SERVER_ID` comes from the original push data, meaning a spoofed push could redirect replies to a different chat. However, the chat ID is a long integer validated against local cache.

## Attack Scenarios

1. **Silent deeplink exploitation:** Attacker sends push with `type=TamtamSpam` and malicious `uri` field → triggers WebView navigation to attacker-controlled page without user tap
2. **Call spoofing:** Attacker sends `InboundCall` push with spoofed `userName` → victim sees fake caller, answers, connects to attacker's call infrastructure
3. **PendingIntent hijack (Android < 12):** Malicious app intercepts mutable reply PendingIntent → reads user's reply text

## Severity

| Finding | Severity | CVSS Est. |
|---------|----------|-----------|
| Push deeplink injection | Critical | 8.1 |
| VoIP push bypass | High | 7.5 |
| Mutable PendingIntent | Medium | 5.5 |
| Image SSRF (server-side) | Low | 3.5 |

## Recommendations

1. Validate push `uri` field against allowlist before navigation
2. Require cryptographic verification of call push payloads
3. Add `FLAG_IMMUTABLE` to all PendingIntents
4. Rate-limit deeplink-type push processing
