---
tags: [security, root-detection, surveillance, emulator-detection]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/r04.java
related:
  - "[[337-crash-report-device-info]]"
  - "[[32-root-detection]]"
---

# Root Detection — обнаружение root-доступа

`r04.java` — обнаружение root-доступа и эмулятора.

## Проверки

| Проверка | Что |
|---|---|
| `Build.PRODUCT == "sdk"` или `"google_sdk"` | **эмулятор** |
| `Build.TAGS` содержит `"test-keys"` | **тестовые ключи** (root) |
| `/system/app/Superuser.apk` существует | **Superuser** |
| `/system/xbin/su` существует | **su бинарник** |

## Логика

```
if (emulator OR Build.TAGS contains "test-keys") → false (не root)
if (/system/app/Superuser.apk exists) → true (root)
if (/system/xbin/su exists AND NOT emulator) → true (root)
```

## Что важно

1. **Эмулятор не считается рутированным** — специальная обработка для эмуляторов.

2. **`test-keys`** — устройства с тестовыми ключами подписи считаются рутированными.

3. Результат передаётся в `isRooted` поле crash-репорта (см. [[337-crash-report-device-info]]).

## Сводка

Root detection: Build.PRODUCT(sdk/google_sdk) + Build.TAGS(test-keys) + /system/app/Superuser.apk + /system/xbin/su.
