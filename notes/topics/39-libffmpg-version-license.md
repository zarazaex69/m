---
tags: [native-libs, ffmpeg, license, gpl, cve]
status: confirmed
sources:
  - findings/native/libffmpg.strings.txt
  - work/apktool_arm64/lib/arm64-v8a/libffmpg.so
related:
  - "[[09-native-libs]]"
---

# libffmpg.so — FFmpeg n4.4.3 в составе MAX (с GPLv3-нагрузкой)

В [[09-native-libs]] уже зафиксирован сам факт — `libffmpg.so` (1.9 MB) содержит FFmpeg, версия `n4.4.3`. Здесь — детали по версии, лицензии и поверхности атаки.

## 1. Версия и компиляция

В `.rodata` встречается строка `FFmpeg version n4.4.3` (трижды, видимо в разных модулях бинаря). FFmpeg n4.4.3 — релиз ноября 2022 года. На май 2026 это **3.5 года старая ветка** с известными опубликованными CVE.

Поверхностная статистика по CVE: за 2023-2025 годы в FFmpeg <= 4.4.x были закрыты уязвимости в декодерах HEVC, MOV, JPEG2000, PNG, ASF, MP3, WebVTT и других. Многие из этих CVE применимы к media-perekod-payload-ам, которые принимаются мессенджером (отправка специально сформированного видео/аудио → попадание в декодер при автогенерации thumbnail / seek-предпросмотра).

`libavcodec license: GPL version 3 or later` в строках бинаря. Это значит, что:

- FFmpeg в `libffmpg.so` собран с `--enable-gpl --enable-version3` (иначе строка `GPL version 3` не появилась бы).
- Это **GPLv3-licensed component** в составе **проприетарного приложения** MAX (per Wikipedia: «Лицензия — Проприетарное ПО»).

## 2. Лицензионная сторона

GPLv3 требует, чтобы при распространении бинаря, использующего GPLv3-код, был доступен источник всего «работающего вместе» (corresponding source) по запросу пользователя. Для проприетарного Android-приложения это ставит в неудобное положение:

- Либо `libffmpg.so` динамически линкуется и distributable отдельно (с full source available) — что не противоречит GPLv3 при условии publicaiton оригинального FFmpeg-source с применёнными патчами.
- Либо FFmpeg должен быть собран с `--enable-lgpl` (LGPL только), что снимает GPLv3-нагрузку, но строка `GPL version 3 or later` бы тогда не присутствовала.

В коде клиента я **не нашёл UI-элемента «о лицензиях третьих сторон»** или встроенного списка GPL-software и инструкции, где взять source. При распространении через `download.max.ru` (см. [[12-force-update-killswitch]]) и Google Play, это потенциально нарушает GPLv3 §6 (требование offer для письменного source).

Это лицензионная, не security-проблема. Но это «аномальный факт» в продукте такого уровня.

## 3. Точки атаки

`libffmpg.so` подключается через `System.loadLibrary("ffmpg")` для перекодирования медиа: thumbnail-генерация, seek-предпросмотр, преобразование форматов. Атака — отправить пользователю файл, который при попытке воспроизведения попадёт в FFmpeg. Сценарии:

- Зайти в чат с присланным видео → клиент строит thumbnail → срабатывание парсера в `libavcodec`.
- Open-link на медиа-URL → авто-preview через `libavcodec`.
- Forward сообщения с медиа → re-encoding.

Без E2E (см. [[FINDINGS]] §14) сервер MAX тоже технически может перекодировать сообщения — но клиент-side всё равно получает оригинальный bitstream и парсит его через тот же `libffmpg.so`.

## 4. Что нужно проверить

- Включает ли сборка все декодеры или только используемые. Если все — поверхность атаки шире.
- Применены ли security-patches поверх n4.4.3 (имя `libffmpg` без upstream `libffmpeg` намекает на private build VK; патчи могут быть применены, могут и нет).
- Отдельный исходник на github.com/max-messenger в `max-bot-*` репозиториях не лежит (см. [[26-upstream-public-repos]]).

## 5. Сводка

`libffmpg.so` — кастомный VK-build FFmpeg n4.4.3 (3.5 года старый), собранный с `--enable-gpl --enable-version3`. Используется для media transcoding в клиенте. Поверхность атаки = все codecs FFmpeg, которые клиент пускает на присланные медиа. Лицензионная сторона: GPLv3-component в проприетарном продукте без видимой attribution-страницы и без публикации `corresponding source`.
