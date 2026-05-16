---
tags: [onelog, telemetry, surveillance, dangerous-content, registration]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/x4k.java
  - work/jadx_base/sources/defpackage/s35.java
  - work/jadx_base/sources/defpackage/rf0.java
  - work/jadx_base/sources/defpackage/rk8.java
  - work/jadx_base/sources/defpackage/lq0.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[62-white-list-links-content-control]]"
---

# DANGEROUS_URL_ACTIONS, DANGEROUS_FILE_ACTIONS, REGISTRATION, INFORMER, BANNER OneLog

## DANGEROUS_URL_ACTIONS

`x4k.java` — аналитика предупреждения об опасных URL:
- `showed` — показан модальный диалог «опасная ссылка» (`UIElementType=procced_url_modal_window`)
- `clicked` — нажата кнопка (`clickType=go` или `close`)

Сервер знает: когда пользователю показывалось предупреждение об опасной ссылке, и перешёл ли он по ней (`go`) или закрыл (`close`).

## DANGEROUS_FILE_ACTIONS

`s35.java` — аналитика предупреждения об опасных файлах:
- `modal_is_shown` — показан диалог (с `source_id`, `source_type`)
- `download_file` — пользователь скачал файл
- `not_download_file` — пользователь отказался

Сервер знает: когда пользователю показывалось предупреждение об опасном файле, в каком чате (`source_id`), и скачал ли он файл.

## REGISTRATION

`rf0.java` — аналитика регистрации. Отправляет события с `screen` и произвольными атрибутами из `q2Var`. Сервер получает детальный лог каждого шага регистрации.

## INFORMER

`rk8.java` — аналитика informer-баннеров (см. [[44-informer-banners-fakeboss-livestreams]]): `informer_id`, `informer_type`. Подтверждение: каждое взаимодействие с баннером логируется в OneLog.

## BANNER

`lq0.java` — аналитика баннеров (отдельная категория от INFORMER).

## Что важно

`DANGEROUS_URL_ACTIONS` и `DANGEROUS_FILE_ACTIONS` — сервер знает, когда пользователь переходит по «опасным» ссылкам или скачивает «опасные» файлы. Это может использоваться для мониторинга поведения пользователей, которые пытаются получить доступ к заблокированным ресурсам.

## Сводка

5 дополнительных OneLog категорий: `DANGEROUS_URL_ACTIONS` (переход по опасным ссылкам), `DANGEROUS_FILE_ACTIONS` (скачивание опасных файлов с source_id), `REGISTRATION` (лог регистрации), `INFORMER` (взаимодействие с баннерами), `BANNER`.
