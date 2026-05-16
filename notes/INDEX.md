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
- [[topics/110-audio-playback-pmskey]] — audio playback PmsKey: audio-play-opus/audio-download(default=false)/audio-prefetch #audio #server-control
- [[topics/111-invite-friends-frequency]] — invite-friends-sheet-frequency: server-controlled invite schedule (array of days) #growth #server-control
- [[topics/112-user-debug-report-chat]] — user-debug-report + userLogReportChatId (server directs user logs to specific chat) + debug-mode #debug #surveillance
- [[topics/113-invalidate-db-force]] — invalidate-db-force: server can force-delete local DB via version bump #database #destructive #server-control
- [[topics/114-views-count-enabled]] — views-count-enabled: server-gated post view tracking in channels #channels #surveillance #server-control
- [[topics/115-quotes-media-playlist]] — quotes-enabled (markdown quotes, default=false) + media-playlist-enabled (default=false) #messaging #server-control
- [[topics/116-join-requests]] — join-requests: server-gated join requests for closed chats/channels #groups #server-control
- [[topics/117-call-ui-pmskey]] — call UI PmsKey: horizontal-call-mode/hide-incoming-call-notif/ringtone-player-focus #calls #ui #server-control
- [[topics/118-media-ux-pmskey]] — media UX PmsKey: speedy-voice-messages/open-video-from-start/new-media-upload-ui/video-fast-seek #media #ux #server-control
- [[topics/119-folders-pmskey]] — folders PmsKey: folders-max-count(30)/enable-filters-for-folders/channels-suggests-folder #folders #server-control
- [[topics/120-multiselect-messages-meta]] — multi-select-bars-redesign + log-messages-meta (meta info on click) #ui #server-control
- [[topics/121-notifications-bots-bugfix-pmskey]] — cancel-stale-notifications + new-intent-fix (server-gated bugfixes) + bots-channel-adding #notifications #bots #server-control
- [[topics/122-vpn-ui-pmskey]] — VPN UI PmsKey: show-vpn-snackbar/show-vpn-chat-bottomsheet(int)/show-vpn-call-bottomsheet(int) #vpn #anti-vpn #server-control
- [[topics/123-group-calls-pmskey]] — group calls PmsKey: gc-from-p2p/gc-wait-admin/group-call-part-limit #calls #group-calls #server-control
- [[topics/124-calls-signaling-pmskey]] — calls signaling PmsKey: outgoing-call-uri(default=https://max.ru)/calls-android-signaling-to(JSON timeouts)/early-call-start #calls #signaling
- [[topics/125-p2p-relay-webtransport]] — calls-use-p2p-relay (server routes call media through relay) + calls-sdk-wt-enabled (WebTransport) #calls #p2p #server-control
- [[topics/126-energy-saving-pmskey]] — energy-saving-bottom-sheet (server pressures users to disable battery saving) + call-permissions-interval #energy #server-control
- [[topics/127-calls-sdk-stats-ice-pmskey]] — calls SDK stats/ICE PmsKey: calls-sdk-incall-stat(real-time)/direct-ice-restart/no-ice-restart/opus-adapt #calls #server-control
- [[topics/128-h265-unknown-contact-pmskey]] — H265 codec PmsKey + enable-unknown-contact-bottom-sheet (3 modes for unknown caller UI) #calls #codec #server-control
- [[topics/129-opus-bwe-audio-pmskey]] — Opus BWE PmsKey: calls-sdk-ai-opus-bwe/calls-sdk-linear-opus-bwe(Linear BWE)/calls-sdk-log-audio(log local audio) #calls #opus #server-control
- [[topics/130-calls-audio-pipeline-simulcast]] — calls audio pipeline: calls-sdk-disable-pipeline(Отключить аудио пайплайн)/calls-sdk-dnt-disable-audio/simulcast-sw-vp8 #calls #audio #server-control
- [[topics/131-calls-vtv2-lla-nidm]] — calls-android-vtv2(видеотрансформер v2)/calls-android-lla(LL audio)/calls-android-nidm(маппинг только собственного ID) #calls #video #server-control
- [[topics/132-calls-fastjoin-ns-newpms]] — calls-android-fast-join/calls-android-ns(шумодав)/calls-android-new-pms(новый механизм настроек) #calls #noise-suppression #server-control
- [[topics/133-calls-early-offer-genpeerid]] — calls-android-early-set-offer/calls-android-gen-peerid(клиентская генерация peer-id)/calls-android-update-endpoint-params #calls #webrtc #server-control
- [[topics/134-webrtc-field-trials]] — WebRTC field trials: 8 hardcoded + server-controlled (EarlyStartPlayout/EarlyStartRecording) + arbitrary server field trials #webrtc #calls #server-control
- [[topics/135-callssdk-opus-dred-fec-flags]] — CallsSDK Opus DRED/FEC: OpusFECWithDRED/OpusDREDByBitrate/OpusFileLogs(write to file)/LogDCTraffic #calls #opus #dred
- [[topics/136-ev1-webrtc-session-config]] — ev1 WebRTC session config: emulatedSignalingError(server force-terminates call)/emulatedIceCandidateError/simulcastState #calls #webrtc #server-control
- [[topics/137-rating-limits-p2prelay-config]] — android.rating.limits (RateManagerConfig: RTT/loss/candidate type) + android.p2prelay.config (P2P relay switch) #calls #quality #server-control
- [[topics/138-settings-get-api]] — settings.get API: HTTP endpoint for 6 RemoteSettings keys (separate from WS/PmsKey) #calls #api #server-control
- [[topics/139-api-protocol-constants]] — ApiProtocol: p2p_forbidden(server bans P2P)/stun_server/turn_server(server-controlled ICE)/wt_endpoint/onlyAdminCanRecord #calls #api #server-control
- [[topics/140-conversation-params]] — vchat.getConversationParams: ispAsNo/ispAsOrg/locCc/locReg(server knows ISP+geo)/isP2PForbidden/stunTurnServers/wsIps #calls #api #surveillance
- [[topics/141-rate-call-data]] — RateCallData: server-pushed questions and scale for call rating dialog #calls #quality-rating #server-control
- [[topics/142-conversation-params-compact]] — ConversationParams compact: LZ4+Base64 JSON with TURN credentials(trne/trnu/trnp)/STUN/WS/WebTransport endpoints #calls #stun #turn
- [[topics/143-start-conversation-api]] — vchat.startConversation: hex-capabilities/protocolVersion(6=VOIP_MULTIPLE_DEVICES)/domainId/onlyAdminCanShareMovie #calls #api
- [[topics/144-hangup-join-api]] — vchat.hangupConversation(reason enum) + vchat.joinConversationByLink(peerId/anonymToken/capabilities) #calls #api
- [[topics/145-ok-external-ids-mapping]] — vchat.getOkIdsByExternalIds + vchat.getExternalIdsByOkIds: bidirectional ID mapping between MAX and VK/OK services #calls #identity #api
- [[topics/146-log-upload-device-fingerprint]] — vchat.getLogUploadUrl + och.java: device model/osVersion/ispAsOrg/locCc/locReg in every call API request #calls #surveillance #fingerprint
- [[topics/147-call-native-analytics-request]] — CallNativeAnalyticsApiRequest: platform=ANDROID/appVersion/sdkVersion=0.1.13 + 40 metrics #calls #analytics #telemetry
- [[topics/148-call-external-analytics]] — CallExternalAnalyticsApiRequest: external analytics with application/collector/platform (two independent analytics channels) #calls #analytics
- [[topics/149-upload-config-calls-analytics]] — UploadConfig: DEFAULT_DISABLE_UPLOAD_IN_CALL=true/DEFAULT_MAX_EVENT_COUNT=800/DEFAULT_MAX_FILE_SIZE_KB=15 #calls #analytics #upload
- [[topics/150-apply-bitrate-dump-config]] — applyBitrateDumpGatheringConfig: server-controlled bitrate dump file per call (android.dump.bitrate.isEnabled) #calls #bitrate-dump #server-control
- [[topics/151-conversation-impl-callbacks]] — ConversationImpl: onCustomData(arbitrary JSON via DataChannel)/onCallParticipantFingerprint(DTLS tracking)/onRateCall(server-initiated) #calls #webrtc #datachannel
- [[topics/152-asr-online-manager]] — AsrOnlineManager: real-time call transcription (AsrOnlineChunk: participantId+text), three audio analysis levels #calls #asr #surveillance
- [[topics/153-signaling-commands]] — signaling commands: 24 commands (mute-participant/request-asr/enable-feature-for-roles/report-network-stat) #calls #signaling #server-control
- [[topics/154-conversation-features-roles]] — ConversationFeature: ADD_PARTICIPANT/RECORD/MOVIE_SHARE/ASR_RECORD with role-based access #calls #features #asr #recording
- [[topics/155-call-roles]] — kv1 call roles: CREATOR/ADMIN/SPEAKER for role-based feature access #calls #roles
- [[topics/156-microphone-manager]] — MicrophoneManager: registerAudioSampleCallback (KWS/ASR/energy all access mic audio) #calls #microphone #surveillance
- [[topics/157-watch-together-url-sharing]] — WatchTogether (add-movie: movieId/gain/moveToAdminOnHangup) + URL Sharing (start-url-sharing/stop-url-sharing) #calls #watch-together
- [[topics/158-change-media-settings]] — change-media-settings: handleForceChangeMediaSettings (server/admin force-disables camera/mic/screen_capture) #calls #force-mute #server-control
- [[topics/159-stereo-room-manager]] — StereoRoomManager: rooms/roles/hands (requestPromotion/grantAdmin/revokeAdmin/handsQueue) #calls #rooms #moderation
- [[topics/160-update-media-modifiers]] — update-media-modifiers: denoise/denoiseAnn (server force-changes noise suppression during call) #calls #denoise #server-control
- [[topics/161-change-simulcast]] — change-simulcast: simulcast layers (rid/width/height/fps/bitrateKbps) for CAMERA #calls #simulcast #video
- [[topics/162-perf-stat-display-layout]] — report-perf-stat(framesReceived/framesDecoded) + update-display-layout(server knows UI layout: size/fit/screenshare) #calls #signaling #surveillance
- [[topics/163-conversation-stats]] — ConversationStats: 18+ components (kwsStat/mlDownloadStat/iceCandidatePairChangedStat/serverTopologyRequestedStat) #calls #stats #telemetry
- [[topics/164-signaling-transport-stat]] — SignalingTransportStat: WS signaling stats (connect/disconnect/ping/pong/errors/timeouts) #calls #signaling #stats
- [[topics/165-ice-candidate-pair-stat]] — IceCandidatePairChangedStat: server gets local_address/remote_address(real IPs)/SDP/reason on every ICE pair change #calls #ice #surveillance
- [[topics/166-accept-call-stat]] — AcceptCallStat: call_accepted_incoming with isCaller/isMe/isConcurrent flags #calls #stats
- [[topics/167-p2p-relay-requested-stat]] — P2PRelayRequestedStat: client_requested_p2p_relay with trigger/threshold/violationsCount #calls #p2p-relay #stats
- [[topics/168-server-topology-stat]] — ServerTopologyRequestedStat: client_requested_server_topology (media through VK/OK servers) #calls #topology #stats
- [[topics/169-negotiation-audio-error-stat]] — NegotiationErrorStat(SDP on error) + AudioErrorStat(audio_error with 3 fields) #calls #errors #stats
- [[topics/169-negotiation-audio-error-stat]] — NegotiationErrorStat(SDP on error) + AudioErrorStat(audio_error with 3 fields) #calls #errors #stats
- [[topics/170-ice-candidate-error-stats]] — IceCandidateGatheringFailedStat(local_address/remote_url/transport) + IceCandidateAddFailedStat #calls #ice #errors
- [[topics/171-ice-restart-peer-connection-stat]] — IceRestartStat(ice_restart) + PeerConnectionChangedStat(connection_state_changed/p2p_relay) #calls #ice #peer-connection
- [[topics/172-conversation-started-signaling-stat]] — ConversationStartedStat(call_start: callType/warmupStatus) + ConversationConnectedToSignalingStat(signaling_connected: time_ms) #calls #stats
- [[topics/173-call-finish-init-stat]] — CallFinishStat(call_finish: reason/rate_reasons/errorText) + CallInitStat(call_init: source=callType+isAnon) #calls #stats #end-reason
- [[topics/174-conversation-prepared-stat]] — ConversationPreparedStat: call_warmup (WebRTC warmup time in ms) #calls #stats #warmup
- [[topics/175-conversation-end-reason]] — ConversationEndReason: 17 reasons (banned/killed/obsolete_client/another_device/socket_closed) #calls #end-reason
- [[topics/176-rate-hints]] — RateHint: rtt_<ms>/audioloss_<loss>/videoloss_<loss>/candidate_type, server-controlled thresholds #calls #quality #rate-hints
- [[topics/177-network-stat]] — NetworkStat: rttMs/audioLoss/videoLoss/activeCandidateType(host/srflx/relay) #calls #network #stats
- [[topics/178-call-type]] — Conversation.CallType(Incoming/Outgoing/Join) + Conversation.State(None/Preparing/Starting/Connecting/Connected/Finished) #calls #state
- [[topics/179-conversation-interface]] — Conversation interface: 30+ managers (AsrManager/AsrOnlineManager/KWS/Camera/Microphone/NoiseSuppressionManager/RecordManager/DebugManager) #calls #interface
- [[topics/180-record-manager]] — RecordManager: startRecord(isStream/movieId/albumId/privacy) + stopRecord, VK/OK video platform integration #calls #recording #stream
- [[topics/181-noise-suppression-manager]] — NoiseSuppressionManager: 4 modes (serversideBasic/serversideAnn/clientsidePlatform/clientsideAnn) + filePath(ML model) #calls #noise-suppression #ann
- [[topics/182-chat-manager]] — ChatManager: in-call chat via DataChannel (OutboundMessage: participantId+text), P2P relay routes through VK/OK servers #calls #chat #datachannel
- [[topics/183-media-mute-participant-states]] — MediaMuteManager(updateMediaOptionsForAll/ForParticipant) + ParticipantStatesManager(participantId/isOn/timestamp) #calls #mute #participants
- [[topics/184-asr-manager]] — AsrManager: on-device ASR recording to file (fileName/sessionRoomId), separate from AsrOnlineManager #calls #asr #recording
- [[topics/185-feedback-manager]] — FeedbackManager: emoji reactions in calls (sendFeedback: key/source, setTimeout) #calls #feedback #reactions
- [[topics/186-contact-call-manager]] — ContactCallManager: iAmAnonymous/iWasInitiallyAnonymous (anonymous participants via anonymToken) #calls #anonymous
- [[topics/187-network-connection-manager]] — NetworkConnectionManager: getTopology(P2P/server)/registerBadConnectionCallback/addNetworkConnectivityListener #calls #network #topology
- [[topics/188-camera-manager]] — CameraManager: getNumberOfCameras/isCameraEnabled/isCapturingFromFrontCamera/setCameraEnabled/switchCamera #calls #camera
- [[topics/189-media-connection-manager]] — MediaConnectionManager: onMediaConnected(isFirstConnection)/onMediaDisconnected #calls #media-connection
- [[topics/190-conversation-events-listener]] — ConversationEventsListener: 30+ events (onMicrophoneForciblyMuted/onParticipantsDeAnonymized/onMigratedToServerTopology) #calls #events #surveillance
- [[topics/191-conversation-participant]] — ConversationParticipant: 20+ fields (getAcceptedCallClientType/Platform/Capabilities/NetworkStatus/isTalking/isReported) #calls #participant #surveillance
- [[topics/192-client-capabilities]] — ClientCapabilities: 15 bits hex-encoded (BIT_SCREEN_TRACK/VIDEO/WAITING_HALL/WATCH_MOVIE/P2P_RELAY/WAIT_FOR_ADMIN) #calls #capabilities
- [[topics/193-call-info]] — CallInfo: endpoint/wsIps/wtEndpoint/wtIps/STUN/TURN/isP2PForbidden/deviceIndex (server controls all call routing) #calls #call-info #server-control
- [[topics/194-start-conversation-delegate]] — StartConversationDelegate.Params: conversationId/calleeIds/chatId/isVideo/internalParams #calls #start-call
- [[topics/195-conversation-factory]] — ConversationFactory: 6 call types (startCall/createConf/answerCall/joinCall/joinAnonByLink/joinByLink), ConversationBuilder 18+ params #calls #factory
- [[topics/196-debug-media-dump-manager]] — DebugManager+MediaDumpManager: requestMediaDump(remote audio+video), 6 audio intercept points, enableFullAudioDump in prod #calls #debug #dump #dev-in-prod
- [[topics/197-ml-features-conversation-feature-manager]] — MLFeaturesManager(KWS+NS delegates+DownloadService) + ConversationFeatureManager(enableFeatureForAll/Roles) #calls #ml #kws #ns #server-control
- [[topics/198-session-stereo-rate-manager]] — SessionRoomsManager(breakout/assignParticipants) + StereoRoomManager(promote/handsQueue) + RateManager #calls #rooms #stereo
- [[topics/199-calls-audio-screen-video-render]] — CallsAudioManager(onMutedForever/proximity) + ScreenCaptureManager(setAudioCaptureEnabled=system audio) + VideoRenderManager #calls #audio #screen-capture
- [[topics/200-vchat-api-full-list]] — vchat API: 11 methods (clientSupportedCodecs/getConversationParams/createJoinLink/removeJoinLink/ID mapping) #calls #api #vchat
- [[topics/201-calls-sdk-architecture-summary]] — Calls SDK architecture summary: 24 managers, 24 signaling commands, 18+ stat structs, 10 key indicators (no E2E/ASR/KWS/requestMediaDump/deAnonymize) #calls #architecture #summary
- [[topics/202-onelog-upload-mechanism]] — OneLog upload: Collector(file buffer)/Uploader/OneLogApiRequest(application/platform in every request) #telemetry #onelog #upload
- [[topics/203-live-location]] — Live Location: lat/lon/alt/accuracy/bearing/speed/livePeriod + FCM LocationRequest push (server-initiated geolocation) #location #surveillance #fcm
- [[topics/204-opus-file-writer]] — Opus FileWriter: nativeAudioStartRecord(path/sampleRate/channels)/nativeAudioWriteFrame (used in ASR+MediaDump) #calls #opus #audio-recording #native
- [[topics/205-tasks-proto]] — Tasks.proto: 60+ background task types (LocationRequest/CritLog/SuspendBot/UpdateFireTimeProtoTask/SyncChatHistory) #protocol #tasks #protobuf
- [[topics/206-protos-schemas]] — Protos.java: 50+ protobuf nano schemas (Location/ttl/livePeriod, VideoConversation/previewParticipantIds, LogEvent) #protocol #protobuf #messages
- [[topics/207-critlog-events]] — CritLog: 30+ event types (AUDIO_TRANSCRIPTION/DANGEROUS_FILE_ACTIONS/MINIAPP_BIOMETRY/GET_INSTALL_REFERRER) #telemetry #critlog #surveillance
- [[topics/208-webapp-telemetry]] — WebApp telemetry: WEBAPP_ACTION(OPEN/CLOSE/MINIAPP_TAKE_PHOTO) + WEBAPP_BRIDGE(every JS call: botId/method/code) #webapp #telemetry #surveillance
- [[topics/209-install-referrer]] — GET_INSTALL_REFERRER: getInstallerPackageName()+is_update_version via CritLog (server knows install source) #telemetry #install #attribution
- [[topics/210-search-response-telemetry]] — SEARCH_RESPONSE: inputQuery(search terms sent to server) + counters(RECENTS/ALL_CONTACTS/LOCAL_SEARCH) #telemetry #search #surveillance
- [[topics/211-presence-message-click-telemetry]] — PRESENCE/EVENT_MESSAGE_COUNTER(8 chat-open counters) + MESSAGE_CLICKABLE_ELEMENT_ACTIONS(7 click types with messageId) #telemetry #presence #surveillance
- [[topics/212-channel-recsys-telemetry]] — CHANNEL_RECSYS_FOLDER: channel_folder_click/follow/delete + channel_id/channel_position (recsys training data) #telemetry #channels #recommendation
- [[topics/213-video-audio-stats-telemetry]] — VIDEO_STATS(action_play/first_bytes/content_error + vid/vsid/cdn_host/quality/connection_type) + AUDIO_STATS #telemetry #video #media-stats
- [[topics/214-ws-session-config-extended]] — WS session config extended: proxy/proxy-domains/isVpn/location(geo country)/reg-country-code/callsSeed #server-control #vpn #proxy #network
- [[topics/215-messages-db-schema]] — messages DB schema: 35+ fields (ttl/live_until/delayed_attrs_time_to_fire/channel_views/msg_link chain) #database #messages #schema
- [[topics/216-local-db-tables]] — Local DB: 35+ tables (webapp_biometry/fcm_notifications_history/stat_events/battery/presence/tasks) #database #schema #local-storage
- [[topics/217-webapp-biometry]] — WebApp Biometry: webapp_biometry(user_id/bot_id/token/access_granted) + 8 JS-bridge methods (AuthRequest/UpdateToken) #webapp #biometry #jsbridge
- [[topics/218-fcm-stat-db-schemas]] — FCM+stat_events DB: fcm_notifications_analytics(5 push timestamps)/stat_events(BLOB buffer)/fcm_notifications(text/url/bmd) #database #fcm #push #analytics
- [[topics/219-battery-organizations-db]] — battery(sliceTime/utime/stime/batteryCapacity/instantAmperage) + organizations(parentId/folderTemplateId) DB #database #battery #organizations
- [[topics/220-quic-tls-disabled]] — QUIC TLS disabled: null X509TrustManager for WebTransport QUIC, app prints SECURITY WARNING but uses it anyway #tls #security #quic #mitm
- [[topics/221-network-security-config]] — network_security_config.xml: cleartext HTTP explicitly allowed for 6 RU operator domains (Megafon/MTS/Tele2/T2/Beeline) #tls #cleartext #mobile-id #operators
- [[topics/222-deeplink-schemes]] — Deep link schemes: max:// yandexmaps:// yandexnavi:// dgis:// petalmaps://(Huawei) https://max.ru #deeplinks #maps #huawei
- [[topics/223-device-performance-telemetry]] — Device performance telemetry: bucket(AppStandbyBucket)/memory/exit_reason(getHistoricalProcessExitReasons)/img_cache #telemetry #device-info #surveillance
- [[topics/224-yag-shared-prefs-full]] — yag SharedPreferences: 60+ fields (okToken/pushToken/currentProxyList/pushProxyList/firstLoginTime/isDebugHostRotationEnabled/isIceCandidateEmulationEnabled) #shared-prefs #account #session
- [[topics/225-auth-anonym-login]] — auth.anonymLogin: device_id/version=2/client_version=android_8/client_type=SDK_ANDROID (anonymous call join with device_id) #api #auth #anonymous
- [[topics/226-log-external-log-api]] — log.externalLog: POST+gzip, collector/application/platform/items (base for all OneLog telemetry) #api #telemetry #onelog
- [[topics/227-dns-over-https-endpoint-discovery]] — DoH: dns.google.com/resolve?name=api._endpoint.ok.ru.&type=16 (API endpoint discovery, bypass DNS blocks) #dns #doh #network #api-discovery
- [[topics/228-libtracernative-exports]] — libtracernative.so: 24 exports (tracer_aurora_collect_minidump/tracer_supports_arbitrary_dump/tracer_set_api_endpoint/tracer_disable_upload) #native #apptracer #crash #aurora-os
- [[topics/229-lib-enhancement-exports]] — libEnhancementLibShared: ASR(predict/confidence/punct) + KWS(BCResNetKWS) + Denoise + SpeakerRecognitionEngine+Verifier (voice identification) #native #asr #kws #speaker-recognition #surveillance
- [[topics/230-lib-enhancement-build-info]] — libEnhancementLibShared build: /home/good/mainframer/webrtc4/one-ann-audio-processing + TFLite + Kaldi #native #build-info #infrastructure
- [[topics/231-mytracker-params]] — MyTrackerParams: age/gender/email/phone/okId/vkId/vkConnectId/icqId (cross-platform user ID linking to OK/VK/ICQ) #mytracker #telemetry #user-profile #surveillance
- [[topics/232-mytracker-config-antifraud]] — MyTracker AntiFraudConfig(5 sensors) + InstalledPackagesProvider(installed apps list) + getApkPreinstallParams #mytracker #antiFraud #sensors #installed-apps
- [[topics/233-digital-id-url-params]] — DigitalID URL: &digitalId=<id>&oid=<userId>&photo=<base64> (user photo in GET param) #digital-id #identity #surveillance
- [[topics/234-ri9-shared-prefs]] — ri9 SharedPreferences: phoneNumber/locationCountryCode/gostLicenseCheckEnabled/allowLogSensitiveData/isDisableWebAppSsl/tenorAnonId #shared-prefs #session #debug #gost
- [[topics/235-qp6-additional-pmskeys]] — qp6 additional PmsKeys: gostEnvironmentCheckFlags/isWebRtcLoggingEnabled/isLogVisibleMessagesMetaEnabled/liveStreamsEnabled/callsTrafficMarkers #pmskey #server-control #gost #live-streams
- [[topics/236-webapp-phone-hash]] — WebApp Phone Hash: mini-apps get phone+hash+authDate via JS-bridge (server-controlled via useWebAppPhoneHash PmsKey) #webapp #phone-hash #jsbridge #surveillance
- [[topics/237-calls-signaling-timeouts]] — CallsSignalingTimeouts: enabled/connectTimeout(5s)/initialReconnectDelay(2s)/reconnectDelayScaleFactor/maxReconnectDelay (server-controlled) #calls #signaling #timeouts
- [[topics/238-chats-uploads-db-schemas]] — chats(id/server_id/data BLOB/favourite_index/cid) + uploads(15 fields incl thumbhash_base64) DB schemas #database #chats #uploads
- [[topics/239-informer-banner-db]] — informer_banner DB: 14 fields (click_time/show_time/close_time/show_count — full interaction lifecycle) #database #informer-banner #tracking
- [[topics/240-metrics-db-schema]] — metrics DB: traceId/metricName/spanAndPropertiesDump(BLOB)/attempt (distributed tracing with retry) #database #metrics #tracing #performance
- [[topics/241-tasks-recent-db-schemas]] — tasks(id/type/status/fails_count/data BLOB protobuf) + recent(recent_type/sticker_id/emoji/gif BLOB) DB schemas #database #tasks #recent
- [[topics/242-notifications-tracker-db]] — notifications_tracker_messages: chat_id/message_id/time/fcm/drop_reason (tracks why notifications dropped) #database #notifications #tracking
- [[topics/243-chat-folder-db]] — chat_folder DB: 14 fields (filters/filterSubjects/widgets/templateId/sourceId) #database #chat-folder #filters
- [[topics/244-log-sensitive-server-flag]] — log-sensitive/log-full in LOGIN response: server enables auth token logging + full contacts logging #server-control #logging #sensitive-data #surveillance
- [[topics/245-login-ws-response]] — LOGIN WS response: config/drafts/presence/contacts/messages/profile/updates/time/calls/chats + log-full/log-sensitive #protocol #login #ws #session-init
- [[topics/246-ws-session-fingerprint-extended]] — WS session fingerprint extended: enable-audio/video-messages-transcription + calls-sdk-log-audio + 40+ total fields #protocol #ws #session-fingerprint #transcription
- [[topics/247-user-settings-full]] — UserSettings: 26+ keys (audio.transcription.enabled/app.family.protection.status/app.privacy.phone.number.privacy/app.privacy.inactive.ttl) #user-settings #privacy #transcription #family-protection
- [[topics/248-task-types]] — Task Types: 46 types (TYPE_LOCATION_REQUEST/STOP/TYPE_STAT_CRIT_EVENT/TYPE_SUSPEND_BOT/TYPE_WARM_CHAT_HISTORY) #protocol #tasks #types
