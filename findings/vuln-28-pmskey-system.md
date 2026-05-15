# VULN-28: PmsKey Server-Controlled Behavior System

## Summary
Total PmsKey enum values: **335** (indices 0-334)
Source: `ru/p027ok/tamtam/android/prefs/PmsKey.java`
Consumer: `p000/qp6.java`

The server can remotely control 335 client behaviors without user consent via the PmsKey system. Values are pushed from server and consumed by `qp6.java` which wraps them in typed accessors (ko6=boolean, no6=boolean, oo6=long, po6=string, xo6=int, lcc=JSON object, jai=lazy, kp6=parsed config).

## CRITICAL DANGER: Most Dangerous Keys

### Surveillance Keys
| Key | Index | Risk |
|-----|-------|------|
| `log-full` | 70 | **CRITICAL** - Enables full logging of all app activity |
| `log-sensitive` | 71 | **CRITICAL** - Logs sensitive user data (messages, credentials) |
| `log-messages-meta` | 277 | Logs metadata of visible messages |
| `log-chat-meta` | 278 | Logs chat metadata |
| `log-violations` | 224 | Logs policy violations |
| `presence-offline-log` | 287 | Logs user online/offline presence |
| `presence-stat` | 283 | Presence statistics collection |
| `calls-sdk-webrtc-logs` | 103 | Logs WebRTC call data |
| `calls-sdk-log-audio` | 131 | Logs audio from calls |
| `calls-sdk-traffic-markup` | 104 | Marks/logs call traffic |
| `calls-sdk-incall-stat` | 109 | In-call statistics |
| `debug-mode` | 2 | Enables debug mode remotely |
| `android-use-logcat-logger` | 5 | Forces logcat output |
| `debug-profile-info` | 73 | Exposes profile debug info |
| `analytics-enabled` | 158 | Master analytics toggle |
| `mytracker-enabled` | 98 | MyTracker analytics |
| `wm-analytics-enabled` | 62 | WorkManager analytics |
| `net-stat-config` | 184 | Network statistics config |
| `opcode-stat-config` | 190 | Protocol opcode statistics |
| `perf-events` | 175 | Performance event collection |

### Censorship/Blocking Keys
| Key | Index | Risk |
|-----|-------|------|
| `host-reachability` | 227 | **CRITICAL** - Server controls which hosts are "reachable" |
| `gost-check-env` | 332 | **CRITICAL** - GOST crypto environment check (Russian state crypto) |
| `white-list-links` | 209 | Server controls allowed links |
| `show-warning-links` | 182 | Warns users about specific links |
| `blocked-users` | 306 | Server-side user blocking |
| `dps` | 228 | Deep packet inspection flag |
| `show-vpn-chat-bottomsheet` | 321 | VPN usage detection/warning in chats |
| `show-vpn-call-bottomsheet` | 322 | VPN usage detection/warning in calls |
| `show-vpn-snackbar` | 328 | VPN snackbar warning |

### Security Degradation Keys
| Key | Index | Risk |
|-----|-------|------|
| `net-ssl-session-validate` | 246 | **CRITICAL** - Can DISABLE SSL validation |
| `net-session-rbc-enabled` | 247 | Network session certificate check |
| `net-client-dns-enabled` | 164 | Client DNS resolution control |
| `creation-2fa-config` | 165 | 2FA configuration - can weaken |
| `disconnect-timeout` | 28 | Force disconnections |
| `conn-timeouts` | 140 | Connection timeout manipulation |
| `keep-connection` | 19 | Persistent connection control |
| `keep-background-socket` | 302 | Background socket persistence |
| `proxy` | 1 | Proxy configuration |
| `proxy-domains` | 6 | Which domains use proxy |
| `calls-endpoint` | 74 | Call server endpoint (MITM risk) |
| `calls-android-signaling-ip` | 119 | Direct IP for call signaling |
| `invalidate-db-force` | 275 | Force database invalidation |
| `invalidate-db-msg-exception` | 274 | Message DB corruption |

### Service Degradation Keys
| Key | Index | Risk |
|-----|-------|------|
| `bad-networ-indicator-config` | 157 | Fake "bad network" indicator |
| `disconnect-timeout` | 28 | Force disconnect timing |
| `net-session-suppress-bad-disconnected-state` | 245 | Hide connection issues |
| `subscription-timeout-seconds` | 53 | Subscription timeout |
| `calls-sdk-disable-pipeline` | 130 | Disable audio pipeline in calls |
| `calls-sdk-dnt-disable-audio` | 134 | Disable audio on call start |
| `hide-incoming-call-notif` | 194 | Hide incoming call notifications |
| `typing-send-enabled` | 219 | Disable typing indicators |

## Type Classification

### Boolean flags (ko6/no6/oo6/po6 in qp6.java)
~220 keys are boolean toggles enabling/disabling features.

### Integer/Long values (xo6/ro6/so6/to6)
- `call-permissions-interval`, `energy-saving-request-interval`, `landscape`
- `system-thread-pool-queue`, `fresco-executor`
- Various timeouts, limits, counts

### String values (po6 with string return)
- `outgoing-call-uri`, `calls-endpoint`, `live-streams-url-prefix`
- `settings-business`, `bot-start-param`

### JSON/Object configs (lcc/bbi/kp6/ew0)
- `calls-android-ac` → CallsAudioFormatConfig
- `calls-android-signaling-to` → CallsSignalingTimeouts
- `one-video-uploader-config` → OneVideoUploaderConfig
- `devnull` → DevNullServerConfig
- `perf-events` → PerfEventsServerConfig
- `perf-registrar-config` → PerfRegistrarServerSettings
- `upload-video-config` → UploadVideoConfig
- `net-stat-config` → NetStatConfig
- `watchdog-config` → parsed config
- `keep-background-socket` → BackgroundWakeConfig
- `webapp-pr` → parsed via bp9
- `media-transform` → ija parser

## Categories

### Calls (50 keys)
calls-endpoint, calls-sdk-*, calls-android-*, calls-use-p2p-*, call-rate, call-pinch-to-zoom, call-ping-fix, call-permissions-interval, call-chat-members-load-config, early-call-start, outgoing-call-uri, group-call-part-limit, gc-from-p2p, gc-link-pre-settings, gc-wait-admin, reconnect-call-ringtone, energy-saving-*, horizontal-call-mode, set-audio-device, hide-incoming-call-notif, calls-fakeboss-incoming-call-enabled

### Security/Network (25 keys)
proxy, proxy-domains, net-ssl-session-validate, net-session-rbc-enabled, net-client-dns-enabled, net-session-suppress-bad-disconnected-state, conn-timeouts, disconnect-timeout, keep-connection, keep-background-socket, ping-background-interval, host-reachability, gost-check-env, dps, creation-2fa-config, spin-lock-enabled, net-stat-config, white-list-links, show-vpn-*, blocked-users

### Analytics/Logging (25 keys)
log-full, log-sensitive, log-violations, log-messages-meta, log-chat-meta, analytics-enabled, mytracker-enabled, mytracker-log-level, wm-analytics-enabled, perf-events, perf-registrar-config, presence-stat, presence-offline-log, opcode-stat-config, error-stat-limit, stat-session-background-threshold, debug-mode, debug-profile-info, debug-broken-contact, android-use-logcat-logger, min-log-level, user-debug-report, tracer-non-fatal-crashed-enabled, anr-config, push-delivery

### UI/UX (80 keys)
image-*, sticker sizes, new-collage, new-media-*, adapt-bubble-width, chatlist-subtitle-ver, informer-*, landscape, settings-entry-banners, multi-select-bars-redesign, channel-view-config, themes (new-year, february, march), mediasaves-*, organization-placeholder, not-contact-placeholder

### Features (100 keys)
channels-enabled, comments-enabled, scheduled-*, reactions-*, polls-*, drafts-sync-enabled, quotes-enabled, live-streams, story, two-account-mvp, folders-*, join-requests, bots-channel-adding, sticker-set-edit-enabled, video-fast-seek-enabled, media-playlist-enabled, send-location-enabled, account-nickname-enabled, webapp-*

### Media/Performance (55 keys)
video-*, audio-*, media-*, player-load-control, one-video-*, upload-*, speedy-*, opus-recorder-*, video-transcoding-class, camera-freeze-detector-timeout, fresco-executor, system-thread-pool-queue, db-query-ex-count, db-tr-ex-count, wm-*

## Architecture
```
Server Push → PmsKey enum (335 values) → qp6.java (typed wrappers) → App behavior
```

The server has COMPLETE remote control over the client. No user consent or notification is required for any change. The `log-full` + `log-sensitive` combination allows full surveillance. The `net-ssl-session-validate` key can disable TLS verification enabling MITM. The `gost-check-env` key enforces Russian state cryptography compliance checks.
