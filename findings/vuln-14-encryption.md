# VULN-14: Encryption Implementation Analysis — Max Messenger

## Summary

Max messenger (ru.ok.tamtam) has **NO end-to-end encryption for messages**. Messages are protected only by TLS in transit. Calls use standard WebRTC with SRTP-DTLS (no custom E2E layer). Local data encryption exists only for WebApp bot storage and biometric keys.

## 1. Message Encryption: TLS-Only

**Finding: Messages are NOT encrypted client-side before sending.**

- The `ChatCommandExecutorImpl.sendMessage()` sends plaintext JSON via signaling:
  ```java
  dp7VarM10809b.f13316a.put("message", text);
  pbhVar.m17818d(dp7VarM10809b, ...);
  ```
- No `Cipher`, `SecretKey`, or encryption call exists in the message send path.
- No Signal Protocol, Double Ratchet, pre-keys, or session ciphers found anywhere in codebase.
- No `encrypt`/`decrypt` methods in the messaging pipeline (`ru/p027ok/tamtam/` package).
- Protection relies entirely on TLS transport (OkHttp with certificate pinning via SHA-256).

**Risk: HIGH** — Server has full access to all message content. No forward secrecy for messages.

## 2. Secret Chat Feature

- Resource IDs exist: `secret_chat_control_additional_option_1` through `_4` (in `l5f.java`).
- **No cryptographic implementation found** — only UI resource references. The feature appears to be either placeholder/removed or server-side only (no client-side E2E crypto).

## 3. Key Exchange Protocol

**Finding: No application-level key exchange exists.**

- `DiffieHellman`, `ECDH`, `KeyAgreement`, `X25519`, `Curve25519` — all results are TLS cipher suite definitions in `wu3.java` (OkHttp/BoringSSL TLS config), not application-level key exchange.
- No custom key agreement protocol for messaging.

## 4. WebRTC Call Encryption (SRTP)

**Finding: Standard SRTP-DTLS, no custom E2E encryption layer for calls.**

- Uses standard WebRTC library (`org.webrtc.*`) with `PeerConnection`, `RtpSender`, `RtpReceiver`.
- `FrameEncryptor`/`FrameDecryptor` interfaces exist in the WebRTC library but are **never used** by the app code (no `setFrameEncryptor`/`setFrameDecryptor` calls outside `org/webrtc/`).
- TURN/STUN servers parsed via `TurnStunParser` — standard ICE connectivity.
- Call media is protected by WebRTC's built-in DTLS-SRTP (key exchange via DTLS handshake during call setup).
- No additional encryption layer on top of SRTP.

**Risk: MEDIUM** — SRTP-DTLS is adequate but the server controls signaling and could theoretically MITM the DTLS handshake (no out-of-band verification).

## 5. Local Data Encryption

### WebApp Bot Storage (`vbk.java`, `bpk.java`)
- AES/CBC/PKCS7Padding with AndroidKeyStore-backed keys.
- IV prepended to ciphertext, extracted on decrypt: `new IvParameterSpec(bytes[0..blockSize])`.
- `setRandomizedEncryptionRequired(true)` — IV is random per encryption. ✓
- Keys stored in AndroidKeyStore with alias pattern `webapp_s_key_{userId}_{botId}`.

### Biometric Crypto (`zql.java`)
- AES/CBC/PKCS7Padding via AndroidKeyStore key `"androidxBiometric"`.
- Used only for biometric prompt authentication, not message encryption.

### File Cache Encryption (`vbk.m23540n`)
- AES with `SecretKeySpec` + `IvParameterSpec` (16-byte IV read from file header).
- Used for ExoPlayer media cache, not messages.

## 6. Key Storage

- AndroidKeyStore used for: biometric keys, WebApp bot encryption keys.
- **No key storage for message encryption** (because none exists).
- RSA public key in `wa8.java` is for Firebase/GCM push token identity, not message encryption.

## 7. Key Verification Mechanism

**Finding: NONE.**

- No safety numbers, key fingerprints, QR code verification, or any out-of-band key verification mechanism exists.
- No way for users to verify they're not being MITM'd on calls.

## 8. Crypto Weaknesses

| Issue | Location | Severity |
|-------|----------|----------|
| No E2E encryption for messages | Entire messaging pipeline | **CRITICAL** |
| No E2E for calls (SRTP-DTLS only, server-controlled) | WebRTC calls | HIGH |
| MD5 used for file checksum validation | `MLFeatureDelegate` (`hy7.MD5`) | LOW |
| SHA-1 used for push key fingerprint | `wa8.java` line 71 | LOW |
| AES/CBC (not AES-GCM) for local storage | `vbk.java` | LOW |
| No certificate transparency or public key pinning enforcement visible | TLS config | MEDIUM |

## Conclusion

Max messenger provides **zero end-to-end encryption** for messages. The server has full plaintext access to all communications. Calls use standard WebRTC SRTP-DTLS which provides hop-by-hop encryption but the server controls the signaling path. The "secret chat" feature referenced in resources has no corresponding cryptographic implementation in the client — it is either cosmetic or server-side only. This is a fundamental architectural gap, not a bug.
