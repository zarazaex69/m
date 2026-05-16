---
tags: [moc, index, max, max-26.15.3]
status: living
---

# MAX 26.15.3 — Map of Content

Главная точка входа в реверс-конспект. Каждый узел — отдельная заметка.

## TL;DR

См. [[FINDINGS|FINDINGS — финальная выжимка]] (TL;DR + 14 разделов) и [[00-INVENTORY|INVENTORY — инвентарь APK и красные флаги манифеста]].

## Темы

### Аутентификация и идентичность

- [[topics/01-mobile-id-cleartext]] — cleartext HTTP к Mobile ID операторов РФ #cleartext #auth
- [[topics/13-deeplinks-idp]] — `:auth?externalCallback=1` (MAX как Identity Provider) #idp #deeplink
- [[topics/10-webapp-jsbridge]] — JS-bridge `verify_mobile_id` отдаёт мини-апе MSISDN #webapp #js-bridge
- [[topics/11-state-bots-and-content-policy]] — `familyProtection`, `digitalid-botid`, GOST #government #user-settings

### Серверный контроль и killswitch

- [[topics/03-pms-server-flags]] — 334 серверно-управляемых параметра #server-control
- [[topics/12-force-update-killswitch]] — серверный killswitch + `download.max.ru` мимо Play #killswitch
- [[topics/05-dev-menu-in-prod]] — DevMenu в release #dev-menu

### Телеметрия и аналитика

- [[topics/04-telemetry-endpoints]] — Apptracer / MyTracker / OneLog x2 / api.oneme.ru #telemetry
- [[topics/14-stat-prefs-metrics]] — `stat_prefs`: что улетает в метрики #metrics

### Слежка и сбор данных

- [[topics/02-vpn-warning]] — серверно-управляемая «отключите VPN» плашка #anti-vpn
- [[topics/06-contacts]] — ContentObserver + двусторонняя синхронизация контактов #contacts
- [[topics/07-nfc-hce-webapp]] — мини-апа эмулирует NFC-карту через JS-bridge #nfc
- [[topics/08-background-wake]] — фоновое присутствие, wake на boot #background
- [[topics/24-host-reachability-probe]] — server-controlled probe хостов (Telegram/WhatsApp/AWS/Google) + отчёт IP/operator/VPN #host-probe #anti-vpn

### Native-стек

- [[topics/09-native-libs]] — 16 .so, FFmpeg n4.4.3, VK enhancement DSP #native-libs
- [[topics/15-on-device-asr-kws-diarization]] — ASR / KWS / Diarization в `libEnhancementLibShared.so` #asr #kws #ml
- [[topics/16-server-pushed-ml-models-in-calls]] — KWS/NS-модели качаются с сервера в рантайме #ml #server-control

### Аудит

- [[topics/25-resources-audit]] — assets, raw-ресурсы, встроенная фича записи звонка #resources #call-recording
- [[topics/26-upstream-public-repos]] — `github.com/max-messenger`: только bot SDK, без клиента #upstream
- [[topics/27-hardcoded-keys-audit]] — Firebase / MyTracker / Google Maps; приватных ключей нет #secrets

## Источники (raw)

- `findings/raw/pms_keys.txt` — все 334 серверных PmsKey
- `findings/raw/urls_quoted_jadx.txt`, `findings/raw/hosts_quoted_jadx.txt` — реквизиты outbound
- `findings/raw/wss_smali.txt` — WebSocket-эндпоинты (пусто — только wss-протокол к api.oneme.ru)
- `findings/native/` — strings/exports каждой `.so`

## Tag-cloud

- #server-control · 03 · 12 · 16 · 02 · 11
- #surveillance · 01 · 02 · 04 · 06 · 14 · 15 · 16
- #government · 01 · 11 · 13
- #ml · 15 · 16
- #native-libs · 09 · 15 · 16
- #webapp · 07 · 10 · 13
- #anti-vpn · 02 · 10
- #killswitch · 12

## Граф

```
                01-mobile-id-cleartext
                       │
       ┌───────────────┼───────────────┐
       ▼               ▼               ▼
10-webapp-jsbridge   13-deeplinks-idp  04-telemetry
       │
       ├──→ 02-vpn-warning ──→ 03-pms-flags
       │
       └──→ 07-nfc-hce-webapp

09-native-libs ──→ 15-asr-kws ──→ 16-ml-server
                                   │
                                   └──→ 03-pms-flags

11-user-settings ──→ 03-pms-flags ──→ 12-killswitch ──→ 04-telemetry
06-contacts ──→ 14-stat-prefs ──→ 04-telemetry
08-background-wake ──→ 03-pms-flags
05-dev-menu-in-prod ──→ 03-pms-flags
```
