---
tags: [permissions, surveillance, telemetry, contacts, camera, microphone, location, storage, notification]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vdd.java
  - work/jadx_base/sources/defpackage/aed.java
related:
  - "[[472-log-controller-event-types]]"
  - "[[18-manifest-deep-dive]]"
---

# vdd + aed — Permission Tracking (отслеживание разрешений)

## vdd — Permission Analytics

Событие `PERMISSION.permission_changed_state`:

```json
{
  "pType": "permission_type",
  "pStatus": "allowed" | "denied"
}
```

## aed — Permission Groups

| Группа | Разрешения |
|---|---|
| `e`/`g` | READ_CONTACTS |
| `f` | READ_CONTACTS + WRITE_CONTACTS |
| `h` | WRITE_CONTACTS |
| `i` | RECORD_AUDIO |
| `j` | CAMERA + RECORD_AUDIO + READ_PHONE_STATE |
| `k` | CAMERA + RECORD_AUDIO |
| `l` | ACCESS_FINE_LOCATION + ACCESS_COARSE_LOCATION |
| `m` | POST_NOTIFICATIONS |
| `n` | CAMERA |
| `o` | READ_MEDIA_VIDEO + READ_MEDIA_IMAGES + READ_MEDIA_AUDIO |
| `p` | READ_MEDIA_VIDEO + READ_MEDIA_IMAGES + CAMERA |
| `q` | USE_FULL_SCREEN_INTENT |
| `r` | CAMERA + RECORD_AUDIO |

## Дополнительные проверки

| Метод | Что |
|---|---|
| `b()` | `canUseFullScreenIntent()` |
| `c()` | `isIgnoringBatteryOptimizations()` |
| `f()` | `areNotificationsEnabled()` |
| `g()` | READ_MEDIA_VISUAL_USER_SELECTED (Android 14+) |

## Что важно

1. **`permission_changed_state`** — каждое изменение разрешения логируется с типом и статусом.

2. **`isIgnoringBatteryOptimizations()`** — проверяется в startup_report.

3. **`READ_PHONE_STATE`** в группе `j` — запрашивается вместе с камерой и микрофоном.

4. **`ACCESS_BACKGROUND_LOCATION`** — запрашивается на Android 10+.

## Сводка

`vdd`: `PERMISSION.permission_changed_state {pType, pStatus}`. `aed`: 13 групп разрешений. Ключевые: CAMERA+RECORD_AUDIO+READ_PHONE_STATE / ACCESS_FINE_LOCATION+ACCESS_BACKGROUND_LOCATION / POST_NOTIFICATIONS.
