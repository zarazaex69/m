# VULN-21: Clipboard Monitoring, Screenshot Detection & Screen Recording Capabilities

## Summary

Max messenger uses clipboard access for standard copy/paste operations and implements FLAG_SECURE-based screenshot prevention tied to a pin lock setting. Screen capture/sharing is used for video calls only. **No evidence of clipboard exfiltration, screenshot notification to other participants, or covert screen recording detection was found.**

## Findings

### 1. Clipboard Access

**Files involved:**
- `p000/ex3.java` — Core clipboard utility class
- `p000/hgl.java` — Paste handling for EditText widgets
- `p000/RunnableC0553l0.java` — Clipboard write (copy text/image)
- `p000/uc2.java` — Copy image URI to clipboard
- `p000/d9d.java` — ActionMode paste handler (extracts digits from clipboard for phone number fields)

**Behavior:**
- `ex3.m6812a()` — Copies text to clipboard (used when user copies messages, sticker links, profile info)
- `ex3.m6814c()` — Reads clipboard content (used for paste operations and pre-filling URL fields)
- `hgl.m9516b()` — Standard paste handling for EditText fields
- `uc2` case 10 — Copies image URI to clipboard via `setPrimaryClip`

**Usage contexts (all user-initiated):**
- `MessagesListWidget` — Copy message text
- `ProfileScreen` — Copy profile info
- `StickersScreen` / `StickerPreviewScreen` — Copy sticker links
- `AddLinkBottomSheet` — Pre-fills URL field from clipboard if it matches a URL pattern
- `d9d` — Extracts digits from clipboard for phone number input

**Risk Assessment: LOW**
- All clipboard access is for standard copy/paste UX
- No `OnPrimaryClipChangedListener` registered (no background clipboard monitoring)
- No evidence of clipboard content being sent to any server/API
- Clipboard reading in `AddLinkBottomSheet` is a common UX pattern (auto-detect URLs)

### 2. Screenshot Prevention (FLAG_SECURE)

**Files involved:**
- `p000/AbstractActivityC1180u7.java` (lines 153-155)
- `p000/ugj.java` (line 97-98)
- `one/p011me/webapp/rootscreen/WebAppRootScreen.java` (lines 661, 792)

**Mechanism:**
```java
// AbstractActivityC1180u7.java - Main activity base class
if (screenshotEnabled) {
    getWindow().clearFlags(8192);  // 8192 = FLAG_SECURE, allow screenshots
} else {
    getWindow().addFlags(8192);    // FLAG_SECURE, prevent screenshots
}
```

**Behavior:**
- Controlled by user preference: `app.pinLock.screenshotEnabled` (default: `true` = screenshots allowed)
- When pin lock is enabled and screenshot setting is disabled, FLAG_SECURE is set
- This is a **privacy protection for the user** — prevents other apps from capturing the screen
- WebApp screens also respect this setting
- String resource: `privacy_setting_pin_lock_screenshot_enabled` with description

**Risk Assessment: LOW (beneficial privacy feature)**
- Standard Android FLAG_SECURE usage
- User-controlled opt-in setting
- No screenshot detection or notification to other participants
- No evidence of screenshot events being reported to the server

### 3. Screen Capture / Screen Sharing (Video Calls Only)

**Files involved:**
- `ru/p027ok/android/externcalls/sdk/video/ScreenCaptureManager.java` — Interface
- `ru/p027ok/android/externcalls/sdk/video/internal/ScreenCaptureManagerImpl.java` — Implementation
- `ru/p027ok/android/externcalls/sdk/ConversationImpl.java` — Uses ScreenCaptureManager
- `ru/p027ok/tamtam/android/calls/MediaProjectionService.java` — Foreground service for screen sharing
- `one/p011me/calls/p014ui/p015ui/call/CallScreen.java` — Initiates screen capture intent
- `p000/gh7.java`, `p000/b7g.java` — MediaProjection.Callback implementations
- `p000/ra1.java` — Gets MediaProjection for screen sharing

**Behavior:**
- `ScreenCaptureManager` is part of the calls SDK — used exclusively for screen sharing during video calls
- `MediaProjectionService` is a foreground service (required by Android for screen capture)
- `CallScreen` launches `createScreenCaptureIntent()` — standard Android permission flow
- `screenCaptureState` is tracked per call participant (visible to other call participants as expected)
- `dataChannelScreenshareRecvEnabled` / `dataChannelScreenshareSendEnabled` — WebRTC data channel for screen share

**Risk Assessment: LOW**
- Standard screen sharing feature for video calls
- Requires explicit user permission via Android's MediaProjection consent dialog
- No covert screen recording capability
- No evidence of recording user's screen without their knowledge

### 4. Screen Recording Detection

**No evidence found.** There is no:
- FileObserver monitoring screenshot directories
- ContentObserver watching for screenshot media
- Screenshot event reporting to server or other chat participants
- Screen recording detection mechanism

### 5. Accessibility Service

**No accessibility service implementation found.**
- Only `AccessibilityManagerTouchExplorationStateChangeListenerC0299e5` exists — a listener for touch exploration state changes (standard accessibility support)
- No `extends AccessibilityService` class
- No `BIND_ACCESSIBILITY_SERVICE` permission usage
- The `accessibility_custom_action_*` entries are standard AndroidX resource IDs for accessibility actions

**Risk Assessment: NONE**
- No accessibility service that could monitor other apps

### 6. WebApp Screen Capture Behavior

**File:** `p000/enk.java`
- `SETUP_SCREEN_CAPTURE_BEHAVIOR` — A WebApp event type for configuring screen capture behavior in web apps
- This appears to be a configuration message for web mini-apps, not a surveillance mechanism

## Conclusion

| Capability | Present | Malicious Use | Risk |
|---|---|---|---|
| Clipboard read/write | Yes | No — standard copy/paste | Low |
| Clipboard monitoring (background) | No | N/A | None |
| Clipboard exfiltration to server | No | N/A | None |
| FLAG_SECURE (screenshot prevention) | Yes | No — user privacy feature | Low (beneficial) |
| Screenshot detection/notification | No | N/A | None |
| Screen sharing (calls) | Yes | No — requires user consent | Low |
| Screen recording detection | No | N/A | None |
| Accessibility service abuse | No | N/A | None |

**Overall Risk: LOW** — All clipboard and screen-related functionality is standard Android behavior for a messaging app. No covert monitoring, exfiltration, or notification mechanisms were identified.
