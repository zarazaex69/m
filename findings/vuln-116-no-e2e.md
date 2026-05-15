# vuln-116: NO End-to-End Encryption — Messages Readable by Server and MITM

## Severity: CRITICAL

## Summary

Max messenger has **ZERO end-to-end encryption** for messages. All messages are:
1. Sent in plaintext over the custom binary protocol
2. Protected ONLY by TLS transport encryption
3. TLS is broken (trust-all TrustManager on QUIC transport)
4. Therefore: **ALL messages readable by any MITM attacker**

## Evidence

### What's NOT present:
- ❌ No `encryptMessage()` / `decryptMessage()` functions
- ❌ No Diffie-Hellman / ECDH key exchange for message keys
- ❌ No Signal Protocol / Double Ratchet
- ❌ No per-message encryption keys
- ❌ No key verification UI (safety numbers / QR codes)
- ❌ No SQLCipher (database is plaintext)
- ❌ No encrypted message fields in protobuf schema

### What IS present:
- ✓ TLS for transport (but trust-all on QUIC = broken)
- ✓ AES/CBC for media file decryption (key from SERVER, not E2E)
- ✓ AndroidKeyStore AES for biometric auth (local only)
- ✓ "Secret chat" UI strings exist but NO crypto implementation found

### AES Usage Analysis:
- `C1187ue.java` — CipherInputStream for server-encrypted media files
- `C1224ve.java` — Same pattern, different media type
- `zql.java` — AndroidKeyStore for biometric prompt (not messages)
- `ubk.java` — NFC WebApp data (local encryption)

### Database:
- Room database with 42 tables — **NO encryption**
- Messages stored in plaintext SQLite
- No PRAGMA key, no SQLCipher dependency
- Extractable via root/ADB/backup

## Attack Chain

```
1. Attacker on same WiFi network
2. MITM the QUIC connection (trust-all, no cert validation)
3. Read ALL messages in real-time (no E2E, plaintext in protocol)
4. Inject fake messages (no per-message authentication)
5. Modify messages in transit (no integrity protection)
```

## "Secret Chats"

String resources `secret_chat_control_additional_option_1` through `_4` exist,
suggesting a "secret chat" UI feature. However:
- NO cryptographic implementation found
- NO key exchange for secret chats
- Likely just disappearing messages (timer-based deletion, not encryption)
- Server retains messages even when "deleted" (notDeleteMessageFromDb flag)

## Comparison

| Messenger | E2E Default | E2E Optional | Protocol |
|-----------|-------------|--------------|----------|
| Signal | ✓ | N/A | Signal Protocol |
| WhatsApp | ✓ | N/A | Signal Protocol |
| Telegram | ❌ | ✓ (Secret Chats) | MTProto + DH |
| **Max** | **❌** | **❌** | **Custom (no crypto)** |

Max is WORSE than Telegram — at least Telegram offers optional E2E secret chats
with verified DH key exchange. Max has nothing.

## Impact

- Server (VK/Mail.ru) can read ALL messages
- Any MITM can read ALL messages (due to trust-all)
- Law enforcement can access messages via server
- No forward secrecy — compromise of transport key exposes all history
- Database forensics trivially recovers all messages
- Combined with `calls-sdk-log-audio` PmsKey: full surveillance capability
