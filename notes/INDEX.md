---
tags: [moc, index, max, max-26.15.3]
status: living
---

# MAX 26.15.3 — Map of Content

Главная точка входа в реверс-конспект. Каждый узел — отдельная заметка.

## TL;DR

См. [[FINDINGS|FINDINGS — финальная выжимка]] и [[00-INVENTORY|INVENTORY — инвентарь APK и красные флаги манифеста]].

## Темы

### Аутентификация и идентичность

- [[topics/01-mobile-id-cleartext]] — cleartext HTTP к Mobile ID операторов РФ #cleartext #auth
- [[topics/10-webapp-jsbridge]] — JS-bridge `verify_mobile_id` отдаёт мини-апе MSISDN #webapp #js-bridge
- [[topics/11-state-bots-and-content-policy]] — `familyProtection`, `digitalid-botid`, GOST #government #user-settings
- [[topics/13-deeplinks-idp]] — `:auth?externalCallback=1` (MAX как Identity Provider) #idp #deeplink
- [[topics/22-gost-digitalid-family]] — wiring GOST/DigitalID/FamilyProtection (без GOST-крипто) #government
- [[topics/29-external-callback-idp-flow]] — детальный flow ExternalCallback / OK_TOKEN #idp #ws
- [[topics/40-multi-account-server-gated]] — multi-account (two-account-mvp) — server-gated #multi-account

### Серверный контроль и killswitch

- [[topics/03-pms-server-flags]] — 334 серверно-управляемых параметра #server-control
- [[topics/05-dev-menu-in-prod]] — DevMenu в release #dev-menu
- [[topics/12-force-update-killswitch]] — серверный killswitch + `download.max.ru` мимо Play #killswitch
- [[topics/16-server-pushed-ml-models-in-calls]] — KWS/NS-модели качаются с сервера в рантайме #ml #server-control
- [[topics/36-channels-feature-gated]] — каналы — server-gated rollout + CHAT_HIDE для тихой модерации #channels

### Телеметрия и аналитика

- [[topics/04-telemetry-endpoints]] — Apptracer / MyTracker / OneLog x2 / api.oneme.ru #telemetry
- [[topics/14-stat-prefs-metrics]] — `stat_prefs`: что улетает в метрики #metrics
- [[topics/17-apptracer-uplink]] — sample/heap/perf/crash endpoints sdk-api.apptracer.ru #telemetry #tracer

### Слежка и сбор данных

- [[topics/02-vpn-warning]] — серверно-управляемая «отключите VPN» плашка #anti-vpn
- [[topics/06-contacts]] — ContentObserver + двусторонняя синхронизация контактов #contacts
- [[topics/07-nfc-hce-webapp]] — мини-апа эмулирует NFC-карту через JS-bridge #nfc
- [[topics/08-background-wake]] — фоновое присутствие, wake на boot #background
- [[topics/19-fcm-push-payload]] — FCM payload: InboundCall, MessageRemoved, TamtamSpam, LocationRequest #push #command-channel
- [[topics/23-camera-mic-screen-entry-points]] — точки запуска камеры/микрофона/screen capture #camera #microphone
- [[topics/24-host-reachability-probe]] — server-controlled probe хостов + отчёт IP/operator/VPN #host-probe #anti-vpn
- [[topics/30-root-detection-telemetry]] — root-detection без блокировки, isRooted в телеметрию #root-detection
- [[topics/31-onelog-event-categories]] — карта event-категорий OneLog (PUSH/CALL/PERMISSION/...) #telemetry
- [[topics/32-presence-server-controlled]] — hiddenOnline сервероуправляемый, presence-* PmsKey #presence
- [[topics/41-server-side-client-diagnostic-struct]] — серверная структура клиент-диагностики (proxy/isVpn/country/app-update) #server-control #anti-vpn

### Серверная контроль (медиа и сообщения)

- [[topics/42-voice-messages-opus-server-controlled]] — голосовухи: OPUS encoder/bitrate/sample-rate server-tunable #voice-messages #server-control

### Протокол и манифест

- [[topics/18-manifest-deep-dive]] — 55 пермов, 26 services, 16 receivers, 8 providers #manifest
- [[topics/20-ws-protocol-opcodes]] — 159 опкодов TamTam/MAX-протокола #ws #protocol
- [[topics/21-shared-prefs-map]] — карта SharedPreferences файлов #storage
- [[topics/33-ws-msgpack-framing]] — MessagePack как сериализация WS-payload #ws #protocol

### Native-стек

- [[topics/09-native-libs]] — 16 .so, FFmpeg n4.4.3, VK enhancement DSP #native-libs
- [[topics/15-on-device-asr-kws-diarization]] — ASR / KWS / Diarization в `libEnhancementLibShared.so` #asr #kws #ml
- [[topics/28-vk-enh-decrypt-key]] — статический 16+16 ключ для obfuscation моделей #crypto #ml
- [[topics/39-libffmpg-version-license]] — FFmpeg n4.4.3 (3.5y old), GPLv3 в проприетарном продукте #ffmpeg #license
- [[topics/43-libjingle-webrtc-custom-build]] — кастомный WebRTC-форк VK/OK (WebRTC-VK/OK flags, StunCustomAttr) #webrtc #calls

### Серверный контроль (медиа и звонки)

- [[topics/42-voice-messages-opus-server-controlled]] — голосовухи: OPUS encoder/bitrate/sample-rate server-tunable #voice-messages #server-control
- [[topics/45-calls-sdk-pmskey-cluster]] — 30+ PmsKey для звонков (log-audio, signaling-ip, wt-enabled) #calls #server-control
- [[topics/46-server-controlled-logging-flags]] — log-full/log-sensitive/log-messages-meta — server-gated verbose logging #logging #surveillance
- [[topics/44-informer-banners-fakeboss-livestreams]] — informer banners (server-pushed URL), FakeBoss, LiveStreams #server-control #banners

### Аудит

- [[topics/25-resources-audit]] — assets, raw-ресурсы, встроенная фича записи звонка #resources #call-recording
- [[topics/26-upstream-public-repos]] — `github.com/max-messenger`: только bot SDK, без клиента #upstream
- [[topics/27-hardcoded-keys-audit]] — Firebase / MyTracker / Google Maps; приватных ключей нет #secrets
- [[topics/34-yandex-maps-integration]] — Yandex Maps endpoints и утечка координат #yandex #geolocation
- [[topics/35-mytracker-antifraud-sensors]] — сенсорный fingerprint через MyTracker #antifraud #fingerprinting
- [[topics/37-webapp-privilege-configuration]] — server-controlled allowlist привилегированных мини-апок #webapp #server-control
- [[topics/38-deeplink-routes-full-map]] — 8 deeplink-роутов (`:auth`, `:share`, `:join`, `:profile`, `:chats`, ...) #deeplink #intent-surface
- [[topics/47-stickers-server-pipeline]] — stickers-botid, STICKER_SUGGEST, welcome-sticker-ids #stickers #server-control
- [[topics/48-search-opcodes-server-controlled]] — PUBLIC_SEARCH/MSG_SEARCH_TOUCH/CHAT_SEARCH_COMMON_PARTICIPANTS + pub-search-limit #search #surveillance

## Источники (raw)

- `findings/raw/pms_keys.txt` — все 334 серверных PmsKey
- `findings/raw/ws_opcodes.txt` — все 159 WS-опкодов
- `findings/raw/urls_quoted_jadx.txt`, `findings/raw/hosts_quoted_jadx.txt` — outbound
- `findings/raw/wss_smali.txt` — WS-эндпоинты
- `findings/native/` — strings/exports каждой `.so`
- `findings/native/decomp_tracer/` — rzghidra-декомпиляция libtracernative + skeptic README

## Tag-cloud

- #server-control · 03 · 12 · 16 · 02 · 11 · 24
- #surveillance · 01 · 02 · 04 · 06 · 14 · 15 · 16 · 17 · 19 · 24
- #government · 01 · 11 · 13 · 22
- #ml · 15 · 16 · 28
- #native-libs · 09 · 15 · 16 · 28
- #webapp · 07 · 10 · 13
- #anti-vpn · 02 · 24
- #killswitch · 12
- #push · 19
- #ws · 20
- #manifest · 18
- #storage · 21
- #call-recording · 25
- #upstream · 26
- #secrets · 27 · 28

## Граф связей

```
                          01-mobile-id-cleartext
                                 │
              ┌──────────────────┼──────────────────┐
              ▼                  ▼                  ▼
    10-webapp-jsbridge   13-deeplinks-idp      04-telemetry
              │                  │                  │
              ├──→ 02-vpn-warning │                  ├──→ 17-apptracer-uplink
              ├──→ 07-nfc-hce-webapp                 │
              └──→ 22-gost-family ──→ 11-user-settings

09-native-libs ──→ 15-asr-kws ──→ 16-ml-server-pull
                        │                │
                        ├──→ 28-decrypt-key
                        └──→ 03-pms-flags

03-pms-flags ──┬──→ 12-killswitch ──→ 04-telemetry
               ├──→ 02-vpn-warning ──→ 24-host-probe
               ├──→ 05-dev-menu
               ├──→ 08-background-wake
               └──→ 16-ml-server-pull

19-fcm-push  ──┬──→ 12-killswitch  (InboundCall blocked on force-update)
               ├──→ 06-contacts    (MessageRemoved)
               ├──→ 13-deeplinks   (TamtamSpam)
               └──→ 24-host-probe  (LocationRequest wake)

20-ws-opcodes ──→ 03-pms-flags + 19-fcm-push  (parallel command channels)
18-manifest   ──→ 23-camera-mic + 07-nfc + 08-background
21-shared-prefs ──→ 14-stat-prefs + 05-dev-menu + 17-apptracer
06-contacts ──→ 14-stat-prefs ──→ 04-telemetry
25-resources ──→ 23-camera-mic (call recording UX)
27-keys + 28-decrypt ──→ 09-native-libs
```

### Дополнительные фичи (server-gated)

- [[topics/49-polls-server-gated]] — опросы: render-polls/polls-in-p2p/p2g/channels/poll-ttl, GET_POLL_UPDATES #polls #server-control
- [[topics/50-reactions-scheduled-messages]] — реакции (per-chat settings, MSG_GET_DETAILED_REACTIONS) + отложенные сообщения #reactions #server-control
- [[topics/51-organizations-money-transfer]] — org-profile/official-org (верификация), money-transfer-botid, comments-enabled #organizations #money
- [[topics/52-network-session-tls-flags]] — net-ssl-session-validate (server can disable TLS), watchdog-config #network #tls #security
- [[topics/53-ab-status-cfs-pmskey]] — ab-status (A/B group sent to server), cfs (P2P fast-start) #ab-testing #server-control
- [[topics/54-ws-session-config-fingerprint]] — 21-field client fingerprint sent on every WS connect #ws #telemetry #surveillance
- [[topics/55-2fa-stories-misc-pmskey]] — 2FA flow (tracking), Stories (server-gated), ilm, blocked-users #2fa #stories
- [[topics/56-perf-events-apptracer-upload]] — server-configurable perf metrics to apptracer /api/perf/upload #telemetry #apptracer
- [[topics/57-devnull-telemetry-filter]] — devnull (server-controlled analytics blacklist), net-stat-config, opcode-stat-config #telemetry
- [[topics/58-non-contact-sync]] — non-contact sync — server-controlled interaction graph beyond phonebook #contacts #surveillance
- [[topics/59-keep-background-socket]] — keep-background-socket + ping-background-interval — persistent WS in background #network #background
- [[topics/60-user-settings-full-map]] — UserSettings full map: 26+ server-pushed fields (safeMode, searchByPhone, audioTranscription) #user-settings #server-control
- [[topics/61-leakcanary-gost-debug-flags]] — LeakCanary in production APK, gostLicenseCheckEnabled, isDisableWebAppSsl #debug #gost #security
- [[topics/62-white-list-links-content-control]] — white-list-links (server-controlled trusted domains), show-warning-links #links #content-moderation
- [[topics/63-complaints-server-controlled]] — available-complaints (server-controlled reasons), server-side-complains-enabled #complaints #moderation
- [[topics/64-calls-endpoint-server-redirect]] — calls-endpoint (server-pushed call server URL, full routing control) #calls #server-control
- [[topics/65-in-app-review-fake]] — in-app-review-triggers (server-controlled) + FakeInAppReviewBottomSheet #in-app-review #fake
- [[topics/66-utm-tag-link-tracking]] — utm-tag-for-trigger-link-share (auto-append utm_source=trigger to shared links) #utm #tracking
- [[topics/67-yag-shared-prefs-per-account]] — yag SharedPrefs: proxy lists, okToken, deviceId, debug flags (host rotation, DNS emulation) #shared-prefs #proxy #debug
- [[topics/68-cis-enabled-multilang]] — cis-enabled (CIS UI adaptations, server-gated) + multi-lang #localization #geo
- [[topics/69-remote-settings-calls-sdk]] — RemoteSettings: android.dump.bitrate (bitrate dump), p2prelay.config, rating.limits #calls #server-control
- [[topics/70-keyword-spotter-in-calls]] — KWS active during calls, server-controlled turnOffInMs, NativeDoubleArrayConsumer #kws #calls #surveillance
- [[topics/71-call-onelog-events]] — CALL OneLog: 12+ operations (FINISH_CALL, ADMIN_CALL_SETTINGS_TO_USER, camera/mic/screenshare state) #calls #telemetry #surveillance
- [[topics/72-video-message-onelog]] — VIDEO_MESSAGE OneLog: start_recording/delete/hands_free/error with source_id #video-messages #telemetry
- [[topics/73-permission-onelog-daily]] — PERMISSION OneLog: DailyAnalyticsWorker sends 7 permissions daily #permissions #surveillance
- [[topics/74-background-mode-onelog]] — BACKGROUND_MODE OneLog: system_curtain_shown/hidden, work_in_background_permission #background #telemetry
- [[topics/75-settings-power-saving-onelog]] — SETTINGS OneLog: MINIAPP_BIOMETRY (webappId), THEME/BACKGROUND/TEXT_SIZE + POWER_SAVING #settings #telemetry
- [[topics/76-contact-or-block-onelog]] — CONTACT_OR_BLOCK OneLog: server knows every add/block decision #contacts #surveillance
- [[topics/77-share-to-max-invite-onelog]] — SHARE_TO_MAX (chatsInfo with chat types/IDs) + INVITE_MAX_BANNER #sharing #telemetry
- [[topics/78-push-onelog-events]] — PUSH OneLog: show/drop/open_chat/m_as_read/n_q_rep with chat_id #push #surveillance
- [[topics/79-additional-onelog-categories]] — AUDIO_TRANSCRIPTION (chat_id/waiting_time), AUTH_QR (full QR auth log), CHANNEL_RECSYS_FOLDER, PRESENCE #onelog #surveillance
- [[topics/80-action-click-show-onelog]] — ACTION/CLICK/SHOW OneLog: GET_INSTALL_REFERRER (install source), FCM_ON_DELETED_MESSAGES, profile_button_click #onelog #telemetry
- [[topics/81-dangerous-actions-registration-onelog]] — DANGEROUS_URL_ACTIONS/DANGEROUS_FILE_ACTIONS (server knows dangerous link clicks), REGISTRATION, INFORMER, BANNER #onelog #surveillance
- [[topics/82-message-clickable-element-onelog]] — MESSAGE_CLICKABLE_ELEMENT_ACTIONS: server knows every link click/phone tap/copy (message_id, source_id) #onelog #surveillance
- [[topics/83-ok9f-geolocation-search-inline]] — ok9.f() events: geolocation_send_click, search_click, inline_button_click #onelog #telemetry #location
- [[topics/84-transcribe-media-ws-opcode]] — TRANSCRIBE_MEDIA(202): audio/video content sent to server for transcription #transcription #surveillance #ws
- [[topics/85-notif-ws-opcodes]] — NOTIF_* WS opcodes: 25 server-to-client push (NOTIF_CONFIG/NOTIF_LOCATION_REQUEST/NOTIF_DRAFT/NOTIF_BANNERS) #ws #server-push
- [[topics/86-neuroavatars-ai]] — NeuroAvatars: AI avatar generation at registration, photo likely sent to server #ai #biometrics #registration
- [[topics/87-location-ws-opcodes]] — LOCATION WS: NOTIF_LOCATION_REQUEST (server requests geo), liveLocation stream, MAX+Yandex recipients #location #surveillance #ws
- [[topics/88-call-rate-quality-rating]] — Call Rate: quality rating dialog (callId, sdkReasons), server-controlled conditions #calls #analytics
- [[topics/89-video-transcoding-config]] — video-transcoding-class (server-controlled quality: low/average/high) + one-video-uploader-config #video #server-control
- [[topics/90-media-upload-opcodes]] — media upload WS opcodes + speedy-upload (fake progress), file-upload-unsupported-types (content filter) #media #upload #server-control
- [[topics/91-additional-ws-opcodes]] — additional WS opcodes: DEBUG (server debug cmd), LOG (logs to server), LINK_INFO (server sees pasted links), DRAFT_SAVE #ws #surveillance
- [[topics/92-webapp-complaints-folders-polls-ws]] — WEB_APP_INIT_DATA (miniapp launch), VOTERS_LIST_BY_ANSWER (non-anonymous voting), AUDIO_PLAY (voice listen tracking) #ws #webapp #polls
- [[topics/93-client-conv-id-send-queue]] — client-conv-id (P2P conversation ID), send-queue-size (server-controlled queue limit) #messaging #server-control
- [[topics/94-battery-memory-monitoring]] — battery-slice-interval (server-controlled battery monitoring) + memory-slice-interval + battery_level_change in call stats #battery #telemetry
- [[topics/95-calls-stats-40-metrics]] — calls stats: 40 metrics (CPU/Memory/Battery/AudioLevel/VideoQuality) sent to server after each call #calls #telemetry #surveillance
- [[topics/96-vchat-api-methods]] — vchat.* API: 14 methods (clientStats/getLogUploadUrl/clientSupportedCodecs) #calls #api #telemetry
- [[topics/97-api-oneme-ru-endpoints]] — api.oneme.ru endpoints: production + test servers (api-tg.oneme.ru unknown) in dev menu #api #network
- [[topics/98-mytracker-api-details]] — MyTracker API: tracker-api.vk-analytics.ru paths (v3/mobile/v1/mlapi/beta-ml), mt_deeplink attribution #mytracker #telemetry
- [[topics/99-mytracker-preinstall-oem]] — MyTracker PreInstall: OEM preinstall detection via vendor app resources + ro.mytracker.preinstall.path #mytracker #oem #attribution
- [[topics/100-call-chat-members-load-config]] — call-chat-members-load-config: maxLoadCount/minInCall/newLoadingContactsLogicEnabled #calls #server-control
- [[topics/101-anr-watchdog-config]] — anr-config (server-controlled ANR detector: enabled/timeout.low/avg/high) + watchdog-config #anr #server-control
- [[topics/102-conn-timeouts]] — conn-timeouts: server-controlled connection timeouts (JSON map by connection type) #network #server-control
- [[topics/103-media-order]] — media-order: 4 modes (0-3), mode 3 = server-controlled media placement in messages #media #ui #server-control
- [[topics/104-bad-network-video-msg-config]] — bad-networ-indicator-config (RTT/loss algorithm) + video-msg-config #calls #network #server-control
- [[topics/105-chat-history-warm]] — chat-history-warm-opts (server-controlled chat history preloading) #performance #server-control
- [[topics/106-perf-registrar-net-stat-config]] — perf-registrar-config (maxAttempts/rawPersistInterval/TTL) + net-stat-config (loggableOpcodes) #telemetry #server-control
- [[topics/107-opcode-stat-session-stats]] — opcode-stat-config + session.opcode_stats (opcodeStats/anrDetected/caughtExceptionCount/phonebookSize) #telemetry #surveillance
- [[topics/108-fresco-executor-thread-pool]] — fresco-executor + system-thread-pool-queue (server-gated performance hacks) #threading #server-control
- [[topics/109-presence-pmskey-full]] — presence PmsKey full map: presence-external/presence-stat/presence-ttl=300s #presence #server-control
