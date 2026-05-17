---
tags: [critical, webrtc, encryption, sorm, stun, turn, vpn-bypass, surveillance, server-control]
status: verified
severity: critical
sources:
  - findings/native/libjingle_peerconnection_so.strings.txt
  - work/jadx_base/sources/defpackage/rj5.java
  - work/jadx_base/sources/defpackage/oad.java
  - work/jadx_base/sources/org/webrtc/PeerConnection.java
related:
  - "[[43-libjingle-webrtc-custom-build]]"
  - "[[134-webrtc-field-trials]]"
  - "[[139-api-protocol-constants]]"
  - "[[537-quic-tls-bypass-mitm]]"
---

# 540. WebRTC: отключение шифрования + маркировка пакетов для СОРМ + принудительный обход VPN

## Суть

Кастомный WebRTC-форк VK/OK содержит три механизма, которых нет в upstream WebRTC:
1. **Переключатель отключения шифрования** RTP-трафика (скомпилирован в production)
2. **Серверно-управляемые маркеры** в STUN/TURN пакетах для идентификации звонков DPI-оборудованием
3. **Enum `VpnPreference`** для принудительного обхода VPN при звонках

## 1. Отключение шифрования RTP

В `libjingle_peerconnection_so.strings.txt`:
```
getDisableEncryption
Creating UnencryptedRtpTransport, becayse encryption is disabled.
Disabling encryption. This should only be done in tests.
```

- `getDisableEncryption` — Java-доступный getter на PeerConnection
- `UnencryptedRtpTransport` — полностью скомпилированный транспорт без шифрования
- Комментарий "only in tests" — но код в production binary

**Импликация**: сервер потенциально может отключить SRTP-шифрование для конкретного звонка, делая медиа-поток читаемым для любого сетевого посредника.

## 2. Маркировка пакетов для DPI/СОРМ

### STUN Custom Attribute
`rj5.java:198`:
```java
fieldTrials += "WebRTC-OK-StunCustomAttr/Enabled-" + pad.a + "/";
// pad.a = udpMarker (серверно-задаваемая строка)
```

### TURN Channel Data Mark
`rj5.java:200`:
```java
fieldTrials += "WebRTC-OK-TurnChannelDataMark/" + pad.b + "/";
// pad.b = tcpMarker (серверно-задаваемая строка)
```

- `pad.a` (udpMarker) и `pad.b` (tcpMarker) приходят с сервера через `ConversationFactoryInitParams`
- Маркеры внедряются в **каждый** STUN/TURN пакет звонка
- DPI-оборудование (СОРМ-2/3) может использовать эти маркеры для:
  - Идентификации конкретного звонка в потоке трафика
  - Селективного перехвата без расшифровки
  - Привязки сетевого потока к userId

## 3. Принудительный обход VPN

`PeerConnection.VpnPreference` enum (НЕ в upstream WebRTC):
```java
DEFAULT, ONLY_USE_VPN, NEVER_USE_VPN, PREFER_VPN, AVOID_VPN
```

- Устанавливается в `RTCConfiguration.vpnPreference` (`oad.java:568-570`)
- Контролируется сервером через `experimentsManager`
- `NEVER_USE_VPN` / `AVOID_VPN` — сервер **заставляет** звонок идти мимо VPN

**Импликация**: даже если пользователь включил VPN для защиты, сервер может заставить медиа-трафик звонка идти напрямую, раскрывая реальный IP и позволяя перехват.

## 4. Принудительный relay (p2p_forbidden)

`ApiProtocol.KEY_P2P_FORBIDDEN = "p2p_forbidden"`:
- Сервер устанавливает `forceRelayPolicy = true`
- **Весь** медиа-трафик идёт через TURN-серверы VK/OK
- На relay-сервере трафик доступен для записи

## Комбинированный сценарий СОРМ

```
1. Сервер устанавливает p2p_forbidden=true → медиа через relay
2. Сервер устанавливает VpnPreference=NEVER_USE_VPN → обход VPN
3. Сервер задаёт udpMarker="SORM_TARGET_12345" → маркировка пакетов
4. Сервер отключает шифрование (getDisableEncryption) → plaintext RTP
5. DPI-оборудование фильтрует по маркеру → перехватывает конкретный звонок
```

Или проще:
```
1. p2p_forbidden=true → всё через relay VK/OK
2. collect-debug-dump → запись на relay-сервере
3. Готово — прослушка без участия клиента
```

## Статус в 26.16.0

Без изменений. Все три механизма на месте.
