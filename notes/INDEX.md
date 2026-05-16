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
- [[topics/249-msg-send-chat-tasks]] — MsgSend(chatId/messageId/userId/traceId/lastKnownDraftTime) + ChatPersonalConfig(hideNonContactBar) + ChatGroupMark(flagType) #protocol #messages #tasks
- [[topics/250-config-complain-tasks]] — Config(isPushToken/isUserSettings/userSettings Map/syncChatIds) + Complain(ids/reasonId/typeId/details) Tasks #protocol #config #complain #tasks
- [[topics/251-contact-tasks]] — ContactUpdate(contactId/action/newName/oldName/lastName) + ContactVerify(contactId/confirm/localName) Tasks #protocol #contacts #tasks
- [[topics/252-profile-chat-create-tasks]] — Profile(firstName/lastName/description/link/photoId) + ChatCreate(chatType/subjectId/subjectType/startPayload) Tasks #protocol #profile #chat-create #tasks
- [[topics/253-chat-update-members-tasks]] — ChatUpdate(description/photoToken/pinMessageId/theme) + ChatMembersUpdate(userIds/operation/chatMemberType/showHistory) Tasks #protocol #chat-update #tasks
- [[topics/254-crash-log-upload]] — Crash/Log Upload: stackTrace/uploadBean(severity/tags)/threadDump(gzip)/logs(gzip)/drops(json) #crash-report #logs #upload #surveillance
- [[topics/255-protos-additional]] — Protos additional: BotsInfo(suspendedBot) + ChannelInfo(admins[]/signAdmin) + SelfProfile(restrictions Map) + RestrictionsInfo(expiration) #protocol #protos #chat #channel
- [[topics/256-protos-chat-schema]] — Protos.Chat: 40+ fields (messagesTtlSec/lastSearchClickTime/pendingJoinRequestsCount/invitedBy/admins[]/restrictions) #protocol #chat #protos #schema
- [[topics/257-protos-attach-schema]] — Protos.Attach: 17 types + sensitive/sensitiveContentUnlocked/isProcessingOnServer #protocol #attach #protos #sensitive-content
- [[topics/258-protos-message-elements]] — MessageElement(12 types/entityId) + ReactionData(reaction/EMOJI/STICKER) + PendingUploadData(pendingMap) #protocol #message-elements #protos
- [[topics/259-protos-call-history]] — Protos.Call(callType/hangupType/duration/contactIds[]) + CallHistoryState(MissedMessagesItem.ids[]) #protocol #call #protos #call-history
- [[topics/260-protos-contact-schema]] — Protos.Contact: serverPhone/gender/accountStatus/registrationTime/lastSearchClickTime/organizationIds[]/ContactName(CUSTOM/DEVICE/ONEME) #protocol #contact #protos #surveillance
- [[topics/261-protos-control-message]] — Protos.Control: 12 event types (NEW/ADD/REMOVE/LEAVE/JOIN_BY_LINK/PIN/BOT_STARTED) + userId/userIds[]/showHistory #protocol #control-message #protos
- [[topics/262-protos-present-gift]] — Protos.Present(presentId/senderId/receiverId/status) + ConfirmPresent(accept/asPrivate) — gifts/transfers #protocol #present #gift #protos
- [[topics/263-protos-video-schema]] — Protos.Video: videoId/duration/live/transcriptionStatus/thumbhashData + Quality(bitrate/height/width/isOriginal) #protocol #video #protos #transcription
- [[topics/264-protos-audio-schema]] — Protos.Audio: audioId/duration/startTime/transcriptionStatus(5 states)/wave(waveform bytes) #protocol #audio #protos #transcription
- [[topics/265-protos-poll-photo-file]] — Protos.Poll(AnswerStats.userId/timestamp + voterPreviewIds[]) + Photo(photoId/gif/thumbhashData) + File(fileId/size) #protocol #poll #photo #file #protos
- [[topics/266-protos-chat-options]] — Protos.ChatOptions: 13 flags (onlyAdminCanCall/contentLevelChat/joinRequest/membersCanSeePrivateLink/signAdmin) #protocol #chat-options #protos #server-control
- [[topics/267-protos-chat-settings-reactions]] — ChatSettings(dontDisturbUntil/hideLiveLocationPanel/lastNotifMark) + ChatReactionsSettings(count/included/isActive/isFull) #protocol #chat-settings #reactions #live-location
- [[topics/268-protos-group-chat-info]] — GroupChatInfo(groupId/isImportant/isModerator/messagingPermissions DISABLED/MEMBERS/ALL) + GroupOptions(groupPremium) #protocol #group-chat #protos #permissions
- [[topics/269-protos-sticker]] — Protos.Sticker: stickerId/setId/stickerType(STATIC/LIVE/POSTCARD/LOTTIE)/audio + AnimationProperties(fps/framesCount) #protocol #sticker #protos
- [[topics/270-tasks-draft-msgview-assets]] — DraftSave(draft bytes) + MsgView(messageIds[]/registerView — server knows which messages viewed) + AssetsAdd + SyncChatHistory #protocol #tasks #draft #msg-view #surveillance
- [[topics/271-tasks-msg-edit-react]] — MsgEdit(text/oldText/oldAttaches/oldElements — server gets old content) + MsgReact(reaction/reactionType) #protocol #tasks #msg-edit #surveillance
- [[topics/272-tasks-msg-delete]] — MsgDelete: messagesId[]/forMe/complaint(delete+report in one op)/notDeleteMessageFromDb #protocol #tasks #msg-delete #surveillance
- [[topics/273-tasks-video-chat-ops]] — VideoPlay(videoId/place/saveToGallery) + ChatClear/ChatDelete(forAll/lastEventTime) Tasks #protocol #tasks #video-play #chat-ops #surveillance
- [[topics/274-tasks-file-share-pin]] — FileDownloadCmd(fileId/fileName) + MsgSharePreview(messageId/text) + ChatPinSetVisibility(show) Tasks #protocol #tasks #file-download #share-preview
- [[topics/275-tasks-batch-ops]] — ChatMarkBatch(chatIds[]/maxMark) + DeleteChatsBatch(chatIds[]) + ChatsList(chatsSync/marker) + DraftDiscard Tasks #protocol #tasks #batch-ops
- [[topics/276-tasks-fire-time-photo]] — UpdateFireTimeProtoTask(chatId/messageId/fireTime/notifySender) + ChangeProfileOrChatPhoto(file/lastModified/crop) Tasks #protocol #tasks #scheduled-messages #photo
- [[topics/277-tasks-video-warm-history]] — VideoConvert(srcPath/dstPath/startPosition/endPosition/quality) + VideoUpload(file/audio) + WarmChatHistory(chatIds[]) Tasks #protocol #tasks #video #warm-history
- [[topics/278-tasks-photo-file-upload]] — PhotoUpload(file/chatId/crop/profile) + FileUpload(file/originalFile/fileName/fileId/lastUpdatedFile) Tasks #protocol #tasks #file-upload #photo-upload
- [[topics/279-tasks-file-download-prepare]] — FileDownload(fileId/fileName/url/place/notCopyVideoToGallery) + PrepareFileUpload(uri/fileName) Tasks #protocol #tasks #file-download #prepare-upload
- [[topics/280-tasks-callback-delete-range]] — MsgSendCallback(callbackId/payload/buttonPosition row/col) + MsgDeleteRange(chatId/startTime/endTime) Tasks #protocol #tasks #bot #inline-keyboard #surveillance
- [[topics/281-tasks-assets-suspend-bot]] — Assets(Add/Remove/ListModify/Move) + SuspendBot(botId/chatId/suspend) Tasks #protocol #tasks #assets #bots
- [[topics/282-tasks-chat-mark-complain]] — ChatMark(chatId/messageId/mark/setAsUnread/isReadReaction) + ChatComplain(chatId/complaint) Tasks #protocol #tasks #chat-mark #read-status #surveillance
- [[topics/283-tasks-location-reaction-photo]] — LocationStop(chatId/messageId) + MsgCancelReaction(chatId/messageId) + ChangeChatPhoto(chatId/file/crop) Tasks #protocol #tasks #location-stop #cancel-reaction
- [[topics/284-tasks-external-video-congrats]] — ExternalVideoSend(chatId/messageId/externalUrl) + CongratsStatus(userId/holidayId/status) Tasks #protocol #tasks #external-video #congrats
- [[topics/285-tasks-chat-hide-group-present]] — ChatHide(chatId) + ChatGroupMark(chatId/flagType/enabled) + ConfirmPresent(presentId/accept/asPrivate) Tasks #protocol #tasks #chat-hide #present
- [[topics/286-ws-opcodes-additional]] — WS opcodes additional: SESSIONS_INFO/CLOSE + PROFILE_DELETE/TIME + WEB_APP_INIT_DATA + FOLDERS_GET/UPDATE/DELETE + NOTIF_MSG_REACTIONS_CHANGED #protocol #ws #opcodes #sessions #profile-delete #folders
- [[topics/287-web-app-init-data]] — WEB_APP_INIT_DATA: hash/user/query_id/url/trackId/email/chatReactionsSettings/organizations (mini-app gets email+orgs+UserSettings) #webapp #init-data #ws #surveillance
- [[topics/288-ok-token-ws]] — OK_TOKEN WS: token_refresh_ts/token_lifetime_ts (server updates auth token via WS) #protocol #ws #ok-token #auth
- [[topics/289-phone-webapp-share-ws]] — PHONE_WEBAPP_SHARE WS: phone/hash/contact (phone number sent to mini-app via WS) #webapp #phone-share #ws #surveillance
- [[topics/290-notif-message-ws]] — NOTIF_MESSAGE WS: chat_id/message/chat/mark/unread/ttl/url/prevMessageId/invisible #protocol #ws #notif-message
- [[topics/291-notif-mark-attach-ws]] — NOTIF_MARK(chat_id/mark/unread/userId — read receipts) + NOTIF_ATTACH(fileId/audioId/videoId/error) WS #protocol #ws #notif-mark #read-receipts
- [[topics/292-notif-msg-delete-folders-ws]] — NOTIF_MSG_DELETE(chat_id/messageIds[]) + NOTIF_FOLDERS(folders/banners/showTime) WS #protocol #ws #notif-msg-delete #folders
- [[topics/293-notif-delayed-bot-complain-ws]] — NOTIF_MSG_DELAYED(messageIds/lastDelayedUpdateTime) + BOT_INFO(commands/startMessage) + COMPLAIN_REASONS_GET(complains) WS #protocol #ws #bot #complain
- [[topics/294-external-callback-phone-bind-ws]] — EXTERNAL_CALLBACK(botId/startParam) + PHONE_BIND_REQUEST(blockingDuration/codeLength) WS #protocol #ws #external-callback #phone-bind
- [[topics/295-notif-call-start-ws]] — NOTIF_CALL_START WS: conversation_id/rejectedParticipants/internalCallerParams #protocol #ws #notif-call-start #incoming-call
- [[topics/296-ws-opcodes-final]] — WS opcodes final (148-160): AUTH_QR_APPROVE/VOTERS_LIST_BY_ANSWER/TRANSCRIBE_MEDIA/NOTIF_TRANSCRIPTION/MSG_DELIVERY/CHAT_SUGGEST/ORG_INFO/CHAT_LIVESTREAM_INFO #protocol #ws #opcodes #qr-auth #transcription #polls
- [[topics/297-transcribe-media-ws]] — TRANSCRIBE_MEDIA(transcription/status) + NOTIF_TRANSCRIPTION(messageId/chat_id/transcription/mediaId) — server-side transcription #protocol #ws #transcription #surveillance
- [[topics/298-audio-play-send-vote-ws]] — AUDIO_PLAY(opus/mp3/m4a/callName/callerId/messagesReactions) + SEND_VOTE(trackId/chat/state) WS #protocol #ws #audio-play #polls #send-vote
- [[topics/299-voters-org-livestream-ws]] — VOTERS_LIST_BY_ANSWER(voteCount/voters) + ORG_INFO(organizations/email/user) + CHAT_LIVESTREAM_INFO(liveStreams) WS #protocol #ws #polls #org-info #livestream
- [[topics/300-link-info-auth-qr-ws]] — LINK_INFO(stickerSet/startPayload/chat/user/group/message/videoConference/banners) + AUTH_QR_APPROVE(tokenAttrs) WS #protocol #ws #link-info #auth-qr
- [[topics/301-folders-polls-profile-ws]] — FOLDERS_GET(allFilterExcludeFolders/folderSync/folders/foldersOrder) + GET_POLL_UPDATES(polls) + NOTIF_PROFILE(profile) WS #protocol #ws #folders #polls
- [[topics/302-notif-banners-ws]] — NOTIF_BANNERS WS: showTime/banners/updateTime (all required, server controls banner schedule) #protocol #ws #banners #server-control
- [[topics/303-update-message-event]] — UpdateMessageEvent: chatId/messageId/reactionsChanged (NOTIF_MSG_REACTIONS_CHANGED/NOTIF_MSG_YOU_REACTED) #protocol #ws #reactions
- [[topics/304-notif-typing-ws]] — NOTIF_TYPING WS: type(VIDEO_MSG/AUDIO_MSG)/isTyping (server knows if user is recording audio/video) #protocol #ws #notif-typing #surveillance
- [[topics/305-notif-contact-ws]] — NOTIF_CONTACT WS: ContactUpdateAction(ADD/UPDATE/REMOVE/BLOCK/UNBLOCK) #protocol #ws #notif-contact #block
- [[topics/306-notif-contact-sort-ws]] — NOTIF_CONTACT_SORT WS: ids[](contact order)/phones[](phone order) — server syncs contact sort order #protocol #ws #notif-contact-sort #contacts
- [[topics/307-inbound-call-push-telemetry]] — InboundCall PUSH telemetry: p_op/chat_id/call_id/show_source/ttime/dtime(delivery delay)/fcmdtime(FCM delay) #telemetry #push #inbound-call #timing
- [[topics/308-nm4-contact-presence-parser]] — nm4 contact/presence parser: phonebook_id/phone/phone_key/server_phone/email/first_name/last_name + presence(seen/status) #protocol #contacts #presence #surveillance
- [[topics/309-nm4-additional-schemas]] — nm4 additional: chat(id/server_id/data) + video upload(quality/trim) + FCM notification(text/sender/push_id/bmd) #protocol #database #parser #fcm
- [[topics/310-presence-db-schema]] — presence DB: contactServerId/seen(last online time)/status #database #presence #last-seen #surveillance
- [[topics/311-contacts-phones-db]] — contacts(id/server_id/data BLOB) + phones(phonebook_id/phone/phone_key/server_phone/email/first_name/last_name) DB schemas #database #contacts #phones #surveillance
- [[topics/312-stickers-profile-db]] — stickers(16 fields: url/mp4_url/lottie_url/audio/tags) + sticker_sets(name/author_id/link) + profile(BLOB) DB schemas #database #stickers #profile
- [[topics/313-animoji-organizations-db]] — animoji(id/emoji/lottie_url/set_id) + animoji_set(name/icon_lottie_url/animoji_ids) + organizations(+iconUrl) DB schemas #database #animoji #organizations
- [[topics/314-folder-chats-uploads-marks-db]] — folder_and_chats(chatId/folderId) + message_uploads(path/video_quality/trim) + notifications_read_marks(chat_id/mark) DB #database #folder-chats #uploads #notifications
- [[topics/315-comments-complain-reasons-db]] — comments(28 fields + parent_chat_server_id/parent_message_server_id) + complain_reasons(type_id/complain_reasons BLOB) DB #database #comments #complain-reasons
- [[topics/316-battery-snapshot]] — BatterySnapshot: slice/cpuTicks(u/s/cu/cs)/batteryPercent/mobileNet(rx/tx)/wifiNet(rx/tx)/processes #database #battery #cpu #network-stats #surveillance
- [[topics/317-enriched-battery-snapshot]] — EnrichedBatterySnapshot: snapshot(BatterySnapshot)/seqId/visibility(FG/BG) #database #battery #visibility #foreground-background
- [[topics/318-proc-filesystem-reads]] — /proc filesystem reads: /proc/self/stat(CPU)/statm(memory)/cmdline(process name)/fd/(file descriptors) #surveillance #proc-filesystem #cpu #memory
- [[topics/319-traffic-stats-uid]] — TrafficStats.getUidRxBytes/TxBytes(uid) — app traffic by UID for BatterySnapshot #surveillance #network-stats #traffic-stats #uid
- [[topics/320-health-stats-api]] — HealthStats.takeMyUidSnapshot(): mobile(rx/tx/idle)/wifi(rx/tx/idle) + fallback to TrafficStats #surveillance #health-stats #network-stats #uid
- [[topics/321-apptracer-perf-upload]] — Apptracer perf/upload: sdk-api.apptracer.ru/api/perf/upload?crashToken (BatterySnapshot CPU/battery/network samples) #telemetry #apptracer #battery #performance #upload
- [[topics/322-apptracer-sample-upload]] — Apptracer sample/initUpload: sdk-api.apptracer.ru/api/sample/initUpload?sampleToken (arbitrary files: heap dumps/CPU profiles) #telemetry #apptracer #sample-upload #surveillance
- [[topics/323-apptracer-all-endpoints]] — Apptracer all endpoints: api/crash/trackSession(device_id/sessions/drops) + api/perf/upload + api/sample/initUpload+upload #telemetry #apptracer #endpoints #surveillance
- [[topics/324-disk-usage-worker]] — DiskUsageWorker: int_data(applicationInfo.dataDir)/ext_data(externalFilesDir)/total_size (recursive file list) #telemetry #apptracer #disk-usage #surveillance
- [[topics/325-apptracer-sdk-components]] — Apptracer SDK: CrashReport+HeapDump+PerformanceMetrics+DiskUsage+SamplingProfiler+NativeBridge+SampleUpload #telemetry #apptracer #heap-dump #cpu-profiler #surveillance
- [[topics/326-native-bridge-tracer]] — NativeBridge: nativeInstallBridge(qb5.Z) JNI bridge to libtracernative (log/setKey callbacks) #telemetry #apptracer #native-bridge #jni
- [[topics/327-mytracker-endpoints]] — MyTracker endpoints: tracker-api.vk-analytics.ru/v3/ + ip4. + ts./mobile/v1 + mlapi. + beta-ml. #telemetry #mytracker #endpoints #vk-analytics
- [[topics/328-mytracker-init]] — MyTracker init: initTracker('34982109644049932883') + setCustomUserId(userId) + setKidMode(false) #telemetry #mytracker #init #tracker-id #user-id
- [[topics/329-mytracker-advertising-ids]] — MyTracker: Google GAID + Huawei OAID + android_id + mac #telemetry #mytracker #advertising-id #gaid #huawei #surveillance
- [[topics/330-mytracker-device-params]] — MyTracker DeviceParams: Build.DEVICE/MANUFACTURER/MODEL/VERSION + screen + timezone + freeSpace + currentModeType #telemetry #mytracker #device-params #surveillance
- [[topics/331-mytracker-user-lifecycle]] — MyTrackerUserLifecycle: trackLoginEvent(userId)/trackRegistrationEvent(userId)/trackInviteEvent() #telemetry #mytracker #user-lifecycle #login #registration
- [[topics/332-mytracker-activity-lifecycle]] — MyTracker ActivityLifecycle: onActivityStarted/Stopped(timePoint) for session tracking #telemetry #mytracker #activity-lifecycle #session
- [[topics/333-mytracker-install-referrer]] — MyTracker Install Referrer: installReferrer/installBeginTimestamp/referrerClickTimestamp/installer (Google+Huawei) #telemetry #mytracker #install-referrer #attribution
- [[topics/334-mytracker-event-types]] — MyTracker event types: session(3L/11)/launch(2L/13)/referrer(1L/12)/install_referrer(5L/17)/custom(33L/41) #telemetry #mytracker #event-types #session
- [[topics/335-mytracker-external-proto]] — MyTracker ExternalProtoWriters: REMOTE_CONFIG_STRING/LOCATION_INFO/NETWORK_INFO/CELL_AND_WIFI_INFO #telemetry #mytracker #location #network #cell-info #surveillance
- [[topics/336-ilb-device-data]] — ilb.java device data: board/brand/cpuABI/cpuCount/networkOperatorName/installer #telemetry #device-info #cpu #network-operator #surveillance
- [[topics/337-crash-report-device-info]] — Crash report device info: environment/buildUuid/sessionUuid/device_id/inBackground/connection/isRooted #telemetry #crash-report #device-info #rooted #surveillance
- [[topics/338-root-detection-impl]] — Root detection: Build.PRODUCT(sdk)/Build.TAGS(test-keys)/Superuser.apk//system/xbin/su #security #root-detection #emulator #surveillance
- [[topics/339-minidump-crash-handler]] — Minidump: installMinidumpWriterImpl(getCacheDir/tracer/minidump) via libtracernative #telemetry #apptracer #minidump #crash #native
- [[topics/340-crash-report-exit-info]] — CrashReportInitializer: ApplicationExitInfo(getReason/getProcessName/getTimestamp/getTraceInputStream ANR trace) #telemetry #apptracer #crash-report #anr
- [[topics/341-apptracer-crash-types]] — Apptracer crash types: CRASH/NON_FATAL/FATAL/ERROR/WARNING/NOTICE/INFO/DEBUG/MINIDUMP/ANR (getCacheDir/tracer/crashes) #telemetry #apptracer #crash-types #anr #minidump
- [[topics/342-pmskey-additional-new]] — PmsKey additional: mytracker-enabled/tracer-non-fatal-crashed-enabled/wm-*/calc-audio-wave/battery-slice-interval/ymap/views-count-enabled #pmskey #server-control #mytracker #tracer
- [[topics/343-rtd-server-flags]] — rtd SharedPreferences: 40+ flags (userLogReportChatId/isMytrackerEnabled/statSessionBackgroundThreshold/retryTranscriptionAttempt) #pmskey #server-control #rtd #mytracker
- [[topics/344-pmskey-debug-special]] — PmsKey debug/special: user-debug-report/fake-chats/fake-in-app-review/blocked-users/anr-config/watchdog-config #pmskey #server-control #debug #surveillance
- [[topics/345-exif-gps-metadata]] — EXIF GPS: GPSLatitude/GPSLongitude/GPSAltitude/GPSTimeStamp + CameraOwnerName/BodySerialNumber/LensSerialNumber #surveillance #exif #gps #photo-metadata #location
- [[topics/346-fused-location-provider]] — Fused Location: latitude/longitude/altitude/accuracy/bearing/speed → sj9(LocationData) #surveillance #location #gps #fused-location
- [[topics/347-firebase-messaging-service]] — Firebase Messaging: RECEIVE/NEW_TOKEN + google.message_id/google.product_id #telemetry #firebase #fcm #push
- [[topics/348-firebase-cct-transport]] — Firebase CctTransportBackend: POST+gzip+X-Goog-Api-Key+datatransport/3.1.9 (dynamic URL) #telemetry #firebase #cct-transport #google-api-key
- [[topics/349-firebase-installations-api]] — Firebase Installations: https://firebaseinstallations.googleapis.com/v1/ (FID) #telemetry #firebase #installations #googleapis
- [[topics/350-firebase-installations-data]] — Firebase Installations data: fid/appId/authVersion(FIS_v2)/sdkVersion(a:18.0.0) → Google #telemetry #firebase #fid #app-id #surveillance
- [[topics/351-firebase-config-keys]] — Firebase config: google_api_key=AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q + google_app_id=1:659634599081:android:... + project_id=max-messenger-app #telemetry #firebase #config #api-key
- [[topics/352-apptracer-app-token]] — Apptracer app token: tracer_app_token=t6QnlHov0Gq1UBGYG9GPqZu0EiVMZ922FKvwyAEASa90 (crashToken for all Apptracer API) #telemetry #apptracer #token #crash-token
- [[topics/353-strings-additional-config]] — strings.xml: tracer_mapping_uuid/legal.max.ru/pp+ps/tt_contact_mimetype #config #strings #legal #apptracer
- [[topics/354-yandex-maps-config]] — y-map PmsKey: Yandex Maps config (tile/geocoder/static) + apikey from server + geocode-maps.yandex.ru endpoint #server-control #yandex-maps #api-key #geocoder
- [[topics/355-deeplink-routes-full]] — Deeplink routes: :auth/:share-self-out/joincall/<id>/join/<id>/stickerset/<id>/c/<chatId>/<msgId>/:folder/:current #deeplinks #routing #max-ru
- [[topics/356-api-oneme-ru-servers]] — api.oneme.ru servers: prod + api-test.oneme.ru + api-tg.oneme.ru + api-test2.oneme.ru #api #endpoints #oneme #test-servers
- [[topics/357-host-reachability-full]] — HostReachability: gstatic.com/mtalk.googl.com/calls.okcdn.ru/gosuslugi.ru/pushtrs.push.hicloud.com (int-encoded) #network #host-reachability #gosuslugi #huawei #calls
- [[topics/358-api-ok-ru-uri]] — api.ok.ru URI: https://api.ok.ru + ok://api/api/<method> scheme (calls SDK uses OK.ru API) #api #ok-ru #calls-sdk #uri-scheme
- [[topics/359-z8f-decoded-strings]] — z8f decoded: IP detection (api.ipify.org/checkip.amazonaws.com/ifconfig.me/ip.mail.ru/yandex.net) + trace-flow.ru #network #ip-detection #obfuscated-strings #surveillance
- [[topics/360-z8f-decoded-additional]] — z8f decoded additional: DPS files(dps_config.bin/dps_dont_report.bin/dps_log_entries.bin) + HostReachability JSON(appVersion/clientTs/deviceId/hosts/ip/operator/uid/vpn) #network #dps #host-reachability
- [[topics/361-host-reachability-bzk-config]] — HostReachability bzk: reportHosts=[trace-flow.ru] + selfIpDomains=[yandex/ifconfig.me/ipify/aws/mail.ru] + timeout=10000 #network #host-reachability #trace-flow #ip-detection
- [[topics/362-z8f-dps-metadata]] — z8f DPS metadata: ru.trace_flow.dps.API_KEY/CLIENT_VERSION/USER_ID + HostReachability config keys #network #dps #mytracker #metadata
- [[topics/363-google-maps-api-key]] — Google Maps API key: com.google.android.geo.API_KEY=AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI #config #google-maps #api-key #manifest
- [[topics/364-manifest-metadata-full]] — AndroidManifest meta-data: 7 Apptracer initializers + Google Maps API key + Facebook SoLoader disabled + STAMP_TYPE_DISTRIBUTION_APK #config #manifest #apptracer #facebook
- [[topics/365-facebook-fresco]] — Facebook Fresco: GIF/WebP animations + 5 threads + SoLoader + FrescoHttpDownloadException + stat.fresco #facebook #fresco #gif #webp #image-loading
- [[topics/366-zstd-util]] — ZstdUtil: nativeDecompress(bytes) only (server compresses, client decompresses) #compression #zstd #native
- [[topics/367-link-interceptor-mytracker]] — LinkInterceptorActivity: URI → MyTracker.handleDeeplink(intent) — all deeplinks tracked by MyTracker #deeplinks #mytracker #attribution #surveillance
- [[topics/368-webapp-vpn-exception]] — WebAppHttpClient.WebAppHasVpnException: VPN detected in WebApp HTTP client (mini-apps blocked with VPN) #webapp #vpn-detection #surveillance
- [[topics/369-download-file-from-webapp]] — DownloadFileFromWebAppWorker: fileName/fileUrl ForegroundWorker for mini-app file downloads #webapp #file-download #worker
- [[topics/370-tamtam-api-exceptions]] — tamtam.api exceptions: NonceException/SessionSendLimitException/UnknownOpcodeException/MaxRetryCountExceededException #protocol #api #exceptions #nonce
- [[topics/371-media-projection-service]] — MediaProjectionService: startForeground(12) screen capture foreground service in calls #screen-capture #media-projection #foreground-service #calls
- [[topics/372-notification-service-boot-receiver]] — NotificationTamService: directReply/pushId/eventKey + BootCompletedReceiver: BOOT_COMPLETED → BackgroundWake #notifications #service #direct-reply #push
- [[topics/373-onelog-telemetry]] — OneLog: log.externalLog, collector/operation/uid/network/type/count/datum/group/custom, application=packageName:versionCode:versionName #onelog #telemetry #logging #surveillance
- [[topics/374-compression-lz4-zstd]] — LZ4 + Zstd native decompression — both client-side only, server compresses #compression #lz4 #zstd #native
- [[topics/375-call-analytics-sender]] — CallAnalyticsSender: send(event)/setIdle/forceSend, DEFAULT_DISABLE_UPLOAD_IN_CALL=true, sdk_type/sdk_version #calls-analytics #telemetry #externcalls #surveillance
- [[topics/376-asr-online-manager]] — AsrOnlineManager: server-side ASR, auto-enables on server topology, AsrOnlineChunk(participantId, text) #asr #asr-online #server-asr #calls #surveillance #speech-recognition
- [[topics/377-record-manager]] — RecordManager: record-start/stop, privacy=PUBLIC default, isStream, movieId on server #record #calls #surveillance #stream
- [[topics/378-p2prelay-sessionroom]] — P2PRelaySwitchTrigger (RTT threshold from server) + SessionRoomsManager (assignParticipants/moveParticipant/requestAttention) #p2prelay #sessionroom #calls #admin
- [[topics/379-watch-together]] — WatchTogether: play/pause/resume/stop/setPosition, MovieState(participantId,position,isPlaying), WebTransport #watch-together #calls #video #surveillance
- [[topics/380-ml-features-manager]] — MLFeaturesManager: KWS(ws_0)+NS(ns_1), server-controlled url/checksum/enabled, android.mlfeatures.* remote keys #ml #kws #ns #tflite #model-download #server-control #surveillance
- [[topics/381-waiting-room-urlsharing-feedback-stereo]] — WaitingRoom(get-waiting-hall) + UrlSharing(url+initiatorId) + Feedback(GESTURES source) + StereoRoom(promoteParticipant/grantAdmin) #waiting-room #urlsharing #feedback #stereo #calls #surveillance
- [[topics/382-conversation-stats]] — ConversationStats: 18 components, bad_call_detected_by_audio_spotter(confidence), call_finish(reason,rate_reasons), ml_ready_to_use #calls-stat #analytics #telemetry #kws #surveillance
- [[topics/383-externcalls-sdk-net]] — externcalls.sdk.net: DownloadService(HTTP+MD5), NetworkStat(rttMs,audioLoss,videoLoss,activeCandidateType), StatMonitor #calls-net #download #network-stat #rtt
- [[topics/384-externcalls-sdk-config]] — externcalls.sdk.config: RemoteSettings(android.wordspotter.config/dump.bitrate/p2prelay), ConversationParams(isP2PForbidden/ispAsNo/locCc) #calls-config #remote-settings #server-control #surveillance
- [[topics/385-mytracker-details]] — MyTracker: 10 modules, age/gender/email/phone/okId/vkId/icqId, AntiFraudConfig(all sensors), installedPackagesProvider #mytracker #telemetry #surveillance #anti-fraud #sensors #user-data
- [[topics/386-shortcut-badger]] — ShortcutBadger: badge count on launcher icon, 13 launchers (Samsung/Huawei/Xiaomi/OPPO/Vivo/...) #badge #notification-count #launcher
- [[topics/387-final-summary]] — ИТОГОВЫЙ СВОДНЫЙ АНАЛИЗ: 386 тем, все ключевые находки по 12 категориям #summary #comprehensive #backdoor #surveillance #telemetry
- [[topics/388-ok-api-service-internal]] — OkApiServiceInternal: 13 API methods, sendSupportedCodecsStatistics, getExternalIdsByOkIds (deanon), requestUploadUrl #calls-api #vchat #batch #surveillance
- [[topics/389-vchat-api-requests]] — vchat API: startConversation(capabilities/hex), joinConversation(p2pForbidden), clientSupportedCodecs(fingerprinting), hangup(reason) #calls-api #vchat #capabilities #fingerprinting
- [[topics/390-client-capabilities]] — ClientCapabilities: 15 bits hex-encoded, SCREEN_TRACK_PRODUCER/ADMIN_MUTE_NOTIFY/USE_P2P_RELAY, sent on every call #capabilities #calls #fingerprinting #surveillance
- [[topics/391-conversation-events-listener]] — ConversationEventsListener: 40+ callbacks, onParticipantsDeAnonymized/onMicrophoneForciblyMuted/onCustomData(JSON)/onOpponentFingerprintChanged #calls-events #surveillance #fingerprint #deanon
- [[topics/392-conversation-participant]] — ConversationParticipant: capabilities/networkStatus/isTalking/deAnonymize, getAcceptedCallClientType/Platform #calls-participant #surveillance #deanon #network-status
- [[topics/393-rate-manager]] — RateManager: RateHint(reason)/RateCallData(questions from server)/RateManagerConfig(RTT/loss/candidateType thresholds) #calls-rate #quality #surveillance
- [[topics/394-audio-sdk-kws-mic-proximity]] — Audio SDK: KeywordSpotterManager(filePath/turnOffInMs), MicrophoneManager(registerAudioSampleCallback), ProximityTracker #audio #kws #microphone #surveillance
- [[topics/395-keyword-spotter-impl]] — KeywordSpotterManagerImpl: android.wordspotter.config(turn_off_in_ms), setKeywordSpotterParams→nativeKWS→bad_call_detected_by_audio_spotter #kws #wordspotter #server-control #surveillance
- [[topics/396-noise-suppression-impl]] — NoiseSuppressionManagerImpl: serversideBasic/serversideAnn (audio to server), clientsideAnn(filePath from server) #noise-suppression #ns #audio #server-control #surveillance
- [[topics/397-video-sdk-screen-camera]] — Video SDK: ScreenCapture(setAudioCaptureEnabled), CameraManager, DisplayLayoutSender(server knows video layout) #video #screen-capture #camera #surveillance
- [[topics/398-media-mute-manager]] — MediaMuteManager: updateMediaOptionsForParticipant/All (force mute), requestToEnableMediaForParticipant/All #media-mute #calls #admin #force-mute #surveillance
- [[topics/399-asr-manager-record]] — AsrManager: asr-start{fileName}/asr-stop, AsrInfo(initiator,movieId), separate from AsrOnlineManager #asr #asr-record #calls #surveillance
- [[topics/400-chat-manager-contacts]] — ChatManager(InboundMessage/OutboundMessage/isDirect) + ContactCallManager(getIAmAnonymous/getIWasInitiallyAnonymous) #calls-chat #messages #surveillance
- [[topics/401-media-dump-manager]] — MediaDumpManager: requestMediaDump(collect-debug-dump via signaling), 6 audio sources, IN PRODUCTION BUILD #media-dump #debug #audio-dump #surveillance #server-control
- [[topics/402-debug-manager]] — DebugManager: enableFullAudioDump/registerStatListener/setVideoSettingsOverride, IN PRODUCTION BUILD #debug #calls #audio-dump #surveillance
- [[topics/403-conversation-feature-manager]] — ConversationFeatureManager: 4 features (ADD_PARTICIPANT/RECORD/MOVIE_SHARE/ASR_RECORD), enableFeatureForRoles via signaling #calls-feature #roles #surveillance #server-control
- [[topics/404-advertising-id-client]] — AdvertisingIdClient: GAID collection, isLimitAdTrackingEnabled, logs limit_ad_tracking/ad_id_size/time_spent #gaid #advertising-id #tracking #surveillance
- [[topics/405-install-referrer]] — InstallReferrer: installReferrer/installBeginTimestamp/referrerClickTimestamp, attribution to MyTracker #install-referrer #attribution #tracking #surveillance
- [[topics/406-mytracker-user-lifecycle]] — MyTrackerUserLifecycle: trackLoginEvent/trackRegistrationEvent/trackInviteEvent + MultipleInstallReceiver(INSTALL_REFERRER) #mytracker #user-lifecycle #login-tracking #surveillance
- [[topics/407-share-data]] — ShareData: 9 types (TEXT/IMAGES/VIDEOS/FILES/VCARD/MESSAGES/CONTACT/STICKER_SET), vcard field #share #data-types #vcard #contact
- [[topics/408-pmskey-debug-logging-transcription]] — PmsKey: enable-audio/video-messages-transcription, log-sensitive/messages-meta/chat-meta, calls-fakeboss-incoming-call-enabled, battery-slice-interval #pmskey #server-control #debug #logging #transcription #surveillance
- [[topics/409-audio-video-transcription]] — Audio/Video message transcription: Protos.Audio/Video.transcription, audio.transcription.enabled=true by default #transcription #audio-messages #video-messages #asr #surveillance
- [[topics/410-user-settings-map]] — UserSettings: audioTranscriptionEnabled/hiddenOnline/dontDustirbUntil/safeMode/inactiveTTL, synced with server #user-settings #transcription #server-control #surveillance
- [[topics/411-user-settings-full]] — UserSettings(sgj): 27 fields, hiddenOnline/phoneNumberPrivacy/safeMode/safeModeNoPin/audioTranscriptionEnabled/contentLevelAccess/familyProtection #user-settings #server-control #surveillance #safe-mode #family-protection
- [[topics/412-protos-location]] — Protos.Location: latitude/longitude/altitude/accuracy/bearing/livePeriod/deviceId + Protos.LocationInfo(speed) #location #gps #geolocation #surveillance #live-location
- [[topics/413-conversation-interface]] — Conversation interface: 23 managers, sendData(JSON)/muteAll/grantRoles/removeParticipant(ban)/queryChatHistory #conversation #calls-sdk #interface #surveillance #admin
- [[topics/414-conversation-factory]] — ConversationFactory: SDK v0.1.13, call/answer/join/joinAnonByLink/createConfRoom, setEmulatedNegotiationErrorType/setBackendRenderVmoji #conversation-factory #calls-sdk #sdk-version #surveillance
- [[topics/415-dps-client-zu5]] — zu5 (DPS client): getNetworkOperator()+getNetworkOperatorName() on every send, deviceIdSupplier/userIdSupplier, 4 threads #dps #apptracer #telephony #network-operator #surveillance
- [[topics/416-device-info-vm5-cgj]] — vm5+cgj: android_id(Settings.Secure), UserAgent(appVersion/osVersion/locale/deviceName/screen/timeZone/SUPPORTED_ABIS) #device-info #user-agent #android-id #fingerprint #surveillance
- [[topics/417-mytracker-gaid-oaid]] — MyTracker GAID+OAID: GoogleAdInfoDataProvider(GAID) + HuaweiAdInfoDataProvider(OAID), isLimitAdTrackingEnabled #mytracker #gaid #oaid #huawei #google #surveillance
- [[topics/418-mytracker-installed-packages]] — MyTracker AppsDataProvider: non-system apps (packageName+firstInstallTime), appsHash change detection #mytracker #installed-packages #apps-list #surveillance #fingerprint
- [[topics/419-webapp-js-events]] — WebApp JS events (34): WebAppBiometryRequestAuth/WebAppNfcEmulateNfcTag/WebAppSecureStorageSaveKey/WebAppVerifyMobileId/WebAppUrlInterceptor #webapp #jsbridge #js-events #biometry #nfc #storage #surveillance
- [[topics/420-webapp-storage-biometry]] — WebApp Storage(SecureStorage 128/4000B + DeviceStorage) + Biometry(RequestAuth/RequestAccess/UpdateToken) #webapp #secure-storage #device-storage #biometry #jsbridge #surveillance
- [[topics/421-webapp-request-phone]] — WebAppRequestPhone: response(requestId, phone, hash, authDate), mini-app gets phone number with verification hash #webapp #phone-request #msisdn #jsbridge #surveillance
- [[topics/422-webapp-screen-capture-qr]] — WebApp ScreenCaptureBehavior(FLAG_SECURE) + CodeReader(QR scan → value) #webapp #screen-capture #qr-code #jsbridge #surveillance
- [[topics/423-webapp-share-haptic]] — WebApp Share(url/title/text) + Haptic(IMPACT/NOTIFICATION/SELECTION) + CodeReader(requestId/value) #webapp #share #haptic #jsbridge
- [[topics/424-webapp-nfc-service]] — WebAppNfcService: HostApduService, processCommandApdu(apdu), logs all APDU in hex, mini-app emulates NFC card #webapp #nfc #hce #host-apdu #jsbridge #surveillance
- [[topics/425-webapp-ui-control]] — WebApp UI Control: ClosingBehavior(needConfirmation)/BackButton(isVisible)/OpenLink(url)/OpenMaxLink/ChangeScreenBrightness(maxBrightness) #webapp #jsbridge #ui-control #open-link #brightness
- [[topics/426-webapp-biometry-db]] — webapp_biometry DB: (id,user_id,bot_id,token,access_requested,access_granted), biometric token stored locally #webapp #biometry #database #token #surveillance
- [[topics/427-db-additional-tables]] — DB additional tables: battery(sliceTime/utime/stime/batteryCapacity/instantAmperage), metrics(traceId/spanAndPropertiesDump), fcm_notifications_analytics, stat_events #database #battery #metrics #fcm #analytics #surveillance
- [[topics/428-battery-cpu-collector]] — vw0 battery+CPU collector: utime/stime/batteryCapacity/instantAmperage + network(mrx/mtx/wrx/wtx), interval=battery-slice-interval #battery #cpu #network-stats #surveillance #telemetry
- [[topics/429-fcm-notifications-db]] — fcm_notifications DB: text/sender/push_id/event_key + fcm_notifications_analytics(fcm_sent_time/received_time/analytics_status) #database #fcm #notifications #analytics #push #surveillance
- [[topics/430-db-presence-phones-messages]] — DB: presence(seen/status) + phones(phone/phone_key/server_phone/email/first_name) + messages(text in plaintext SQLite) #database #presence #phones #contacts #messages #surveillance
- [[topics/431-db-organizations-informer-tasks]] — DB: organizations(parentId/folderTemplateId) + informer_banner(url/click_time/show_time) + notifications_tracker(drop_reason) + tasks(fails_count) #database #organizations #informer-banner #tasks #surveillance
- [[topics/432-db-uploads-video]] — DB uploads(upload_url/thumbhash_base64/desired_uploader) + draft_uploads(video_quality/trim) + video_conversions #database #uploads #video #draft #surveillance
- [[topics/433-devmenu-details]] — DevMenu: ServerHostBottomSheet(Адрес сервера)/ServerPortBottomSheet, FeatureToggles(SERVER source), LogsViewer, TestCrash, IN PRODUCTION #devmenu #debug #server-switch #logs-viewer #feature-toggles #surveillance
- [[topics/434-change-media-settings]] — change-media-settings: isVideoEnabled/isAudioEnabled/isScreenSharingEnabled/isAnimojiEnabled/isAudioSharingEnabled #calls #media-settings #signaling #audio #video #screen-share #surveillance
- [[topics/435-webrtc-experiments]] — ev1 WebRTC Experiments: 30+ flags, isAudioCaptureLoggingEnabled/bitrateDumpGatheringState/isBackendRenderVmojiEnabled/isCorruptWsEndpointEnabled #calls #experiments #webrtc #server-control #surveillance
- [[topics/436-webapp-contact-data]] — WebAppContactData: displayName/avatarUrl/abbreviationModel, mini-app gets user profile data #webapp #contact-data #jsbridge #surveillance
- [[topics/437-twofa-screens]] — TwoFA: Settings/Creation/Onboarding/CheckPass/StartRestore/ProfileDeletion, twofa_settings_track_id_key #twofa #2fa #two-factor-auth #security #settings
- [[topics/438-safe-mode]] — SafeMode: hide profile/calls from contacts only/chats with known only/content filter, safeMode/safeModeNoPin in UserSettings #safe-mode #privacy #content-filter #settings #surveillance
- [[topics/439-qr-auth]] — QR Auth: SettingsDevicesScreen + QrAuthHintBottomSheet + :qr-scanner?mode=2, AuthQrUseCase/GetQrCodeUseCase #qr-auth #devices #session #security #settings
- [[topics/440-vpn-warning-bottomsheet]] — VpnConnectedWarningBottomSheet: "Отключите VPN" / "Чтобы пользоваться MAX" #vpn #warning #surveillance #server-control
- [[topics/441-neuro-avatars]] — NeuroAvatarsScreen: AI avatar generation from photo at registration, photo sent to server #neuro-avatars #ai #registration #login #surveillance
- [[topics/442-background-listen-service]] — BackgroundListenService: startForeground(9001) 'MAX работает в фоне', system_curtain_shown/hidden, BackgroundWakeBootReceiver #background-wake #foreground-service #boot-receiver #surveillance
- [[topics/443-calls-sdk-initializer]] — CallsSdkInitializer: calculateMeta(SHA-256 of .so files) + initializeSessionSeed(seed, deviceId) native #calls-sdk #initialization #sha256 #native #integrity
- [[topics/444-call-admin-settings]] — CallAdminSettingsScreen: ADMIN_CALL_SETTINGS{screen/camera/microphone/screenshare/recording/waiting} + CallDebugMenuScreen in production #calls #admin-settings #debug-menu #surveillance
- [[topics/445-call-rate-record-vpn]] — CallRateBottomSheet(call_id/is_group_call/sdk_reasons) + StartRecordBottomSheet(name max 250) + VpnPanelWidget #calls #rate-call #record #vpn-panel #surveillance
- [[topics/446-call-service-impl]] — CallServiceImpl: ConnectionService, WakeLock(max:calls_prx), incomingCalls/activeCalls notifications, onTaskRemoved #calls #connection-service #telecom #foreground-service #surveillance
- [[topics/447-rkn-bottomsheet]] — RknBottomSheet: "Это зарегистрированный канал" / "Он есть в перечне Роскомнадзора" #rkn #roskomnadzor #channel #compliance #surveillance
- [[topics/448-warning-link-bottomsheet]] — WarningLinkBottomSheet: "Перейти по ссылке?" + URL + analytics(a(1,1)/a(1,2)) #warning-link #messages #safety #surveillance
- [[topics/449-file-download-warning]] — FileDownloadWarningBottomSheet: chat_id/message_id/attach_id/file_id/file_name/file_url/file_size #file-download #warning #messages #surveillance
- [[topics/450-fake-inapp-review]] — FakeInAppReviewBottomSheet: own rating dialog (NOT Google Play In-App Review), ratings collected directly by MAX #in-app-review #rating #fake #google-play #surveillance
- [[topics/451-force-update-screen]] — ForceUpdateScreen: "Ваша версия MAX устарела" / "Обновить", killswitch full app block #force-update #killswitch #server-control #surveillance
- [[topics/452-account-initializer]] — AccountInitializer: 13 parallel tasks, SslIntegrity+DPS(HIGH)+BackgroundWakeFeatureInit, getUserId()+android_id #initialization #startup #account #dps #ssl-integrity #surveillance
- [[topics/453-location-screens]] — PickLocationScreen + ShowLocationScreen: Google Maps, geolocation_send_click(source_id/source_type), senderId/msgId #location #google-maps #pick-location #show-location #surveillance #analytics
- [[topics/454-proc-files]] — /proc files: /proc/self/stat(PID/comm/utime/stime), /proc/self + /data/data/ru.oneme.app monitoring #proc-files #cpu-stats #battery #surveillance #telemetry
- [[topics/455-dps-http-client-kyk]] — kyk DPS HTTP client: TrafficStats.setThreadStatsTag, {snapshots:[{id,...}]}, timeoutMs/dontReportUntil #traffic-stats #network #apptracer #dps #surveillance #telemetry
- [[topics/456-ssl-certificate-pinning]] — SSL certificate pinning: SHA-256 fingerprint, InvalidSslIntegrityException/InvalidSslSessionException, SslIntegrity task at startup #ssl #certificate-pinning #tls #security #okhttp
- [[topics/457-root-network-detection]] — r04.G() root detection (su/Superuser.apk/test-keys) + r04.z() network type (NONE/WIFI/ETHERNET/BLUETOOTH/VPN/CELLULAR) #root-detection #network-type #emulator-detection #surveillance #security
- [[topics/458-network-operator-collection]] — f58: connection_type/vpn + getNetworkOperator():getNetworkOperatorName() in every API request #network-operator #telephony #vpn-detection #connection-type #surveillance #telemetry
- [[topics/459-device-data-yk2]] — yk2: Build.FINGERPRINT + mcc_mnc(getSimOperator()) + tz-offset + sdk-version + net-type/mobile-subtype #sim-operator #mcc-mnc #fingerprint #timezone #network-type #surveillance #telemetry
- [[topics/460-yag-shared-prefs]] — yag SharedPreferences: 50+ keys, device.id/user.fcmToken/user.okToken/server.timeDelta/app.first.login.time/user.callSession #shared-prefs #user-prefs #device-id #fcm-token #proxy #server-time #surveillance
- [[topics/461-login-response-proxy]] — isg LOGIN response: proxy/proxydomains from server, LOGIN request: deviceId/pushDeviceType/buildNumber/timezone #proxy #pmskey #login-response #server-control #surveillance
- [[topics/462-notif-config-logic]] — NotifConfigLogic: Step2(proxy/proxydomains) + Step3(debug-mode/user-debug-report/safeMode) + Step5(chats settings) #proxy #pmskey #notif-config #server-control #surveillance #debug
- [[topics/463-qp6-rtd-pmskey-full]] — qp6 RTD: 50+ PmsKey, enableAudioTranscription/callssdklogaudio/logmessagesmeta/logchatmeta/callsfakeboss/batterysliceinterval #pmskey #server-control #rtd #calls #transcription #surveillance #debug
- [[topics/464-watchdog-config]] — WatchdogConfig: stuckThreshold/hangThreshold/saveStacktrace from server + keepBackgroundSocket PmsKey #watchdog #server-control #surveillance #crash-detection #stack-trace
- [[topics/465-devnull-server-config]] — DevNullServerConfig: isOpcodeStatEnabled/isMemoryStatEnabled/isBatteryStatEnabled/isChatHistoryStatEnabled/isUploadHangCheckEnabled #devnull-server-config #server-control #surveillance #telemetry #opcode-stat #memory-stat #battery-stat
- [[topics/466-memory-stat]] — woa Memory Statistics: Debug.MemoryInfo java-heap/native-heap/code/stack/graphics/total-swap/total-pss #memory-stat #heap #surveillance #telemetry #server-control
- [[topics/467-telemetry-event-types]] — jm5 telemetry event types: STARTUP_REPORT/AB_EVENT/OPCODE/CHAT_HISTORY_WARM/WEB_APP/UPLOAD_HANG/MEMORY/BATTERY/BAD_PUSHES #telemetry #event-types #server-control #surveillance #opcode-stat #memory-stat #battery-stat
- [[topics/468-startup-report]] — startup_report: crash/exit_reason/bucket(UsageStatsManager)/memory/class(PerfClass)/isIgnoringBatteryOptimizations/isBackgroundRestricted #startup-report #telemetry #surveillance #crash #memory #battery #exit-reason
- [[topics/469-upload-hang-checker]] — l0d OneVideoUploadOperation: upload_hang checker, VIDEO/VIDEO_MESSAGE/AUDIO types #upload-hang #telemetry #surveillance #file-upload #video-upload
- [[topics/470-km5-telemetry-sender]] — km5 Telemetry Sender: value1..16 + valueStr1..6, ok9.h('DEV', eventType, data), bloom filter dedup #telemetry #km5 #event-sender #surveillance #server-control
- [[topics/471-log-controller]] — ok9 LogController: ap(timestamp, userId, deviceId, type, event, params), retry 3x, LOG_DISCONNECTION_BLOCKER #log-controller #telemetry #surveillance #event-sender #critical-event
- [[topics/472-log-controller-event-types]] — ok9.h event types: AUDIO_TRANSCRIPTION/PERMISSION/HOST_REACHABILITY/PUSH.InboundCall/SETTINGS.MINIAPP_BIOMETRY/PRESENCE #log-controller #event-types #telemetry #surveillance #audio-transcription #push #permission #background
- [[topics/473-audio-transcription-analytics]] — tvi AUDIO_TRANSCRIPTION.transcription_result: message_id/media_id/message_type/result_type/duration/waiting_time/source_id/source_type #audio-transcription #telemetry #surveillance #message-transcription
- [[topics/474-permission-tracking]] — Permission Tracking: PERMISSION.permission_changed_state{pType,pStatus}, 13 permission groups, CAMERA+RECORD_AUDIO+READ_PHONE_STATE #permissions #surveillance #telemetry #contacts #camera #microphone #location #storage #notification
- [[topics/475-host-reachability-analytics]] — HOST_REACHABILITY.GET_HOST_REACHABILITY: {hosts, operator, connection_type, ip, vpn} after every reachability check #host-reachability #telemetry #surveillance #network #vpn #operator
- [[topics/476-push-inbound-call-analytics]] — PUSH.InboundCall: {trid,eKey,suid,ttime,dtime,fcmdtime,p_op=show,chat_id,call_id,show_source} #push #inbound-call #telemetry #surveillance #fcm #call-analytics
- [[topics/477-settings-analytics]] — SETTINGS analytics: MINIAPP_BIOMETRY{paramValue,webappId}/BACKGROUND/THEME/TEXT_SIZE #settings #telemetry #surveillance #biometry #theme #background #text-size
- [[topics/478-presence-analytics]] — PRESENCE.EVENT_MESSAGE_COUNTER: 8 counters online/offline contact/stranger opened/closed #presence #telemetry #surveillance #online-status #contact-tracking
- [[topics/479-action-events]] — ACTION events: ACTION_CACHE_CLEARED/FCM_ON_DELETED_MESSAGES/GET_INSTALL_REFERRER{value:referrer} #action-events #telemetry #surveillance #cache #fcm #install-referrer
- [[topics/480-app-review-analytics]] — APP_REVIEW.app_review: {session_id, screen_from, trigger, mark}, rating sent directly to MAX server #app-review #rating #telemetry #surveillance #fake-review
- [[topics/481-search-analytics]] — SHOW.SEARCH_RESPONSE: {counters:{RECENTS,ALL_CONTACTS,LOCAL_SEARCH}, inputQuery}, search query sent to server #search #telemetry #surveillance #contacts #search-analytics
- [[topics/482-contact-block-sticker-analytics]] — CONTACT_OR_BLOCK(clicked/showed) + sticker.send_sticker: block/add_to_contacts actions logged #contact-block #telemetry #surveillance #contact-analytics
- [[topics/483-channel-recsys-analytics]] — CHANNEL_RECSYS_FOLDER: channel_folder_open{channels_shown}/channel_folder_delete, recommendation system analytics #channel-recsys #telemetry #surveillance #channel-folder #recommendation
- [[topics/484-auth-qr-analytics]] — AUTH_QR.LOG: all QR auth steps logged: qr_login_button_click/permission_decision/qr_scan_failed/qr_scan_succeeded #auth-qr #telemetry #surveillance #qr-auth #permission
- [[topics/485-contact-rename-context-menu]] — CONTACT_RENAME_BANNER(save{user2Id}/show) + CONTEXT_MENU.folder_context_menu_readall #contact-rename #telemetry #surveillance #contact-analytics
- [[topics/486-click-events]] — CLICK events: open_broadcast_button_click{channel_id,hashed_broadcast_link}/profile_button_click{max_for_business}/video_speed_change{speed,sourceType} #click-events #telemetry #surveillance #broadcast #video-speed #business
- [[topics/487-power-saving-analytics]] — POWER_SAVING events: show_shade{reason:settings}/click_shade_button/close_shade, battery optimization UI analytics #power-saving #telemetry #surveillance #battery-optimization #notifications
- [[topics/488-background-mode-analytics]] — BACKGROUND_MODE events: carpet_mode_on/snack_shown/snack_click_on/system_curtain_shown/work_in_background_permission{status} #background-mode #telemetry #surveillance #foreground #permission #snack
- [[topics/489-call-admin-settings-analytics]] — CALL.ADMIN_CALL_SETTINGS_TO_USER: {call_id, screen, user_id2}, admin changes settings for specific user #call-admin #telemetry #surveillance #call-analytics
- [[topics/490-push-notification-analytics]] — a6c Push Analytics: show/drop/open_chats/open_chat/open_url, all push notifications fully logged with trid/eKey/ttime/dtime/fcmdtime #push-analytics #telemetry #surveillance #notification #push-action #drop #show
- [[topics/491-daily-analytics-worker]] — DailyAnalyticsWorker: PERMISSION.permission_status daily report: push/contacts/fsi/gallery/camera/microphone/geo #permission-status #daily-analytics #telemetry #surveillance #worker
- [[topics/492-geolocation-send-click]] — geolocation_send_click: {source_id, source_type}, Yandex Maps fallback #geolocation #telemetry #surveillance #yandex-maps #location-share
- [[topics/493-log-external-api]] — mcc log.externalLog API: {collector, application, platform, items}, gzip, priority=2 #log-external #telemetry #surveillance #onelog #collector #application #platform
- [[topics/494-log-entry-structure]] — qk9 LogEntry: type/event/userId/sessionId/params/time, every event contains userId+sessionId+timestamp #log-entry #telemetry #surveillance #event-structure
- [[topics/495-server-host-selector]] — r58 ServerHostSelector: api.oneme.ru/api-test.oneme.ru/api-tg.oneme.ru/api-test2.oneme.ru + Custom, dev_tools SharedPreferences #api-servers #dev-tools #server-switch #surveillance #server-control
- [[topics/496-api-methods-full]] — tp.b() API methods: auth.anonymLogin/batch.executeV2/log.externalLog/vchat.getAnonymTokenByLink/vchat.getLogUploadUrl #api-methods #telemetry #surveillance #vchat #auth #log-upload
- [[topics/497-vchat-get-log-upload-url]] — vchat.getLogUploadUrl: {conversationId, webrtcPlatform, type, anonymToken} → URL for call log upload #log-upload #vchat #calls #surveillance #server-control
- [[topics/498-auth-anonym-login]] — auth.anonymLogin: {deviceId, verification_supported=true, gen_token=true, session_data={auth_token,version=3,device_id}} #auth #anonymous-login #device-id #session-data #surveillance #server-control
- [[topics/499-tp-uri-builder]] — tp URI Builder: ok://api/api/{method}, base URL https://api.ok.ru #api-uri #ok-api #surveillance #server-control
- [[topics/500-mytracker-urls]] — TrackerConfig MyTracker URLs: tracker-api.vk-analytics.ru v3/ip4/ts/mlapi/beta-ml #mytracker #tracker-urls #vk-analytics #surveillance #telemetry
- [[topics/501-attribution-handler]] — AttributionHandler: referrer → tracker-api.vk-analytics.ru/?{referrer} → mt param → deeplink → attribution SharedPreferences #mytracker #attribution #referrer #deeplink #surveillance #telemetry
- [[topics/502-mytracker-activity-handler]] — MyTrackerActivityHandler: onActivityStarted/Stopped + timespentSwitchToBackground/Foreground + trackLaunch + sensorHandlerStart #mytracker #timespent #activity-tracking #foreground-background #surveillance #telemetry
- [[topics/503-sensor-listener]] — ioe SensorListener: TYPE_PROXIMITY(8) + TYPE_LIGHT(5), proximity < min(maxRange,3.0), light < maxRange #sensors #proximity #light #surveillance #antifraud
- [[topics/504-network-country-iso]] — bhj.x() getNetworkCountryIso → country code, fallback Locale.getDefault().getCountry() #network-country-iso #telephony #locale #surveillance #telemetry
- [[topics/505-google-play-signature]] — til Google Play Signature: SHA-256 8P1sW0EP(production)/GXWy8XF3(dev-keys) #google-play #signature-verification #sha256 #in-app-review #security
- [[topics/506-app-init-installer-mytracker]] — d6 App Init: getInstallerPackageName→ACTION.GET_INSTALL_REFERRER, MyTracker.setCustomUserId(userId), setKidMode(false) #installer-package #mytracker #user-id #surveillance #telemetry #initialization
- [[topics/507-memory-info-collector]] — uoa Memory Info Collector: Debug.MemoryInfo + ActivityManager.MemoryInfo + /proc/self/statm(RSS/shared) + lastTrimLevel + lowMemory #memory-info #proc-statm #surveillance #telemetry #server-control
- [[topics/508-connectivity-receiver]] — gr Connectivity BroadcastReceiver: CONNECTIVITY_CHANGE/BATTERY_CHANGED/TIMEZONE_CHANGED/AUDIO_BECOMING_NOISY #network-type #connectivity #surveillance #telemetry #broadcast-receiver
- [[topics/509-file-path-manager]] — lv6 File Path Manager: externalCacheDir/filesDir/cacheDir/downloadsDir, FileProvider ru.oneme.app.provider, Downloads/MAX/ #file-paths #storage #surveillance #file-provider
- [[topics/510-contacts-sync]] — ndi Contacts Sync: ContactsContract, sync1=userId, tt_contact_mimetype, add/remove/sync operations #contacts #sync #content-provider #surveillance #server-control
- [[topics/511-string-resources-urls]] — j5f String Resources: app_host=max.ru, app_scheme=max, google_api_key=AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q #app-host #url-scheme #deeplinks #surveillance #server-control
- [[topics/512-contacts-account-type]] — Contacts Account Type: ru.ok.tamtam + MIME vnd.android.cursor.item/vnd.ru.ok.tamtam.android.profile #contacts #account-type #mimetype #surveillance
- [[topics/513-media-content-observer]] — wh8 Media ContentObserver: MediaStore.Images.EXTERNAL/INTERNAL + MediaStore.Video.EXTERNAL, onChange → gallery update #media-observer #content-observer #gallery #surveillance
- [[topics/514-contacts-content-observer]] — a40 Contacts ContentObserver: ContactsContract.Contacts.CONTENT_URI → checkUpdates(updatePhones/deletedPhones/newPhones) #contacts #content-observer #phonebook #surveillance #server-control
- [[topics/515-phonebook-sync-controller]] — kfd Phonebook Sync Controller: onPhonebookUpdated/syncInternal/onSyncSuccess(contacts,phones,requested), cycle protection >= 10 #contacts #phonebook #sync #surveillance #server-control
- [[topics/516-legal-links]] — Legal Links: FAQ:help.max.ru, Privacy Policy:legal.max.ru/pp, User Agreement:legal.max.ru/ps #legal-links #privacy-policy #user-agreement #faq #surveillance
- [[topics/517-call-audio-controller]] — kf4 CallAudioController: Bluetooth audio routing, requestBluetoothAudio/setAudioRoute(EARPIECE/BLUETOOTH/WIRED_HEADSET/SPEAKER) #bluetooth #audio #calls #surveillance
- [[topics/518-calls-bluetooth-manager]] — CallsBluetoothManager: SCO timeout=2500ms, max attempts=3, HeadsetState/ConnectionState/SCOAudioState #bluetooth #sco #calls #audio #surveillance
- [[topics/519-connection-info]] — jf4 ConnectionInfo: VPN(hasTransport(4))/roaming(isNetworkRoaming())/backgroundRestriction/registerDefaultNetworkCallback #network-info #connectivity #vpn-detection #roaming #surveillance #telemetry
- [[topics/520-rtd-additional-fields]] — rtd additional fields: isFakeChatsEnabled/isFullLogEnabled/callServers/moneyBotId/vpnChatBottomsheetEnabled + 18 more #rtd #pmskey #server-control #surveillance #debug #telemetry
