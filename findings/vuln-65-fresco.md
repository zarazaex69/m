# VULN-65: Fresco Image Pipeline — SSRF, Cache Poisoning & Native Decoder Risks

## Summary

Max messenger uses Facebook's Fresco library with OkHttp3 backend (`OkHttpNetworkFetchProducer`) for image loading. Analysis reveals multiple attack surfaces including potential SSRF via arbitrary URL injection, cache poisoning through URL-keyed disk cache, unvalidated redirects, native code exposure through animated image decoders, and no apparent download size limits.

## 1. Fresco Components Identified

Key classes in the decompiled source:
- `OneMeDraweeView` — custom `SimpleDraweeView` subclass (main image widget)
- `ce8` — `ImageRequest` (wraps URI + decode options)
- `de8` — `ImageRequestBuilder`
- `ld8` — `ImagePipeline` (orchestrates fetch/decode/cache)
- `od8` — `ImagePipelineFactory` (singleton factory)
- `gei` — `OkHttpNetworkFetcher` (network layer using OkHttp)
- `yo5` — Disk cache config (`image_cache` directory, 40MB default)
- `GifImage` / `C1338yh` — Native animated image decoders

## 2. Image URL Construction — Arbitrary URL Injection

**Finding: HIGH RISK — No URL domain validation**

Image URLs originate from server-provided data (`photoUrl` fields in protobuf messages) and are passed directly to Fresco:

```java
// mth.java — directly sets server-provided string as image URI
((OneMeDraweeView) view).setImageURI(str);

// ce8.m3560b() — parses any string into ImageRequest
public static ce8 m3560b(String str) {
    if (str == null || str.length() == 0) return null;
    return m3559a(Uri.parse(str));  // No validation!
}
```

The `trb.m22146K()` method only checks if the URL already has a known scheme prefix (`file:`, `http`, `content`, `android.resource:/`, `res:/`, `data`). If it does, it passes through unchanged. **No domain whitelist or URL validation is applied.**

**Attack vector:** A compromised server or MITM attacker can inject arbitrary URLs (including `http://10.0.0.1/...`, `http://169.254.169.254/...`, `file:///data/...`) into `photoUrl` fields in protobuf messages, causing the client to fetch from those locations.

## 3. Cache Poisoning

**Finding: MEDIUM RISK — URL-keyed cache without integrity verification**

Disk cache configuration (`yo5`):
- Directory name: `image_cache`
- Max size: 40MB (`f83986c = 41943040`)
- Low disk space limit: 10MB (`f83987d = 10485760`)
- Default eviction threshold: `PlaybackStateCompat.ACTION_SET_SHUFFLE_MODE` (2097152 = 2MB)

The cache key is derived from the image URI (`ce8.f8340b`). The `DiskCacheProducer` (`bp5`) stores fetched images keyed by URI. There is **no content integrity verification** (no hash/signature check on cached images).

**Attack vector:** If an attacker can serve a malicious image for a legitimate URL (via MITM on HTTP connections, DNS poisoning, or redirect manipulation), the poisoned image will be cached and served for all subsequent requests to that URL until cache eviction.

## 4. Redirect Handling — Follows Redirects to Arbitrary Domains

**Finding: HIGH RISK — OkHttp default redirect behavior enabled**

The OkHttp client builder (`yac`) shows:
```java
public boolean f82774f = true;   // retryOnConnectionFailure = true
public boolean f82776h;          // followRedirects (set to true in constructor line 96)
```

OkHttp's default behavior follows HTTP 3xx redirects (up to 20 hops) to **any domain** without restriction. The `ydi` interceptor handles redirects and even has a `TAM_TAM_ORIGINAL_HOST` header mechanism, but this appears to be for proxy routing, not security filtering.

The network fetcher (`gei.mo8479t`) constructs requests with `Cache-Control: no-cache` and `Accept: image/webp,/;q=0.8` but applies **no redirect domain restrictions**.

**Attack vector:** An attacker controlling an image CDN or performing MITM can redirect image requests to internal network addresses (SSRF) or to attacker-controlled servers to serve malicious content.

## 5. SSRF via Image Loading

**Finding: HIGH RISK — No scheme or host restrictions on image fetch**

The `ce8` constructor categorizes URIs by scheme:
```java
if (efj.m6405d(uri)) { i = 0; }           // http/https → network fetch
else if ("file".equals(scheme)) { i = 3; } // file → local read
else if ("content".equals(scheme)) { i = 4; }
else if ("data".equals(scheme)) { i = 7; }
```

`efj.m6405d()` returns true for both `http` and `https` schemes — **no host validation**. The `gei` network fetcher will fetch from any HTTP/HTTPS URL.

**Attack vectors:**
1. **Internal network scanning:** Inject `http://192.168.x.x/`, `http://10.x.x.x/`, `http://172.16.x.x/` URLs
2. **Cloud metadata:** Inject `http://169.254.169.254/latest/meta-data/` (AWS), `http://metadata.google.internal/` (GCP)
3. **Local file read:** Inject `file:///data/data/one.me.messenger/...` URIs (handled as local file type 3)
4. **Content provider access:** Inject `content://` URIs to read from other apps' exported providers

## 6. Disk Cache Location & Accessibility

**Finding: LOW RISK (standard Android sandboxing)**

Disk cache location: `{app_cache_dir}/image_cache/` (from `yo5.f83984a = "image_cache"`)

The cache is stored in the app's private cache directory (`context.getCacheDir()`), which on non-rooted devices is:
- `/data/data/one.me.messenger/cache/image_cache/`

This is **not accessible to other apps** under normal Android sandboxing. However:
- On rooted devices, the cache is fully readable
- The `AttachPreviewDiskCache` (`ru.ok.messages.video.AttachPreviewDiskCache`) is a separate cache for video thumbnails
- Cache files have no additional encryption

## 7. Animated Images — Native Decoders

**Finding: MEDIUM-HIGH RISK — Native code parsing untrusted data**

Fresco uses native (JNI) decoders for animated images:

```java
// GifImage.java — native GIF decoder
private static native GifImage nativeCreateFromDirectByteBuffer(ByteBuffer buf, int maxSize, boolean z);
private static native GifImage nativeCreateFromNativeMemory(long ptr, int size, int maxSize, boolean z);

// C1338yh — also loads WebPImage via reflection
Class.forName("com.facebook.animated.webp.WebPImage").newInstance();
```

The `AnimatedFactoryV2Impl` is initialized via reflection with parameters including frame count limits (30 frames, 1000ms). The native `gifimage` library is loaded via `trb.m22154S("gifimage")`.

**Attack vector:** Malformed GIF/WebP images can trigger memory corruption vulnerabilities in the native decoders. The `nativeCreateFromDirectByteBuffer` accepts a `ByteBuffer` directly from network-fetched data with `Integer.MAX_VALUE` as the max size parameter — effectively no limit on parsed content.

## 8. Download Size Limits — DoS via Large Images

**Finding: HIGH RISK — No apparent download size limit**

Analysis of the network fetch path:
- `gei.mo8479t()` constructs the HTTP request with no `Range` header limit
- No `Content-Length` check before downloading
- The `GifImage.m3862a()` passes `Integer.MAX_VALUE` as max size: `nativeCreateFromDirectByteBuffer(byteBuffer, Integer.MAX_VALUE, false)`
- Server-side `PmsKey.f88712imagesize` ("image-size") exists as a config parameter, but this appears to control **upload** size, not download limits
- Memory cache limit is 1-4MB depending on device RAM (`dd5` class), but disk writes have no pre-check

**Attack vector:** An attacker can serve an extremely large image (hundreds of MB or GB) causing:
1. Memory exhaustion (OOM) during download buffering
2. Disk space exhaustion filling the 40MB cache rapidly
3. Network bandwidth exhaustion on metered connections
4. CPU exhaustion during decode of large animated images

## Risk Assessment

| Issue | Severity | Exploitability |
|-------|----------|----------------|
| SSRF via arbitrary image URLs | High | Medium (requires server compromise or MITM) |
| No download size limit (DoS) | High | High (any attacker-controlled image URL) |
| Redirect to arbitrary domains | High | Medium (requires initial URL control) |
| Native decoder memory corruption | Medium-High | Medium (crafted image required) |
| Cache poisoning | Medium | Medium (requires MITM or redirect) |
| No URL domain validation | High | Medium (requires message injection) |

## Recommendations

1. **Implement URL whitelist** — Only allow image fetches from known CDN domains
2. **Add Content-Length limit** — Reject responses exceeding a reasonable size (e.g., 20MB)
3. **Restrict redirect targets** — Only follow redirects to whitelisted domains
4. **Block private IP ranges** — Reject URLs targeting RFC1918, link-local, and cloud metadata addresses
5. **Add cache integrity** — Verify image content hash against server-provided hash
6. **Limit native decoder input** — Set reasonable max size for GIF/WebP parsing instead of `Integer.MAX_VALUE`
7. **Block non-HTTP schemes** — Prevent `file://` and `content://` URIs from server-provided URLs
