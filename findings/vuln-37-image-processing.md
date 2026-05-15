# VULN-37: Image Processing Pipeline - Memory Corruption Analysis

## Summary

Max messenger uses Facebook's Fresco image pipeline with native JPEG transcoding (`libnative-imagetranscoder.so`) and custom SVG rendering via JNI. Several potential memory corruption vectors exist in the image processing path, particularly in SVG rendering and native JPEG transcoding.

## Architecture

### Image Pipeline Components
1. **Fresco (Facebook)** - Primary image loading/caching framework
2. **NativeJpegTranscoder** - Native JPEG transcoding via `libnative-imagetranscoder.so` (585KB, arm64)
3. **SVG Renderer** - Custom native SVG rendering via `QrCodeGenerator.nativeRenderSvg()`
4. **GIF decoder** - `libgifimage.so` (319KB)
5. **Native filters** - `libnative-filters.so` (24KB)
6. **Image processing utils** - `libimage_processing_util_jni.so` (33KB)

### Native Libraries (arm64-v8a)
```
libgifimage.so              318,992 bytes
libimagepipeline.so           8,760 bytes
libimage_processing_util_jni.so  32,544 bytes
libnative-filters.so         23,712 bytes
libnative-imagetranscoder.so 585,376 bytes  <-- Primary attack surface
libtracernative.so          798,992 bytes
```

## PmsKey Server-Controlled Image Limits

From `PmsKey.java` and `rtd.java`:

| Key | Enum Index | Default | Usage |
|-----|-----------|---------|-------|
| `image-width` | 7 | 1920 | Max width for resize |
| `image-height` | 8 | 1920 | Max height for resize |
| `image-quality` | 9 | 0.8 (80%) | JPEG quality |
| `image-size` | 10 | 40,000,000 (40MB) | Max file size |
| `min-image-side-size` | - | 64 | Minimum dimension |

**Critical finding**: The `image-size` default is 40MB. This is the raw file size limit, NOT the decoded bitmap size. A 40MB JPEG could decode to a multi-gigabyte bitmap (e.g., 10000x10000x4 = 400MB ARGB).

## Vulnerability Analysis

### 1. SVG Rendering - Integer Overflow / OOM (HIGH)

**File**: `p000/ij7.java` (SVG decoder in Fresco pipeline)
**File**: `p000/nwl.java` (SVG pixel array allocation)

```java
// ij7.java - SVG decode path
int iM25031b = z ? ((x8i) qc8Var).m25030b() : m10375b(regex, str);  // width from SVG
int iM25030a = z ? ((x8i) qc8Var).m25030a() : m10375b(regex, str);  // height from SVG
cy3 cy3VarMo20095c = ((rqd) this.f27669b.getValue()).mo20095c(iM25031b, iM25030a, config);
Bitmap bitmap = (Bitmap) cy3VarMo20095c.m4877u0();
int[] iArrM15366a = nwl.m15366a(iM25031b, iM25030a, str);  // native render
bitmap.setPixels(iArrM15366a, 0, iM25031b, 0, 0, iM25031b, iM25030a);
```

**Issues**:
- SVG width/height parsed via regex (`m10375b`) with fallback to 100 if parsing fails
- No maximum dimension validation before `Bitmap.createBitmap(width, height)`
- `nwl.m15366a()` calls `QrCodeGenerator.nativeRenderSvg(str, width, height)` - passes raw SVG string to native code
- Integer overflow: `width * height * 4` can overflow int32 for large dimensions
- The native `nativeRenderSvg` receives attacker-controlled SVG content AND dimensions

**Attack vector**: Craft SVG with `width="99999" height="99999"` → attempts 40GB allocation, or use carefully chosen values to trigger integer overflow in native allocation (width * height * 4 wraps to small value → heap buffer overflow).

### 2. NativeJpegTranscoder - Native Code Attack Surface (HIGH)

**File**: `com/facebook/imagepipeline/nativecode/NativeJpegTranscoder.java`
**Library**: `libnative-imagetranscoder.so`

```java
private static native void nativeTranscodeJpeg(InputStream, OutputStream, int rotation, int scaleNumerator, int quality);
private static native void nativeTranscodeJpegWithExifOrientation(InputStream, OutputStream, int exifOrientation, int scaleNumerator, int quality);
```

The native transcoder:
- Loaded via `trb.m22154S("native-imagetranscoder")` → `System.loadLibrary()`
- Processes JPEG data from InputStream directly in native code
- Based on Facebook's Fresco native-imagetranscoder (uses libjpeg-turbo internally)
- Scale numerator validated: 1-16 range
- Quality validated: 0-100 range
- Rotation validated: 0, 90, 180, 270

**Attack vector**: Malformed JPEG with crafted headers could trigger buffer overflow in libjpeg-turbo parsing within the native transcoder. The 585KB library size suggests it bundles libjpeg-turbo.

### 3. BitmapFactory.decodeByteArray Without Size Validation (MEDIUM)

**File**: `p000/wjl.java`

```java
public static Bitmap m24467a(byte[] bArr, int i, int i2, BitmapFactory.Options options) {
    options.inJustDecodeBounds = true;
    BitmapFactory.decodeByteArray(bArr, 0, i, options);
    options.inJustDecodeBounds = false;
    options.inSampleSize = 1;
    for (int iMax = Math.max(options.outWidth, options.outHeight); iMax > i2; iMax /= 2) {
        options.inSampleSize *= 2;
    }
    Bitmap bitmapDecodeByteArray = BitmapFactory.decodeByteArray(bArr, 0, i, options);
}
```

**Issues**:
- `inSampleSize` calculation uses power-of-2 downsampling only
- If `i2` (max dimension) is -1, NO downsampling occurs at all
- No validation that decoded bitmap won't exceed memory limits
- The `outWidth`/`outHeight` from bounds check could be spoofed by malformed image headers

### 4. GIF Processing - libgifimage.so (MEDIUM)

**Library**: `libgifimage.so` (319KB)
- GIF parsing in native code is historically vulnerable to buffer overflows
- No visible dimension validation before native GIF decode
- GIF bomb: small file size but enormous frame dimensions

### 5. Image Size Validation Gap (MEDIUM)

**File**: `p000/t6e.java`

```java
// File size check only:
return j <= ((long) ((int) rtdVar.m20170m(PmsKey.f88712imagesize, (long) 40000000)));
```

This validates **file size** (40MB default) but NOT decoded bitmap dimensions. A carefully crafted image can have:
- Small file size (passes the 40MB check)
- Enormous decoded dimensions (e.g., JPEG with high compression ratio)

### 6. Untrusted Image Pipeline (HIGH)

**Finding**: Images from other users go through the SAME Fresco pipeline:
- `FrescoHttpDownloadException` in `ru.ok.messages.controllers.image` package confirms images are downloaded from remote servers
- The `bbi.java` network fetcher feeds directly into Fresco's decode pipeline
- No separate sandboxed pipeline for untrusted content vs local content
- The `t6e.java` resize logic applies to outgoing images only (before send)
- Incoming images from other users are decoded by Fresco with whatever dimensions the server provides

### 7. SVG Detection - Weak Header Check (LOW)

**File**: `p000/jj7.java`

```java
// Only checks first 4 bytes for "<svg" or "<?xm"
f31062b = ilb.m10515b("<svg");
f31063c = ilb.m10515b("<?xm");
```

SVG content type detection relies on a 4-byte magic check. This is used to route to the SVG decoder which then passes raw SVG XML to native code.

## Attack Scenarios

### Scenario A: SVG Heap Overflow via Integer Overflow
1. Attacker sends message with SVG attachment
2. SVG contains `width="65536" height="65536"` 
3. Native allocation: `65536 * 65536 * 4 = 17,179,869,184` → overflows int32 to 0
4. Small buffer allocated, large write → heap corruption

### Scenario B: JPEG Decompression Bomb
1. Attacker sends JPEG file < 40MB (passes size check)
2. JPEG decodes to enormous bitmap (e.g., 30000x30000 = 3.6GB ARGB)
3. No decoded-size validation → OOM crash or memory pressure attack

### Scenario C: Native JPEG Transcoder Exploit
1. Attacker sends crafted JPEG with malformed markers/headers
2. `nativeTranscodeJpeg()` processes the stream in native code
3. Buffer overflow in libjpeg-turbo parsing → code execution

### Scenario D: SVG XXE / SSRF
1. SVG content passed as raw string to native renderer
2. If native SVG parser supports external entities or `<use xlink:href="...">`
3. Could trigger SSRF or local file read

## Recommendations

1. **Add decoded bitmap size validation**: Check `width * height * bytesPerPixel` before allocation, with overflow-safe multiplication
2. **Cap SVG dimensions**: Enforce maximum width/height (e.g., 4096x4096) before passing to native renderer
3. **Sandbox native image processing**: Use separate process for untrusted image decoding
4. **Update libjpeg-turbo**: Check version in `libnative-imagetranscoder.so` for known CVEs
5. **Validate image dimensions server-side**: Don't rely solely on client-side PmsKey limits
6. **Add integer overflow checks**: Use `Math.multiplyExact()` or manual overflow detection before bitmap allocation

## Files of Interest

| File | Purpose |
|------|---------|
| `p000/wjl.java` | BitmapFactory decode with inSampleSize |
| `p000/ij7.java` | SVG decoder - bitmap allocation from parsed dimensions |
| `p000/nwl.java` | SVG native render bridge |
| `p000/jj7.java` | SVG format detection (4-byte magic) |
| `p000/t6e.java` | Image size validation (file size only) |
| `p000/trb.java:111` | inSampleSize calculation |
| `p000/ncj.java:241` | Image resize using PmsKey limits |
| `p000/C1153th.java:74` | DecodeException for invalid dimensions |
| `com/facebook/imagepipeline/nativecode/NativeJpegTranscoder.java` | Native JPEG transcoding |
| `p000/prb.java` | Native lib loader for image-transcoder |
| `p000/klb.java` | NativeJpegTranscoderFactory instantiation |
| `one/me/sdk/uikit/qr/QrCodeGenerator.java` | nativeRenderSvg JNI |

## Risk Rating

**Overall: HIGH** - Native code processing of untrusted image data with insufficient dimension validation creates realistic memory corruption attack surface. The SVG integer overflow path is particularly concerning as it combines attacker-controlled dimensions with native memory allocation.
