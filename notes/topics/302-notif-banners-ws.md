---
tags: [protocol, ws, notif-banners, banners-get, server-control, informer]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ayb.java
related:
  - "[[48-informer-banners]]"
  - "[[239-informer-banner-db]]"
  - "[[296-ws-opcodes-final]]"
---

# NOTIF_BANNERS + BANNERS_GET WS

## NOTIF_BANNERS (опкод 149) — уведомление о баннерах

Обязательные поля:

| Поле | Что |
|---|---|
| `showTime` | **время показа** (обязательное) |
| `banners` | **список баннеров** (обязательное) |
| `updateTime` | **время обновления** (обязательное) |

## BANNERS_GET (опкод 150) — получить баннеры

Использует тот же обработчик что и `NOTIF_FOLDERS` (dq0.b).

## Что важно

1. **Все три поля обязательны** — сервер всегда передаёт `showTime`, `banners` и `updateTime`.

2. **`showTime`** — время, когда нужно показать баннер. Сервер управляет расписанием показа.

3. Это подтверждение [[48-informer-banners]] — баннеры управляются сервером через WS.

## Сводка

`NOTIF_BANNERS`: showTime(обязательное)/banners(обязательное)/updateTime(обязательное). Сервер управляет расписанием показа баннеров.
