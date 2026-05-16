---
tags: [server-control, killswitch, feature-flags, telemetry, renamed-in-26.16.0]
status: renamed-in-26.16.0
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
  - findings/raw/pms_keys.txt
related:
  - "[[12-force-update-killswitch]]"
  - "[[02-vpn-warning]]"
  - "[[04-telemetry-endpoints]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[530-version-26.16.0-diff]]"
---

> ## ⚠️ В MAX 26.16.0 — переименован в `PmsProperty`
>
> ❌ Класс `ru.ok.tamtam.android.prefs.PmsKey` (единый enum с 334 значениями) — **удалён**.  
> ✅ Заменён на `one.me.sdk.prefs.PmsProperty` — **каждый флаг теперь отдельный property-метод**.  
> ✅ Все 334 строковых ключа (`log-sensitive`, `show-vpn-chat-bottomsheet`, `invalidate-db-force`, `fake-chats`, `calls-sdk-log-audio` и т.д.) — **на месте**.  
> ✅ Серверный пуш конфигурации — **работает как раньше**, поведение клиента не изменилось.
>
> Метрика: `PmsKey` упоминаний 3254 → **0** (всё перенесли). Это **обфускация без изменения функциональности** — реверсу теперь нельзя получить список флагов одним `grep PmsKey`. Но строки самих ключей сохранились в smali — их можно собрать через `grep "log-\|show-\|fake-"`.
>
> Пример из smali нового класса:
> ```smali
> const-string v15, "log-sensitive"
> const-string v14, "logSensitive()Lone/me/sdk/prefs/PmsProperty;"
> ```
>
> Полный список 334 флагов в `findings/raw/pms_keys.txt` остаётся актуальным — это **имена ключей с сервера**, они не меняются. См. также [[530-version-26.16.0-diff]].
>
> ---


# PmsKey: 334 серверно-управляемых параметра поведения

## Класс

`ru/ok/tamtam/android/prefs/PmsKey.java` — `@Keep` enum с **334** значениями. PMS = Parameter Management System (наследие TamTam/OK.ru). Сервер пушит ключ-значение, клиент применяет в рантайме.

Хранилище — `defpackage/rtd.java` (`SharedPreferences("settings.prefs")`) и `defpackage/qp6.java` (account-scope, ~170 ключей сверху). Делегаты `kbg/jbg/lbg/mbg/ko6/lo6/mo6/no6/oo6/po6` — типизированные геттеры (`bool/long/int/string/json`). Меняется в рантайме без апдейта приложения.

## Полный список (334)

Сохранён в `findings/raw/pms_keys.txt`. Ниже — что реально интересно с точки зрения «что MAX может с пользователем делать удалённо».

### Логирование чувствительных данных серверно-включаемое

- `log-full` — `isFullLogEnabled` — полный лог
- `log-sensitive` — `needToLogSensitive` — **разрешить логи с чувствительными данными**
- `android-use-logcat-logger` — `useLogcatLogger` — слив в logcat
- `min-log-level` — порог
- `user-debug-report` — `userLogReportChatId` — куда репортить лог
- `log-violations` — `isLogViolationEnabled` — лог StrictMode-нарушений
- `log-messages-meta` — `isLogVisibleMessagesMetaEnabled` — лог метаданных видимых сообщений
- `log-chat-meta` — `isLogChatMetaEnabled` — лог метаданных чатов
- `debug-mode`, `debug-profile-info`, `debug-broken-contact` — debug-флаги в production-сборке
- `tracer-non-fatal-crashed-enabled` — non-fatal crashes в трейсер OK.ru

То есть: «дебаг-режим со сливом чувствительных данных и метаданных чатов/сообщений в логи и в трейсер OK.ru» включается **серверным флагом**. Пользователь об этом не узнаёт.

### Прокси и DNS

- `proxy` — путь до прокси
- `proxy-domains` — список доменов через прокси
- `net-client-dns-enabled` — `isNetClientDnsEnabled` — включить кастомный DNS клиент (DoH через `dns.google.com/resolve`, см. `defpackage/xl2.smali`)
- `net-stat-config` — конфиг сетевой статистики
- `net-session-suppress-bad-disconnected-state` — прятать плохие дисконнекты
- `net-ssl-session-validate` — валидация TLS-сессии (можно выключить!)
- `net-session-rbc-enabled` — режим reverse-byte-channel у session
- `host-reachability` — `isHostReachabilitySendEnabled` — слив информации о достижимости хостов на сервер
- `dps` — `isDpsEnabled` — DPS (см. провайдер `ru.trace_flow.dps.internal.DpsInitProvider` в манифесте)

«ВЫКЛЮЧИ TLS validation на лету» — это серверный флаг. На клиент можно по PMS-команде сказать «не валидируй сессию TLS», и оно перестанет.

### Сбор данных

- `non-contact-sync-time` — `nonContactSyncTimeInSec`
- `non-contact-max-chunk-size`
- `non-contact-collection-interval` — **периодичность сбора non-contacts** (записи телефонной книжки, которые НЕ пользователи MAX, тоже отправляются на сервер)
- `send-location-enabled` — `isSendLocationEnabled` — серверный switch отправки геолокации
- `mytracker-enabled` — `isMytrackerEnabled` — VK MyTracker
- `mytracker-log-level`
- `analytics-enabled` — общий kill-switch аналитики
- `presence-stat`, `presence-external`, `presence-keep-bg-cache`, `presence-offline-log` — presence-телеметрия (online/offline события)
- `webapp-phone-hash` — `useWebAppPhoneHash` — отдавать в WebApp хеш телефона как идентификатор
- `error-stat-limit`, `opcode-stat-config` — конфиг error/opcode статистики

### «Темный UX»

- `fake-chats` — `isFakeChatsEnabled` — фейковые чаты включаемые сервером
- `fake-in-app-review` — `isFakeInAppReviewEnabled` — поддельный in-app review prompt (флаг `fake-in-app-review` буквально)
- `calls-fakeboss-incoming-call-enabled` — `callFakeBossesEnabled` — режим «фальшивого начальника» в звонке (явный класс `gj6.FakeBossListItem` с полями contactServerId/phoneNumber/country/registrationDate/...)
- `show-vpn-chat-bottomsheet` / `show-vpn-call-bottomsheet` / `show-vpn-snackbar` — навязчивая антиVPN-плашка
- `migrate-unsafe-warn` — `migrateUnsafeWarn` — миграция «в небезопасном режиме»
- `show-warning-links` — `showWarningLinks` — отдельный warning на ссылки
- `white-list-links` — белый список ссылок
- `enable-unknown-contact-bottom-sheet`, `contact-add-bottom-sheet`, `not-contact-placeholder`, `non-contact-complaints-enabled` — спец-UX вокруг не-контактов

### Управление крипто/гос-функционалом

- `gost-check-env` — `gostEnvironmentCheckFlags` (Int) — флаги проверки ГОСТ-окружения. Включается серверно.
- `creation-2fa-config` — конфиг 2FA при регистрации
- `phone-privacy-config` — `phonePrivacySettingEnabled`
- `family-protection-botid` — botID **«семейной защиты»** (РФ-закон-friendly хук в виде системного бота)
- `digitalid-botid` — botID для **«Цифрового ID»** (государственный DigitalID Россия)
- `money-transfer-botid` — bot-ID финансовых переводов
- `stickers-botid`, `channel-statistics-botid` — служебные

То есть в MAX вшиты «системные боты» под государственные сервисы (цифровой ID, семейная защита) — и идентификаторы этих ботов задаются сервером.

### Сетевой контроль

- `bad-networ-indicator-config` (sic, опечатка `networ`) — `isBadNetworkIndicator…`
- `webview-cache-enabled`
- `host-reachability` — описано выше
- `keep-background-socket` — постоянный сокет в фоне
- `ping-background-interval` — интервал пинга
- `keep-connection` — `keepConnection`
- `disconnect-timeout`
- `net-session-rbc-enabled`

### «Чёрная дыра» (devnull)

- `devnull` — `devNullConfig` типа `DevNullServerConfig` (см. `qp6.java`)

Это **отдельный** серверный конфиг — куда события «уходят». Не будем гадать, но имя класса `DevNull` и наличие конфига — само по себе любопытная находка: типичная схема, чтобы выборочно отбрасывать события у отдельных пользователей или, наоборот, направлять их в спец-канал.

### Производительность / профилирование

- `perf-events`, `perf-registrar-config` — серверно-управляемые наборы событий профайлера (что слать в трейсер OK.ru)
- `memory-slice-interval`, `battery-slice-interval` — periodic snapshot heap/battery в трейсер
- `system-thread-pool-queue`, `fresco-executor`, `watchdog-config`, `fb-exec-replace` — внутренние executor-ы (можно подменить с сервера)

### Праздничные/тематические

- `february-23-26-theme` (23 февраля 2026)
- `march-8-26-theme` (8 марта 2026)
- `new-year-theme-2026`

Темы привязаны к российским датам и к 2026 году, и тоже включаются сервером.

### Ringtone-странности

- `system-default-ringtone-opt`
- `ringtone-content-type`
- `ringtone-player-focus`
- `outgoing-call-uri` — URI исходящего звонка (можно подменять)
- `reconnect-call-ringtone` — отдельный звук на переподключение

`outgoing-call-uri` особенно интересен: серверно-задаваемый URI, по которому исходит звонок. Это удобный рычаг подмены маршрута голоса.

## Что важно

Около **двух третей поведения клиента — серверно-управляемые флаги**. Обновление приложения не нужно, чтобы:

- включить «полные логи с чувствительными данными» у конкретного аккаунта/группы аккаунтов;
- начать активно собирать non-contacts;
- включить отправку геолокации;
- выключить TLS-валидацию сессии;
- увеличить агрессивность «Отключите VPN»-плашки;
- подменить URI исходящего звонка;
- включить/выключить mytracker;
- включить «семейную защиту» как обязательный системный бот;
- активировать DigitalID-флоу;
- развернуть «фейковые чаты» и «фейковый IA-review».

Все эти ручки УЖЕ есть в клиенте, они только ждут серверного значения. PMS работает поверх обычного бизнес-сокета (TamTam-протокол), без подписи со стороны пользователя — клиент тупо доверяет серверу.

## Список файлов

- `findings/raw/pms_keys.txt` — все 334 ключа
- `defpackage/rtd.java` — основной storage-класс
- `defpackage/qp6.java` — account-scope storage (~170 ключей)
- `ru/ok/tamtam/android/prefs/PmsKey.java` — enum
