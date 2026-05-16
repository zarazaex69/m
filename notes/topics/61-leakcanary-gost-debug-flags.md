---
tags: [debug, leakcanary, gost, server-control, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ri9.java
  - work/jadx_base/sources/defpackage/qp6.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[22-gost-digitalid-family]]"
  - "[[03-pms-server-flags]]"
---

# LeakCanary в production + gostLicenseCheckEnabled + gostEnvironmentCheckFlags

## LeakCanary в production build

В SharedPreferences (`ri9.java`) присутствует поле `leakCanaryEnabledStateFlow` — `MutableStateFlow<Boolean>`. LeakCanary — это библиотека для детекции утечек памяти в Android, обычно используемая только в debug-сборках.

Наличие `leakCanaryEnabledStateFlow` означает, что LeakCanary **включён в production APK** и может быть активирован через SharedPreferences. Это подтверждает паттерн из [[05-dev-menu-in-prod]] — debug-инструменты остаются в release.

Что делает LeakCanary при включении: мониторит heap, детектирует утечки памяти, создаёт heap dump. Heap dump содержит все объекты в памяти, включая сообщения, токены, ключи. В сочетании с `tracer_supports_arbitrary_dump` (см. [[17-apptracer-uplink]]) — heap dump может быть отправлен на сервер.

## gostLicenseCheckEnabled

`ri9.gostLicenseCheckEnabled` — локальный bool в SharedPreferences. Это флаг, который включает проверку GOST-лицензии. Отдельно от серверного PmsKey `gost-check-env` (#127).

Два независимых канала управления GOST-проверкой: серверный PmsKey и локальный SharedPreferences флаг.

## gostEnvironmentCheckFlags

`qp6.gostEnvironmentCheckFlags` — int (битовые флаги). Accessor: `qp6.B2 = new no6(this, PmsKey.f127gostcheckenv, i29)`. Это server-pushed битовая маска, определяющая, какие именно GOST-проверки окружения выполнять. Разные биты — разные проверки (наличие GOST-провайдера, версия ОС, наличие сертификатов и т.п.).

## areMockCommentsEnabled

`ri9.areMockCommentsEnabled` — локальный bool. «Моки комментариев» — debug-фича для тестирования UI комментариев с фиктивными данными. Присутствует в production SharedPreferences.

## Сводка

LeakCanary включён в production APK и может быть активирован через SharedPreferences — потенциально позволяет создавать heap dump с данными приложения. `gostLicenseCheckEnabled` — локальный флаг GOST-проверки. `gostEnvironmentCheckFlags` — server-pushed битовая маска GOST-проверок окружения. `areMockCommentsEnabled` — ещё один debug-флаг в production.
