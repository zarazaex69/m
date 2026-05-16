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
