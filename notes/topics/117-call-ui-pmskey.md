---
tags: [calls, server-control, pms, ui]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# horizontal-call-mode и hide-incoming-call-notif — UI звонков

| Ключ | # | Default | Описание |
|---|---|---|---|
| `horizontal-call-mode` | 130 | false | «Включить горизонтальное отображение разметки» |
| `hide-incoming-call-notif` | 129 | false | «Скрывать уведомление при открытии экрана входящего звонка» |
| `ringtone-player-focus` | 258 | 0 | «Настройка аудио фокуса плеера рингтонов» (0=временный/1=постоянный/2=игнорировать) |
| `calls-android-ssttl` | 117 | — | «Время жизни общих настроек звонка (сек)» |

Все server-controlled. `horizontal-call-mode` — landscape UI звонка. `hide-incoming-call-notif` — UX-оптимизация. `ringtone-player-focus` — 3 режима аудио-фокуса.
