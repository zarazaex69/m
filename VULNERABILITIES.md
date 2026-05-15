# Max Messenger (ru.oneme.app) v26.15.3 — Vulnerability Summary

## Executive Summary

**53 vulnerability reports** covering **100+ individual security issues** discovered through static reverse engineering of Max messenger by VK.

| Severity | Count | Examples |
|----------|-------|---------|
| CRITICAL | 18 | 0-click RCE, no E2E encryption, trust-all QUIC, zip-slip |
| HIGH | 38 | Session theft, EXIF leak, draft sync, SVG heap overflow |
| MEDIUM | 25 | Race conditions, weak crypto, two-account isolation |
| LOW | 12 | SQL concat (internal), clipboard (clean), minor info leaks |

**Overall Security Rating: F (Failing)**

---

## Top 5 Attack Chains

### 1. Push → RCE (0-click, remote)
`Push(TamtamSpam)` → `deeplink(no validation)` → `WebApp(attacker bot)` → `intent://(ppk.java:729)` → `zip-slip(kv6.java:222)` → native lib overwrite → **code execution**

### 2. Sticker → RCE (0-click, remote)
Crafted animated sticker → auto-download → `libjlottie.so` (rlottie 2021, CVE-2021-28021+) or `libstatic-webp.so` (CVE-2023-4863) → **heap corruption → code execution**

### 3. MITM → Full Compromise (network)
Trust-all QUIC TrustManager + empty CertificatePinner + bundled Russian CA → **full MITM** → inject push notifications → trigger RCE chain above

### 4. Server → Total Surveillance (by design)
335 PmsKey flags + no E2E + draft sync + search index + contact sync + location push + VPN detection + GOST ready = **complete surveillance platform**

### 5. SVG → Heap Overflow (0-click, remote)
Crafted SVG in message → `ij7.java` regex parse → `nativeRenderSvg()` with `width*height*4` int32 overflow → **heap buffer overflow in native code**

---

## CRITICAL Vulnerabilities

1. **No E2E encryption** — all messages plaintext on server (vuln-14)
2. **0-click RCE via push** — full kill chain documented (vuln-13)
3. **Trust-all QUIC** — zero cert validation on HTTP/3 (vuln-08)
4. **Zip-Slip** — arbitrary file overwrite via ZIP extraction (vuln-04)
5. **LZ4 decompression bomb** — 2.1GB allocation from single packet (vuln-12)
6. **Push deeplink injection** — TamtamSpam type, no URI validation (vuln-10)
7. **SVG integer overflow** — heap corruption via crafted image (vuln-37)
8. **335 server-controlled flags** — complete unilateral control (vuln-28)
9. **MD5 API signing** — broken crypto for request authentication (vuln-34)
10. **Draft sync to server** — server reads what you type before sending (vuln-48)
11. **Server-forced location** — push requests location without consent (vuln-39)
12. **DevNull + Informer** — suppress evidence + push fake alerts (vuln-30)
13. **Open redirect via MyTracker** — mt_deeplink parameter (vuln-44)
14. **WebRTC no SDP validation** — remote SDP to native without checks (vuln-43)
15. **Force-update lockout** — server can block all communications (vuln-50)
16. **Remote audio quality increase** — server changes recording params (vuln-47)
17. **109 deeplink routes** — 6 without auth, parameter injection (vuln-51)
18. **NFC card cloning** — bots silently activate HCE (vuln-32)

---

## Privacy Violations

| What | How | Opt-out? |
|------|-----|----------|
| All messages | Plaintext on server, full-text indexed | No |
| Drafts | Synced in real-time before sending | No |
| Contacts | Phones+names in plaintext, batches of 100 | No (only deny permission) |
| Location | Server can request via push without UI | No |
| VPN usage | BAD_CONNECTION_ALERT telemetry | No |
| Search queries | Sent to server | No |
| Photos GPS | EXIF not stripped, leaks to recipients | No |
| Device info | Model, carrier, SIM, fingerprint | No |
| Online status | Always tracked, no hide option | No |
| Read receipts | Always sent, no disable | No |
| Typing | Server-controlled, no user toggle | No |
| Call audio | Transcription streams to server | No |
| Battery/CPU | Periodic slices with amperage | No |
| Sensors | Proximity, gyroscope, accelerometer | No |

---

## Surveillance Capabilities (Server-Side)

- **Read all messages** (no E2E)
- **Track VPN usage** and report
- **Request location** via silent push
- **Enable GOST crypto** remotely
- **Check host reachability** (censorship infrastructure)
- **Swap KWS model** to detect arbitrary keywords in calls
- **Increase recording quality** silently
- **Force app lockout** for targeted users
- **Suppress telemetry** to hide evidence (DevNull)
- **Push fake alerts** (Informer with non-dismissable banners)
- **Log sensitive data** remotely (log-full, log-sensitive flags)
- **Disable SSL validation** remotely (net-ssl-session-validate)
- **Track all contacts** in real-time
- **Read drafts** before user sends

---

## Comparison with Signal/Telegram

| Feature | Max | Signal | Telegram |
|---------|-----|--------|----------|
| E2E encryption (messages) | ❌ None | ✅ Always | ⚠️ Secret chats only |
| E2E encryption (calls) | ❌ SRTP only | ✅ Always | ✅ Always |
| Certificate pinning | ❌ None | ✅ Yes | ✅ Yes |
| EXIF stripping | ❌ No | ✅ Yes | ✅ Yes |
| Contact hashing | ❌ Plaintext | ✅ Truncated SHA-256 | ❌ Plaintext |
| Draft sync to server | ✅ Yes (privacy!) | ❌ No | ✅ Cloud drafts |
| VPN detection | ✅ Yes + telemetry | ❌ No | ❌ No |
| Server-controlled flags | 335 flags | Minimal | Moderate |
| Open source | ❌ No | ✅ Yes | ⚠️ Client only |
| Independent audit | ❌ No | ✅ Multiple | ⚠️ Partial |

---

## Statistics

- **APK**: Max 26.15.3 (versionCode 6695), package ru.oneme.app
- **Decompiled**: 23,795 Java files, 13 native libraries
- **Analysis**: 53 vulnerability reports, ~9000 lines of findings
- **Commits**: 24 detailed git commits
- **Tools**: jadx 1.5.5, apktool 2.7.0, radare2 6.1.4, grep/strings
- **Duration**: ~2 hours of automated analysis

---

*Analysis by zarazaex (openlibrecommunity) — May 2026*
