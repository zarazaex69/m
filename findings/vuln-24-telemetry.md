# VULN-24: Hidden Telemetry & Behavioral Data Collection in Max Messenger

## Summary

Max messenger contains extensive telemetry infrastructure collecting behavioral data through multiple systems: MyTracker (VK's analytics SDK), AppTracer (ru.ok crash/performance profiler), OneLog event numbering, a local `stat_events` SQLite database, and performance registrars. Data is sent to VK/Mail.ru servers.

## 1. Analytics Infrastructure Found

### Key files and packages:
- `com.my.tracker.*` — MyTracker SDK (VK's proprietary tracker)
- `ru.ok.tracer.*` — AppTracer (uploads to `https://sdk-api.apptracer.ru`)
- `ru.ok.android.onelog.*` — OneLog session event numbering
- `ru.ok.messages.analytics.DailyAnalyticsWorker` — Daily permission/behavior reporting
- `ru.ok.android.externcalls.analytics.*` — Call analytics pipeline
- `ru.ok.tamtam.stats.LogController` — Central stat event dispatcher

### Database tables for telemetry:
- `stat_events` — queued analytics events
- `fcm_notifications_analytics` — push notification tracking
- `notifications_tracker_messages` — notification interaction tracking
- `battery` — battery/CPU usage slices
- `metrics` — performance metrics
- `presence` — user online/offline status tracking

## 2. MyTracker (VK Analytics) Implementation

**Package:** `com.p006my.tracker.*`

**Data collected:**
- Google Advertising ID (GAID) — even checks `isLimitAdTrackingEnabled`
- User age, gender, custom user IDs
- Custom parameters (key-value map)
- App lifecycle events (foreground/background transitions)
- Deep link attribution tracking
- `time_spent` metric sent with advertising ID

**Remote config keys:**
- `mytracker-enabled` — server-side kill switch
- `mytracker-log-level` — controls verbosity

**Initialization:** `AccountInitializer` registers "Mytracker" as an initialization task on app start.

## 3. Tracked Event Categories (via `ok9.m16044h`)

| Category | Events |
|----------|--------|
| BACKGROUND_MODE | `system_curtain_shown`, `system_curtain_hidden`, `carpet_mode_on`, `snack_shown/hidden/click_on`, `work_in_background_permission` |
| PUSH | Push actions with `trid`, `eKey`, `p_op` params |
| CALL | `ADMIN_CALL_SETTINGS`, `ADMIN_CALL_SETTINGS_TO_USER` |
| CLICK | `profile_button_click`, `open_broadcast_button_click`, `video_speed_change` |
| SETTINGS | `TEXT_SIZE` |
| PERMISSION | `permission_status` (daily report of all permissions) |
| VIDEO_MESSAGE | Video message interactions |
| VIDEO_STATS | Video playback statistics |
| REGISTRATION | Registration flow events |
| WEBAPP_ACTION | Web app interactions |
| CHANNEL_RECSYS_FOLDER | `channel_folder_open`, `channel_folder_delete` |
| CONTACT_OR_BLOCK | `showed`, `clicked` |
| INFORMER | Banner interactions |
| DANGEROUS_FILE_ACTIONS | File actions with `source_id`, `source_type` |
| APP_REVIEW | `app_review` |
| AUDIO_TRANSCRIPTION | `transcription_result` |
| INVITE_MAX_BANNER | Invite banner interactions |
| DEV | Developer/debug events |
| CONTEXT_MENU | `folder_context_menu_readall` |

## 4. Screen/Navigation Tracking

- `MessagesListRecyclerViewAnalyticsListener` — tracks scroll behavior in message lists
- `ChatsListRecyclerViewAnalyticsListener` — tracks scroll behavior in chat lists
- Presence tracking system records when user is online/offline with timestamps (`presence` table with `contactServerId`, `seen`, `status`)
- `presence-view-port` — tracks which contacts are visible on screen
- `presence-stat` — enables presence statistics collection

## 5. Message Read Timing

- `readMark` system tracks per-chat read positions
- `max-readmarks` config limits concurrent read mark tracking
- `chat-mark-batch-fail-interval` — batched read mark reporting
- `read-listener-fix` — ensures read events are captured
- `log-messages-meta` / `isLogVisibleMessagesMetaEnabled` — logs metadata of visible messages

## 6. Typing Pattern Collection

- `typing-send-enabled` PmsKey — server-controlled toggle for typing indicator transmission
- `notif-typing-presence` — updates presence status when user types
- Typing events sent to server revealing when user is composing messages
- No direct keystroke speed measurement found, but typing state transitions are tracked

## 7. Location Tracking

- `send-location-enabled` PmsKey — controls location sharing feature
- `Tasks.LocationRequest` protobuf message for location requests
- `ACCESS_FINE_LOCATION`, `ACCESS_COARSE_LOCATION`, `ACCESS_BACKGROUND_LOCATION` permissions requested
- `LocationManager` usage in `ppk.java` — accesses system location service
- Google Location Services (`com.google.android.gms.location.internal.IGoogleLocationManagerService`)
- Daily analytics reports geo permission status (`pType: "geo"`)
- `y-map` PmsKey suggests map/location feature integration

## 8. Device Sensor Data Collection

- `SensorManager` used in multiple contexts:
  - `PhotoEditScreen` — sensor for device orientation during photo editing
  - `koe.java` — registers sensor listener with `getDefaultSensor(8)` (TYPE_PROXIMITY, sensor type 8)
  - `ioe.java` — gets SensorManager from system service
  - `tkh.java` — registers sensor listener with rate `0` (SENSOR_DELAY_FASTEST)
  - `t3d.java` — `getRotationMatrixFromVector`, `remapCoordinateSystem`, `getOrientation` — full device orientation tracking
  - `w3d.java` — reads `accelerometer_rotation` system setting
- `ProximityTracker` in calls SDK — monitors proximity sensor during calls

## 9. `stat-session-background-threshold` PmsKey

- **Definition:** `PmsKey.f88856statsessionbackgroundthreshold` (ordinal 153)
- **Usage:** In `rtd.java` line 216 — `new kbg(this, PmsKey.f88856statsessionbackgroundthreshold, i8)`
- **Exposed as:** `getStatSessionBackgroundThreshold()` returning a `Long` value
- **Purpose:** Defines the time threshold (in ms) after which a background period is considered a new session. Used to segment user sessions for analytics — determines when "app went to background" becomes "user left the app" for stat reporting purposes.

## 10. Performance Events & Registrar Config

### `perf-events` (PmsKey ordinal 175)
- Server config type: `PerfEventsServerConfig`
- Controls which performance events are collected and reported

### `perf-registrar-config` (PmsKey ordinal 176)
- Server config type: `PerfRegistrarServerSettings`
- Configures performance measurement registrars

### Active Performance Registrars:
| ID | Registrar | What it measures |
|----|-----------|-----------------|
| 9 | `NetRegistrar` | Network request latency/errors |
| 11 | `LoginPerfRegistrar` | Login flow timing |
| 13 | `DownloadPerfRegistrar` | Download performance |
| 14 | `MsgRoundTripRegistrar` | Message send-to-delivery latency |
| 15 | `ChatListPerfRegistrar` | Chat list load performance |
| 17 | `ChatHistoryWarmPerfRegistrar` | Chat history cache warming |
| 18 | `SingleShotErrorRegistrar` | One-time error reporting |
| 206 | `WebAppsPerfRegistrar` | Web app load performance |

### Additional monitoring:
- `battery-slice-interval` — periodic battery/CPU usage sampling (stored in `battery` table with `sliceTime`, `utime`, `stime`, `batteryCapacity`, `instantAmperage`)
- `memory-slice-interval` — periodic memory usage sampling
- `net-stat-config` / `NetStatConfig` — network statistics with `loggableOpcodes`
- `opcode-stat-config` — protocol opcode statistics
- `error-stat-limit` — caps error event reporting

## 11. AppTracer (ru.ok.tracer)

**Upload endpoint:** `https://sdk-api.apptracer.ru/api/sample/initUpload` and `/api/sample/upload`

**Components:**
- `SampleUploadWorker` — uploads profiling samples
- `CrashReportInitializer` — crash reporting
- `HeapDumpInitializer` — heap dump collection
- `PerformanceMetricsInitializer` — performance metrics
- `SamplingProfilerInitializer` — CPU sampling profiler
- `DiskUsageWorker` — disk usage reporting
- `NativeBridge` — native code profiling

**Control:** `tracer-non-fatal-crashed-enabled` PmsKey

## 12. Daily Analytics Worker

Runs daily via WorkManager, reports:
- Push notification permission status
- Contacts permission status
- Full storage/gallery permission status (with partial access detection on Android 14+)
- Camera permission status
- Microphone permission status
- Geo/location permission status

Sent as event type `PERMISSION` / `permission_status`.

## Risk Assessment

**Severity: HIGH**

- Extensive behavioral profiling without clear user consent disclosure
- Google Advertising ID collected even when user opts out of ad tracking
- Battery, memory, and CPU profiling reveals device usage patterns
- Presence/typing tracking enables real-time surveillance of user activity
- Session background threshold enables precise app usage duration measurement
- Data sent to VK (Mail.ru Group) servers and AppTracer infrastructure
- All telemetry is server-configurable — can be expanded without app update
- `analytics-enabled` kill switch suggests awareness of privacy concerns
