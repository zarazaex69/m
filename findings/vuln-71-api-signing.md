# API Request Signing — Max Messenger v26.15.3

## Mechanism (y78.java)

### Signature Computation

```
sig = hex(MD5(sorted_params_concatenated + session_secret_key))
```

### Implementation Details

1. **Algorithm**: MD5 (cryptographically broken since 2004)
2. **Construction**: `H(message || key)` — vulnerable to length-extension attacks
3. **Parameter sorting**: Parameters are sorted alphabetically before concatenation
4. **Secret**: `session_secret_key` from login response, appended AFTER all params
5. **Output**: 32-char hex string added as `sig` parameter

### Code Path (y78.java)

```java
// Constructor receives session_secret_key as 'str'
public y78(OutputStream outputStream, ArrayList arrayList, String str) {
    this.f82492c = str;  // session_secret_key
    MessageDigest messageDigest = MessageDigest.getInstance("MD5");
    this.f82494o = messageDigest;
    // All params written through zsa which feeds MessageDigest
}

// At end of request, compute sig:
byte[] bytes = this.f82492c.getBytes(cr2.f10376a);  // secret key bytes
MessageDigest messageDigest = this.f82494o;
messageDigest.update(bytes);  // append secret to hash state
byte[] bArrDigest = messageDigest.digest();  // compute MD5
m25752D0("sig");  // write "sig" param name
// write hex(digest) as value
```

### Sorted Parameter Insertion

The `mo17122w0()` method inserts parameters in sorted order using `str.compareTo()`. This ensures deterministic ordering for signature verification.

## Vulnerabilities

### 1. MD5 Collision (CRITICAL)

MD5 has been broken since 2004. Chosen-prefix collision attacks allow:
- Forging requests with different parameters but same signature
- Creating two requests with identical sig but different actions

### 2. Length-Extension Attack (CRITICAL)

The construction `MD5(params || secret)` is vulnerable to length-extension:
- Attacker sees: `params` and `sig = MD5(params || secret)`
- Attacker can compute: `MD5(params || padding || extra_params || secret)` 
- This allows APPENDING parameters to a signed request without knowing the secret

**However**: The sorted parameter requirement partially mitigates this — appended params would need to sort AFTER all existing params alphabetically.

### 3. Hardcoded App Key

```
app_key = CMBGJFMGDIHBABABA
```

This is the application identifier, not the signing secret. But combined with a stolen `session_secret_key` (stored in plaintext SharedPreferences), full request forgery is possible.

### 4. No Timestamp/Nonce

The signature does NOT include a timestamp or nonce. Captured signed requests can be replayed indefinitely until the session expires.

### 5. Request Forgery Scenario

1. Attacker extracts `session_secret_key` from SharedPreferences (root/physical access)
2. OR: Attacker performs MITM (no cert pinning) and captures signed requests
3. With length-extension: append `&zzz_param=malicious_value` (sorts last)
4. Compute new sig without knowing secret
5. Send forged request to api.ok.ru

## Remediation

1. Replace MD5 with HMAC-SHA256
2. Use proper HMAC construction: `HMAC(key, message)` not `H(message || key)`
3. Add timestamp + nonce to prevent replay
4. Implement certificate pinning to prevent MITM capture
5. Store session_secret_key in Android Keystore (not SharedPreferences)
