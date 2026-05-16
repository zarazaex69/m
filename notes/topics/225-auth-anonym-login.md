---
tags: [api, auth, anonymous-login, device-id, client-version]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nn.java
  - work/jadx_base/sources/defpackage/pqi.java
related:
  - "[[29-external-callback-ok-token]]"
  - "[[224-yag-shared-prefs-full]]"
---

# auth.anonymLogin — анонимная авторизация

`auth.anonymLogin` — API-метод для анонимной авторизации (используется calls SDK).

## Параметры запроса

```
session_data: {
  device_id: "<device_id>",
  version: 2,
  client_version: "android_8",
  client_type: "SDK_ANDROID"
}
```

| Поле | Что |
|---|---|
| `device_id` | ID устройства |
| `version` | версия протокола (2) |
| `client_version` | `android_8` — версия клиента |
| `client_type` | `SDK_ANDROID` — тип клиента |

## Что важно

1. **`client_version=android_8`** — фиксированная строка версии клиента. Не меняется между версиями приложения.

2. **`client_type=SDK_ANDROID`** — тип клиента. Это calls SDK, не основное приложение.

3. **`device_id`** — ID устройства передаётся при анонимной авторизации. Анонимность относительная.

4. Используется для анонимного входа в звонки по ссылке (см. [[144-hangup-join-api]]).

## Сводка

`auth.anonymLogin`: device_id/version=2/client_version=android_8/client_type=SDK_ANDROID. Анонимная авторизация для calls SDK с device_id.
