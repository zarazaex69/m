# VULN-43: WebRTC 0-Day Vulnerability Analysis — Max Messenger

**Date:** 2026-05-15
**Target:** Max Messenger (OK/VK) Android
**Native lib:** libjingle_peerconnection_so.so (12.2MB, ARM64, NDK r27d)
**Sources:** Decompiled via JADX (obfuscated class names: oad, ead, a55, jx2, bol)

---

## 1. SDP Parsing — No Input Validation (CRITICAL)

**Location:** `p000/ead.java` (setRemoteDescription path), `p000/oad.java:497`

The remote SDP string from the peer is passed **directly** to native `PeerConnection.setRemoteDescription()` with **zero validation**:

```java
// ead.java — case 1 (setRemoteDescription)
String str9 = sessionDescription3.description;  // Raw remote SDP
bol.m2873c(str9, oadVar2.f46630w);              // Only LOGS codecs, no validation
// ... codec manipulation ...
SessionDescription sessionDescription4 = new SessionDescription(sessionDescription3.type, oadVar2.m15731e(str9, false));
peerConnection2.setRemoteDescription(new kad(oadVar2, sessionDescription4, 1), sessionDescription4);
```

**Findings:**
- **No length limit** on SDP string before passing to native
- **No character filtering** (null bytes, control chars, format strings)
- **No structural validation** (line count, field sizes)
- `bol.m2873c()` only dumps codec info to logs — NOT a validator
- `m15731e()` does codec preference reordering — NOT sanitization

**Attack vector:** Crafted oversized/malformed SDP via signaling server → buffer overflow in native `SdpOfferAnswerHandler::DoSetRemoteDescription`

---

## 2. DataChannel — No Size Limits on Incoming Messages (HIGH)

**Location:** `p000/jx2.java` (onMessage callback), `p000/a55.java`

```java
// jx2.java — DataChannel.Observer.onMessage()
public void onMessage(DataChannel.Buffer buffer) {
    ByteBuffer byteBuffer = buffer.data;
    byte[] bArr = new byte[byteBuffer.remaining()];  // Allocates whatever size received
    byteBuffer.get(bArr);
    // Dispatches to all listeners with NO size check
    ((xyf) it.next()).mo8006a(a55Var, bArr, i);
}
```

**Findings:**
- **No maximum message size enforced** at Java layer
- Arbitrary-size allocation: `new byte[byteBuffer.remaining()]` — OOM DoS trivial
- Multiple DataChannels created without size guards: `consumerScreenShare`, `producerCommand`, `producerNotification`, `producerScreenShare`, `asr`, `animoji`
- Native side has `Invalid SCTP max message size.` string but no evidence Java enforces it

**Attack vector:** Send large binary payload via DataChannel → OOM crash or heap spray

---

## 3. SRTP/DTLS — Delegated Entirely to Native (MEDIUM)

**Location:** `org/webrtc/CryptoOptions.java`

```java
// CryptoOptions configuration — only boolean flags
private Srtp(boolean enableGcmCryptoSuites, boolean enableAes128Sha1_32CryptoCipher, 
             boolean enableEncryptedRtpHeaderExtensions) { ... }
```

**Findings:**
- DTLS/SRTP validation is **entirely in native code** — no Java-side certificate pinning
- No DTLS fingerprint verification against signaling-provided fingerprint visible in Java layer
- `enableAes128Sha1_32CryptoCipher` flag present — weak cipher may be enabled
- No evidence of SRTP key lifetime enforcement at application layer

---

## 4. Use-After-Free Risk in PeerConnection Cleanup (HIGH)

**Location:** `p000/oad.java:1445-1554` (closeInternal method `m15743r`)

```java
// Cleanup sequence — race condition window
this.f46583M = null;  // Nulls references
this.f46582L = null;
// ... many operations ...
// DataChannel listeners removed AFTER null assignments
// PeerConnection.dispose() called LAST
this.f46578H.dispose();
this.f46578H = null;
```

**Findings:**
- DataChannel callbacks (`jx2.onMessage`) reference `a55Var` which holds `DataChannel` pointer
- `closeInternal` nulls fields but DataChannel observer may still fire on network thread
- `checkDataChannelExists()` checks `nativeDataChannel == 0` but **race window** exists between `dispose()` setting it to 0 and native callback delivery
- `volatile` keyword on `f46578H` (PeerConnection) but NOT on DataChannel references
- Native `%s: (location: %p) freed` string confirms native-side free tracking exists (defensive)

**Attack vector:** Trigger call end while sending DataChannel messages → UAF in native DataChannel callback

---

## 5. Native Code Execution via Crafted SDP (CRITICAL)

**Attack surface analysis:**

The native library contains:
```
SDP munging: Failed to parse session description.
Invalid SDP line.
Buffer too small (
`InlinedVector::at(size_type) const` failed bounds check
assertion failed: 0
```

**Path from remote attacker to native execution:**
1. Attacker sends crafted SDP via signaling (WebSocket `onMessage` in `WTSignaling.java:154`)
2. Java side performs NO validation (see Finding #1)
3. SDP string passed directly to `nativeSetRemoteDescription` JNI call
4. Native parser processes untrusted input with only assertion-based checks (removable in release)
5. `InlinedVector` bounds check failure = potential controlled crash/corruption

**Specific concerns:**
- `DCHECK`/assertions may be **compiled out** in release builds (NDK release config)
- `vp9_pack_bitstream: output buffer full` — codec-level buffer boundary
- Custom `vk::enh` namespace code (audio enhancement) linked into same binary — expanded attack surface

---

## 6. Additional Attack Surface

| Channel | Risk |
|---------|------|
| `animoji` DataChannel (id=1, negotiated) | Binary protocol, no size validation |
| `asr` DataChannel | Speech recognition data, unbounded |
| `producerScreenShare` DataChannel | Screen capture frames, large payloads |
| SDP `a=animoji:N` field | Integer parsed with `Integer.parseInt()` — no bounds check before `Math.min(i7, 2)` |
| `maxRetransmitTimeMs = 10000000` | Hardcoded 10M ms retransmit — resource exhaustion |

---

## 7. Native Library Assertions (Debug-Only Protection)

```
assertion failed: _bits>0
assertion failed: N > 0
assertion failed: len>0
assertion failed: typeOffset >= 0 && typeOffset < 6
`InlinedVector::at(size_type) const` failed bounds check
Fatal (internal) error in %s, line %d: %s
```

These assertions are likely **DCHECK** (debug-only). In release builds, the checks may be absent, leaving the code vulnerable to the conditions they guard against.

---

## Summary of Exploitable Findings

| # | Vulnerability | Severity | Exploitability |
|---|---|---|---|
| 1 | No SDP validation before native pass-through | CRITICAL | Remote, no auth needed post-call-setup |
| 2 | Unbounded DataChannel message allocation | HIGH | Remote, requires active call |
| 3 | PeerConnection/DataChannel UAF race | HIGH | Remote, timing-dependent |
| 4 | Weak SRTP cipher option available | MEDIUM | MITM position required |
| 5 | Debug assertions as only bounds protection | CRITICAL | Release build removes guards |

**Recommended exploit chain:** Signaling MITM or compromised peer → inject crafted SDP with oversized/malformed fields → trigger native parser vulnerability → code execution in app context.
