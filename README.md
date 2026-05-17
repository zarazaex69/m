# MAX 26.15.3 → 26.16.0 — реверс-конспект

Анализ APK российского мессенджера MAX (`ru.oneme.app`) с фокусом на бэкдор-индикаторы, телеметрию, серверный контроль и «странности» поведения, а не на прямые CVE/RCE.

> **🔥 14 мая 2026 вышла версия 26.16.0.** Они **убрали KWS** (детектор ключевых слов в звонках) — единственное, на что громко возмущалась пресса. Всё остальное (cleartext Mobile ID, серверный ASR, killswitch, MediaDump в production, 334 серверных флага, HostReachability, Apptracer heap-dumps) **на месте без изменений**. PmsKey переименовали в PmsProperty — поведение то же, реверсу сложнее. Полный diff: [`notes/topics/530-version-26.16.0-diff.md`](notes/topics/530-version-26.16.0-diff.md).
>
> Я **сохранил** все 529 тем по 26.15.3 — даже то, что они «испугались и удалили». Если на следующих релизах ещё что-то «исправят», я и это сохраню.

> **🔥🔥 17 мая 2026 — финальная волна реверса.** Глубокая параллельная вычитка через субагентов нашла три находки серьёзнее всех ранее задокументированных:
>
> - **[`notes/topics/542-traceflow-dps-deanonymization.md`](notes/topics/542-traceflow-dps-deanonymization.md)** — отдельный обфусцированный SDK `ru.trace_flow.dps` для деанонимизации абонентов под VPN: реальный публичный IP через 6 внешних сервисов (yandex/ifconfig/ipify/amazonaws/mail.ru) + двойной VPN-детект (включая обход через перечисление `tun`/`ppp`/`tap`/`ipsec`) + привязка к `userId` + `deviceId` + оператору, отправка на `https://trace-flow.ru/api/v1/report` с hardcoded API-ключом `ply5hDvhupghrHVA5rqQD1ypiXAxbmE4A68ZzBa8ioc=`. Список хостов серверно обновляемый. Домен `trace-flow.ru` нигде в Privacy Policy не упомянут. **Самая жёсткая находка реверса.**
> - **[`notes/topics/543-reconnect-ws-server-host-takeover.md`](notes/topics/543-reconnect-ws-server-host-takeover.md)** — WS-опкод 3 переписывает `server.host`/`server.port`/`server.useTls` в SharedPreferences без валидации (нет whitelist/regex/pinning), переживает logout+re-login (явная конструкция «сохранить → clear → восстановить» в `ri9.c()`), разрешает TLS downgrade на raw TCP. Scope: все 159 опкодов основного протокола. Адресный server-side MITM.
> - **[`notes/topics/544-debug-ws-opcode-c2-channel.md`](notes/topics/544-debug-ws-opcode-c2-channel.md)** — WS-опкод 2 (`DEBUG`) с именованными командами `SYNC_CONTACTS` (принудительная выгрузка адресной книги) и `SEND_LOG` (искусственный crash → выгрузка логов на apptracer.ru). Поле `args:List<String>` парсится но не используется — готовая площадка для расширения. Без UI, без opt-out, без rate-limiting.
>
> Все три — production-код, проверены живьём в jadx + smali, без изменений в 26.16.0 (только обфускация имён). Связка 542+543+544 даёт server-side адресную компрометацию конкретного пользователя без обновления приложения. См. `notes/wave1/`, `notes/wave2/` для сырых отчётов и `notes/FINDINGS.md` (раздел «Финальные находки реверса»).

Анализ ниже описывает **26.15.3** (база заметок). Diff к 26.16.0 — отдельным файлом, с пометками вида «в 26.16.0 удалено / переименовано / без изменений».

## Что в этом репозитории

```
apk/
  max-26.15.3-base.apk          — базовый APK (DEX + ресурсы + assets)
  max-26.15.3-arm64.apk         — конфиг-сплит arm64-v8a (нативные .so)

apk_26.16.0/                    — обновление от 14 мая 2026 (RuStore)
  max-26.16.0-base.apk          — base APK (28.7 MB, versionCode 6698)
  max-26.16.0-arm64.apk         — arm64-v8a сплит
  max-26.16.0-xxhdpi.apk        — density сплит

work_26.16.0/apktool_base/      — распакованная 26.16.0 для side-by-side
native_diff/                    — diff нативных либ (libEnhancementLibShared, libjingle)

notes/
  INDEX.md                      — Map of Content (Obsidian-style, [[wiki-links]] + #tags)
  00-INVENTORY.md               — что лежит в APK + что бросается в глаза в манифесте
  FINDINGS.md                   — финальная выжимка (TL;DR + 14 разделов; нужно расширить до 23)
  topics/
    01-mobile-id-cleartext.md   — cleartext HTTP к Mobile ID операторов РФ
    02-vpn-warning.md           — серверно-управляемая «отключите VPN» плашка
    03-pms-server-flags.md      — 334 серверно-управляемых параметра поведения
    04-telemetry-endpoints.md   — Apptracer / MyTracker / OneLog x2 / api.oneme.ru
    05-dev-menu-in-prod.md      — DevMenu остался в release (переключение API-серверов)
    06-contacts.md              — ContentObserver + двусторонняя синхронизация контактов
    07-nfc-hce-webapp.md        — мини-апа эмулирует NFC-карту через JS-bridge
    08-background-wake.md       — «работа в фоне при перебоях интернета»
    09-native-libs.md           — 16 .so, FFmpeg n4.4.3, VK enhancement DSP
    10-webapp-jsbridge.md       — verify_mobile_id отдаёт мини-апе MSISDN абонента
    11-state-bots-and-content-policy.md — familyProtection/contentLevelAccess/digitalid/GOST
    12-force-update-killswitch.md — серверный killswitch + download.max.ru мимо Play
    13-deeplinks-idp.md         — :auth?externalCallback=1 (MAX как Identity Provider)
    14-stat-prefs-metrics.md    — phonebook size как серверная метрика
    15-on-device-asr-kws-diarization.md — on-device ASR/KWS/Diarization в libEnhancementLibShared
    16-server-pushed-ml-models-in-calls.md — KWS/NS .tflite-модели качаются с сервера
    17-apptracer-uplink.md      — sample/heap/perf/crash endpoints sdk-api.apptracer.ru
    18-manifest-deep-dive.md    — 55 пермов, 26 сервисов, 16 receivers, 8 providers
    19-fcm-push-payload.md      — FCM payload: InboundCall, MessageRemoved, TamtamSpam, LocationRequest
    20-ws-protocol-opcodes.md   — 159 опкодов TamTam/MAX-протокола
    21-shared-prefs-map.md      — карта SharedPreferences файлов
    22-gost-digitalid-family.md — GOST/DigitalID/FamilyProtection wiring
    23-camera-mic-screen-entry-points.md — точки запуска камеры/микрофона/screen capture
    ...
    (темы 24–386 — см. notes/INDEX.md)

findings/raw/
  pms_keys.txt                  — все 334 серверных PmsKey
  ws_opcodes.txt                — все 159 WS-опкодов протокола
  hosts_quoted_jadx.txt
  hosts_quoted.txt
  hosts_smali.txt
  urls_quoted_jadx.txt
  urls_smali.txt
  wss_smali.txt

findings/native/
  *.strings.txt / *.exports.txt — символы и строки из .so (rz-bin, nm)
  decomp_tracer/                — rzghidra-декомпиляция libtracernative + README
```

## С чего читать

1. `notes/INDEX.md` — Obsidian-стайл навигация по всем темам с тегами и графом ссылок.
2. `notes/FINDINGS.md` — общий обзор и TL;DR (386 тем).
3. `notes/00-INVENTORY.md` — структура APK и красные флаги манифеста.
4. По интересам — конкретные `notes/topics/*`.

## Главное в трёх абзацах

MAX — это форк-клиент TamTam (`ru.ok.tamtam.*`) с полным OK.ru-стеком (трейсер `apptracer.ru`, `OneLog`, `externcalls SDK`) и VK-овскими DSP/трекерами (`tracker-api.vk-analytics.ru`, `libEnhancementLibShared.so` с встроенным ASR/KWS/Diarization). Без E2E-шифрования. WS-протокол на 159 опкодов; черновики, превью ссылок и presence — серверно-синхронизируемые.

Для входа MAX напрямую интегрирован с Header Enrichment-эндпоинтами всех крупных операторов РФ (МТС, Мегафон, Билайн, Tele2 / T2). Эти запросы идут в **открытом HTTP** — операторы не могут подставлять заголовки в TLS-соединение. Параллельно есть JS-bridge `verify_mobile_id`, через который мини-приложения внутри MAX могут получать MSISDN абонента, и deeplink `https://max.ru/:auth?externalCallback=1` для использования MAX как Identity Provider в других приложениях.

Поведение клиента почти полностью контролируется сервером: 334 PmsKey-флага + UserSettings-объект + ML-модели KWS/NS с серверно-задаваемым URL. Серверный killswitch версии превращает любую клиентскую версию в `ForceUpdateScreen` с кнопкой на собственный CDN `https://download.max.ru/`, мимо Google Play. Apptracer-стек штатно умеет heap-dumps процесса и `sample`-uploader произвольных артефактов на `sdk-api.apptracer.ru`. Push-канал FCM — командный (`InboundCall`, `MessageRemoved`, `LocationRequest` wake-trigger, `TamtamSpam` с произвольным URI). В release-сборке сохранилось dev-меню для переключения API-сервера и всех фича-флагов.

В звонках работают два параллельных ASR: on-device (`libEnhancementLibShared.so`) и серверный (`AsrOnlineManager`). Серверный ASR включается **автоматически** при переходе на серверную топологию (групповые звонки) и возвращает `AsrOnlineChunk(participantId, text)` — транскрипцию с атрибуцией по участникам. Запись звонков (`RecordManager`) сохраняется на сервере как видео с `privacy="PUBLIC"` по умолчанию и поддерживает стриминг. Каждое срабатывание KWS отправляется на сервер событием `bad_call_detected_by_audio_spotter(confidence)`. MyTracker собирает полный профиль пользователя (age/gender/email/phone/okId/vkId/vkConnectId/icqId), список установленных приложений и данные всех датчиков (гироскоп/магнитное поле/давление/освещённость/близость) под видом «антифрода». Все deeplink-переходы проходят через `MyTracker.handleDeeplink(intent)`. Мини-приложения блокируются при обнаружении VPN (`WebAppHttpClient.WebAppHasVpnException`).

## Стиль заметок

Заметки оформлены в стиле Obsidian: `notes/INDEX.md` — Map of Content, каждый topic имеет YAML-frontmatter (`tags`, `status`, `sources`, `related`) и `[[wiki-links]]` на связанные темы.

## Источники

- Декомпиляция: `apktool 2.x` + `jadx 1.5.5`
- Декомпиляция нативных либ: `rizin 0.8.2` (rzghidra plugin)
- Размер декомпиляции: ≈15 782 классов / ≈23 795 java-файлов / ≈2 783 ресурса.

## Лицензия / использование

Конспект публичный, для статьи. Сами APK-файлы — собственность правообладателя; репозиторий содержит только аналитический текст и извлечённые идентификаторы (имена функций, опкоды, ключи конфигов).
