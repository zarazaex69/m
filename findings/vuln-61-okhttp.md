# VULN-61: OkHttp Interceptors & HTTP Client Configuration

## Summary

Analysis of OkHttp interceptors and HTTP client configuration in Max messenger (v26.15.3) reveals several security concerns including verbose User-Agent leaking device info, remotely-toggleable debug mode that enables request/response body logging, aggressive retry logic, and redirect handling that follows redirects to arbitrary domains when proxy mode is active.

## OkHttp Version

- **OkHttp 4.12.0** (confirmed via default User-Agent fallback string `okhttp/4.12.0` in `lcf.java:247` and `z31.java:94`)

## 1. Custom Interceptors Identified

### a) Network Request Interceptor (`ydi.java` — implements `sp8`)
- **Purpose**: Adds User-Agent header, handles proxy routing, processes HTTP redirects (300-303, 307, 308)
- **Behavior**: When proxy mode is active (`f86423c == true`), it rewrites request URLs to route through a proxy host and handles redirect responses manually

### b) MethodListenerInterceptor (`ru.ok.android.externcalls.sdk.api.interceptor`)
- **Purpose**: Listens for specific API method responses (e.g., `auth.anonymLogin`)
- **Behavior**: Intercepts API chain, checks method name, notifies registered listeners with response data

### c) ExecutionTimeInterceptor (`ru.ok.android.externcalls.sdk.api.interceptor`)
- **Purpose**: Measures API call execution time for statistics
- **Behavior**: Records timing data via `ApiStats`

### d) LoggingApiRequestDebugger (`ru.ok.android.externcalls.sdk.api.log`)
- **Purpose**: Logs API request/response bodies for debugging
- **Behavior**: **Logs full request params and response bodies** — see Section 3

## 2. OkHttpClient Builder Defaults (`yac.java`)

| Setting | Value | Security Implication |
|---------|-------|---------------------|
| `followRedirects` (f82774f) | `true` | Follows HTTP redirects |
| `followSslRedirects` (f82776h) | `true` | Follows HTTPS→HTTP downgrades |
| `retryOnConnectionFailure` (f82777i) | `true` | Auto-retries on failure |
| Connect timeout | 10000ms | Standard |
| Read timeout | 10000ms | Standard |
| Write timeout | 10000ms | Standard |
| Protocols | HTTP/2, HTTP/1.1 | Standard |

## 3. Request/Response Body Logging (DATA LEAK RISK)

**File**: `LoggingApiRequestDebugger.java`

**Finding**: The debugger logs full request parameters and response bodies:

```java
// Logs request params
this.log.log(TAG, "API request " + transformUriForLog(request.getUri()) + " start with params " + getParams(request));

// Logs full response body on success
this.log.log(TAG, "API request " + ... + " success with response " + eraseSecrets(strMo10973j0));

// Logs full response body on failure  
this.log.log(TAG, "API request " + ... + " failed with response " + eraseSecrets(strMo10973j0));
```

**Mitigation attempt**: A `RequestSecretEraser` strips known secret fields (`token`, `auth_data`, `credential`, `auth_token`, `session_data`) from logged JSON. However:
- Only 5 field names are erased — other sensitive data (message content, phone numbers, etc.) is logged in full
- The eraser only works on JSON — non-JSON bodies are logged raw
- Response bodies are fully consumed and re-wrapped, meaning the entire response is held in memory

**Severity**: MEDIUM — Sensitive user data may be written to device logs accessible to other apps on rooted devices or via ADB.

## 4. Remotely-Enabled Debug Mode

**File**: `zdi.java:57`

```java
this.f86424d = y75.m25750a((int) rtdVar.m20170m(PmsKey.debugmode, 0)) != y75.DISABLED;
```

**Finding**: The debug mode flag is controlled by `PmsKey.debugmode`, which is a **remote server setting** (PMS = Parameter Management System). The `rtd` class stores these as remotely-configurable parameters (all fields use `gne` — remote setting descriptors).

Additionally, `rtd` exposes:
- `isFullLogEnabled` — enables full logging remotely
- `needToLogSensitive` — enables sensitive data logging remotely

And in local prefs (`ri9`):
- `allowLogSensitiveData` — MutableStateFlow that can be toggled

**Severity**: MEDIUM-HIGH — Server can remotely enable verbose logging of API traffic including request/response bodies for targeted users.

## 5. Redirect Handling — Arbitrary Domain Following

**File**: `ydi.java` (case 0 in switch)

When proxy mode is active, the interceptor manually handles redirects (300-303, 307, 308):

```java
String strM20685a = rqfVarM14940b.f61200X.m20685a("Location");
// Parses Location header into URL
p94 p94Var = new p94();
p94Var.m17787m(null, strM20685a);  // No domain validation!
k88VarM17776b = p94Var.m17776b();
```

**Finding**: The redirect Location URL is parsed and followed **without validating the target domain**. The code only stores the original host in a `TAM_TAM_ORIGINAL_HOST` header but does not restrict which domains the redirect can point to.

The domain validation (`m26885d`) only checks if a host is in the allowed proxy domain list — it does NOT validate redirect targets.

**Severity**: MEDIUM — An attacker controlling a redirect response (MITM or compromised CDN) could redirect requests to arbitrary domains, potentially exfiltrating auth tokens sent in subsequent requests.

## 6. Retry Logic — Amplification Potential

**File**: `RetryKt.java`

Two retry strategies:
- **Fast backoff**: `yg6(15)` → initial interval 500ms, multiplier 1.5x, max delay 5000ms
- **Slow backoff**: `yg6(10)` → initial interval 1000ms, multiplier 2.0x, max delay 10000ms

Retry filter (`retryApiExceptionFilter`) retries on:
- `UnknownHostException`, `ConnectException`, `NoRouteToHostException`
- `SocketException`, `SSLProtocolException`, `SSLPeerUnverifiedException`
- `SSLHandshakeException`, `SSLException`, `HttpRetryException`
- `ProtocolException`, `IOException`
- HTTP 502, 504 responses

The retry uses `Integer.MAX_VALUE` as the max retry count:
```java
return ((e7c) obj).m6267d(new p0j(...), Integer.MAX_VALUE);
```

**Severity**: LOW-MEDIUM — While exponential backoff limits rate, the effectively unlimited retry count means a failing endpoint will be hammered indefinitely. A malicious server returning 502/504 could keep the client retrying, draining battery and generating traffic. The `MaxRetryAttemptCountException` exists but is only used in call-specific flows.

## 7. Cookie Handling

**Finding**: The OkHttp client uses a custom `byb` class as its cookie jar (`f86101A0 = yacVar.f82778j`), defaulting to `byb.f6961o` (instance 3). The `byb` class is a complex multi-interface implementation that handles messaging, not a standard cookie jar.

No standard `CookieJar` or `CookieManager` implementation was found in the HTTP client configuration. The app appears to manage session state via custom headers and tokens rather than HTTP cookies, which reduces cross-domain cookie leakage risk.

**Severity**: LOW — No traditional cookie-based session management found; auth is token-based.

## 8. User-Agent String — Device Info Leakage

**File**: `zdi.java:66-78`, `cgj.java`

The User-Agent is constructed as:
```
OKMessages/<appVersion> (<osVersion>; <deviceName>; <screenResolution>)
```

Example: `OKMessages/26.15.3 (Android 14; Pixel 8 Pro; 1080x2400)`

Fields leaked from `cgj` (UserAgent data class):
| Field | Content | Example |
|-------|---------|---------|
| `f8564a` | Device type | "android" (SDK_TYPE_STRING) |
| `f8565b` | App version | "26.15.3" |
| `f8566c` | Build number | 6695 |
| `f8567d` | OS version | "Android 14" |
| `f8568e` | Architecture | "arm64-v8a" |
| `f8569f` | Locale | "en_US" |
| `f8570g` | Device locale | "en_US" |
| `f8571h` | Device name | "Pixel 8 Pro" |
| `f8572i` | Screen resolution | "1080x2400" |

Additionally, WebView appends `MAX/26.15.3` to the default WebView User-Agent.

**Severity**: LOW-MEDIUM — The User-Agent leaks device model, OS version, screen resolution, and locale to every HTTP endpoint. This enables device fingerprinting and targeted attacks against known-vulnerable OS versions.

## Recommendations

1. **Disable response body logging in production** — or at minimum, truncate logged bodies to prevent sensitive data exposure
2. **Remove remote debug toggle** — or require local developer opt-in; server should not be able to enable verbose logging
3. **Validate redirect domains** — restrict redirects to a whitelist of known service domains
4. **Cap retry count** — replace `Integer.MAX_VALUE` with a reasonable limit (e.g., 5-10 retries)
5. **Minimize User-Agent** — remove device name and screen resolution; use only `OKMessages/<version> (Android <major_version>)`
6. **Disable followSslRedirects** — prevent HTTPS→HTTP downgrade redirects that could expose tokens in cleartext
