# Finding: Network Protocol

## Transport Layer
- **Primary**: Custom WebSocket over TLS (OkHttp-based)
- **Calls signaling**: WebSocket + WebTransport (QUIC) with WT→WS fallback
- **Extensions**: `permessage-deflate` compression
- **Keep-alive**: Background socket with configurable ping interval (`ping-background-interval`)

## Message Format: Protobuf Nano
Package: `ru.p027ok.tamtam.nano.Protos`

### Message attachment types:
```
PHOTO=2, VIDEO=3, AUDIO=4, STICKER=5, SHARE=6, APP=7,
CALL=8, MUSIC=9, FILE=10, CONTACT=11, PRESENT=12,
INLINE_KEYBOARD=13, LOCATION=14, DAILY_MEDIA=15, WIDGET=16, POLL=17
```

### Audio transcription states:
```
UNKNOWN=0, PROCESSING=1, SUCCESS=2, FAILED=3, NOT_SUPPORTED=4, MEDIA_NOT_READY=5
```

## Session Management
- `net-session-suppress-bad-disconnected-state` — подавление плохих состояний
- `net-ssl-session-validate` — валидация SSL сессии
- `net-session-rbc-enabled` — RBC (Request-Based Connection?)
- `subscription-timeout-seconds` — таймаут подписки на события
- `disconnect-timeout` — таймаут отключения

## Opcode System
Сообщения используют систему opcodes (как в WebSocket frames):
- `one.me.sdk.stat.OpcodeRegistrar` — регистрация opcodes
- `opcode-stat-config` — статистика по opcodes
- Opcodes используются для обновления URL медиа в реальном времени

## Custom DNS
- `one.me.net.dns.api.Dns` — собственный DNS resolver
- `net-client-dns-enabled` — включение кастомного DNS
- Позволяет обходить DNS-блокировки и контролировать резолвинг

## API Endpoints
- Base: `https://api.ok.ru` / `https://api.odnoklassniki.ru`
- Calls: `vchat.*` methods (startConversation, join, leave, etc.)
- Downloads: `https://download.max.ru/`
- Live: `https://vkvideo.ru/live`

## Certificate Pinning
Обнаружен CertificatePinner (OkHttp) — 104 строки конфигурации.
Пины привязаны к sha256 хешам сертификатов серверов.
