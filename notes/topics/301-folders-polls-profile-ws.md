---
tags: [protocol, ws, folders-get, polls, profile, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mv6.java
related:
  - "[[286-ws-opcodes-additional]]"
  - "[[243-chat-folder-db]]"
---

# FOLDERS_GET + GET_POLL_UPDATES + NOTIF_PROFILE WS

## FOLDERS_GET (опкод 142) — получить папки

| Поле | Что |
|---|---|
| `allFilterExcludeFolders` | **исключить папки из фильтра «Все»** |
| `folderSync` | синхронизация папок |
| `folders` | **список папок** |
| `foldersOrder` | **порядок папок** |

## GET_POLL_UPDATES (опкод 156) — обновления опроса

| Поле | Что |
|---|---|
| `polls` | **обновления опросов** |

## NOTIF_PROFILE (опкод 136) — уведомление об изменении профиля

| Поле | Что |
|---|---|
| `profile` | **данные профиля** |

## Что важно

1. **`FOLDERS_GET.allFilterExcludeFolders`** — флаг исключения папок из фильтра «Все». Сервер управляет видимостью папок.

2. **`FOLDERS_GET.foldersOrder`** — порядок папок. Сервер знает порядок папок пользователя.

3. **`GET_POLL_UPDATES.polls`** — обновления опросов. Сервер уведомляет об изменениях в опросах.

## Сводка

`FOLDERS_GET`: allFilterExcludeFolders/folderSync/folders/foldersOrder. `GET_POLL_UPDATES`: polls. `NOTIF_PROFILE`: profile.
