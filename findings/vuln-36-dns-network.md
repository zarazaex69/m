# VULN-36: DNS and Network Configuration Interception Vulnerabilities

## Summary

Max messenger has a custom DNS resolution layer (`one.me.net.dns.api.Dns`) with DNS-over-HTTPS via Google DNS, server-controlled proxy domain routing, and remotely configurable connection timeouts — all creating interception and censorship attack surfaces.

## 1. DNS Configuration Overview

### Grep Results (key findings):
```
p000/xl2.java:152: Uri.Builder().scheme("https").authority("dns.google.com").appendPath("resolve")
p000/mel.java:333: k54Var.m11978c(150, "one.me.net.dns.api.Dns")
p000/v48.java:192: " -> dns=true ("  / " -> dns=false ("
p000/tk6.java:189: "createConnection, reset dns after socket timeout"
p000/jf4.java:486: "updateInet, vpn changed to , reset dns ..."
p000/de9.java:207: lobVar.m13536j("cached_dns", 1)
p000/de9.java:212: u6d u6dVar3 = new u6d("dns_resolve", Long.valueOf(j))
PmsKey.java:516: PmsKey f88774netclientdnsenabled = new PmsKey("net-client-dns-enabled", 164)
```

## 2. Custom DNS (DoH) Implementation

**File:** `p000/xl2.java`

The app implements DNS-over-HTTPS using Google's DNS API:
```java
Uri.Builder builderAppendPath = new Uri.Builder()
    .scheme("https").authority("dns.google.com").appendPath("resolve");
builderAppendPath.appendQueryParameter(NAME_KEY, "api._endpoint.ok.ru.");
builderAppendPath.appendQueryParameter("type", String.valueOf(16));  // TXT record
uRLConnectionOpenConnection.setConnectTimeout(3000);
uRLConnectionOpenConnection.setReadTimeout(3000);
```

**Vulnerability:** This DoH query resolves `api._endpoint.ok.ru.` TXT records to discover API endpoints. It uses `dns.google.com` which:
- Is blockable by network operators (single point of failure)
- Uses plain `URLConnection` without certificate pinning for the DoH request itself
- 3-second timeout allows fallback to system DNS on timeout

### PmsKey `net-client-dns-enabled`

The DNS resolver is **server-controlled** via remote config. The server can disable custom DNS resolution at will, forcing all clients to use system DNS (exposing queries to ISP).

**Class:** `rtd.java` exposes `isNetClientDnsEnabled()` — a boolean toggle from server.

## 3. Proxy Domains Configuration

**PmsKey:** `proxy-domains` (index 6 in PmsKey enum)

**Routing logic in `zdi.java`:**
```java
public final boolean m26885d(String str) {
    for (String str2 : this.f86426f) {
        if (str.equals(str2)) return true;
        if (str.endsWith("." + str2)) return true;  // subdomain match
    }
    return false;
}
```

The proxy domain list is:
- Received from server at login (parsed in `ksg.java` from session response)
- Stored as `d60` object (list of strings)
- Updated dynamically via `moe.java` → `zdi.mo14266b()`
- Propagated to network layer via `ly4.mo13821c("proxyDomains", ...)`

**Vulnerability:** The server decides which domains route through proxy. This means:
- Server can selectively route/not-route traffic through proxy
- Domains NOT in the list go direct (DNS leaks to ISP)
- Server can change routing at any time without user consent

## 4. DNS Query Leakage to ISP

**CONFIRMED: DNS queries leak to ISP when:**

1. `net-client-dns-enabled` is set to `false` by server
2. DoH to `dns.google.com` times out (3s) → falls back to system DNS
3. VPN changes trigger DNS cache reset (`jf4.java`: "reset dns ...")
4. Socket timeout triggers DNS reset (`tk6.java`: "reset dns after socket timeout")
5. Domains not in `proxy-domains` list resolve via system DNS

**Evidence from `lc6.java`:** Uses `InetAddress.getAllByName(hostName)` — standard system DNS resolution with no DoH wrapper for the main connection path.

**Evidence from `v48.java`:** DNS resolution has a boolean result path — `dns=true` (resolved) or `dns=false` (failed, try socket fallback). No encrypted DNS fallback.

## 5. DNS Rebinding in WebView

**File:** `p000/xyc.java` (WebViewClient)

**Findings:**
- `shouldOverrideUrlLoading` delegates to `opk.mo6702y(uri)` — URL filtering exists but scope unclear
- **No `shouldInterceptRequest` override** — subresource requests are NOT filtered
- SSL errors: calls `sslErrorHandler.cancel()` normally, BUT has a debug path `mo6696i()` that calls `sslErrorHandler.proceed()` (accepts invalid certs)
- No explicit DNS rebinding protection (no IP validation after resolution)

**Vulnerability:** WebView loads URLs without validating that resolved IPs haven't changed between initial check and resource load. A DNS rebinding attack could:
1. Initial DNS → public IP (passes domain check)
2. TTL expires, re-resolves → internal IP (192.168.x.x)
3. WebView accesses internal network resources

## 6. Connection Timeouts (`conn-timeouts`)

**PmsKey:** `conn-timeouts` (index 140)

**Structure in `sch.java`:**
```java
public final class sch {
    long f63902a;   // connectTimeout
    long f63903b;   // initialReconnectDelay
    float f63904c;  // reconnectDelayScaleFactor
    long f63905d;   // maxReconnectDelay
}
```

**Vulnerability:** Server controls ALL timeout parameters:
- `connectTimeout` — server can set extremely high values, forcing slow connections
- `reconnectDelayScaleFactor` — exponential backoff multiplier controlled by server
- `maxReconnectDelay` — server can force clients to wait indefinitely before reconnecting
- Combined with `disconnect-timeout` and `subscription-timeout-seconds` PmsKeys

**Attack:** A compromised or malicious server can effectively DoS clients by setting:
- Very high `connectTimeout` (client waits forever)
- High `reconnectDelayScaleFactor` (exponential backoff grows rapidly)
- High `maxReconnectDelay` (client gives up reconnecting)

## 7. Domain Fronting / CDN Analysis

**Hardcoded domains found:**
- `https://api.ok.ru` — main API (AbstractC1161tp.java)
- `https://api.odnoklassniki.ru` — alternate API (aag.java)
- `https://sdk-api.apptracer.ru` — crash reporting
- `dns.google.com` — DoH resolver
- `api._endpoint.ok.ru.` — TXT record for endpoint discovery

**Proxy infrastructure:**
- `zdi.java` implements proxy routing with configurable proxy host (`f86425e`)
- Proxy uses port 443 (TLS) or 80 based on connection type
- `currentProxyList`, `pushProxyList`, `lastSuccessProxy` in user prefs (`yag.java`)
- `isDebugHostRotationEnabled` and `isDebugUaDnsEmulationEnabled` debug flags exist

**Vulnerability:**
- No domain fronting detected — direct connections to `.ok.ru` / `.odnoklassniki.ru`
- All domains are easily blockable by network operators
- Proxy list is server-provided (can be poisoned or blocked)
- No CDN diversity — single infrastructure point of failure
- VPN detection (`isVpn` field in `ksg.java`) allows server to change behavior for VPN users

## Risk Assessment

| Issue | Severity | Impact |
|-------|----------|--------|
| Server-controlled DNS toggle | HIGH | Server can force ISP DNS exposure |
| DoH fallback to system DNS | MEDIUM | Network timeout → DNS leak |
| Server-controlled proxy domains | HIGH | Selective traffic interception |
| Remote timeout manipulation | MEDIUM | Client DoS via slow timeouts |
| No DNS rebinding protection in WebView | MEDIUM | Internal network access |
| Single DoH provider (Google) | MEDIUM | Blockable, no fallback DoH |
| No domain fronting | LOW | Easy to block at network level |
| VPN detection by server | MEDIUM | Differential treatment of VPN users |

## Recommendations

1. DNS resolution should not be server-disableable — make DoH mandatory
2. Add multiple DoH providers as fallback (Cloudflare 1.1.1.1, etc.)
3. Pin certificates for DoH connections
4. Validate resolved IPs in WebView against rebinding
5. Cap maximum timeout values client-side regardless of server config
6. Don't expose VPN status to server
7. Implement domain fronting or use CDN for censorship resistance
