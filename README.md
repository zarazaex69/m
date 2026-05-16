# MAX 26.15.3 — реверс-конспект

Анализ APK российского мессенджера MAX (`ru.oneme.app`, версия 26.15.3) с фокусом на бэкдор-индикаторы, телеметрию, серверный контроль и «странности» поведения, а не на прямые CVE/RCE.

## Что в этом репозитории

```
apk/
  max-26.15.3-base.apk          — базовый APK (DEX + ресурсы + assets)
  max-26.15.3-arm64.apk         — конфиг-сплит arm64-v8a (нативные .so)

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
2. `notes/FINDINGS.md` — общий обзор и TL;DR (исходно 14 разделов; темы 15-23 нужно интегрировать).
3. `notes/00-INVENTORY.md` — структура APK и красные флаги манифеста.
4. По интересам — конкретные `notes/topics/*`.

## Главное в трёх абзацах

MAX — это форк-клиент TamTam (`ru.ok.tamtam.*`) с полным OK.ru-стеком (трейсер `apptracer.ru`, `OneLog`, `externcalls SDK`) и VK-овскими DSP/трекерами (`tracker-api.vk-analytics.ru`, `libEnhancementLibShared.so` с встроенным ASR/KWS/Diarization). Без E2E-шифрования. WS-протокол на 159 опкодов; черновики, превью ссылок и presence — серверно-синхронизируемые.

Для входа MAX напрямую интегрирован с Header Enrichment-эндпоинтами всех крупных операторов РФ (МТС, Мегафон, Билайн, Tele2 / T2). Эти запросы идут в **открытом HTTP** — операторы не могут подставлять заголовки в TLS-соединение. Параллельно есть JS-bridge `verify_mobile_id`, через который мини-приложения внутри MAX могут получать MSISDN абонента, и deeplink `https://max.ru/:auth?externalCallback=1` для использования MAX как Identity Provider в других приложениях.

Поведение клиента почти полностью контролируется сервером: 334 PmsKey-флага + UserSettings-объект + ML-модели KWS/NS с серверно-задаваемым URL. Серверный killswitch версии превращает любую клиентскую версию в `ForceUpdateScreen` с кнопкой на собственный CDN `https://download.max.ru/`, мимо Google Play. Apptracer-стек штатно умеет heap-dumps процесса и `sample`-uploader произвольных артефактов на `sdk-api.apptracer.ru`. Push-канал FCM — командный (`InboundCall`, `MessageRemoved`, `LocationRequest` wake-trigger, `TamtamSpam` с произвольным URI). В release-сборке сохранилось dev-меню для переключения API-сервера и всех фича-флагов.

## Стиль заметок

Заметки оформлены в стиле Obsidian: `notes/INDEX.md` — Map of Content, каждый topic имеет YAML-frontmatter (`tags`, `status`, `sources`, `related`) и `[[wiki-links]]` на связанные темы.

## Источники

- Декомпиляция: `apktool 2.x` + `jadx 1.5.5`
- Декомпиляция нативных либ: `rizin 0.8.2` (rzghidra plugin)
- Размер декомпиляции: ≈15 782 классов / ≈23 795 java-файлов / ≈2 783 ресурса.

## Лицензия / использование

Конспект публичный, для статьи. Сами APK-файлы — собственность правообладателя; репозиторий содержит только аналитический текст и извлечённые идентификаторы (имена функций, опкоды, ключи конфигов).
