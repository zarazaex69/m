# VULN-74: Data Exfiltration Channels in Max Messenger

## Summary

Max messenger (ru.oneme.app, v26.15.3) sends data to multiple external endpoints across 4 distinct categories. All traffic is HTTPS.

---

## 1. Complete Endpoint Map

### A. Primary API Endpoints (VK/OK ecosystem)
| Endpoint | Purpose | Data Sent |
|----------|---------|-----------|
| `https://api.ok.ru` | Main OK API | Messages, user data, auth |
| `https://api.odnoklassniki.ru` | Main OK API (alias) | Same as above |
| `https://max.ru` | App deep links, auth | User navigation, auth tokens |
| `https://download.max.ru` | App updates | Version checks |
| `https://vkvideo.ru/live` | Live streaming | Video stream data |

### B. Analytics/Tracking Endpoints
| Endpoint | Purpose | Data Sent |
|----------|---------|-----------|
| `https://sdk-api.apptracer.ru` | Crash/perf reporting (OK Tracer) | Crashes, ANRs, perf samples, session data |
| `https://tracker-api.vk-analytics.ru` | MyTracker (VK analytics) | User IDs, device info, events, attribution |
| `https://pagead2.googlesyndication.com` | Google Ads ID reporting | Advertising ID, device params |
| `https://firebaseinstallations.googleapis.com` | Firebase Installations | App instance ID, FCM token |

### C. Map/CDN Endpoints
| Endpoint | Purpose | Data Sent |
|----------|---------|-----------|
| `https://static-maps.yandex.ru` | Static map images | GPS coordinates, zoom level |
| `https://geocode-maps.yandex.ru` | Reverse geocoding | GPS coordinates |
| `https://tiles.api-maps.yandex.ru` | Map tiles | Viewport coordinates |

### D. External Navigation (user-triggered)
| Endpoint | Purpose |
|----------|---------|
| `https://play.google.com/store/apps/details?id=ru.oneme.app` | App store |
| `https://yandex.ru/maps` | Map fallback |
| `https://www.google.com/maps/dir/` | Google Maps navigation |

---

## 2. AppTracer.ru (OK Tracer) — Detailed

**Base URL:** `https://sdk-api.apptracer.ru`

**API Endpoints:**
- `/api/crash/upload` — Full crash reports (stack traces, device state)
- `/api/crash/trackSession` — Session tracking with crash token
- `/api/sample/initUpload` — Performance sample init
- `/api/sample/upload` — Performance sample data upload
- `/api/perf/upload` — Performance metrics

**Data collected:**
- Package name, version name, version code
- Build UUID (tracer_mapping_uuid)
- Environment identifier
- Full stack traces on crash
- Performance sampling data
- Session duration/lifecycle

**Source files:** `ru/p027ok/tracer/upload/SampleUploadWorker.java`, `p000/yxb.java`, `p000/nei.java`, `p000/ktg.java`

---

## 3. MyTracker (VK Analytics) — Detailed

**Base URL:** `https://tracker-api.vk-analytics.ru`

**User PII collected (MyTrackerParams.java):**
- Age, Gender
- Email addresses
- Phone numbers
- OK IDs, VK IDs, VK Connect IDs, ICQ IDs
- Custom user IDs (set to OK user ID on login)
- Custom parameters (key-value)
- Language/locale

**Device data collected (C0216m.java):**
- `Build.DEVICE`, `Build.MANUFACTURER`, `Build.MODEL`
- Android version (`Build.VERSION.RELEASE`)
- Screen resolution
- Timezone
- Locale/language
- Disk space (total/free)
- Touchscreen capability
- UI mode
- Root detection (checks su binary paths)
- Google Advertising ID (via `AdvertisingIdClient.getAdvertisingIdInfo()`)

**Lifecycle events tracked:**
- `trackLoginEvent(userId)` — on user login
- `trackRegistrationEvent(userId)` — on registration
- `trackInviteEvent()` — on invite sent
- `trackEvent(name, params)` — custom events
- Attribution/deeplink handling

**Feature flags:** Controlled by server via `mytracker-enabled` and `mytracker-log-level` PmsKey settings.

**Source files:** `com/p006my/tracker/`, `p000/rqb.java`, `p000/qqb.java`, `p000/glf.java`, `p000/C0263d6.java`

---

## 4. Google Services

**Firebase Cloud Messaging (FCM):**
- `https://firebaseinstallations.googleapis.com/v1/` — Installation registration
- Push token registration and refresh
- Source: `one/p011me/sdk/vendor/push/FcmMessagingService.java`

**Google Ads:**
- `https://pagead2.googlesyndication.com/pagead/gen_204?id=gmob-apps` — Advertising ID ping
- Sends map of device/app parameters
- Source: `com/google/android/gms/ads/identifier/zza.java`

**No Crashlytics detected** — app uses OK's own AppTracer instead.

---

## 5. OK.ru/Odnoklassniki Beyond Main API

- `https://api.ok.ru` — Primary API (defined in `AbstractC1161tp.java`)
- `https://api.odnoklassniki.ru` — Same API, alternate domain (in `aag.java`)
- DNS metric reporting: `api._endpoint.ok.ru.` (in `xl2.java`) — network stat events

Both domains appear to serve the same backend. No additional covert OK endpoints found.

---

## 6. Chinese/Non-Russian Endpoints

**None found.** All endpoints are either:
- Russian (.ru domains): OK, VK, Yandex, Max
- Google (.com/.googleapis.com): Firebase, Ads, Maps
- No Chinese, Asian, or other non-Russian/non-Google endpoints detected

Huawei references are limited to device-specific battery optimization UI and Petal Maps navigation (user-triggered intent only).

---

## 7. Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     MAX MESSENGER APP                         │
└──────┬──────────┬──────────┬──────────┬──────────┬──────────┘
       │          │          │          │          │
       ▼          ▼          ▼          ▼          ▼
┌──────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────────┐
│ api.ok.ru│ │AppTracer│ │MyTracker│ │Firebase│ │ Yandex Maps│
│ (OK API) │ │  (.ru)  │ │(VK/.ru)│ │(Google)│ │   (.ru)    │
└──────────┘ └────────┘ └────────┘ └────────┘ └────────────┘
     │            │          │          │          │
     │            │          │          │          │
  Messages    Crashes     User IDs   Push token  GPS coords
  Auth        ANRs        Device ID  Install ID  Location
  Media       Perf data   Events     Ad ID ping  queries
  Contacts    Sessions    PII(email,
  Settings               phone,age)
```

---

## 8. Risk Assessment

| Channel | Sensitivity | Controllable by User? |
|---------|------------|----------------------|
| api.ok.ru | HIGH (all user data) | No (required for app) |
| AppTracer | MEDIUM (crash+device) | No (always active) |
| MyTracker | HIGH (PII + device fingerprint) | Partially (server flag) |
| Firebase | LOW (push tokens) | No |
| Google Ads | LOW (ad ID) | System-level opt-out |
| Yandex Maps | MEDIUM (GPS) | Only when using location |

**Key concern:** MyTracker sends hashed emails, phone numbers, and cross-platform IDs (OK, VK, ICQ) to VK's analytics infrastructure, enabling cross-service user tracking across the entire VK ecosystem.
