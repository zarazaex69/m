# VULN-73: Hidden/Undocumented Features in Max Messenger

**Severity:** Medium  
**Category:** Hidden Developer Functionality / Abuse Potential  
**Date:** 2026-05-15  

## Summary

Max messenger (built on TamTam/OneMe codebase) ships production APKs with a full developer menu module (`dev-menu_release`), multiple "fake" features controllable via server-side PMS (Parameter Management System) keys, and debug/test functionality that remains accessible in release builds.

---

## 1. Developer Menu

**Location:** `one/p011me/devmenu/`

The dev menu is a full module shipped in the release APK with the following screens:

| Class | Purpose |
|-------|---------|
| `DevMenuScreen` | Main tabbed dev menu with ViewPager2 |
| `DevMenuGeneralPageScreen` | General settings/toggles page |
| `DevMenuFeatureTogglesPageScreen` | Feature flag toggles with search |
| `DevMenuInfoScreen` | App/device info display |
| `CallDebugMenuScreen` | Call-specific debug options |
| `LogsViewerScreen` | View application logs |
| `IntegrityLogsViewerScreen` | View integrity/security logs |
| `MemoryDebuggerScreen` | Memory debugging tools |
| `ThreadsStateViewerScreen` | Thread state viewer |
| `ServerHostBottomSheet` | **Change server host** (custom input + button) |
| `ServerPortBottomSheet` | Change server port |
| `TestCrash` | Intentional crash trigger (`@Keep` annotated) |

### Dev Menu Access Control

The dev menu is gated by a feature flag `"dev_menu"` checked via `o0i.m15496y0("dev_menu")`. Access is controlled by:
- `isDevOptionsRoaming` preference (stored in SharedPreferences class `ri9`)
- The string resource `oneme_settings_old_dev_menu` suggests a settings entry point

**Risk:** The `ServerHostBottomSheet` allows redirecting all app traffic to an arbitrary server — a critical MITM vector if dev mode can be enabled.

---

## 2. Hidden Debug Settings (ri9 preferences)

The local preferences class `ri9` exposes these debug-relevant flags:

| Preference | Purpose |
|-----------|---------|
| `isDevOptionsRoaming` | Enables dev options without being on dev build |
| `isCallsDebugMenuEnabled` | Enables call debug menu |
| `isVideoDebugViewAvailable` | Shows video debug overlay |
| `isDebugProfileInfoEnabled` | Shows debug profile info |
| `isDebugFresco` | Enables Fresco image library debug |
| `isDisableWebAppSsl` | **Disables SSL for WebApps** |
| `isDisableInAppReviewTimeCondition` | Bypasses review timing |
| `isEnableInAppReviewNotFromMarketBuild` | Enables review on non-market builds |
| `allowLogSensitiveData` | **Logs sensitive user data** (MutableStateFlow) |
| `leakCanaryEnabledStateFlow` | LeakCanary in production |
| `areMockCommentsEnabled` | Enables mock/fake comments |
| `isIgnoringTranscodeCaching` | Bypasses transcode cache |

**Risk:** `isDisableWebAppSsl` and `allowLogSensitiveData` are particularly dangerous — they can expose user data in transit and in logs respectively.

---

## 3. Enabling Developer Mode Without Root

Developer mode can potentially be enabled without root through:

1. **SharedPreferences manipulation** — `isDevOptionsRoaming` is stored in standard SharedPreferences (`ri9` class). On a non-encrypted device or via ADB backup, this can be toggled.
2. **Feature flag `"dev_menu"`** — checked via `o0i.m15496y0()` which appears to be a simple string-blank check utility, suggesting the flag may be server-controlled.
3. **Settings entry point** — resource `oneme_settings_old_dev_menu` (ID 2131954286) suggests there may be a hidden settings entry that navigates to the dev menu.

No secret gesture was found in the decompiled code — access appears to be preference/flag-based rather than gesture-based.

---

## 4. `fake-chats` PmsKey (Index 75)

**Definition:** `PmsKey.java:251` — `new PmsKey("fake-chats", 75)`  
**Server config:** `rtd.isFakeChatsEnabled` (boolean)  
**UI integration:** `PickerChatsListWidget`, `ChatsListWidget`

### What Are Fake Chats?

`FakeChatModel` (`tj6.java`) represents synthetic chat entries injected into the chat list with:
- `contactId` (long)
- `avatar` (Uri)
- `isOnline` (boolean)
- `isVerified` (boolean)
- `title` (CharSequence)
- `subtitle` (cki object)
- `isRegistered` (boolean)
- `abbreviation` (CharSequence)

The chat list has handlers for: `onFakeChatItemClick`, `onFakeChatItemLongTap`, `onFakeChatItemButtonClick`

**Risk:** Server-controlled fake chats can be injected into a user's chat list to impersonate contacts, display phishing content, or social-engineer users into interacting with malicious entities. The `isVerified` flag means fake chats can appear as verified/official.

---

## 5. `fake-in-app-review` PmsKey (Index 79)

**Definition:** `PmsKey.java:263` — `new PmsKey("fake-in-app-review", 79)`  
**Server config:** `rtd.isFakeInAppReviewEnabled` (boolean)  
**UI resources:** Full bottom sheet with rating bar, send button, title, subtitle, thank-you view

### What Does It Fake?

Instead of using Google Play's real In-App Review API, this feature displays a **custom fake review dialog** that:
- Shows a rating bar (`fake_in_app_review_bottom_sheet_rate_view_rating_bar`)
- Has a "Send" button (`_send_btn`)
- Has a "Not Now" button (`_not_now_btn`)
- Shows a "Thank You" view after submission (`_thank_view`)

The rating data is sent to Max's own servers, not to Google Play. Combined with `isDisableInAppReviewTimeCondition` and `isEnableInAppReviewNotFromMarketBuild`, this allows:
- Harvesting user sentiment data without Google Play involvement
- Displaying the fake review at arbitrary times (bypassing Google's rate limits)
- Potentially misleading users about where their feedback goes

---

## 6. Test/Mock Functionality in Production

| Feature | Location | Risk |
|---------|----------|------|
| `TestCrash` | `devmenu/tools/TestCrash.java` | `@Keep` annotated RuntimeException — survives ProGuard |
| `areMockCommentsEnabled` | `ri9` preferences | Enables fake/mock comments in production |
| `leakCanaryEnabledStateFlow` | `ri9` preferences | Memory leak detector in production |
| `isIgnoringTranscodeCaching` | `ri9` preferences | Can bypass media caching |
| `log-sensitive` PmsKey | Server-controlled | Enables logging of sensitive data |
| `log-full` PmsKey | Server-controlled | Enables full logging |
| `debug-profile-info` PmsKey | Server-controlled | Shows debug profile information |

---

## 7. `debug-broken-contact` PmsKey (Index 251)

**Definition:** `PmsKey.java:769` — `new PmsKey("debug-broken-contact", 251)`  
**Server config:** `qp6.isDebugBrokenContactEnabled` (boolean)

This is a server-controlled feature flag that enables "broken contact" debugging. When enabled, it likely displays contacts in a broken/error state for testing purposes. The concern is that this is a **server-side toggle** — meaning the server operator can enable debug contact display for any user without their knowledge, potentially:
- Disrupting the user's contact list display
- Injecting test/debug UI elements visible to the user
- Being used as a fingerprinting mechanism to identify specific users

---

## 8. `calls-fakeboss-incoming-call-enabled` PmsKey (Index 320)

**Bonus finding:** `PmsKey.java:974` — `new PmsKey("calls-fakeboss-incoming-call-enabled", 320)`  
**Server config:** `qp6.callFakeBossesEnabled` (boolean)  
**UI:** `FakeBossListItem` with fields: `contactServerId`, `phoneNumber`, `country`, `registrationDate`, `mutualChatsState`, `organizationInfoTextRes`

This feature creates **fake incoming call notifications** that appear to come from a "boss" contact. The `FakeBossListItem` model shows it displays:
- Organization info
- Registration date
- Mutual chats
- Phone number

**Risk:** This is a social engineering feature that can simulate incoming calls from authority figures. While possibly intended as a "fake call to escape meetings" feature, it's server-controlled and could be weaponized.

---

## 9. Server-Controlled Sensitive Logging

The `log-sensitive` PmsKey enables sensitive data logging remotely:
- `needToLogSensitive` in server config (`rtd`)
- `allowLogSensitiveData` as a local MutableStateFlow
- Parsed from server response in `ul9.java:107-108`

**Risk:** The server can silently enable sensitive data logging on any client, capturing user data that would normally not be logged.

---

## Abuse Scenarios

1. **Targeted surveillance:** Enable `log-sensitive` + `allowLogSensitiveData` for a specific user to capture their sensitive data in logs, then retrieve via `user-debug-report`.
2. **Phishing via fake chats:** Inject verified-looking fake chats into a target's chat list to impersonate trusted contacts.
3. **MITM via dev menu:** If dev mode is enabled (via preference manipulation), redirect traffic through `ServerHostBottomSheet` to an attacker-controlled server.
4. **Social engineering via fake boss calls:** Trigger fake incoming calls appearing to come from authority figures.
5. **SSL bypass:** Enable `isDisableWebAppSsl` to intercept WebApp traffic.

---

## Recommendations

1. Strip the entire `dev-menu` module from production builds
2. Remove `fake-chats` capability or add clear visual indicators that cannot be spoofed
3. Remove `fake-in-app-review` — use Google Play's real API or nothing
4. Remove `log-sensitive` server-side toggle capability from production
5. Remove `isDisableWebAppSsl` from production builds entirely
6. Audit all server-controlled PmsKeys for abuse potential
7. Add client-side consent/notification when debug features are enabled remotely
