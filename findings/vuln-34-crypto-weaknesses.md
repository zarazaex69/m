# VULN-34: Cryptographic Weaknesses in Max Messenger

**Severity:** Medium-High  
**Component:** Multiple crypto subsystems  
**Source:** `~/max/decompiled/jadx/base/sources/p000/`

---

## 1. Crypto Algorithms Inventory

All algorithms found in the codebase:

| Algorithm | Usage | File(s) |
|-----------|-------|---------|
| AES/CBC/PKCS7Padding | Media decryption, AndroidKeyStore biometric | C1187ue.java, C1224ve.java, zql.java, ubk.java |
| AES/ECB/NoPadding | QUIC header protection | C1261we.java:95 |
| AES/GCM/NoPadding | QUIC packet encryption | C1261we.java:291 |
| ChaCha20 | QUIC header protection (alt) | C1261we.java:114 |
| ChaCha20-Poly1305 | QUIC packet encryption (alt) | C1261we.java |
| MD5 | **API request signing (sig computation)** | y78.java:51 |
| MD5 | File integrity checksums | bj6.java:101 |
| SHA-1 | Certificate pinning, misc hashing | wa8.java, tcf.java, q9k.java, kp5.java |
| SHA-256 | Device ID hashing, TLS, general hashing | CallsSdkInitializer.java, t2m.java, jq4.java, til.java, tlc.java, kc5.java |
| HmacSHA256 | HKDF key derivation (QUIC/TLS) | kcb.java:94, xr0.java |
| HmacSHA384/512 | TLS handshake verification | xr0.java |
| SHA256withECDSA | TLS signatures | xr0.java |
| SHA384withECDSA | TLS signatures | xr0.java |
| SHA512withECDSA | TLS signatures | xr0.java |
| RSASSA-PSS | TLS signatures | xr0.java |

---

## 2. ECB Mode Usage (CRITICAL)

**File:** `p000/C1261we.java:95`

```java
this.f76750j = Cipher.getInstance("AES/ECB/NoPadding");
this.f76750j.init(1, new SecretKeySpec(this.f76749i, "AES"));
```

**Context:** Used for QUIC header protection (HP mask generation). This is actually a **standard and correct** usage per RFC 9001 §5.4.3 — AES-ECB on a single block for header protection is the specified approach. **Not a vulnerability in this context**, but worth noting.

---

## 3. MD5 for API Request Signing (VULNERABILITY)

**File:** `p000/y78.java:51`  
**Severity:** HIGH

```java
MessageDigest messageDigest = MessageDigest.getInstance("MD5");
```

**The `sig` computation flow:**
1. `C0615mo.java` creates a `y78` writer, passing `session_secret_key` as the `str` parameter
2. All API request parameters are written through the `y78` writer, which feeds them into an MD5 `MessageDigest`
3. At the end (`m25755u0`), the `session_secret_key` bytes are appended to the MD5 state
4. The MD5 digest is hex-encoded and appended as the `sig` parameter

```java
// y78.java lines 274-278
byte[] bytes = this.f82492c.getBytes(cr2.f10376a);  // session_secret_key
MessageDigest messageDigest = this.f82494o;          // MD5 instance
messageDigest.update(bytes);
byte[] bArrDigest = messageDigest.digest();
m25752D0("sig");  // writes "sig" = hex(md5(params + secret))
```

**Impact:** The API request authentication uses `sig = MD5(sorted_params + session_secret_key)`. MD5 is cryptographically broken:
- Length extension attacks possible
- Collision attacks practical
- An attacker who observes requests could potentially forge signatures

---

## 4. Static/Hardcoded IVs

**Files:** `C1187ue.java`, `C1224ve.java`

The IV (`f70371c` / `f73613c`) is passed as a constructor parameter from the caller. It is stored as a final field and reused for the lifetime of the object:

```java
public C1187ue(p55 p55Var, byte[] bArr, byte[] bArr2) {
    this.f70369a = p55Var;
    this.f70370b = bArr;   // AES key
    this.f70371c = bArr2;  // IV - passed from caller, reused
}
```

**Risk:** If the same key+IV pair is reused across multiple media files (which is likely for cached/stored media), this breaks CBC semantic security. The IV source needs to be verified — if it comes from the server per-file, it may be acceptable. If it's derived from a static value, it's a vulnerability.

---

## 5. Key Derivation

**HKDF (RFC 5869) implementation found:**

**File:** `p000/kcb.java` (lines 512-570)

```java
// HKDF-Extract (m12220s):
Mac mac = o18.m15507a();  // HmacSHA256
mac.init(secretKeySpec);   // salt as key
return mac.doFinal(bArr2); // IKM as data

// HKDF-Expand (m12219r):
// Standard HKDF-Expand with counter byte
```

**Assessment:**
- QUIC/TLS layer: Uses proper HKDF with HmacSHA256 ✓
- Media encryption (C1187ue/C1224ve): **Raw key passed directly** — no KDF observed. Key is used as-is from `byte[] bArr` parameter.
- API signing: **No KDF** — `session_secret_key` is used directly as the MD5 suffix.
- No PBKDF2 or scrypt found anywhere — no password-based key derivation.

---

## 6. Session Secret Signing Mechanism (API Authentication)

**Flow:**

1. Login response (`dw8.java:316`) returns `session_secret_key` (field `f80404c` in `xm9`)
2. For each API request, `C0615mo.java` passes `session_secret_key` to `y78` writer
3. `y78` computes: `sig = hex(MD5(param1=val1&param2=val2&...&session_key=XXX + session_secret_key))`
4. Parameters are sorted alphabetically before hashing (via `v04.m23270q0` with comparator)
5. The `sig` is appended to the request body

**Weakness:** MD5-based HMAC-like construction (but NOT actual HMAC — it's `MD5(message || key)` which is vulnerable to length-extension attacks).

---

## 7. Deprecated/Weak Algorithms

| Algorithm | Location | Usage | Risk |
|-----------|----------|-------|------|
| **MD5** | y78.java | API request signing | HIGH - broken hash for auth |
| **MD5** | bj6.java | File checksum | LOW - integrity only |
| **MD5** | DigestScheme.java | HTTP Digest auth | MEDIUM - legacy auth |
| **SHA-1** | wa8.java, tcf.java, q9k.java, kp5.java | Cert pinning, misc | LOW-MEDIUM |

No DES, RC4, or RC2 usage found in application code.

---

## 8. Auth Hash Computation (Login)

**File:** `p000/dw8.java` (lines 300-340)

The login response is parsed into `xm9` object containing:

```java
xm9.f80402a = uid
xm9.f80403b = session_key
xm9.f80404c = session_secret_key
xm9.f80405d = auth_token
xm9.f80406e = api_server
xm9.f80407f = auth_hash
```

The `auth_hash` is received from the server as part of the login response — it is **not computed client-side**. It appears to be a server-generated token used for session validation. The client stores and potentially sends it back for re-authentication.

---

## Summary of Vulnerabilities

| # | Finding | Severity | Impact |
|---|---------|----------|--------|
| 1 | MD5 used for API request signing (`sig`) | **HIGH** | Signature forgery, request tampering |
| 2 | `MD5(message \|\| key)` construction instead of HMAC | **HIGH** | Length extension attacks |
| 3 | No KDF for media encryption keys | **MEDIUM** | Key material quality depends entirely on server |
| 4 | Potential IV reuse in AES-CBC media decryption | **MEDIUM** | Breaks semantic security if key+IV reused |
| 5 | SHA-1 still used for certificate operations | **LOW** | Collision attacks theoretically possible |
| 6 | No PBKDF2/scrypt — no local password-based encryption | **INFO** | All crypto keys come from server |

---

## Recommendations

1. **Replace MD5 signing with HMAC-SHA256** — The `sig` computation should use `HMAC-SHA256(session_secret_key, sorted_params)` instead of `MD5(params || key)`
2. **Use proper HMAC construction** — Current `H(m||k)` is vulnerable to length extension; switch to standard HMAC
3. **Ensure unique IVs per encryption** — Media encryption should use random IVs prepended to ciphertext
4. **Deprecate SHA-1** — Migrate certificate operations to SHA-256
5. **Consider local key protection** — Add PBKDF2/Argon2 for any locally-stored secrets
