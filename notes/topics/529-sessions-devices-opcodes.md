---
tags: [sessions, devices, ws-opcodes, surveillance, auth]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/settings/devices/SettingsDevicesScreen.java
  - work/jadx_base/sources/defpackage/eyg.java
  - work/jadx_base/sources/defpackage/prg.java
  - work/jadx_base/sources/defpackage/utg.java
  - work/jadx_base/sources/defpackage/rtg.java
  - work/jadx_base/sources/defpackage/sed.java
  - work/jadx_base/sources/defpackage/red.java
  - work/jadx_base/sources/defpackage/owb.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[437-two-fa]]"
  - "[[439-qr-auth]]"
---

# SettingsDevicesScreen — управление сессиями + опкоды 96-99

`SettingsDevicesScreen` — экран управления активными сессиями (устройствами).

## Структура сессии (prg)

| Поле | Тип | Описание |
|---|---|---|
| `a` | `long` | ID сессии |
| `b` | `String` | client (название клиента/устройства) |
| `c` | `String` | info (информация о сессии) |
| `d` | `String` | **location** (геолокация сессии) |
| `o` | `boolean` | current (текущая сессия) |

`toString()`: `Session{=<id>, current=<bool>}`

## WS-опкоды управления сессиями

| Опкод | Класс | Описание |
|---|---|---|
| **96** | `utg` | Ответ: список сессий (`sessions[]`) |
| **97** | `rtg` | Ответ: завершение сессии (`token`) |
| **98** | `sed` | Ответ: запрос кода подтверждения |
| **99** | `red` | Ответ: регистрация (`profile`/`phone`/`token`) |

## Опкод 98 — sed (запрос кода)

Поля ответа:
- `token` — токен
- `tokenType` — тип токена
- `retries` — количество попыток
- `codeDelay` — задержка кода
- `codeLength` — длина кода
- `callDelay` — задержка звонка
- `requestType` — тип запроса: `SMS`/`CALL`/`CALL_DELAY`/`UNKNOWN`

## Опкод 99 — red (регистрация)

Поля ответа:
- `token` — токен
- `tokenType` — тип токена
- `profile` — профиль пользователя (`ul4`)
- `phone` — номер телефона (`Long`)

## Что важно

1. **`location` в сессии** — сервер возвращает геолокацию каждой активной сессии. Это означает, что сервер знает геолокацию каждого устройства пользователя.

2. **Опкод 96** — список сессий с `client`/`info`/`location`/`current`.

3. **Опкод 97** — завершение сессии по `token`.

4. **Опкод 98** — запрос кода подтверждения: SMS/CALL/CALL_DELAY.

5. **Опкод 99** — регистрация: `profile`/`phone`/`token`.

6. **QR-авторизация** — при нажатии "Добавить устройство" открывается `:qr-scanner?mode=2`.

## Сводка

`SettingsDevicesScreen`: сессии с `location`. Опкоды 96 (список сессий) / 97 (завершить) / 98 (код: SMS/CALL) / 99 (регистрация: profile+phone).
