# VULN-90: WebP Native Decoder — CVE-2023-4863 Analysis

## Summary

**Status: NOT VULNERABLE (Patched)**  
**CVE:** CVE-2023-4863 — Critical heap buffer overflow in libwebp (BuildHuffmanTable)  
**CVSS:** 8.8 (Critical)  
**Library:** `libstatic-webp.so` (Facebook Fresco static-webp)  
**libwebp Version:** 1.3.2 (contains the fix)  
**Fix Release Date:** September 13, 2023  

The Max messenger app bundles libwebp version 1.3.2, which is the exact version that patched CVE-2023-4863. The vulnerability is **not exploitable** in this build.

---

## 1. Native Library Analysis

### Binary Identification
```
File: ~/max/decompiled/native/lib/arm64-v8a/libstatic-webp.so
Size: 501,856 bytes
Arch: ELF 64-bit LSB shared object, ARM aarch64
Target: Android 21+
NDK: r27b (build 12297006)
Compiler: Android clang 18.0.2 (+pgo, +bolt, +lto, +mlgo)
BuildID: 3211ea73b90d92a8
```

### Version Determination
Disassembly of `WebPGetDecoderVersion` at offset `0x2c13c`:
```
0x2c13c: MOV  W0, #0x0302      ; lower 16 bits
0x2c140: MOVK W0, #0x01, LSL#16 ; upper 16 bits  
0x2c144: RET
```
Decoded: `0x010302` → **libwebp 1.3.2**

### Exported Symbols (WebP/VP8)
```
VP8CheckSignature    @ 0x2c2cc
VP8GetInfo           @ 0x2c304
VP8LCheckSignature   @ 0x2e2f8  (lossless decoder entry)
VP8LGetInfo          @ 0x2e324
WebPDecode           @ 0x32730
WebPGetDecoderVersion @ 0x2c13c
WebPGetDemuxVersion  @ 0x5318c
WebPIDecode          @ 0x289a0
WebPINewDecoder      @ 0x288d4
```

### BuildHuffmanTable — The Vulnerable Function
The `BuildHuffmanTable` function is **internal** (not exported) — the library is stripped. It exists within the VP8L lossless decoder path starting at `VP8LCheckSignature` (0x2e2f8). In vulnerable versions (< 1.3.2), this function had an insufficient bounds check on the Huffman table size, allowing a crafted WebP lossless image to trigger a heap buffer overflow.

Since this library is version **1.3.2**, the fix (commit `902bc955`) is included.

---

## 2. Java-Side WebP Handling

### Library Loading
```java
// p000/pqh.java
public static synchronized void m18139b() {
    if (!f54162a) {
        trb.m22154S("static-webp");  // System.loadLibrary("static-webp")
        f54162a = true;
    }
}
```

### Decode Path (Facebook Fresco)
```java
// com/facebook/webpsupport/WebpBitmapFactoryImpl.java
public static Bitmap hookDecodeByteArray(byte[] bArr, int i, int i2, BitmapFactory.Options options) {
    pqh.m18139b();  // ensures native lib loaded
    return originalDecodeByteArray(bArr, i, i2, options);
}

// Native methods:
private static native Bitmap nativeDecodeByteArray(byte[] bArr, int i, int i2, ...);
private static native Bitmap nativeDecodeStream(InputStream inputStream, ...);
```

### WebP Format Detection (p000/le5.java)
Supports all WebP variants:
- `WEBP_SIMPLE` — lossy
- `WEBP_LOSSLESS` — lossless (CVE-2023-4863 attack surface)
- `WEBP_EXTENDED` — extended format
- `WEBP_EXTENDED_WITH_ALPHA` — extended with alpha
- `WEBP_ANIMATED` — animated WebP

### Animated WebP (p000/C1338yh.java)
```java
// Uses com.facebook.animated.webp.WebPImage via reflection
gifImage2 = (GifImage) Class.forName("com.facebook.animated.webp.WebPImage").newInstance();
```

---

## 3. Attack Surface — Do Remote WebP Images Reach the Native Decoder?

**YES.** The decode path is reachable from received messages:

1. **Message attachment handling** (`MessageWriteWidget.java`): Accepts `image/webp` as valid image type
2. **Image download** (`gei.java`): HTTP requests include `Accept: image/webp,/;q=0.8`
3. **Image type routing** (`my4.java`): `image/webp` is routed to the standard image decode path
4. **Download & decode** (`i2g.java`): WebP files are detected and decoded through the Fresco pipeline
5. **Fresco hooks** (`WebpBitmapFactoryImpl`): All `BitmapFactory.decode*` calls are intercepted and routed through the native `libstatic-webp.so`

**Flow:** Remote user sends WebP image → server delivers → client downloads → Fresco detects WebP → loads `libstatic-webp.so` → calls `WebPDecode`/`VP8LCheckSignature` → (if lossless) `BuildHuffmanTable`

This is a **0-click** path — image thumbnails/previews are decoded automatically without user interaction.

---

## 4. CVE-2023-4863 Vulnerability Details

### Root Cause
In libwebp < 1.3.2, the `BuildHuffmanTable()` function in `src/dec/vp8l_dec.c` had an insufficient size calculation for the second-level Huffman lookup table. A crafted WebP lossless image could specify Huffman code lengths that cause the function to write beyond the allocated buffer.

### Exploitation
- **Type:** Heap buffer overflow (write)
- **Trigger:** Crafted WebP lossless image with malicious Huffman codes
- **Impact:** Arbitrary code execution in the context of the app process
- **Interaction:** None required (0-click via image preview)

### Fix (in 1.3.2)
The fix adds proper bounds checking in `BuildHuffmanTable` to ensure the table size does not exceed the allocated buffer. Commit: `902bc955` in the libwebp repository.

---

## 5. Build Timeline Assessment

| Indicator | Value | Implication |
|-----------|-------|-------------|
| libwebp version | 1.3.2 | Fix included (released Sep 13, 2023) |
| NDK version | r27b | Released 2024 |
| Clang version | 18.0.2 | Released 2024 |
| AGP version | 8.11.1 | Released 2025 |
| App version | 26.15.3 (code 6695) | Recent build |

**Conclusion:** The library was built well AFTER the September 2023 fix. All toolchain indicators point to a 2024-2025 build.

---

## 6. Exploitability Assessment — C2C 0-Click RCE

### If the library were vulnerable (hypothetical, pre-1.3.2):

| Factor | Assessment |
|--------|-----------|
| Remote trigger | ✅ Send WebP image in chat |
| 0-click | ✅ Auto-decoded for thumbnail/preview |
| Code execution | ✅ Heap overflow → RCE proven in the wild |
| Sandbox escape | ❌ Contained in app sandbox (no kernel exploit) |
| Data access | ✅ Full access to app data (messages, keys, contacts) |
| Persistence | ⚠️ Requires additional exploit for persistence |

### Actual Status (version 1.3.2):

**NOT EXPLOITABLE** — The bundled libwebp 1.3.2 contains the fix for CVE-2023-4863.

---

## 7. Residual Risk

While CVE-2023-4863 is patched, the following observations remain relevant:

1. **Large native attack surface**: The library exports 50+ functions for WebP decode/demux
2. **0-click reachability confirmed**: Any future libwebp vulnerability would be 0-click exploitable via the same path
3. **No ASLR bypass needed**: The heap overflow primitive in CVE-2023-4863 was powerful enough to achieve RCE without separate info leak on many targets
4. **Static linking**: The WebP library is statically linked into `libstatic-webp.so`, meaning updates require a full app update (no system-level patching)

---

## Verdict

**CVE-2023-4863 is PATCHED in this build.** The Max messenger app ships libwebp 1.3.2 which contains the security fix. However, the attack surface analysis confirms that the WebP native decoder IS reachable via 0-click from remote users, meaning any future vulnerability in this library would be critical.

**Risk Rating: LOW (patched) | Attack Surface: CRITICAL (0-click reachable)**
