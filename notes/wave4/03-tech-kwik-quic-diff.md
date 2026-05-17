---
tags: [quic, tls, cert-bypass, webtransport, kwik, wave4]
status: complete
severity: HIGH (confirms and extends topic 537)
sources:
  - work/jadx_base/sources/tech/kwik/ (33 files)
  - work/jadx_base/sources/defpackage/tpi.java (TLS 1.3 client engine)
  - work/jadx_base/sources/defpackage/rse.java (QUIC connection)
  - work/jadx_base/sources/defpackage/xse.java (transport parameters)
  - work/jadx_base/sources/defpackage/vse.java (frame parser)
  - work/jadx_base/sources/defpackage/z68.java (HTTP/3 client builder)
  - work/jadx_base/sources/defpackage/qse.java (empty TrustManager)
  - work/jadx_base/sources/defpackage/nw3.java (ClientHello)
  - work/jadx_base/sources/defpackage/ox7.java (TLS extension parser)
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/wt/WTSignaling.java
related:
  - "[[537-quic-tls-bypass]]"
  - "[[543-reconnect-ws-server-host-takeover]]"
---

# 03 — tech.kwik QUIC Stack Audit (Wave 4)

## Summary

The `tech.kwik` package in MAX is a **stripped-down, obfuscated fork** of the open-source [Kwik QUIC library](https://github.com/ptrd/kwik) by Peter Doornbosch. The 33 files under `tech.kwik.*` are **only exception/error classes** — all actual logic is in obfuscated `defpackage` classes. The implementation is structurally faithful to upstream Kwik (standard frame types, standard transport parameters, standard TLS 1.3 handshake) with **one critical modification**: certificate chain validation is unconditionally disabled for all QUIC connections.

**Verdict**: No custom frame types, no custom transport parameters, no telemetry injection in handshake, no connection migration abuse. The sole finding is the **unconditional SSL bypass** (extends topic 537 with full mechanism analysis). No new topic needed beyond 537.

---

## 1. Structural Mapping: MAX vs Upstream Kwik

### 1.1 Package Structure

| MAX Package | Upstream Kwik Equivalent | Content |
|---|---|---|
| `tech.kwik.core.impl` | `net.luminis.quic.impl` | 9 exception classes (TransportError, ProtocolError, etc.) |
| `tech.kwik.core.crypto` | `net.luminis.quic.crypto` | 1 class (MissingKeysException) |
| `tech.kwik.core.generic` | `net.luminis.quic.generic` | 2 classes (integer encoding exceptions) |
| `tech.kwik.agent15` | `net.luminis.tls` (Agent15 TLS lib) | 1 base + 11 alert classes |
| `tech.kwik.flupke.impl` | `net.luminis.http3.impl` (Flupke HTTP/3) | 3 exception classes |
| `tech.kwik.flupke.core` | `net.luminis.http3.core` | 1 class (HttpError) |
| `tech.kwik.flupke.webtransport.impl` | `net.luminis.http3.webtransport` | 1 class (BufferedStreamsLimitExceededException) |
| `tech.kwik.qpack.impl` | `net.luminis.qpack` | 2 exception classes |

**Total**: 33 files, all exception/error classes. All logic is in ~50 obfuscated `defpackage` classes.

### 1.2 Key Obfuscated Classes Identified

| Obfuscated | Upstream Equivalent | Role |
|---|---|---|
| `rse` | `ClientConnectionImpl` | Main QUIC client connection (957 lines) |
| `tpi` | `TlsClientEngine` | TLS 1.3 handshake engine |
| `nw3` | `ClientHello` | ClientHello message construction |
| `ox7` | `TlsMessageParser` / `HandshakeMessage` | TLS extension parsing |
| `xse` | `QuicTransportParametersExtension` | Transport parameters encode/decode |
| `oyi` | `TransportParameters` | Transport parameters data class |
| `vse` | `QuicPacket` (subclass) | QUIC frame parser |
| `z68` (6-arg ctor) | `Http3ClientConnection` | HTTP/3 + WebTransport client |
| `qse` | — (NOT in upstream) | **Empty X509TrustManager** |
| `k9d` | `PathChallengeFrame` | PATH_CHALLENGE frame |
| `r9d` | `PathResponseFrame` | PATH_RESPONSE frame |
| `bjj` | `Version` | QUIC version enum (v1=0x1, v2=0x6b3343cf) |
| `jw3` | `QuicConnectionSettings` | Connection settings |
| `uf4` | `Keys` / `ConnectionSecrets` | TLS key management |
| `img` | `SenderImpl` | Packet sender |
| `gme` | `Receiver` | Packet receiver |
| `wpd` (ctor 2,c8) | — (custom) | Hostname verifier delegating to WTSignaling |

---

## 2. Transport Parameters Analysis

### 2.1 Client-Sent Parameters (`rse.g()`)

Standard RFC 9000 parameters only:
- `max_idle_timeout` (ID 1): from `jw3.a` (60000ms)
- `initial_max_data` (ID 4): from `jw3.d` (2500000)
- `initial_max_stream_data_bidi_local` (ID 5): from `jw3.f` (250000)
- `initial_max_stream_data_bidi_remote` (ID 6): from `jw3.f` (250000)
- `initial_max_stream_data_uni` (ID 7): from `jw3.e` (250000)
- `initial_max_streams_bidi` (ID 8): from `jw3.c` (100)
- `initial_max_streams_uni` (ID 9): from `jw3.b` (103)
- `active_connection_id_limit` (ID 14): from `jw3.g` (2)
- `max_udp_payload_size` (ID 3): from `jw3.h` (1500)
- `max_datagram_frame_size` (ID 32): 65535 (only if QUIC v2)
- `initial_source_connection_id` (ID 15): auto-generated
- `version_information` (ID 17): if QUIC v2

**No custom transport parameters (ID >= 0x3000). No telemetry injection.**

### 2.2 Server Parameters Parsing (`xse.e()`)

Parses standard IDs 0–17 + 32. Unknown parameters are **logged and skipped** (not rejected):
```java
// Known named unknowns (logged by name):
64    → "multi-path"
4183  → "loss-bits"
5950  → "discard"
10930 → "grease-quic-bit"
29015 → "timestamp"
29016 → "timestamp"
29659 → "version-negotiation"
56858 → "delayed-ack"
16741339 → "version-information-4-13"
4278378010 → "delayed-ack"
```

All are well-known IETF draft extensions. **No proprietary/custom parameter IDs.**

### 2.3 Server Parameter Validation (`rse.m()`)

Proper validation present:
- `max_udp_payload_size >= 1200` ✓
- `ack_delay_exponent <= 20` ✓
- `max_ack_delay < 16384` ✓
- `active_connection_id_limit >= 2` ✓
- `stateless_reset_token.length == 16` ✓
- `initial_source_connection_id` matches actual ✓
- `original_destination_connection_id` matches actual ✓
- Preferred address validation (non-zero CID) ✓
- Version negotiation validation ✓

---

## 3. Frame Types Analysis

### 3.1 Frame Parser (`vse.java`)

All parsed frame types are **standard RFC 9000 + RFC 9221**:

| Byte | Frame | Class | Standard |
|---|---|---|---|
| 0 | PADDING | `e6d` | RFC 9000 |
| 1 | PING | `fod` | RFC 9000 |
| 2-3 | ACK | `r7` | RFC 9000 |
| 4 | RESET_STREAM | `xpf` | RFC 9000 |
| 5 | STOP_SENDING | `zv9` | RFC 9000 |
| 6 | CRYPTO | `h15` | RFC 9000 |
| 7 | NEW_TOKEN | `vwb` | RFC 9000 |
| 8-15 | STREAM | `pyh` | RFC 9000 |
| 16 | MAX_DATA | `z45` | RFC 9000 |
| 17 | MAX_STREAM_DATA | `zv9` | RFC 9000 |
| 18-19 | MAX_STREAMS | `aw9` | RFC 9000 |
| 20 | DATA_BLOCKED | `z45` | RFC 9000 |
| 21 | STREAM_DATA_BLOCKED | `zv9` | RFC 9000 |
| 22-23 | STREAMS_BLOCKED | `uzh` | RFC 9000 |
| 24 | NEW_CONNECTION_ID | `fwb` | RFC 9000 |
| 25 | RETIRE_CONNECTION_ID | `wrf` | RFC 9000 |
| 26 | PATH_CHALLENGE | `k9d` | RFC 9000 |
| 27 | PATH_RESPONSE | `r9d` | RFC 9000 |
| 28-29 | CONNECTION_CLOSE | `le4` | RFC 9000 |
| 30 | HANDSHAKE_DONE | `nx7` | RFC 9000 |
| 48-49 | DATAGRAM | `i65` | RFC 9221 |

Unknown frame types → `throw new TransportError(8)` (FRAME_ENCODING_ERROR).

**No custom frame types. No exfiltration channel via frames.**

### 3.2 Frame Type Validation (`ci7.java`)

Proper per-packet-type frame filtering:
- Initial packets (`hl8`): only allowed frame types checked
- Handshake packets (`px7`): restricted set
- 1-RTT packets (`bxk`): full set minus restricted

---

## 4. TLS / Certificate Validation — THE FINDING

### 4.1 Mechanism (extends topic 537)

The certificate bypass is **unconditional and hardcoded** in the HTTP/3 client builder:

```java
// z68.java, constructor (String, int, Duration, w68, zu3, nl9), line ~783
rse rseVar = new rse(host, port, millis, jw3Var, bjjVar, szlVar, arrayList, zu3Var);
tpi tpiVar = rseVar.P0;

// 1. Create empty TrustManager
qse qseVar = new qse();

// 2. Print warning (to System.out, invisible on Android)
String property = System.getProperty("tech.kwik.core.no-security-warnings");
if (property == null || !property.toLowerCase().equals("true")) {
    System.out.println("SECURITY WARNING: INSECURE configuration! ...");
}

// 3. Assign to TLS engine — disables PKIX chain validation
tpiVar.s = qseVar;

// 4. Set hostname verifier (first to always-true, then to WTSignaling delegate)
tpiVar.t = new lhd(5);  // always returns true
tpiVar.t = new wpd(2, c8Var);  // delegates to WTSignaling$nal$1 → bch
```

### 4.2 Impact in TLS Handshake (`tpi.b()`)

```java
// CertificateVerify handler:
if (this.s != null) {
    // EMPTY — skips ALL certificate chain validation
    // No PKIX check, no root CA check, no revocation check
} else {
    // Standard PKIX validation (NEVER reached)
    TrustManagerFactory.getInstance("PKIX")...checkServerTrusted(...)
}

// Only remaining check:
if (!this.t.verify(this.g, this.q)) {
    throw new CertificateUnknownAlert("servername does not match");
}
```

### 4.3 Hostname Verification (`wpd` → `WTSignaling$nal$1` → `bch`)

The hostname verifier in `WTSignaling$nal$1.verify()`:
1. Gets expected hostname from `bchVar.a.invoke()` (dynamic, from signaling)
2. Gets alternative hostnames from `bchVar.b.invoke()` (list, from signaling)
3. Checks SAN or Subject DN of the certificate

This means: **any certificate with the correct SAN/CN will be accepted, regardless of who signed it** (self-signed, expired, wrong CA — all pass).

### 4.4 No Opt-Out Path

- `tpi.s` is NEVER set to null after initialization
- No PmsKey/server flag controls this behavior
- No conditional logic (no `if (debug)` or `if (configFlag)`)
- The `System.getProperty("tech.kwik.core.no-security-warnings")` only suppresses the warning message, not the bypass itself

### 4.5 Scope

The QUIC/HTTP3 client is used exclusively by **WebTransport signaling** (`ru.ok.android.externcalls.sdk.wt.WTSignaling`) for the calls SDK. This means:
- All QUIC-based call signaling connections have no certificate chain validation
- A network-level attacker (ISP, corporate proxy, rogue WiFi) can MITM call signaling with a self-signed cert matching the expected hostname
- Combined with topic 543 (WS server host takeover), the server can redirect QUIC connections to arbitrary hosts

### 4.6 Comparison with Upstream Kwik

In upstream Kwik, the `hostnameVerifier` and custom `TrustManager` are **optional builder parameters** intended for testing. The default is standard PKIX validation. MAX has **hardcoded** the insecure configuration with no way to enable real validation.

---

## 5. Connection Migration

### 5.1 PATH_CHALLENGE/PATH_RESPONSE (`k9d.java`)

Standard implementation: on receiving PATH_CHALLENGE, immediately responds with PATH_RESPONSE containing the same 8-byte data. No additional validation, no address change logic — this is correct per RFC 9000 §8.2.

### 5.2 Preferred Address

Server can send `preferred_address` transport parameter (ID 13) with IPv4/IPv6 addresses. MAX parses it (`xse.e()`, `oyi.k`) but there's no visible migration logic that acts on it in the client. The `disable_migration` flag (`oyi.j`) is parsed and stored but not checked before any migration action (because the client doesn't initiate migration).

**No connection migration vulnerability found.**

---

## 6. TLS Extensions in ClientHello

Extensions sent (`nw3` constructor + `rse.n()`):
1. `server_name` (SNI) — standard
2. `supported_versions` (TLS 1.3 only) — standard
3. `supported_groups` (secp256r1) — standard
4. `signature_algorithms` (6 standard algorithms) — standard
5. `key_share` (secp256r1) — standard
6. `quic_transport_parameters` (ID 57 for v1, 65445 for drafts) — standard
7. `psk_key_exchange_modes` — standard (if PSK)
8. `early_data` (`n06`) — standard (if 0-RTT)

**No custom/proprietary TLS extensions. No telemetry in handshake.**

---

## 7. Hardcoded Values

| Value | Location | Purpose | Risk |
|---|---|---|---|
| `maxIdleTimeout = 60000` | `z68` ctor → `jw3.a` | 60s idle timeout | None |
| `maxOpenUniStreams = 103` | `z68` ctor → `jw3.b` | Uni streams limit | None |
| `maxOpenBidiStreams = 100` | `z68` ctor → `jw3.c` | Bidi streams limit | None |
| `maxConnectionBufferSize = 2500000` | `z68` ctor → `jw3.d` | ~2.5MB buffer | None |
| `maxStreamBufferSize = 250000` | `z68` ctor → `jw3.e/f` | ~250KB per stream | None |
| `activeConnectionIdLimit = 2` | `z68` ctor → `jw3.g` | Standard | None |
| `maxUdpPayloadSize = 1500` | `z68` ctor → `jw3.h` | Standard MTU | None |
| `ALPN = "h3"` | `z68` ctor, `rse.e1` | HTTP/3 | None |
| `cipher = TLS_AES_128_GCM_SHA256` | `z68` ctor | Standard cipher | None |
| `namedGroup = secp256r1` | `rse.n()` | Standard curve | None |

All values are standard/reasonable. No suspicious hardcoded IPs, tokens, or backdoor parameters.

---

## 8. What Was NOT Found

- ❌ Custom QUIC transport parameters (ID >= 0x3000)
- ❌ Custom frame types for exfiltration
- ❌ Telemetry injection in QUIC handshake
- ❌ Custom retry packet logic
- ❌ Connection migration without validation (client doesn't migrate)
- ❌ Server-pushed config that controls cert validation (it's always off)
- ❌ Hidden data channels in QUIC layer
- ❌ Non-standard QUIC versions
- ❌ Weakened crypto (standard AES-128-GCM, ECDHE-secp256r1)

---

## 9. Risk Assessment

| Finding | Severity | New? |
|---|---|---|
| Unconditional PKIX bypass for all QUIC/WebTransport connections | **HIGH** | Extends topic 537 with full mechanism |
| Self-signed certs accepted if hostname matches | **HIGH** | Part of above |
| Warning printed to System.out (invisible on Android) | LOW | Cosmetic |
| No opt-out / no PmsKey to enable real validation | MEDIUM | Confirms intentional design |

---

## 10. Conclusion

The Kwik QUIC stack in MAX is a **faithful port** of upstream Kwik with standard protocol behavior. The only modification is the **unconditional disabling of certificate chain validation** (`qse` empty TrustManager), which was already identified in topic 537. This audit confirms:

1. The bypass is **hardcoded** (not configurable, not server-controlled)
2. It affects **all QUIC/WebTransport connections** (call signaling)
3. There is **no code path** that enables real PKIX validation
4. The hostname verifier provides minimal protection (attacker needs cert with correct SAN)
5. No additional backdoor primitives exist in the QUIC layer itself

**No new topic needed.** Topic 537 should be updated with the mechanism details from this audit (specifically: the `tpi.b()` bypass logic, the `wpd`→`WTSignaling$nal$1` hostname verifier chain, and the scope limited to WebTransport/calls signaling).
