---
tags: [version-diff, 26.16.0, kws-removed, pmskey-renamed, cosmetic-fix, surveillance]
status: confirmed
sources:
  - apk_26.16.0/max-26.16.0-base.apk (28.7 MB, RuStore, 2026-05-14, versionCode 6698)
  - apk/max-26.15.3-base.apk (29.2 MB, versionCode 6695)
  - native_diff/old/libEnhancementLibShared.so (5.72 MB, MD5 a16cb079...)
  - native_diff/new/libEnhancementLibShared.so (5.66 MB, MD5 25fd236e...)
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[70-keyword-spotter-in-calls]]"
  - "[[03-pms-server-flags]]"
  - "[[380-ml-features-manager]]"
  - "[[395-keyword-spotter-impl]]"
---

# 26.16.0: «испугались и удалили KWS» — но я всё сохранил

Релиз **MAX 26.16.0** вышел в RuStore **14 мая 2026** (versionCode 6698, через 2 месяца после 26.15.3). Заметки этого репо были написаны по 26.15.3. **Я сохранил весь анализ** старой версии — ниже карта изменений, чтобы было ясно что ушло, что осталось, и что просто переименовали.

## TL;DR

| Что | 26.15.3 | 26.16.0 | Комментарий |
|-----|---------|---------|-------------|
| **KWS (детектор ключевых слов в звонках)** | ✅ полный стек | ❌ **УДАЛЁН** | пресса возмущалась — испугались |
| **PmsKey (334 серверных флага)** | `ru.ok.tamtam.android.prefs.PmsKey` | `one.me.sdk.prefs.PmsProperty` | переименовали, поведение то же |
| **Mobile ID cleartext HTTP** | ✅ 6 операторов | ✅ 6 операторов | **0 изменений** |
| **AsrOnlineManager (серверная транскрипция)** | ✅ 195 упоминаний | ✅ 195 упоминаний | **0 изменений** |
| **Speaker Recognition (по голосу)** | ✅ есть | ✅ есть | без изменений |
| **MediaDumpManager (удалённый дамп)** | ✅ в production | ✅ в production | без изменений |
| **HostReachabilityChecker** | ✅ есть | ✅ есть | без изменений |
| **DevMenu в release** | ✅ есть | ✅ есть | без изменений |
| **Иконка 9 мая** | `ic_launcher_9may` | `ic_launcher` | сезонная, прошёл май |
| **Логин SMS** | SMS-код | **flash-call** (последние 6 цифр входящего номера) | новинка |
| **CameraX** | старый Camera1/2 | `CameraxCameraApiView` | техническое обновление |
| **Транскрипция видеосообщений** | только аудио | + видео | новая фича (whatsnew) |

**Главный вывод:** убрали ровно одну фичу — KWS, на которую указывала пресса как на «прослушку нейросетью». Всё остальное **на месте без изменений**.

---

## 🔥 Что УДАЛИЛИ (испугались)

### 1. KWS (Keyword Spotter) — полный стек

**Java-уровень (smali):**
- `KeywordSpotterManager` (interface)
- `KeywordSpotterManagerImpl` + `Companion` + 6 inner classes (`$initDisposable$1/$2`, `$scheduleTurnOff$1/$2`, `$setKeywordSpotterParams$1/$2`)
- `KeywordSpotterManager$KeywordSpotterConfig`
- `KeywordSpotterManager$KeywordSpotterParams`
- `KeywordSpotterConfigProvider` (interface)
- `KeywordSpotterConfigProviderImpl` + `Companion`
- `KwsFeatureConfigProvider` + `Companion`
- `KwsFeatureDelegate` + `Companion`
- `MLFeaturesManagerImpl$delegates$1$2`
- `ConversationKwsStat` (статистика срабатываний)

**Метрики:**
- `KeywordSpotter` упоминаний: 304 → **0**
- `wordspotter` строк: 16 → **0**
- `bad_call_detected_by_audio_spotter` event: ✅ → ❌

**Native-уровень (libEnhancementLibShared.so):**
- `vk::enh::BCResNetKWS` (класс) — удалён
  - `BCResNetKWS::BCResNetKWS(Config)` (конструктор)
  - `BCResNetKWS::computeProbs()`
  - `BCResNetKWS::doScoreData()`
  - `BCResNetKWS::extractFeatures()`
  - `BCResNetKWS::reset()`
  - `BCResNetKWS::score(float/int/short)` (3 перегрузки)
- `vk::enh::BCResNetExternalStateKWS` (класс) — удалён
  - конструктор + 4 метода + 3 перегрузки `score`
- `vk::enh::FeatureExtractor` — удалён (был для KWS feature pipeline)
  - `extract`, `extract_`, `fft`, `spectrogram`, `buildHammingWindow`, конструктор
- `vk::enh::buildMelBasis` — удалён (Mel filter bank для KWS)

**Диагноз:** убрали и Java-обёртку, и нативный код, и feature extraction, и статистику. Это настоящая ликвидация, не просто скрытие.

**Что НЕ убрали в нативке:**
- `vk::enh::AsrService` (Conformer-CTC ASR) — **на месте**
- `vk::enh::SpeakerRecognitionEngineFactory/VerifierFactory` (идентификация по голосу) — **на месте**
- Diarization (кто когда говорит) — **на месте**
- Wav2Lip (animoji) — на месте
- Denoise — на месте

**Прочтение:** убрали единственное, что юридически трудно оправдать как «полезную фичу для пользователя» — постоянное активное прослушивание триггер-слов. Серверный ASR + Speaker Recognition оставили — они активируются только когда сервер инициирует запись/звонок, что юридически проще оправдать.

### 2. KWS-related в smali (полный список удалённых файлов)

```
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManager.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManager$KeywordSpotterConfig.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManager$KeywordSpotterParams.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl$Companion.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl$initDisposable$1.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl$initDisposable$2.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl$scheduleTurnOff$1.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl$scheduleTurnOff$2.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl$setKeywordSpotterParams$1.smali
ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl$setKeywordSpotterParams$2.smali
ru/ok/android/externcalls/sdk/audio/internal/KeywordSpotterConfigProvider.smali
ru/ok/android/externcalls/sdk/audio/internal/KeywordSpotterConfigProviderImpl.smali
ru/ok/android/externcalls/sdk/audio/internal/KeywordSpotterConfigProviderImpl$Companion.smali
ru/ok/android/externcalls/sdk/audio/a.smali (хелпер)
ru/ok/android/externcalls/sdk/ml/config/kws/KwsFeatureConfigProvider.smali
ru/ok/android/externcalls/sdk/ml/config/kws/KwsFeatureConfigProvider$Companion.smali
ru/ok/android/externcalls/sdk/ml/delegate/KwsFeatureDelegate.smali
ru/ok/android/externcalls/sdk/ml/delegate/KwsFeatureDelegate$Companion.smali
ru/ok/android/externcalls/sdk/ml/MLFeaturesManagerImpl$delegates$1$2.smali
ru/ok/android/externcalls/sdk/stat/kws/ConversationKwsStat.smali
```

### 3. PmsKey (334 серверных флага) — переименован

**БЫЛО:** `ru/ok/tamtam/android/prefs/PmsKey.java` — единый enum со всеми 334 флагами.

**СТАЛО:** `one/me/sdk/prefs/PmsProperty` — каждый флаг как отдельный property-метод. Пример из smali:

```smali
const-string v15, "log-sensitive"
const-string v14, "logSensitive()Lone/me/sdk/prefs/PmsProperty;"

const-string v15, "show-vpn-chat-bottomsheet"
const-string v14, "showVpnChatBottomsheet()Lone/me/sdk/prefs/PmsProperty;"
```

**Метрики:**
- `PmsKey` упоминаний: 3254 → **0**
- `fake-chats` упоминаний: 6 → 1 (почти убрали, но не полностью)

**Диагноз:** реверсу теперь сложнее агрегировать список флагов (нет одного enum-а). **Но серверный контроль не отменён** — все 334 флага продолжают приходить с сервера и применяться. Это **обфускация без изменения поведения**.

### 4. Manifest stamp (Google Play markers)

Удалены три meta-data:
```xml
<meta-data android:name="com.android.stamp.source" android:value="https://play.google.com/store"/>
<meta-data android:name="com.android.stamp.type" android:value="STAMP_TYPE_DISTRIBUTION_APK"/>
<meta-data android:name="com.android.vending.derived.apk.id" android:value="4"/>
```
**Диагноз:** APK был скачан из RuStore, не из Google Play. Технический артефакт, не security-изменение.

### 5. Иконка 9 мая → обычная

```xml
android:icon="@mipmap/ic_launcher_9may"  →  android:icon="@mipmap/ic_launcher"
```
**Диагноз:** сезонный ребрендинг, прошёл май.

---

## 🆕 Что ДОБАВИЛИ

### 1. CameraX API (миграция)

Новый пакет `ru/ok/tamtam/android/widgets/quickcamera/CameraxCameraApiView` + 8 inner-классов (`$a` … `$h`). Это полная переработка с старого Camera1/2 API на CameraX (Jetpack).

Связанная новая native-либа: `libimage_processing_util_jni.so` (32 KB).

### 2. AspectRatiosBottomSheet (UI кропа)

Новый файл `one/me/mediapicker/crop/AspectRatiosBottomSheet.smali` + строки:
- `media_picker_aspect_ratios_bottom_sheet_title` = «Соотношения сторон»
- `media_picker_aspect_ratios_bottom_sheet_album_header` = «Альбомная»
- `media_picker_aspect_ratios_bottom_sheet_portrait_header` = «Портретная»
- `media_picker_aspect_ratios_bottom_sheet_square_ratio` = «Квадрат»

### 3. Flash-call логин

Замена SMS-кода на входящий звонок:

**БЫЛО (26.15.3):**
> «Отправили код на %s»  
> «Если не получили СМС, проверьте чат %s в MAX»

**СТАЛО (26.16.0):**
> «Звоним на %s»  
> «Введите последние 6 цифр входящего номера. Если звонка нет, проверьте СМС и чат %s в MAX»

**Диагноз:** российская практика — flash-call (звонок без снятия трубки) дешевле SMS для оператора. Юзер вводит последние 6 цифр номера, который ему звонит. Не security-изменение, а **оптимизация стоимости рассылки кодов**.

### 4. Транскрипция видеосообщений

Заявлено в whatsnew: «Добавили расшифровку видеосообщений — нажмите на кнопку со стрелкой, чтобы перевести видео в текст». Это означает, что [[84-transcribe-media-ws-opcode|TRANSCRIBE_MEDIA(202)]] теперь применяется и к видео, не только к аудио. Сервер получает аудиодорожку видеосообщений для распознавания.

### 5. CallHistoryClearBatch

Новый таск `ru/ok/tamtam/nano/Tasks$CallHistoryClearBatch.smali` — массовое удаление истории звонков. Связан с whatsnew «Появился мультивыбор чатов для быстрых действий» — но конкретно для истории звонков.

### 6. Новые exception-типы

- `one/me/android/concurrent/ThreadExecutorException`
- `one/me/android/concurrent/ThreadExecutorHangException`
- `one/me/android/concurrent/ThreadExecutorStuckException`
- `one/me/android/externalcallback/ExternalCallbackHelper$ExternalCallbackException`
- `one/me/webapp/domain/jsbridge/WebAppJsonException`
- `ru/ok/tamtam/android/util/locale/ResourceLangException`

**Диагноз:** мониторинг + лучшая обработка ошибок. Не security-изменение.

### 7. Новые UI-строки

- `call_incoming_from_organization` = «Звонок из организации» — UI для входящих от оргов
- `discussions_channel_onboarding_tooltip` = «Включите комментарии в настройках» — для каналов
- `oneme_login_profile_suspended` = «Пользователь заблокирован или удалён»
- `oneme_login_restore_access` = «Восстановить доступ»

### 8. Apptracer mapping UUID — обновлён

```
БЫЛО: 47afde00-4ab5-11f1-9a3d-0c152d90928f
СТАЛО: d6d66620-4f02-11f1-94d6-0c152d90928f
```

Это обычный per-release UUID для маппинга крашей (ProGuard mapping file → Apptracer). Сам стек Apptracer не менялся.

---

## 🟢 Что НЕ ИЗМЕНИЛОСЬ (всё что описано в репо — **на месте**)

### Идентификация и Mobile ID

- `network_security_config.xml` — **0 изменений** (см. [[221-network-security-config]])
  - cleartext HTTP к `mobileid.megafon.ru`, `idgw.mobileid.mts.ru`, `hhe.mts.ru`, `he-mc.tele2.ru`, `he-mc.t2.ru`, `balance.beeline.ru` — на месте
- `verify_mobile_id` JS-bridge — на месте (см. [[10-webapp-jsbridge]])
- ExternalCallback / OK_TOKEN IdP-flow — на месте (см. [[29-external-callback-idp-flow]])

### Звонки (Calls SDK)

- `AsrOnlineManager` (серверная real-time транскрипция с participantId) — **195 упоминаний** в обеих версиях, без изменений (см. [[152-asr-online-manager]], [[376-asr-online-manager]])
- `MediaDumpManager.requestMediaDump(audio, video)` в production — на месте (см. [[401-media-dump-manager]])
- `DebugManager.enableFullAudioDump` в production — на месте (см. [[402-debug-manager]])
- `RecordManager` (запись звонков, `privacy="PUBLIC"`) — на месте (см. [[377-record-manager]])
- `change-media-settings` (форс-выключение микрофона) — на месте (см. [[158-change-media-settings]])
- `MicrophoneManager.registerAudioSampleCallback` — на месте, но без KWS-callback (см. [[156-microphone-manager]])
- `NoiseSuppressionManager` — 70 → 69 упоминаний (минимально, не KWS-related)

### Серверный контроль

- 334 PmsKey-флага (теперь PmsProperty) — все на месте, переименовали класс
- `log-sensitive`, `log-full`, `show-vpn-*`, `invalidate-db-force` — все строки сохранились
- Killswitch + `download.max.ru` CDN — на месте (см. [[12-force-update-killswitch]])
- DevMenu в production (3 вкладки, ServerHostBottomSheet, FeatureToggles) — на месте (см. [[05-dev-menu-in-prod]], [[433-devmenu-details]])

### Слежка и сбор данных

- `HostReachabilityChecker` (probe + IP/operator/VPN report) — на месте (см. [[24-host-reachability-probe]])
- ContentObserver на телефонной книге — на месте (см. [[06-contacts]], [[514-contacts-content-observer]])
- MyTracker SDK (5 сенсоров для fingerprint, GAID/OAID, list of installed apps) — на месте (см. [[35-mytracker-antifraud-sensors]], [[418-mytracker-installed-packages]])
- Apptracer: `sample/initUpload`, `perf/upload`, `crash/trackSession`, heap dumps — на месте (см. [[17-apptracer-uplink]], [[323-apptracer-all-endpoints]])
- DailyAnalyticsWorker (ежедневный отчёт 7 разрешений) — на месте (см. [[491-daily-analytics-worker]])
- FCM `MessageRemoved`, `LocationRequest`, `TamtamSpam` — на месте (см. [[19-fcm-push-payload]])

### Протокол

- 159+ WS-опкодов TamTam/MAX-протокола — на месте (см. [[20-ws-protocol-opcodes]])
- WebSocket MessagePack-framing — на месте (см. [[33-ws-msgpack-framing]])
- E2E-шифрование по-прежнему **отсутствует**

### NFC HCE для мини-апок

- `WebAppNfcService` (HostApduService) — на месте (см. [[07-nfc-hce-webapp]], [[424-webapp-nfc-service]])

### WebApp JS-bridge

- 34 JS-events (включая `WebAppVerifyMobileId`, `WebAppRequestPhone`, `WebAppBiometryAuthRequest`, `WebAppNfcEmulateNfcTag`) — на месте (см. [[419-webapp-js-events]])

### Инфраструктура

- `download.max.ru` CDN — на месте
- `api.oneme.ru` + 3 test-сервера — на месте (см. [[97-api-oneme-ru-endpoints]], [[495-server-host-selector]])
- DPS / `trace-flow.ru` — на месте
- 16 native libs в `lib/arm64-v8a/` (минус -62 КБ из libEnhancementLibShared) — на месте

---

## 📐 Числовые метрики

| Параметр | 26.15.3 | 26.16.0 | Δ |
|----------|---------|---------|---|
| versionCode | 6695 | 6698 | +3 |
| Размер base APK | 29.18 MB | 28.73 MB | **−0.45 MB** |
| Размер libEnhancementLibShared.so | 5.72 MB | 5.66 MB | **−62 KB** (KWS) |
| Размер libjingle_peerconnection_so.so | 12.78 MB | 12.77 MB | −11 KB (рекомпиляция) |
| smali-файлов | 25 579 | 23 725 | **−1 854** (в основном переоптимизация обфускации) |
| `vk::enh` экспортов в native | 306 | 281 | **−25** (KWS+FeatureExtractor) |
| `KeywordSpotter` упоминаний | 304 | **0** | −100% |
| `PmsKey` упоминаний | 3 254 | **0** | переименован в `PmsProperty` |
| `AsrOnline` упоминаний | 195 | 195 | 0% |
| `MediaDump` / `enableFullAudioDump` | есть | есть | без изменений |

## 🎯 Финальный диагноз

**26.16.0 — это косметический ремонт под общественное давление, не реформа.**

Они убрали **ровно одну** фичу — KWS, на которую указывала пресса как на «прослушку». Эта фича технически была наиболее безусловно-вредной (детектор работает всё время звонка, на сыром аудио, без явного согласия). Её удаление вписывается в логику минимального медиа-ущерба: «исправили — больше не подслушиваем».

**Но всё остальное на месте:**
- Серверный ASR (то есть **полная транскрипция речи** при записи звонка) — продолжает работать
- Идентификация по голосу (Speaker Recognition) — на месте
- Серверный killswitch + 334 флага управления поведением — на месте
- Cleartext HTTP к операторам — на месте
- Heap dumps на серверы Apptracer — на месте
- HostReachability probe + утечка IP/оператора/VPN — на месте

**Реверсу труднее:** PmsKey переименовали в PmsProperty, размазали по property-методам. Список 334 флагов теперь нельзя получить одной командой `grep`. Но поведение — то же.

Если медиа скажет «они исправили проблему» — это будет ложь по умолчанию. Они исправили **один** компонент из десятка проблемных. Оставшиеся девять продолжают работать как описано во всех остальных 529 темах этого репо.

---

## 📦 Артефакты diff'а в этом репо

- `apk_26.16.0/max-26.16.0-base.apk` (28.7 MB, RuStore, signature `1684414033eb263e2c615f8b7df5ed8793850a07656304997fbf07e9e21e1e93`)
- `apk_26.16.0/max-26.16.0-arm64.apk` (25.6 MB, native libs)
- `apk_26.16.0/max-26.16.0-xxhdpi.apk` (6.2 MB, density split)
- `work_26.16.0/apktool_base/` — распакованная новая версия
- `native_diff/old/` и `native_diff/new/` — обе libEnhancementLibShared.so для side-by-side
- `native_diff/old_vk_full.txt` / `new_vk_full.txt` — vk::enh экспорты
