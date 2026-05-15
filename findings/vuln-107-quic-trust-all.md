# vuln-107: QUIC Transport Trust-All Certificate (CONFIRMED MITM)

## Severity: CRITICAL

## Summary

The QUIC transport layer uses the trust-all `qse` TrustManager, completely disabling server certificate validation. The code itself prints a security warning about this.

## Evidence

**File**: `p000/z68.java:826-829`

```java
tpi tpiVar = rseVar.f61387P0;
qse qseVar = new qse();  // Trust-ALL TrustManager!
String property = System.getProperty("tech.kwik.core.no-security-warnings");
if (property == null || !property.toLowerCase().equals("true")) {
    System.out.println("SECURITY WARNING: INSECURE configuration! Server certificate validation is disabled; QUIC connections may be subject to man-in-the-middle attacks!");
}
tpiVar.f67981s = qseVar;  // Set trust-all as the TLS trust manager
```

**File**: `p000/qse.java` (the trust-all TrustManager):

```java
public final class qse implements X509TrustManager {
    public final void checkClientTrusted(X509Certificate[] x509CertificateArr, String str) {
        // EMPTY - accepts all certificates
    }
    public final void checkServerTrusted(X509Certificate[] x509CertificateArr, String str) {
        // EMPTY - accepts all certificates
    }
    public final X509Certificate[] getAcceptedIssuers() {
        return null;
    }
}
```

## Impact

- **ALL QUIC connections** are vulnerable to MITM
- QUIC is the primary transport for the custom binary protocol (WebSocket fallback)
- Attacker on same network can:
  - Read ALL messages in transit
  - Inject fake messages into conversations
  - Steal session credentials
  - Inject malicious media (triggering native RCE bugs)
  - Modify server responses (enable/disable features)
  - Impersonate the server completely

## Context

- The kwik QUIC library (tech.kwik) is used for HTTP/3 transport
- The warning is printed to System.out (logcat) but NOT shown to user
- No system property `tech.kwik.core.no-security-warnings` is set to suppress it
- This is **production code** shipped to millions of users
- Combined with the 13 native RCE bugs, MITM → 0-click RCE is trivial

## Attack Scenario

1. Attacker on same WiFi/network
2. ARP spoof or rogue AP
3. Intercept QUIC connection (no cert validation)
4. Inject malicious Lottie sticker into message stream
5. Victim's app processes sticker → heap overflow → code execution
6. **Full device compromise, 0-click, no user interaction**

## Comparison

- Signal: Certificate pinning + key verification
- Telegram: Certificate pinning (MTProto)
- WhatsApp: Certificate pinning + Noise protocol
- **Max: ZERO certificate validation on primary transport**

This is the most critical finding in the entire audit. It makes ALL other vulnerabilities remotely exploitable by any network-adjacent attacker.
