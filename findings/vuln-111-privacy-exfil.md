# VULN-111: Privacy Violations & Data Exfiltration Analysis

**App:** Max Messenger (one.me / ru.ok.tamtam)  
**Source:** ~/max/decompiled/jadx/base/sources  
**Date:** 2026-05-15

---

## 1. Analytics / Telemetry — MyTracker

**SDK:** MyTracker (VK/Mail.ru analytics platform)  
**Remote toggle:** `mytracker-enabled` (PmsKey), `mytracker-log-level`  
**General analytics toggle:** `analytics-enabled` (PmsKey)

**Evidence:**
- `LinkInterceptorActivity.java:107` — `MyTracker.handleDeeplink(getIntent())` — deep link attribution tracking
- `lqb.java:19` — `MyTrackerAppLifecycle.m4195a(engineMiniCore)` — app lifecycle events sent
- `lqb.java:22` — `MyTrackerUserLifecycle.m4528a(engineMiniCore)` — user lifecycle events sent
- `mqb.java:12` — `MyTrackerAppLifecycle.m4194a(...)` with activity handler
- `h50.java:13` — implements `MyTrackerConfig.OkHttpClientProvider, MyTracker.AttributionListener`
- `h50.java:151` — `onReceiveAttribution(MyTrackerAttribution)` — attribution data received

**Database tables for analytics:**
- `stat_events` — local stat event queue
- `fcm_notifications_analytics` — push notification analytics
- `notifications_tracker_messages` — notification tracking

**Risk:** HIGH — MyTracker sends app usage, session data, deep link attribution, and user lifecycle events to VK/Mail.ru servers. Controlled by remote server toggle.

---

## 2. Device Identification

**Evidence:**
- `InternalParamsDto.java:91,139` — `getDeviceId()` — device ID sent as API parameter (`PARAM_DEVICE_ID`)
- `ApiModule.java:15` — `getDeviceIdProvider()` interface for calls SDK
- `ApiModuleImpl.java:84` — `getDeviceIdProvider()` implementation
- `AbstractC0672o5.java:41,46` — `context.getDeviceId()` — direct device ID access
- `yag.java` — stores `deviceId` in preferences alongside `pushToken`, `pushDeviceType`, `installationMarket`
- `zu5.java:275,938` — `getDeviceIdSupplier()` — device ID supplier pattern

**Data collected:**
- Device ID (persistent identifier)
- Push token
- Push device type
- Installation market source
- System language
- Protocol version, SDK version, platform

**Risk:** HIGH — Persistent device fingerprinting via device ID sent with API calls.

---

## 3. Contact Sync & Phone Hash Upload

**Evidence:**
- `a40.java:369` — `registerContentObserver(ContactsContract.Contacts.CONTENT_URI, true, ...)` — monitors ALL contact changes in real-time
- `op4.java:80` — `"Early return in invoke cuz of contactSync is null"` — contact sync mechanism exists
- `g3k.java:103` — `"onAppGoesForeground forceContactSync = %b"` — contacts synced on app foreground
- `yag.java` — stores `contactsLastSync`, `contactSortLastSync`, `phonesSortLastSync`

**Server-controlled parameters (PmsKey / rtd.java):**
- `non-contact-sync-time` — sync interval for non-contacts
- `non-contact-max-chunk-size` — batch upload size
- `non-contact-collection-interval` — how often non-contacts are collected

**Phone hash for WebApps:**
- `PmsKey:webapp-phone-hash` — phone number hashing for web apps
- `zgk.java:41` — `WebAppRequestPhoneSuccess(phone=..., hash=...)`
- `ygk.java:51` — `WebAppRequestPhoneResponse(requestId=..., phone=..., hash=...)`
- `klk.java:41` — `Response(phone=..., hash=..., authDate=...)`

**Database tables:** `contacts`, `phones`

**Risk:** CRITICAL — Contacts are monitored in real-time and synced to server. Non-contacts are also collected and uploaded in chunks. Phone numbers are hashed and shared with web apps.

---

## 4. Location Tracking

**Evidence:**
- `ppk.java:80` — `LocationManager locationManager = (LocationManager) applicationContext.getSystemService("location")`
- `C0358fr.java:64` — `locationManager.getLastKnownLocation("network")` — network-based location
- `C0358fr.java:72` — `locationManager.getLastKnownLocation("gps")` — GPS location
- `f6g.java:103,136,147` — Google Fused Location Provider (`IGoogleLocationManagerService`)
- `zal.java:40,53,59` — Google Location Manager Service binding

**Server toggle:** `send-location-enabled` (PmsKey)  
**Map integration:** `y-map` (PmsKey)

**Risk:** HIGH — Both GPS and network location accessed. Location sharing feature controlled by server-side toggle.

---

## 5. Installed Apps Enumeration

**Evidence:**
- `a3c.java:160` — `getPackageManager().queryIntentServices(...)` — queries notification side channel services
- `rp8.java:30` — `getPackageManager().queryIntentActivities(intent, 0)` — queries activities
- `hu7.java:125` — `getPackageManager().queryIntentServices(intent, 128)` — queries services with metadata
- `j8h.java:1405` — `packageManager.queryIntentActivities(intent, 128)` — queries with metadata flag
- `qcc.java:1425` — `getPackageManager().getPackagesForUid(iMyUid)` — gets packages by UID
- `p5h.java:67`, `az7.java:89` — `queryIntentActivities(intentCreateChooser, 65536)` — share target enumeration

**Risk:** MEDIUM — App queries installed packages/services. Some uses are legitimate (share targets, notification channels), but `getPackagesForUid` and broad intent queries can fingerprint installed apps.

---

## 6. Clipboard Monitoring

**Evidence:** No clipboard monitoring listeners found (`addPrimaryClipChangedListener`, `OnPrimaryClipChangedListener`).

**Risk:** NONE detected.

---

## 7. Media Content Observation (Screenshot/Gallery Detection)

**Evidence:**
- `wh8.java:108` — Registers ContentObserver on ALL media URIs:
  - `MediaStore.Images.Media.EXTERNAL_CONTENT_URI`
  - `MediaStore.Video.Media.EXTERNAL_CONTENT_URI`
  - `MediaStore.Images.Media.INTERNAL_CONTENT_URI`
  - `MediaStore.Video.Media.INTERNAL_CONTENT_URI`
- `wh8.java:111` — `registerContentObserver((Uri) it.next(), true, eh8Var)` — observes all media changes
- `C0669o2.java:176` — `"ManualGalleryContentObserver: on content changed"` — gallery change detection

**Screenshot control in WebApps:**
- `WebAppRootScreen.java:661,792` — `app.pinLock.screenshotEnabled` — screenshot toggle for web apps
- `l5f.java:1134` — `privacy_setting_pin_lock_screenshot_enabled` — privacy setting for screenshots

**Risk:** HIGH — App monitors ALL image/video creation on device (internal + external storage). This enables screenshot detection and potentially tracking what media the user creates/receives outside the app.

---

## Summary

| Category | Risk | Details |
|----------|------|---------|
| Analytics (MyTracker) | HIGH | App/user lifecycle, attribution, deep links sent to VK servers |
| Device ID | HIGH | Persistent device identifier sent with API calls |
| Contact Sync | CRITICAL | Real-time contact monitoring, periodic sync, non-contact collection |
| Phone Hash | HIGH | Phone numbers hashed and shared with web apps |
| Location | HIGH | GPS + network location, server-controlled toggle |
| Installed Apps | MEDIUM | Package queries for fingerprinting potential |
| Clipboard | NONE | Not detected |
| Media Observer | HIGH | Monitors all image/video creation on device |

**Overall Assessment:** The app implements extensive data collection including persistent device identification, real-time contact monitoring with server-side upload, location tracking, and full media filesystem observation. The MyTracker analytics SDK sends behavioral data to VK/Mail.ru infrastructure. Most collection features are controlled by remote server toggles, meaning the scope of data collection can be changed without app updates.
