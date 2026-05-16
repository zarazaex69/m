---
tags: [stickers, server-control, pms, bot, suggest]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
  - work/jadx_base/sources/defpackage/u0d.java
  - work/jadx_base/sources/defpackage/e2h.java
  - work/apktool_base/smali/ugj.smali
related:
  - "[[03-pms-server-flags]]"
  - "[[11-state-bots-and-content-policy]]"
  - "[[20-ws-protocol-opcodes]]"
---

# Стикеры — server-controlled pipeline (stickers-botid, STICKER_SUGGEST, STICKER_CREATE)

Стикерная система MAX полностью управляется сервером: от размеров до бота-создателя.

## PmsKey стикеров

| Ключ | # | Что |
|---|---|---|
| `max-favorite-stickers` | 37 | лимит избранных стикеров |
| `max-favorite-sticker-sets` | 38 | лимит избранных стикерсетов |
| `min-sticker-size` | 39 | минимальный размер стикера (px) |
| `max-sticker-size` | 40 | максимальный размер стикера (px) |
| `default-sticker-size` | 41 | размер по умолчанию |
| `welcome-sticker-ids` | 81 | список ID стикеров для приветственного сообщения |
| `stickers-db-batch` | 226 | размер батча при записи стикеров в БД |
| `stickers-botid` | 272 | **ID бота для создания стикеров** (из `e2h.java`: «Id бота для создания стикеров») |
| `sticker-set-edit-enabled` | 273 | разрешить редактирование стикерсетов |

## WS-опкоды

| Опкод | Enum # | WS code | Что |
|---|---|---|---|
| `STICKER_UPLOAD` | 83 | 81 | загрузить стикер |
| `STICKER_CREATE` | 106 | 193 | создать стикерсет |
| `STICKER_SUGGEST` | 107 | 194 | **предложить стикер** |

## Что важно

1. **`stickers-botid` (#272)** — ID бота, через которого происходит создание стикеров. Это означает, что весь flow создания стикеров идёт через бот-аккаунт, ID которого задаётся сервером. Сервер может в любой момент сменить бота-создателя.

2. **`STICKER_SUGGEST(107, WS 194)`** — клиент отправляет на сервер запрос «предложи стикер» (вероятно, по контексту сообщения или emoji). Сервер возвращает предложения. Это означает, что **контекст набираемого текста или emoji уходит на сервер** для получения стикер-предложений. Аналогично тому, как работает Telegram's sticker suggestion.

3. **`welcome-sticker-ids` (#81)** — список ID стикеров, которые автоматически отправляются новому пользователю при первом входе. Сервер контролирует, какие стикеры получит новый пользователь.

4. **`app.suggest.stickers.status`** (smali `ugj.smali:734`) — SharedPreferences ключ, хранящий статус функции предложения стикеров. Это локальный флаг, который может быть включён/выключен.

## Скептический разбор

- Стикер-suggest — стандартная фича мессенджеров. Telegram делает то же самое.
- Что специфично: `stickers-botid` — сервер контролирует, через какой бот-аккаунт создаются стикеры. Если этот бот имеет расширенные права (как в `webapp-exc`), это может быть вектором.
- `STICKER_SUGGEST` отправляет контекст на сервер — это нормально для cloud-based suggestion, но означает, что сервер видит emoji/текст, который пользователь набирает.

## Сводка

Стикерная система: 9 PmsKey управляют размерами, лимитами и ботом-создателем. `STICKER_SUGGEST(194)` отправляет контекст на сервер для получения предложений. `welcome-sticker-ids` — server-pushed список стикеров для новых пользователей.
