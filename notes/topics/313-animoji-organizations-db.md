---
tags: [database, animoji, organizations, schema]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sk.java
  - work/jadx_base/sources/defpackage/t54.java
related:
  - "[[216-local-db-tables]]"
  - "[[219-battery-organizations-db]]"
---

# animoji + animoji_set + organizations DB схемы

## animoji

| Поле | Что |
|---|---|
| `id` | ID анимодзи |
| `update_time` | время обновления |
| `emoji` | **emoji** |
| `lottie_url` | URL Lottie-анимации |
| `lottie_play_url` | URL воспроизведения |
| `set_id` | ID набора |
| `icon_url` | URL иконки |

## animoji_set

| Поле | Что |
|---|---|
| `id` | ID набора |
| `name` | название |
| `icon_url` / `icon_lottie_url` | иконки |
| `update_time` | время обновления |
| `animoji_ids` | **ID анимодзи** |

## organizations (дополнение к [[219-battery-organizations-db]])

| Поле | Что |
|---|---|
| `id` | ID организации |
| `name` | название |
| `description` | описание |
| `parentId` | родительская организация |
| `folderTemplateId` | шаблон папки |
| `updateTime` | время обновления |
| `iconUrl` | **URL иконки** |

## Сводка

`animoji`: id/emoji/lottie_url/lottie_play_url/set_id. `animoji_set`: name/icon_url/icon_lottie_url/animoji_ids. `organizations`: +iconUrl.
