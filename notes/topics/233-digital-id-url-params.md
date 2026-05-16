---
tags: [digital-id, identity, photo, url-params, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/xh6.java
  - work/jadx_base/sources/defpackage/uq4.java
related:
  - "[[22-gost-digitalid-family]]"
  - "[[10-webapp-jsbridge]]"
---

# DigitalID — передача данных в URL

DigitalID — интеграция с российской системой цифровой идентификации.

## URL-параметры при переходе с DigitalID

```
<base_url>&digitalId=<id>&oid=<user_id>&photo=<base64_photo>
```

| Параметр | Что |
|---|---|
| `digitalId` | **ID цифровой идентификации** |
| `oid` | **ID пользователя** |
| `photo` | **фото пользователя** (Base64) |

## Механизм

1. Bundle с ключами `DIGITAL_ID`, `USER_ID`, `PHOTO_DATA` передаётся через Intent.
2. `xh6.c()` строит URL с параметрами `digitalId`, `oid`, `photo`.
3. Фото кодируется в Base64 (`Base64.encodeToString(byteArray, 2)`).

## Что важно

1. **Фото пользователя** передаётся в URL как Base64. Это означает, что при переходе по DigitalID ссылке фото пользователя передаётся в GET-параметре.

2. **`digitalId`** — ID цифровой идентификации. Это может быть ЕСИА (Госуслуги) или другая российская система.

3. `digitalIdTooltipShown` в SharedPreferences — показан ли тултип DigitalID.

## Сводка

DigitalID URL: `&digitalId=<id>&oid=<userId>&photo=<base64>`. Фото пользователя передаётся в GET-параметре URL.
