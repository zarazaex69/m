---
tags: [resources, assets, strings, call-recording, fake-boss, themes]
status: confirmed
sources:
  - work/apktool_base/res/values/strings.xml
  - work/apktool_base/res/raw/
  - work/apktool_base/assets/
related:
  - "[[18-manifest-deep-dive]]"
  - "[[23-camera-mic-screen-entry-points]]"
  - "[[03-pms-server-flags]]"
---

# Resources / assets audit — что встроено в APK

Краткое содержание подкаталогов APK с акцентом на «штатные UX-фичи, которые в обычном мессенджере встречаются нечасто».

## 1. `assets/`

- **dexopt/** — `baseline.prof`, `baseline.profm` (ART-startup профайл).
- **patterns SVG** для фонов чатов: `february23_pattern.svg`, `march08_pattern.svg` (23 февраля и 8 марта — праздничная тематика РФ), `moscow_pattern.svg`, `lebedev_pattern.svg`, `nature_pattern.svg`, `neon_pattern.svg`, `new_year_pattern.svg`, `space_pattern.svg`.
- **`max_colors_schemes.bin`** — бинарная палитра тем оформления.
- **`shaders/*.glsl`** — 18 OpenGL ES шейдеров для медиа-эффектов / превью видео.
- **`io/michaelrocks/libphonenumber/`** — встроенная offline БД libphonenumber (≈540 файлов `ShortNumberMetadataProto_<XX>` + `PhoneNumberMetadataProto_<XX>` + `PhoneNumberAlternateFormatsProto_<XX>` + `PhoneNumberMetadataProtoForTesting_<XX>`). Полный набор стран. Используется для парсинга номеров оффлайн.

В assets **нет**: `.tflite` или `.onnx` файлов, нет `.so` файлов, нет каких-либо PEM/DER сертификатов, нет protobuf-схем (они вкомпилены в DEX). Это согласуется с тем, что ML-модели KWS/NS качаются с сервера в рантайме (см. [[16-server-pushed-ml-models-in-calls]]).

## 2. `res/raw/`

- **звуки звонков**: `call_busy.m4a`, `call_connected.m4a`, `call_connecting.m4a`, `call_finished.m4a`, `call_incoming.m4a`, `call_ringing.m4a` — стандартные.
- **`call_record_start.m4a`**, **`call_record_stop.m4a`** — звуки начала/остановки **записи звонка**. Встроенные звуковые маркеры (вероятно, проигрываются всем участникам, чтобы было слышно — обычная UX-практика для compliance с требованиями уведомления).
- **`bell_anim.json`** — Lottie-анимация колокольчика (нотификации).
- **`ccp_<язык>.xml`** — XML-списки country codes для UI-пикера телефона (40+ языков).

## 3. Строковые ресурсы — встроенная функция «запись звонка»

`res/values/strings.xml` содержит группу строк UI вокруг записи звонка/экрана:

- `call_admins_settings_screen_record_in_call` = «Записывать звонок» (галочка в настройках администратора звонка).
- `call_admins_settings_screen_record_disabled_in_call` = «Вы запретили запись звонка».
- `call_admins_settings_screen_disable_record` = «Администратор остановил запись экрана».
- `call_screen_record_start_start_btn` = «Начать запись».
- `call_screen_record_start_chat_title` = «Запись пришлём в избранное».
- `call_screen_record_start_chat_subtitle` = «Смотрите с любого устройства».
- `call_screen_record_admin_exit_title` = «Остановить запись?».
- `call_screen_record_me_owner_exit_subtitle` = «Запись продолжится, пока в звонке есть участники».

Что это значит:

- **Запись звонка/экрана — штатная клиентская фича**, не «случайное использование MediaProjection». Запускается участником, отправляется в чат «Избранное» (saved messages) у инициатора. Админ-владелец звонка может разрешать/запрещать другим запись.
- Записанный артефакт **остаётся на серверах MAX в чате «Избранное»** инициатора (без E2E это значит — у сервера полный доступ).
- В звуковых ресурсах есть стартовый/стоповый аудио-маркер — вероятно, играется участникам как уведомление о записи.

Сам код запуска — через MediaProjection из `CallScreen` (см. [[23-camera-mic-screen-entry-points]]). Получаемый поток записывается в локальный файл и заливается на сервер через тот же канал, что и обычные медиа.

## 4. Строки UI про разрешения и сетевые ограничения

Из настроек уведомлений:

- `oneme_notifications_settings_background_wake_section_title` = «Настройки при ограничениях сети».
- `oneme_notifications_settings_how_not_to_miss_calls` = «Как не пропускать звонки и уведомления».
- `oneme_notifications_settings_energy_saving_description` = «Может блокировать уведомления».

Эти строки — обвязка вокруг подсистемы [[08-background-wake]] (foreground service «работа в фоне при перебоях интернета»).

Из VPN-плашки (см. [[02-vpn-warning]]):

- `call_screen_snackbar_title` = «Лучше без VPN».

## 5. Fake Boss / Official Org

`fake_boss_*` строки + `oneme_profile_section_official_org_*`:

- `fake_boss_in_organization` = «Принадлежит организации».
- `fake_boss_no_organization` = «Не принадлежит организации».
- `fake_boss_registration` = «Регистрация».
- `fake_boss_show_mutual_chats` = «Показать».
- `oneme_profile_section_official_org_title` = «Официальный представитель».
- `organization_placeholder_description` = «Профиль организации».

Plus PmsKey `calls-fakeboss-incoming-call-enabled` (см. [[03-pms-server-flags]]).

«Fake boss» — это сценарий, в котором мошенник представляется руководителем организации в звонке/чате. MAX встраивает контр-механизм: если профиль не принадлежит организации (по серверной верификации), показывается предупреждение «Не принадлежит организации».

Это полезная фича, но в пакете с возможностью server-side определять, **что считать «принадлежит организации»**. То есть сервер MAX может в любой момент пометить произвольный профиль как «официальный представитель организации».

## 6. Russia-specific строки

- `oneme_russia_name` = «Россия» — стандартная локализация.
- `auth_error_phone_blacklisted` = «Номер заблокирован. Обратитесь в службу поддержки» — есть отдельное сообщение про чёрный список номеров (видимо, по регуляторным требованиям).
- Праздничные паттерны 23 февраля, 8 марта, новогодний 2026 — отдельные ресурсы.
- Иконка в манифесте `@mipmap/ic_launcher_9may` (9 мая) — фиксируется как факт.

## 7. Чего нет в ресурсах

- `.tflite` / `.onnx` ML-моделей (качаются с сервера, см. [[16-server-pushed-ml-models-in-calls]]).
- Встроенных PEM/DER-сертификатов или public keys.
- Файлов с фразами/wake-word'ами для KWS.
- Списков «заблокированных хостов» или whitelist'ов URL.
- Скрытых конфигурационных JSON.

## 8. Сводка

В ресурсах APK самое примечательное — встроенная **функция записи звонка/экрана** (с UI-обвязкой администрирования звонка) и **звуковые маркеры** старта/остановки этой записи. Записи летят в «Избранное», и без E2E это значит — на сервер MAX в открытом виде.

Праздничные ресурсы (9 мая, 23 февраля, 8 марта, НГ-2026) — продуктовая подача российской тематики; не несут технической нагрузки, фиксируется как стилистический факт.
