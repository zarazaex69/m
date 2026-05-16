---
tags: [energy-saving, server-control, pms, calls, permissions]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[74-background-mode-onelog]]"
---

# Energy saving и call-permissions PmsKey

| Ключ | # | Default | Описание |
|---|---|---|---|
| `energy-saving-request-interval` | 110 | 0 | «Время в секундах, через которое будет осуществлена проверка на отключение режима энергосбережения» |
| `energy-saving-bottom-sheet` | 109 | false | «Включение шторки энергосбережения» |
| `call-permissions-interval` | 23 | 0 | «Время в секундах, через которое будет осуществлена проверка на включенные уведомления» |

## Что важно

1. **`energy-saving-request-interval`** — сервер задаёт, как часто клиент проверяет, включён ли режим энергосбережения. При `0` — старая логика. При `>0` — периодическая проверка.

2. **`energy-saving-bottom-sheet`** — server-gated показ bottomsheet с предложением отключить режим энергосбережения. Это механизм давления на пользователей, у которых включён режим экономии батареи (что мешает фоновой работе MAX).

3. **`call-permissions-interval`** — периодическая проверка разрешений для звонков. При `0` — старая логика.

## Сводка

`energy-saving-bottom-sheet` — server-gated bottomsheet с предложением отключить режим энергосбережения. `energy-saving-request-interval` — интервал проверки. Сервер давит на пользователей с режимом экономии батареи.
