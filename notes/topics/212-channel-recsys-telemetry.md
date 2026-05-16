---
tags: [telemetry, channels, recommendation, critlog, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/bp3.java
  - work/jadx_base/sources/defpackage/db0.java
  - work/jadx_base/sources/defpackage/sg3.java
  - work/jadx_base/sources/defpackage/m57.java
related:
  - "[[207-critlog-events]]"
---

# CHANNEL_RECSYS_FOLDER — телеметрия рекомендательной системы каналов

CritLog `CHANNEL_RECSYS_FOLDER` — события взаимодействия с рекомендованными каналами.

## События

| Событие | Что |
|---|---|
| `channel_folder_open` | открытие папки рекомендаций |
| `channel_folder_click` | клик на канал в папке |
| `channel_folder_follow` | подписка на канал из папки |
| `channel_folder_delete` | удаление папки рекомендаций |

## Параметры (для click/follow)

| Поле | Что |
|---|---|
| `channel_id` | ID канала |
| `channel_position` | **позиция канала в списке** |

## Что важно

1. **`channel_position`** — сервер знает, на какой позиции в списке рекомендаций находился канал, на который кликнул пользователь. Это данные для обучения рекомендательной системы.

2. **`channel_folder_follow`** — подписка на канал из рекомендаций логируется отдельно от обычной подписки.

3. Это полноценная система сбора данных для обучения рекомендательного алгоритма.

## Сводка

`CHANNEL_RECSYS_FOLDER`: channel_folder_open/click/follow/delete + channel_id/channel_position. Данные для обучения рекомендательной системы каналов.
