# MAX 26.15.3 — реверс-конспект

Анализ APK российского мессенджера MAX (`ru.oneme.app`, версия 26.15.3) с фокусом на бэкдор-индикаторы, телеметрию, серверный контроль и «странности» поведения, а не на прямые CVE/RCE.

## Что в этом репозитории

```
apk/
  max-26.15.3-base.apk          — базовый APK (DEX + ресурсы + assets)
  max-26.15.3-arm64.apk         — конфиг-сплит arm64-v8a (нативные .so)

notes/
  00-INVENTORY.md               — что лежит в APK + что бросается в глаза в манифесте
  FINDINGS.md                   — финальная выжимка (TL;DR + 14 разделов)
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

findings/raw/
  pms_keys.txt                  — все 334 серверных PmsKey
  hosts_quoted_jadx.txt
  hosts_quoted.txt
  hosts_smali.txt
  urls_quoted_jadx.txt
  urls_smali.txt
  wss_smali.txt
```

## С чего читать

1. `notes/FINDINGS.md` — общий обзор и TL;DR.
2. `notes/00-INVENTORY.md` — структура APK и красные флаги манифеста.
3. По интересам — конкретные `notes/topics/*`.

## Главное в трёх абзацах

MAX — это форк-клиент TamTam (`ru.ok.tamtam.*`) с полным OK.ru-стеком (трейсер `apptracer.ru`, `OneLog`, `externcalls SDK`) и VK-овскими DSP/трекерами (`tracker-api.vk-analytics.ru`, `libEnhancementLibShared.so`). Без E2E-шифрования.

Для входа MAX напрямую интегрирован с Header Enrichment-эндпоинтами всех крупных операторов РФ (МТС, Мегафон, Билайн, Tele2 / T2). Эти запросы идут в **открытом HTTP** — операторы не могут подставлять заголовки в TLS-соединение. Параллельно есть JS-bridge `verify_mobile_id`, через который мини-приложения внутри MAX могут получать MSISDN абонента, и deeplink `https://max.ru/:auth?externalCallback=1` для использования MAX как Identity Provider в других приложениях.

Поведение клиента почти полностью контролируется сервером — 334 PmsKey-флага (включая `log-sensitive`, `send-location-enabled`, `net-ssl-session-validate`, `fake-chats`, `gost-check-env`, `family-protection-botid`, `digitalid-botid`, `webapp-phone-hash`) и серверный killswitch версии: один флаг — и любая клиентская версия превращается в `ForceUpdateScreen` с кнопкой на собственный CDN `https://download.max.ru/`, мимо Google Play. В release-сборке сохранилось dev-меню для переключения API-сервера и всех фича-флагов.

## Источники

- Декомпиляция: `apktool 2.x` + `jadx 1.5.5`
- Размер декомпиляции: ≈15 782 классов / ≈23 795 java-файлов / ≈2 783 ресурса.

## Лицензия / использование

Конспект публичный, для статьи. Сами APK-файлы — собственность правообладателя.
