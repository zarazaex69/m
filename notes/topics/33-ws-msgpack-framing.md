---
tags: [ws, protocol, msgpack, framing, native-libs]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/j8h.java
  - work/jadx_base/sources/defpackage/kxa.java
  - work/jadx_base/sources/org/msgpack/core/**
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[19-fcm-push-payload]]"
---

# WS framing — MessagePack как сериализация бинарного протокола

WS-сессия с `api.oneme.ru` (см. [[20-ws-protocol-opcodes]]) использует **MessagePack** (`org.msgpack.core.*`) как формат сериализации полезной нагрузки.

## Подтверждение

`defpackage/j8h.java` — основной парсер payload-структур, импортирует `org.msgpack.core.MessageIntegerOverflowException`, `org.msgpack.core.buffer.MessageBuffer`, `org.msgpack.core.buffer.OutputStreamBufferOutput`. Методы:

- `j8h.S(kxa)` — читает MessagePack array header, возвращает int (длина массива).
- `j8h.V(kxa, defaultString)` — читает MessagePack string, возвращает String.
- `j8h.Q(kxa, defaultInt)` — читает int.
- `j8h.e(...)` — собирает map/array из пар.

`kxa` — обёртка вокруг `MessageUnpacker` (msgpack reader). Используется во всех cei-payload парсерах (см. [[19-fcm-push-payload]] §6 — раз FCM push содержит MessagePack-encoded данные? — нужно проверить отдельно; но WS-payload точно MessagePack).

## Что это значит

1. **WS-кадры — бинарный MessagePack**, не JSON. Отладка/перехват требует MessagePack-aware tooling (mitmproxy + msgpack-плагин или Wireshark с msgpack-dissector).
2. Структура запроса/ответа: `array[opcode_short, request_id_or_payload, ...]` — типичный паттерн для msgpack-RPC. Полная структура frame-а — отдельная тема.
3. Типы значений:
   - opcode (short) — uint16 в первом элементе массива
   - long-идентификаторы (chat_id, user_id) — varint64
   - строки — utf-8 length-prefix
   - вложенные map / array — рекурсивно

## Скептический разбор

MessagePack — стандартная сериализация, не «странная». Он используется в Pinterest API, Redis, Aerospike, многих других системах. Сам по себе выбор протокола не несёт privacy-impact.

Что **важно**:
- Бинарный MessagePack делает casual-инспекцию трафика сложнее, чем JSON. Это часть «облегчённой обфускации» (раньше TamTam использовал тот же формат).
- На-уровне WSS (TLS-обёртка) — стандартная HTTP/1.1 wss:// connection к `api.oneme.ru`. Без E2E (см. [[FINDINGS]] §14).
- Серверный флаг `net-ssl-session-validate` (PmsKey #246, см. [[03-pms-server-flags]]) управляет валидацией TLS-сессии — то есть сервер технически может попросить клиент «отключить проверку sessions», что ослабляет canon TLS.

## Что осталось проверить

- Точная структура frame-а (msgpack-array[N] + headers).
- Сжатие (zstd? gzip?) поверх msgpack — `libzstd.so` есть в lib/ (см. [[09-native-libs]]); вероятно используется для крупных payload (синхронизация истории чатов).
- Шифрование payload-ов внутри MessagePack за пределами TLS — отдельная сессия с реальным трафиком.
