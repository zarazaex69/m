---
tags: [database, battery, visibility, foreground-background, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/bw0.java
  - work/jadx_base/sources/defpackage/cw0.java
related:
  - "[[316-battery-snapshot]]"
  - "[[219-battery-organizations-db]]"
---

# EnrichedBatterySnapshot — обогащённый снимок батареи

`EnrichedBatterySnapshot` (`bw0`) — снимок батареи с дополнительными метаданными.

## Поля

| Поле | Что |
|---|---|
| `snapshot` | `BatterySnapshot` (cpu/battery/network) |
| `seqId` | **порядковый номер** |
| `visibility` | **видимость приложения** |

## Visibility (cw0)

| Значение | Что |
|---|---|
| `FG` | **Foreground** — приложение на переднем плане |
| `BG` | **Background** — приложение в фоне |

## Что важно

1. **`visibility`** — каждый снимок батареи помечается, было ли приложение на переднем плане или в фоне. Это позволяет анализировать потребление в разных режимах.

2. **`seqId`** — порядковый номер снимка. Позволяет восстановить хронологию.

3. Это дополнение к [[316-battery-snapshot]] — полная структура: BatterySnapshot + seqId + FG/BG.

## Сводка

`EnrichedBatterySnapshot`: snapshot(BatterySnapshot)/seqId/visibility(FG/BG).
