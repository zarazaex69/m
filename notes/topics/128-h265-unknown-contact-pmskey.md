---
tags: [calls, h265, codec, server-control, pms, unknown-contact]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[43-libjingle-webrtc-custom-build]]"
---

# H265 codec и unknown-contact PmsKey

| Ключ | # | Default | Описание |
|---|---|---|---|
| `calls-sdk-h265-prioritized` | 125 | false | «Включить поддержку и приоритизировать H265» |
| `calls-android-h265-s` | 116 | 0 | «Деградация кодека H265 (%)» |
| `calls-android-early-create-pc` | 118 | false | «Раннее создание pc» |
| `enable-unknown-contact-bottom-sheet` | 107 | 0 | «Отображение индикатора неизвестного контакта» |

## enable-unknown-contact-bottom-sheet

Три режима:
- `0` — не показывать опрос
- `1` — кнопки «Добавить в контакты» и «Заблокировать»
- `2` — кнопки «Всё в порядке» и «Заблокировать»

Это server-controlled UI для входящих звонков от незнакомцев. Сервер выбирает, какие кнопки показывать.

## calls-android-h265-s

«Деградация кодека H265 (%)» — процент деградации H265 при плохой сети. Сервер контролирует, насколько агрессивно снижать качество H265.

## Сводка

`calls-sdk-h265-prioritized` — server-gated H265 приоритизация. `calls-android-h265-s` — деградация H265 (%). `enable-unknown-contact-bottom-sheet` — 3 режима UI для незнакомых звонящих.
