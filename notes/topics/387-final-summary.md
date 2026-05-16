---
tags: [summary, comprehensive, backdoor, surveillance, telemetry, server-control, calls, asr, mytracker]
status: confirmed
sources:
  - notes/topics/01-386 (все темы)
related:
  - "[[00-INVENTORY]]"
  - "[[FINDINGS]]"
  - "[[INDEX]]"
---

# Итоговый сводный анализ MAX 26.15.3

Полный анализ APK `ru.oneme.app` v26.15.3. 386 тем, ~917 коммитов.

## 1. Идентификация

MAX — форк TamTam (`ru.ok.tamtam.*`). Пакет `ru.oneme.app`. Версия 26.15.3. Без E2E-шифрования.

Стек: OK.ru (TamTam протокол, OneLog, Apptracer, externcalls SDK) + VK (libEnhancementLibShared DSP, MyTracker, tracker-api.vk-analytics.ru).

## 2. Серверный контроль поведения

- **334 PmsKey** — серверно-управляемые флаги поведения клиента (темы 03, 21, 342-344)
- **UserSettings** — объект настроек пользователя с сервера
- **RTD-флаги** `log-sensitive`/`log-full` — сервер включает расширенное логирование (тема 343)
- **DevMenu в release** — переключение API-серверов и фича-флагов (тема 05)
- **Killswitch** — `ForceUpdateScreen` + CDN `download.max.ru` мимо Play (тема 12)
- **WebRTC field trials** — параметры WebRTC задаются сервером (тема 16)

## 3. Телефония и идентификация

- **Header Enrichment** — cleartext HTTP к МТС/Мегафон/Билайн/Tele2 для получения MSISDN (тема 01)
- **JS-bridge `verify_mobile_id`** — мини-приложения получают MSISDN абонента (тема 10)
- **Deeplink `:auth?externalCallback=1`** — MAX как Identity Provider (тема 13)
- **GOST/DigitalID/FamilyProtection** — интеграция с госсервисами (тема 22)

## 4. Телеметрия и трекинг

### MyTracker (tracker ID `34982109644049932883`)
- Собирает: GAID, OAID, android_id, MAC, **age, gender, email, phone, okId, vkId, vkConnectId, icqId** (тема 385)
- **AntiFraudConfig**: все датчики включены (гироскоп, магнитное поле, давление, освещённость, близость)
- **`installedPackagesProvider`** — список установленных приложений
- **`MyTracker.handleDeeplink(intent)`** — все deeplink-переходы через трекер (тема 367)
- 10 модулей: AppLifecycle/Ads/Antifraud/Environment/Game/MiniApps/Purchase/RemoteConfig/Timespent/UserLifecycle

### Apptracer (token `t6QnlHov0Gq1UBGYG9GPqZu0EiVMZ922FKvwyAEASa90`)
- Endpoints: crash/trackSession/perf/upload/sample/initUpload/upload (тема 17)
- **heap-dump** и **sample-uploader** произвольных артефактов
- Mapping UUID `47afde00-4ab5-11f1-9a3d-0c152d90928f`

### Firebase
- Project `max-messenger-app`, app_id `1:659634599081:android:9605285443b661167225b8`
- FCM push: `InboundCall`, `MessageRemoved`, `LocationRequest` (wake-trigger), `TamtamSpam` (тема 19)

### OneLog
- Endpoint `log.externalLog`. Поля: collector/operation/uid/network/type/count/datum/group/custom (тема 373)
- Application: `packageName:versionCode:versionName`. Platform: `android:phone/tablet:OS_version`

### DPS / trace-flow.ru
- XOR-декодер `z8f.a`. Ключи: `ru.trace_flow.dps.API_KEY`/`CLIENT_VERSION`/`USER_ID` (тема 362)

## 5. Звонки — наблюдение

### Двойной ASR
- **On-device ASR** (`libEnhancementLibShared.so`) — локальное распознавание (тема 15)
- **Серверный ASR** (`AsrOnlineManager`) — включается **автоматически** при серверной топологии (тема 376)
  - Результат: `AsrOnlineChunk(participantId, text)` — транскрипция с атрибуцией по участникам

### KWS (Keyword Spotting)
- On-device KWS в `libEnhancementLibShared.so` (тема 15)
- Каждое срабатывание → `bad_call_detected_by_audio_spotter(confidence)` на сервер (тема 382)
- Конфиг KWS с сервера: `android.wordspotter.config` (тема 384)
- ML-модели KWS/NS загружаются с сервера: `android.mlfeatures.ws_0`/`ns_1` (тема 380)

### Запись звонков
- `RecordManager.startRecord()` → `record-start {movieId, name, privacy="PUBLIC", isStream}` (тема 377)
- **`privacy="PUBLIC"` по умолчанию** — записи публичны
- Поддержка стриминга (`isStream=true`)

### requestMediaDump
- Серверная команда для дампа аудио+видео в продакшне (тема 101)

### Другие механизмы наблюдения в звонках
- `onParticipantsDeAnonymized` — деанонимизация участников (тема 101)
- `onMicrophoneForciblyMuted` — принудительное отключение микрофона (тема 101)
- `P2pRelaySwitchTrigger` — автопереключение на relay при RTT >= threshold (с сервера) (тема 378)
- `SessionRoomsManager.moveParticipant()` — принудительное перемещение участника (тема 378)
- `StereoRoomManager.grantAdmin()/revokeAdmin()` — динамическое управление правами (тема 381)
- `WatchTogether.MovieState(participantId, position, isPlaying)` — сервер знает, кто что смотрит (тема 379)
- `UrlSharingInfo(url, initiatorId)` — сервер знает, какой URL показывается и кем (тема 381)
- `Feedback(source=GESTURES)` — жесты пользователя отслеживаются (тема 381)
- `ConversationParams.ispAsNo/ispAsOrg/locCc/locReg` — геолокация и провайдер в параметрах звонка (тема 384)

## 6. Сетевой стек

- **WS-протокол**: 159 опкодов (тема 20)
- **WebTransport** (`WTSignaling`) — для WatchTogether (тема 379)
- **LZ4 + Zstd** — нативная декомпрессия (темы 374, 366)
- **HostReachability**: gstatic.com/mtalk.google.com/calls.okcdn.ru/gosuslugi.ru/pushtrs.push.hicloud.com (тема 357)
- **IP detection**: 6 сервисов (ipify/AWS/ifconfig.me/mail.ru/yandex IPv4+IPv6) (тема 361)
- **Yandex Maps** через PmsKey `y-map` (тема 354)
- **Google Maps API key** `AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI` (тема 363)
- **api.oneme.ru** + 3 тестовых сервера (тема 356)

## 7. Мини-приложения

- **VPN-блокировка**: `WebAppHttpClient.WebAppHasVpnException` (тема 368)
- **NFC HCE**: мини-приложение эмулирует NFC-карту через JS-bridge (тема 07)
- **`DownloadFileFromWebAppWorker`**: ForegroundWorker для загрузки файлов из мини-приложений (тема 369)
- **WebAppNfcService** — NFC через мини-приложение

## 8. Нативные библиотеки

16 `.so` файлов. Ключевые:
- `libEnhancementLibShared.so` — ASR/KWS/Diarization/NS/SpeakerRecognition (тема 15)
- `libffmpeg.so` — FFmpeg n4.4.3 (тема 09)
- `libtracernative.so` — нативный трейсер (тема 09)
- `libwebrtc.so` — WebRTC (тема 09)

## 9. База данных

35+ таблиц SQLite. Ключевые: messages, chats, contacts, users, drafts, link_previews, presence (темы 230-260).

## 10. Протокол Tasks.proto

60+ типов задач. Ключевые: `CongratsStatus`, `ExternalVideoSend`, `LocationRequest`, `MessageRemoved` (темы 202-230).

## 11. Разрешения и сервисы

55 разрешений, 26 сервисов, 16 receivers, 8 providers (тема 18).

Ключевые разрешения: `READ_CONTACTS`, `RECORD_AUDIO`, `CAMERA`, `ACCESS_FINE_LOCATION`, `READ_CALL_LOG`, `PROCESS_OUTGOING_CALLS`, `RECEIVE_SMS`, `READ_SMS`.

## 12. Итоговая оценка

MAX содержит полный набор механизмов для:
1. **Идентификации** пользователя (MSISDN через Header Enrichment, GAID/OAID/android_id/MAC через MyTracker)
2. **Транскрипции** звонков (двойной ASR: on-device + серверный, автоматически при групповых звонках)
3. **Записи** звонков (публичной по умолчанию, с поддержкой стриминга)
4. **Серверного контроля** поведения (334 PmsKey, ML-модели с сервера, RTD-флаги)
5. **Профилирования** пользователя (age/gender/email/phone/okId/vkId/icqId + датчики + установленные приложения)
6. **Командного push-канала** (FCM: InboundCall/LocationRequest/TamtamSpam)
7. **Блокировки VPN** для мини-приложений

Все механизмы задокументированы в `notes/topics/01-386`. Гипотезы о намеренности отмечены в соответствующих топиках.
