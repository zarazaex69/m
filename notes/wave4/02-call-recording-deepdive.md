---
tags: [recording, calls-sdk, privacy, server-control, surveillance, production]
status: verified
severity: high
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/record/RecordManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/record/internal/RecordManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/record/RecordDescription.java
  - work/jadx_base/sources/defpackage/qx1.java (server record-started/stopped handler)
  - work/jadx_base/sources/defpackage/z91.java (WS notification dispatch)
  - work/jadx_base/sources/defpackage/v7g.java (ScreenRecordController - UI layer)
  - work/jadx_base/sources/defpackage/my1.java (StartRecordBottomSheet flow)
  - work/jadx_base/sources/defpackage/c92.java (stopRecordBroadcast)
  - work/jadx_base/sources/one/me/calls/ui/bottomsheet/record/StartRecordBottomSheet.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/dev/internal/MediaDumpManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr/internal/commands/AsrCommandsExecutorImpl.java
  - work/jadx_base/sources/one/video/calls/audio/opus/FileWriter.java
  - work/jadx_base/sources/defpackage/s2d.java (opus local recorder)
  - work/jadx_base/sources/defpackage/l7g.java (ScreenRecordBroadcastData)
  - work/jadx_base/sources/defpackage/xz1.java (CallScreenRecordState)
related:
  - "[[531-wiretap-chain-collect-debug-dump]]"
  - "[[545-three-audio-interception-channels]]"
  - "[[544-debug-ws-opcode-c2-channel]]"
  - "[[20-ws-protocol-opcodes]]"
  - "[[17-apptracer-uplink]]"
---

# Wave 4 — Call Recording Deep Dive (RecordManager)

## Архитектура

```
┌─────────────────────────────────────────────────────────────────────┐
│                        КЛИЕНТ (Android)                             │
│                                                                     │
│  StartRecordBottomSheet (UI)                                        │
│       │ user taps "Начать запись"                                   │
│       ▼                                                             │
│  v7g (ScreenRecordController)                                       │
│       │ analytics: ob2.l("CALL_RECORDING", ...)                     │
│       ▼                                                             │
│  RecordManager.startRecord(StartParams)                             │
│       │                                                             │
│       ▼                                                             │
│  RecordManagerImpl.startRecord()                                    │
│       │ JSON: {command: "record-start", movieId, name,              │
│       │        description, privacy, groupId, albumId, streamMovie} │
│       │ + roomId (if multi-room)                                    │
│       ▼                                                             │
│  SignalingProvider → pbh.d(command, ...)                             │
│       │                                                             │
└───────┼─────────────────────────────────────────────────────────────┘
        │ WebSocket
        ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        СЕРВЕР MAX                                    │
│                                                                     │
│  Принимает "record-start", начинает серверную запись                 │
│  Рассылает "record-started" ВСЕМ участникам (включая инициатора)    │
│  recordInfo: {initiator, recordMovieId, recordType, recordStartTime,│
│               recordExternalMovieId, recordExternalOwnerId}         │
│                                                                     │
│  При остановке: рассылает "record-stopped" с participant + movieId   │
│                                                                     │
│  Хранение: OK.ru video infrastructure (movieId → externalMovieId)   │
└─────────────────────────────────────────────────────────────────────┘
        │ WebSocket "record-started" notification
        ▼
┌─────────────────────────────────────────────────────────────────────┐
│  КЛИЕНТ УЧАСТНИКА                                                   │
│                                                                     │
│  z91.java: case "record-started" → qx1.b(json)                     │
│       │ парсит recordInfo → ich (SignalingRecordInfo)                │
│       ▼                                                             │
│  RecordManagerImpl.onRecordStarted(nx1)                             │
│       │ → applyRecordStarted() → reportStarted()                   │
│       ▼                                                             │
│  v7g.onRecordStarted() → UI: показывает "%s записывает · %s"       │
│  (CallScreenRecordState.isRecordStateEnabled = true)                │
│                                                                     │
│  Tooltip: "Оставаясь, вы соглашаетесь на запись"                    │
└─────────────────────────────────────────────────────────────────────┘
```

## Ответы на 6 ключевых вопросов

### 1. Кто инициирует запись?

**Клиент** — через UI (`StartRecordBottomSheet`). Пользователь нажимает кнопку «Начать запись» в bottom sheet, вводит название, и клиент отправляет WS-команду `record-start` на сервер.

**НО**: запись происходит **на сервере**, не на устройстве. Клиент лишь отправляет команду; сервер записывает медиапоток, который и так проходит через него (нет E2E). Клиент не записывает локально в рамках RecordManager (локальная запись — отдельный механизм через `FileWriter`/opus, см. ниже).

Нет WS-опкода для серверного принудительного старта записи в `ws_opcodes.txt`. Однако:
- Сервер **уже имеет доступ** к незашифрованному медиапотоку (нет E2E)
- Сервер может записывать без уведомления клиента — клиент узнаёт о записи только через `record-started` notification
- Если сервер не отправит `record-started`, клиент не узнает о записи

### 2. Можно ли активировать запись скрытно?

**Два уровня:**

**a) Серверная запись (RecordManager):**
- При **легитимном** использовании: все участники получают `record-started` → UI показывает «%s записывает · %s» + tooltip «Оставаясь, вы соглашаетесь на запись»
- **Но**: поскольку E2E нет, сервер имеет доступ к cleartext медиапотоку **всегда**. Серверная сторона может записывать без отправки `record-started` клиентам — клиент не имеет возможности это обнаружить.

**b) Параллельные каналы (topic 531 + 545):**
- `collect-debug-dump` — сервер может молча запросить дамп аудио из 6 точек pipeline через signaling WS → `nativeSubmitDumpRequest` → файл на устройстве → upload на apptracer.ru. **Без UI, без уведомления.**
- `AudioRecordSampleHook` — production API подписки на raw PCM микрофона, любой код в процессе получает буферы.

**Вывод**: RecordManager сам по себе имеет UI-уведомление для участников. Но скрытая запись возможна через:
1. Серверную сторону (нет E2E → сервер всегда видит cleartext)
2. `collect-debug-dump` (topic 531) — полностью скрытый канал

### 3. Куда уходят файлы и в каком виде?

**Серверная запись (RecordManager):**
- Хранится на серверной инфраструктуре OK.ru как «видео» (movie)
- `RecordDescription` содержит: `movieId`, `externalMovieId`, `externalOwnerId`
- Тип: `RECORD` или `STREAM` (enum `khf`: a=unknown, b=RECORD, c=STREAM)
- **privacy = "PUBLIC" по умолчанию** (hardcoded в `StartParams.Builder`)
- Формат: видео (серверный muxing медиапотоков участников)
- Endpoint: OK.ru video platform (movieId → стандартный OK.ru video player)

**⚠️ CRITICAL: `privacy = "PUBLIC"` default**
```java
// RecordManager.java → StartParams.Builder
private String privacy = "PUBLIC";
```
Если пользователь не меняет privacy вручную (а UI для этого в `StartRecordBottomSheet` не обнаружен), запись звонка публикуется с доступом PUBLIC. Это означает, что записи звонков по умолчанию доступны всем.

**Локальная запись (opus FileWriter, s2d.java):**
- Отдельный механизм для voice messages / audio notes
- Пишет в локальный файл через `nativeAudioStartRecord(path, bitrate, sampleRate)`
- Не связан напрямую с RecordManager

**MediaDump (topic 531):**
- Файлы: `calldump_<timestamp>_<duration>s/`
- Upload: `sdk-api.apptracer.ru/api/sample/upload`
- Cleartext (нет шифрования дампов)

### 4. Есть ли механизм «шёпотом» включить запись на устройстве жертвы?

**Через RecordManager — НЕТ.** RecordManager отправляет команду серверу, сервер записывает на своей стороне. Нет механизма, где сервер приказывает клиенту начать локальную запись через RecordManager.

**Через `collect-debug-dump` — ДА (topic 531).** Сервер через signaling WS отправляет `{type: "collect-debug-dump", audio: true, duration: N}`, клиент выполняет `nativeSubmitDumpRequest` → записывает аудио из 6 точек pipeline → upload на apptracer.ru. Без UI, без уведомления, без opt-out.

**Через ASR recording — ЧАСТИЧНО.** `AsrCommandsExecutorImpl.startRecord(fileName, roomId)` отправляет `asr-start` на сервер. Это серверная ASR-транскрипция, не локальная запись. Но сервер получает аудиопоток для транскрипции.

### 5. Server-controlled параметры

**RecordManager StartParams (клиент → сервер):**
| Параметр | Тип | Описание |
|----------|-----|----------|
| `isStream` | boolean | false=запись, true=стриминг (live) |
| `movieId` | Long? | ID существующего видео (для продолжения?) |
| `albumId` | String? | Альбом для сохранения |
| `sessionRoomId` | vsg? | Комната (для multi-room) |
| `name` | CharSequence? | Название (default: timestamp) |
| `description` | CharSequence? | Описание |
| `groupId` | Long? | Группа |
| `privacy` | String | **"PUBLIC" по умолчанию** |

**PmsKey (сервер → клиент):**
| Ключ | Описание |
|------|----------|
| `opus-recorder` | Конфигурация opus-рекордера |
| `opus-recorder-bitrate` | Битрейт записи |
| `opus-recorder-sample-rate` | Sample rate |
| `live-streams` | Конфигурация live-стримов |
| `live-streams-url-prefix` | URL-префикс для стримов |

**CallFeature.RECORD (ck1.b):**
- Серверно-управляемая роль: `FeatureRoles.EnabledForAll` или ограничение по ролям
- Админ может запретить запись для обычных участников
- Но сервер контролирует, кому разрешена запись

**Серверные события (сервер → клиент):**
- `record-started`: JSON с `recordInfo` (initiator, movieId, type, startTime, externalMovieId, externalOwnerId)
- `record-stopped`: JSON с `participant`, `recordMovieId`, roomId
- Приходят при join в звонок (если запись уже идёт — в `recordInfo` поле initial handshake)

### 6. Связь с topic 545 и topic 531

| Механизм | Инициатор | UI-уведомление | Хранение | Скрытность |
|----------|-----------|----------------|----------|------------|
| **RecordManager** (этот topic) | Клиент (UI) | ✅ Все участники видят | Сервер OK.ru (movie) | ❌ Видно всем |
| **collect-debug-dump** (topic 531) | Сервер (signaling) | ❌ Нет UI | Локально → apptracer.ru | ✅ Полностью скрытно |
| **AudioRecordSampleHook** (topic 545) | Любой код в процессе | ❌ Нет UI | Зависит от подписчика | ✅ Скрытно |
| **nativeSubmitDumpRequest** (topic 545) | Сервер через collect-debug-dump | ❌ Нет UI | Локально → apptracer.ru | ✅ Скрытно |
| **ASR recording** (AsrManager) | Клиент/сервер | Частично (ASR UI) | Сервер (транскрипция) | Частично |

**Ключевая связка**: RecordManager — это «легитимный» канал с UI. Но параллельно существует `collect-debug-dump` (topic 531), который делает то же самое **скрытно**. Отсутствие E2E означает, что сервер может записывать всегда, независимо от RecordManager.

## Риск-оценка

### Что работает корректно (не backdoor):
1. ✅ RecordManager требует явного UI-действия пользователя (StartRecordBottomSheet)
2. ✅ Все участники получают уведомление о записи (`record-started` → UI «%s записывает»)
3. ✅ Tooltip предупреждает: «Оставаясь, вы соглашаетесь на запись»
4. ✅ Админ может остановить запись
5. ✅ При выходе из звонка показывается RecordExitBottomSheet с предупреждением

### ⚠️ Проблемы (HIGH severity):

**1. privacy="PUBLIC" по умолчанию**
- Hardcoded default в `StartParams.Builder`
- В UI (`StartRecordBottomSheet`) не обнаружен selector privacy
- Записи звонков по умолчанию публичные на платформе OK.ru
- Пользователь может не осознавать, что запись доступна всем

**2. Серверная запись без E2E**
- Медиапоток проходит через сервер в cleartext
- Сервер **технически может** записывать без отправки `record-started` клиентам
- Клиент не имеет криптографической гарантии, что запись не ведётся
- Это архитектурная проблема, не баг

**3. Стриминг (isStream=true)**
- `streamMovie: true` в JSON → live-стрим записи
- Запись доступна в реальном времени (не только после завершения)
- В связке с `privacy="PUBLIC"` → live-стрим звонка публично доступен

### ❌ Что НЕ является backdoor в RecordManager:
- RecordManager НЕ может быть активирован сервером без клиентского UI
- Сервер НЕ отправляет команду «начни запись» клиенту
- `record-started` — это **уведомление**, не команда на запись
- Локальная запись (opus FileWriter) — отдельный механизм для voice messages

## Вывод

RecordManager сам по себе — **легитимная фича** с UI consent и уведомлением участников. Однако:

1. **privacy="PUBLIC" default** — серьёзная проблема приватности (записи звонков публичны по умолчанию)
2. **Отсутствие E2E** делает RecordManager **театром безопасности**: сервер может записывать всегда, UI-уведомление — лишь courtesy от сервера
3. **Параллельный канал** `collect-debug-dump` (topic 531) позволяет серверу скрытно записывать аудио на устройстве и выгружать на apptracer.ru — это реальный backdoor, а не RecordManager

RecordManager не требует отдельного topic 548+ — он работает как задокументировано. Критические находки уже покрыты в topics 531 и 545. Единственная новая находка — **hardcoded privacy="PUBLIC"** — заслуживает упоминания в FINDINGS.md, но не отдельного critical topic.

## Техническая деталь: два типа записи

```
RecordType (khf enum):
  a = UNKNOWN (fallback)
  b = RECORD  — стандартная запись, сохраняется как видео
  c = STREAM  — live-стрим, доступен в реальном времени
```

`ScreenRecordBroadcastData` (l7g) хранит:
- `id` (movieId)
- `streamId` (для live)
- `initiatorId` (кто начал)
- `startTimeMs`
- `recordType` (1=NOTHING, 2=STREAM, 3=RECORD)

## Версия 26.16.0

Без изменений. RecordManager, privacy="PUBLIC" default, signaling commands — всё идентично 26.15.3 (только обфускация имён классов).
