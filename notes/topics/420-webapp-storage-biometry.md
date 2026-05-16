---
tags: [webapp, secure-storage, device-storage, biometry, jsbridge, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vmk.java
  - work/jadx_base/sources/defpackage/lmk.java
  - work/jadx_base/sources/defpackage/k9k.java
related:
  - "[[419-webapp-js-events]]"
  - "[[10-webapp-jsbridge]]"
---

# WebApp Storage + Biometry — детали реализации

## Хранилище (vmk.java)

Два типа хранилища для мини-приложений:

### SecureStorage (зашифрованное)

| Событие | Внутреннее имя | Что |
|---|---|---|
| `WebAppSecureStorageSaveKey` | `secure_storage_save_key` | сохранить ключ |
| `WebAppSecureStorageGetKey` | `secure_storage_get_key` | получить ключ |
| `WebAppSecureStorageClear` | `secure_storage_clear` | очистить |

### DeviceStorage (незашифрованное)

| Событие | Внутреннее имя | Что |
|---|---|---|
| `WebAppDeviceStorageSaveKey` | `device_storage_save_key` | сохранить ключ |
| `WebAppDeviceStorageGetKey` | `device_storage_get_key` | получить ключ |
| `WebAppDeviceStorageClear` | `device_storage_clear` | очистить |

### Ограничения

- Ключ: максимум **128 байт**
- Значение: максимум **4000 байт**
- Ошибки: `too_many_keys`, `too_large_key`, `too_large_value`

## Биометрия (k9k.java)

| Событие | Что |
|---|---|
| `WebAppBiometryGetInfo` | информация о биометрии |
| `WebAppBiometryRequestAccess` | запросить доступ |
| `WebAppBiometryUpdateToken` | обновить токен |
| `WebAppBiometryRequestAuth` | аутентификация |
| `WebAppBiometryOpenSettings` | открыть настройки |

Ошибки: `access_denied`, `not_found`, `not_supported`, `token_not_found`.

## Что важно

1. **SecureStorage** — зашифрованное хранилище для мини-приложений. Ключ до 128 байт, значение до 4000 байт.

2. **`WebAppBiometryRequestAuth`** — мини-приложения могут запрашивать биометрическую аутентификацию (отпечаток пальца, лицо).

3. **`token_not_found`** — биометрия использует токены. Мини-приложение может хранить токен в SecureStorage.

## Сводка

`vmk`: SecureStorage(128/4000 байт) + DeviceStorage. `k9k`: BiometryRequestAuth/BiometryRequestAccess/BiometryUpdateToken.
