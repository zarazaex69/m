---
tags: [protocol, ws, link-info, auth-qr, banners, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ayb.java
related:
  - "[[296-ws-opcodes-final]]"
  - "[[299-voters-org-livestream-ws]]"
---

# LINK_INFO + AUTH_QR_APPROVE + BANNERS_GET WS

## LINK_INFO (опкод 90) — информация о ссылке

| Поле | Что |
|---|---|
| `stickerSet` | набор стикеров |
| `startPayload` | **стартовый payload** |
| `chat` | данные чата |
| `user` | данные пользователя |
| `group` | данные группы |
| `message` | сообщение |
| `videoConference` | видеоконференция |
| `updateTime` | время обновления |
| `banners` | баннеры |
| `showTime` | время показа |
| `chatReactionsSettings` | настройки реакций |

## AUTH_QR_APPROVE (опкод 148) — авторизация через QR

| Поле | Что |
|---|---|
| `tokenAttrs` | **атрибуты токена** |

## Что важно

1. **`LINK_INFO`** — при переходе по ссылке сервер возвращает полную информацию: чат, пользователь, группа, сообщение, видеоконференция. Это предпросмотр ссылки.

2. **`AUTH_QR_APPROVE.tokenAttrs`** — атрибуты токена при QR-авторизации. Это механизм входа с другого устройства через QR-код.

3. **`LINK_INFO.startPayload`** — стартовый payload для ботов при переходе по ссылке.

## Сводка

`LINK_INFO`: stickerSet/startPayload/chat/user/group/message/videoConference/banners. `AUTH_QR_APPROVE`: tokenAttrs.
