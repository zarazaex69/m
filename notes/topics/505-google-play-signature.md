---
tags: [google-play, signature-verification, sha256, in-app-review, security]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/til.java
related:
  - "[[450-fake-inapp-review]]"
  - "[[456-ssl-certificate-pinning]]"
---

# til — Google Play Signature Verification

`til` — проверка подписи Google Play Store для In-App Review.

## Логика

1. Получает подписи `com.android.vending` (Google Play Store)
2. Вычисляет SHA-256 каждой подписи
3. Сравнивает с ожидаемыми значениями

## Ожидаемые SHA-256 подписи

| Подпись | Что |
|---|---|
| `8P1sW0EPJcslw7UzRsiXL64w-O50Ed-RBICtay1g24M` | **production Google Play** |
| `GXWy8XF3vIml3_MfnmSmyuKBpT3B0dWbHRR_4cgq-gA` | **dev-keys/test-keys** |

## Что важно

1. **SHA-256 fingerprint** — проверяется подпись Google Play Store.

2. **`dev-keys`/`test-keys`** — отдельная подпись для dev-сборок.

3. Если подпись не совпадает → `"Play Store package certs are not valid"`.

4. Используется для Google In-App Review (но MAX использует FakeInAppReview).

## Сводка

`til`: SHA-256 подписи Google Play Store: `8P1sW0EP...` (production) / `GXWy8XF3...` (dev-keys).
