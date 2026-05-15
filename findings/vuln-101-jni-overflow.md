# VULN-101: JNI Buffer Overflow — Native Size/Dimension Mismatch

## Summary

Multiple JNI native calls in Max messenger accept Bitmap or buffer parameters alongside separate size/dimension parameters. If the passed dimensions exceed the actual buffer capacity, native code writes out-of-bounds. Several are reachable from network-delivered media.

---

## Finding 1: RLottieDrawable.getFrame — Bitmap Size vs w*h Mismatch

**Signature:**
```java
public static native int getFrame(long ptr, int frame, Bitmap bitmap, int w, int h, int rowBytes, boolean z);
```

**Location:** `one/p011me/rlottie/RLottieDrawable.java:359`

**Analysis:**
- `w` and `h` are passed as `this.f50005a` and `this.f50007b` (set in constructor from caller-supplied values)
- Bitmap is created with `Bitmap.createBitmap(this.f50005a, this.f50007b, ARGB_8888)`
- The native function receives `w`, `h`, and `rowBytes` separately from the Bitmap object
- If the Lottie file is loaded from network (stickers, animated emoji), the dimensions come from the file metadata parsed by `create()` native call

**Vulnerability:**
- If `w*h*4 > bitmap.getAllocationByteCount()`, native writes OOB into the Bitmap's backing pixel buffer
- In `RLottieDrawableUtils.getLottieFrameAt()` (line 37), caller passes arbitrary `w`, `h` with a bitmap that may have different dimensions:
  ```java
  RLottieDrawable.getFrame(rLottieDrawable.f50018j1, i, bitmap, i2, i3, bitmap.getRowBytes(), true);
  ```
- The bitmap is created externally and dimensions `i2`, `i3` are passed independently

**Attacker Control:** HIGH — Lottie animations are delivered via network (stickers, animated reactions). A malicious .tgs/.lottie file with crafted metadata could cause dimension mismatch.

**Severity:** HIGH — Heap buffer overflow in native, reachable from network input.

---

## Finding 2: AnimatedFileDrawable.getVideoFrame — Bitmap vs Video Frame Dimensions

**Signature:**
```java
public static native int getVideoFrame(long ptr, Bitmap bitmap, int[] metaData, int rowBytes, boolean z, float f, float f2, boolean z2);
```

**Location:** `one/p011me/sdk/media/ffmpeg/AnimatedFileDrawable.java:330`

**Analysis:**
- Bitmap `f50205I1` is created from `f50236d[0]` and `f50236d[1]` (metadata from `createDecoder`)
- In `getNextFrame()` (line 886): bitmap is created as `(f50236d[0] * f50247n1, f50236d[1] * f50247n1)` with a scale factor
- The `metaData` array `f50236d` is passed to native alongside the bitmap
- Native code may use metaData dimensions to determine write size, not the actual bitmap dimensions
- There IS a sanity check: dimensions > 3840 are rejected (line 489), but no check that bitmap size matches frame size

**Key Issue:** In `getFirstFrame()` (line 800), bitmap is created with `(f50244k1, f50243j1)` — constructor-supplied dimensions — but `getVideoFrame` is called with `f50236d` metadata from the actual video file. If the video's actual frame dimensions differ from the requested dimensions, native writes based on video frame size into a smaller bitmap.

**Attacker Control:** HIGH — Video files (WebM stickers, GIFs, video messages) are delivered over network. A crafted video with frame dimensions larger than the metadata initially reported could trigger OOB write.

**Severity:** HIGH — Heap overflow in native ffmpeg decoder, reachable from any received animated media.

---

## Finding 3: QrCodeGenerator.nativeRenderSvg — Integer Overflow in width*height

**Signature:**
```java
public static final native int[] nativeRenderSvg(String svg, int width, int height);
```

**Location:** `one/p011me/sdk/uikit/p019qr/QrCodeGenerator.java:273`

**Analysis:**
- Returns `int[]` — allocation happens in native (JNI `NewIntArray`)
- Native must allocate `width * height * sizeof(int)` bytes internally
- If `width * height` overflows a 32-bit int in native, a smaller buffer is allocated but the SVG rasterizer writes `width * height` pixels → heap overflow
- Caller in `g1g.java:196` passes values from `jniVar.f31503b` and `jniVar.f31504c`
- Result is used: `Bitmap.createBitmap(width, height, ARGB_8888)` then `setPixels(result, 0, width, 0, 0, width, height)`

**Attacker Control:** MEDIUM — The SVG content and dimensions come from QR code rendering. If an attacker can influence the SVG string or dimensions (e.g., via a shared QR code link with crafted parameters), overflow is possible. However, dimensions are typically UI-controlled.

**Severity:** MEDIUM — Integer overflow in native allocation. Exploitable if attacker controls width/height inputs.

---

## Finding 4: LZ4.nativeDecompress — Output Buffer Size from Caller

**Signature:**
```java
private static native int nativeDecompress(ByteBuffer src, int srcOff, int srcLen, ByteBuffer dst, int dstOff, int dstLen);
```

**Location:** `ru/p027ok/android/util/compressor/LZ4.java:23`

**Analysis:**
- Output buffer `bArr2` is allocated by caller: `ByteBuffer.allocateDirect(bArr2.length)`
- `dstLen` is passed as `byteBufferAllocateDirect2.capacity()`
- The decompressed size must be known in advance by the caller
- If the caller underestimates decompressed size, native LZ4 writes past buffer end
- Post-check exists: `if (result > 0 && result <= bArr2.length)` — but this is AFTER native already wrote

**Attacker Control:** HIGH — LZ4 compressed data comes from network (OK.ru protocol messages). If the expected decompression size is derived from a header in attacker-controlled data, a mismatch causes overflow.

**Severity:** MEDIUM-HIGH — Classic decompression buffer overflow. The post-hoc bounds check doesn't prevent the native write.

---

## Finding 5: NativeJpegTranscoder — Stream-Based (Lower Risk)

**Signature:**
```java
private static native void nativeTranscodeJpeg(InputStream in, OutputStream out, int rotation, int scaleNum, int quality);
```

**Location:** `com/facebook/imagepipeline/nativecode/NativeJpegTranscoder.java:108`

**Analysis:**
- Uses stream-based I/O, not fixed buffers with size parameters
- Parameters are validated: `scaleNum` in [1,16], `quality` in [0,100], `rotation` in {0,90,180,270}
- No direct buffer+size mismatch pattern

**Attacker Control:** LOW — Parameters are well-validated before native call.

**Severity:** LOW — No obvious overflow vector from Java side.

---

## Finding 6: ZstdUtil.nativeDecompress — Decompression Bomb

**Signature:**
```java
private static final native byte[] nativeDecompress(byte[] input) throws IOException;
```

**Location:** `one/p011me/sdk/zsrd/ZstdUtil.java:18`

**Analysis:**
- Native allocates output buffer internally based on decompressed size from the zstd frame header
- Attacker-controlled compressed data can claim arbitrary decompressed size
- Could cause OOM or, if native uses unchecked malloc, heap corruption

**Attacker Control:** HIGH — Zstd-compressed data from network.

**Severity:** MEDIUM — Primarily DoS via OOM; heap corruption possible if native doesn't validate malloc return.

---

## Attack Surface Summary

| Finding | Native Function | Attacker Input | Reachable from Network | Severity |
|---------|----------------|----------------|----------------------|----------|
| 1 | RLottieDrawable.getFrame | Lottie stickers/animations | YES | HIGH |
| 2 | AnimatedFileDrawable.getVideoFrame | WebM/GIF/video messages | YES | HIGH |
| 3 | QrCodeGenerator.nativeRenderSvg | QR rendering params | PARTIAL | MEDIUM |
| 4 | LZ4.nativeDecompress | Protocol messages | YES | MEDIUM-HIGH |
| 5 | NativeJpegTranscoder | JPEG images | YES (but validated) | LOW |
| 6 | ZstdUtil.nativeDecompress | Compressed payloads | YES | MEDIUM |

## Recommended Exploitation Priority

1. **AnimatedFileDrawable.getVideoFrame** — Most promising. Video frame dimensions from malicious WebM/MP4 can exceed bitmap allocation. Directly reachable by sending a video message or sticker.
2. **RLottieDrawable.getFrame** — Second priority. Lottie files with crafted dimensions delivered as stickers. The `RLottieDrawableUtils.getLottieFrameAt` path passes external w/h.
3. **LZ4.nativeDecompress** — Third priority. If output buffer size is derived from attacker-controlled protocol headers.
