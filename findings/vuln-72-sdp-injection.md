# VULN-72: SDP Injection Attack Surface in Max Messenger WebRTC

## Summary

The Max messenger WebRTC implementation receives SDP from signaling servers and passes it to native WebRTC code with **codec-level munging but no security-focused sanitization**. Multiple injection vectors exist.

## 1. SDP Reception from Signaling

SDP is received from the signaling server in three paths:

- **`z91.java:433`** (Group calls via `ra1` - "transmitted-data" notification):
  ```java
  JSONObject jSONObjectOptJSONObject3 = jSONObject3.optJSONObject("sdp");
  SessionDescription sessionDescription = new SessionDescription(
      SessionDescription.Type.fromCanonicalForm(jSONObjectOptJSONObject3.getString("type")),
      jSONObjectOptJSONObject3.getString("sdp"));
  ```

- **`lo5.java:1002`** (Direct P2P calls - "transmitted-data"):
  Same pattern — raw `getString("sdp")` from JSON, no validation.

- **`zng.java:357`** (Unified/SFU calls - "producer-updated"):
  ```java
  String string3 = jSONObject.getString("description");
  SessionDescription sessionDescription = new SessionDescription(SessionDescription.Type.OFFER, string3);
  ```

**Finding:** SDP string is extracted directly from JSON with zero content validation.

## 2. SDP Modification Before Native Code

The SDP passes through `oad.m15731e()` which calls `bol.m2875e()` — this only performs **codec preference reordering** (audio/video codec priority). It does NOT:
- Strip unknown `a=` attribute lines
- Validate SDP structure
- Remove potentially dangerous attributes
- Check for malformed lines

Additional munging in `ead.java` (case 0, local SDP path):
- Adds `a=animoji:2\r\n` custom attribute
- Modifies `dred`, `usedtx` fmtp parameters
- Adds simulcast RID lines
- Filters codecs to opus/red (audio) and optionally H265 (video)

**Finding:** Modifications are additive/reordering only. No security filtering exists.

## 3. Arbitrary Attributes (a= lines) Passed to Native

**YES** — arbitrary `a=` lines in the SDP string are passed directly to `nativeSetRemoteDescription()`. The code only parses:
- `a=rtpmap:` lines (for codec identification via `zzk.f88571f` pattern)
- `a=rtcp-fb:` and `a=fmtp:` lines (for codec association via `zzk.f88570e`)
- `a=ssrc:` lines (for SSRC extraction in `l7j.f36182y`)
- `a=mid:`, `a=simulcast:`, `a=rid:` (for simulcast processing)

All other `a=` lines (including attacker-injected ones) pass through unmodified to the native WebRTC SDP parser.

**Risk:** HIGH — attacker can inject arbitrary SDP attributes that reach the native C++ parser.

## 4. Exploitable SDP Munging

The `bol.m2875e()` codec filtering has a logic flaw:
- If the desired codec list is NOT found in SDP, the **original SDP is returned unmodified**
- The `m27489d()` check returns false → original string passes through

The `bol.m2872b()` fmtp modification:
- Parses `a=fmtp:` lines by splitting on `;` and `=`
- If parsing fails, returns original SDP unmodified

**Exploitation:** An attacker can craft SDP that bypasses codec filtering by using non-standard codec names, ensuring their full malicious SDP reaches native code.

## 5. Codec List Validation

Codec validation is **preference-based, not restrictive**:
- `bol.m2875e()` reorders codecs to prefer specified ones (opus, red, VP8, H265)
- If `f46607f` flag is set, non-opus/red audio codecs are removed
- If `f46609g` flag is set, H265 video codec is filtered
- Otherwise, **all codecs in the remote SDP are accepted**

**Risk:** MEDIUM — An attacker can force use of any codec supported by the local WebRTC library by placing it in the SDP. If a vulnerable codec decoder exists in the bundled WebRTC native library, it can be triggered.

Pattern used for H264 detection: `^a=rtpmap:(\d+) H264(/\d+)+[\r]?$` (in `oad.f46567l0`)

## 6. DTLS Fingerprint Injection

**No validation of `a=fingerprint:` lines exists in Java code.** The fingerprint line in SDP is passed directly to native WebRTC which handles DTLS negotiation.

The `onCallParticipantFingerprint` / `onOpponentFingerprintChanged` callbacks in `ConversationListenerProxy.java` and `MultiEventListener.java` handle a separate **application-level** fingerprint (long integer), NOT the DTLS fingerprint from SDP.

**Risk:** HIGH — A MITM on the signaling channel (or compromised server) can inject arbitrary `a=fingerprint:sha-256 <hash>` lines to perform DTLS interception. The app has no out-of-band fingerprint verification for the DTLS certificate.

## 7. SDP Size Bounds

**No size check exists anywhere in the Java layer.** The SDP string from `jSONObject.getString("sdp")` or `jSONObject.getString("description")` is unbounded.

The SDP is:
1. Split by `\r\n` into array (`str.split("\r\n")`) — unbounded array
2. Iterated with regex matching — O(n) per line
3. Passed as Java String to `nativeSetRemoteDescription`
4. JNI converts to C string for native WebRTC SDP parser

**Risk:** MEDIUM-HIGH — A very large SDP (millions of lines) could:
- Cause OOM in Java (String array allocation)
- Trigger stack overflow in native SDP parser (recursive parsing)
- DoS via CPU exhaustion in regex matching

## Attack Scenarios

### Scenario A: Native Parser Exploitation
Craft SDP with malformed `a=` lines containing binary/null bytes or extremely long values targeting buffer overflows in the native WebRTC SDP parser (C++ `webrtc::SdpDeserialize`).

### Scenario B: Codec Downgrade
Force negotiation of a codec with known vulnerabilities by placing only that codec in the remote SDP offer.

### Scenario C: DTLS MITM
Replace `a=fingerprint:` in SDP during signaling relay to intercept media stream.

### Scenario D: DoS via Oversized SDP
Send multi-megabyte SDP to crash the app or exhaust memory.

## Files Analyzed

| File | Role |
|------|------|
| `p000/z91.java:433` | Group call SDP reception from signaling |
| `p000/lo5.java:1002` | Direct call SDP reception |
| `p000/zng.java:357` | SFU/Unified call SDP reception |
| `p000/ead.java` | SDP munging before setLocal/setRemoteDescription |
| `p000/oad.java:497,614` | PeerConnectionClient SDP handling, m15731e codec preference |
| `p000/bol.java` | CodecPrefUtil — SDP codec reordering/filtering |
| `p000/zzk.java` | SDP m= section parser |
| `p000/l7j.java` | UnifiedPeerConnection, SSRC extraction |
| `org/webrtc/PeerConnection.java:980` | Native bridge — nativeSetRemoteDescription |
| `org/webrtc/SessionDescription.java` | Simple type+description string container |

## Severity: HIGH

No input validation, sanitization, or size bounding on SDP content received from signaling before it reaches native WebRTC code.
