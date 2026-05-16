---
tags: [calls-sdk, initialization, sha256, native, integrity, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/callssdk/CallsSdkInitializer.java
related:
  - "[[414-conversation-factory]]"
  - "[[390-client-capabilities]]"
---

# CallsSdkInitializer — инициализация SDK звонков

`CallsSdkInitializer` — инициализатор SDK звонков.

## Методы

| Метод | Что |
|---|---|
| `calculateMeta(context, ext, sizeLimit, filterByArch)` | **SHA-256 хэш нативных библиотек** |
| `initializeSessionSeed(context, seed, deviceId)` | **нативная инициализация сессии** |

## calculateMeta

Вычисляет SHA-256 хэш нативных библиотек из APK:

1. Открывает APK как ZIP
2. Фильтрует по архитектуре (`lib/<ABI>/`)
3. Фильтрует по расширению (`.so`)
4. Вычисляет SHA-256 хэш содержимого

## initializeSessionSeed

`native byte[] initializeSessionSeed(Context, byte[] seed, byte[] deviceId)` — нативная инициализация сессии с seed и deviceId.

## Что важно

1. **`calculateMeta`** — хэш нативных библиотек. Используется для верификации целостности SDK.

2. **`initializeSessionSeed`** — нативная инициализация с `deviceId`. Это означает, что deviceId используется для инициализации сессии звонков.

3. **SHA-256** — криптографический хэш для верификации.

## Сводка

`CallsSdkInitializer`: `calculateMeta(SHA-256 of .so files)` + `initializeSessionSeed(seed, deviceId)` (native).
