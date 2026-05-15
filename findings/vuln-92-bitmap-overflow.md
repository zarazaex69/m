# VULN-92: Bitmap/Pixel Buffer Overflow Vulnerabilities

## Summary

Multiple bitmap buffer overflow vulnerabilities exist in Max messenger's media handling code. Native functions receive bitmap objects alongside separately-specified dimensions, creating mismatches where native code can write beyond allocated pixel buffers. Additionally, bitmap dimensions derived from untrusted media files lack sufficient validation against integer overflow.

---

## Finding 1: Native getFrame() Width/Height Mismatch with Bitmap Dimensions

**Severity: HIGH**  
**Location:** `one/me/rlottie/RLottieDrawable.java:359`, `RLottieDrawableUtils.java:37`

### Description

The native `getFrame()` signature is:
```java
public static native int getFrame(long j, int i, Bitmap bitmap, int i2, int i3, int i4, boolean z);
// Parameters: nativePtr, frameIndex, bitmap, width, height, rowBytes, precache
```

The width (`i2`) and height (`i3`) are passed **separately** from the Bitmap object. The native code uses these width/height values to determine how many pixels to write, but the Bitmap may have been allocated with different dimensions.

### Vulnerable Call Sites

1. **RLottieDrawable.java:689** — `getFrame(j, this.f50036z1, bitmap, this.f50005a, this.f50007b, bitmap.getRowBytes(), true)`
   - `bitmap` is `f49992Q0` created at `w6f.java:90` with `f50005a × f50007b`
   - If `f50005a`/`f50007b` change between bitmap creation and getFrame call (race condition on volatile fields), native writes OOB.

2. **RLottieDrawableUtils.java:37** — `RLottieDrawable.getFrame(rLottieDrawable.f50018j1, i, bitmap, i2, i3, bitmap.getRowBytes(), true)`
   - Bitmap created with `Bitmap.createBitmap(i2, i3, ...)` — dimensions match here, but `rowBytes` may differ from `width * 4` due to alignment padding.

3. **w6f.java:121,164** — Uses `rLottieDrawable11.f49992Q0.getRowBytes()` but passes `rLottieDrawable11.f50005a` and `f50007b` as width/height. If the bitmap `f49992Q0` was recycled and recreated with different dimensions (possible via `m16804p` bitmap rotation), native writes to wrong-sized buffer.

### Impact

Native lottie renderer writes `width * height * 4` bytes into the bitmap's pixel buffer. If the bitmap was allocated smaller than the passed width/height indicate, this causes a heap buffer overflow in the native heap.

---

## Finding 2: Native getVideoFrame() Trusts Bitmap rowBytes Without Dimension Validation

**Severity: HIGH**  
**Location:** `one/me/sdk/media/ffmpeg/AnimatedFileDrawable.java:330`

### Description

The native `getVideoFrame()` signature:
```java
public static native int getVideoFrame(long j, Bitmap bitmap, int[] iArr, int i, boolean z, float f, float f2, boolean z2);
// Parameters: nativePtr, bitmap, metadataArray, rowBytes, ...
```

The `int[] iArr` (f50236d) contains video dimensions from the native decoder. The native code reads dimensions from this array and writes decoded frame data into the bitmap. The bitmap's actual allocation size is never validated against what native intends to write.

### Vulnerable Pattern (AnimatedFileDrawable.java:888):
```java
this.f50188A0 = Bitmap.createBitmap((int)(f * f2), (int)(r0[1] * f2), Bitmap.Config.ARGB_8888);
// ...
getVideoFrame(j, bitmap, this.f50236d, bitmap.getRowBytes(), false, ...);
```

The bitmap is created with scaled dimensions `(width * scaleFactor, height * scaleFactor)`, but `f50236d[0]` and `f50236d[1]` contain the **original unscaled** video dimensions. If native uses `f50236d` dimensions to determine write size, it writes more data than the scaled bitmap can hold.

### Specific Vulnerable Flows

1. **AnimatedFileDrawable.java:1247** — `Bitmap.createBitmap(iArr[0], iArr[1], ...)` then immediately `getVideoFrame(nativePtr, bitmap, f50236d, bitmap.getRowBytes(), ...)` — dimensions come directly from native decoder parsing the downloaded file.

2. **AnimatedFileDrawable.java:810** — `Bitmap.createBitmap(Math.max(1, iArr[0]), Math.max(1, iArr[1]), ...)` — only validates > 0, no upper bound beyond the 3840 check at decoder creation time.

3. **RunnableC1227vh.java:283-285** — Background thread uses `bitmap.getRowBytes()` but the bitmap (`f50188A0`) could have been replaced by another thread with different dimensions.

---

## Finding 3: Integer Overflow in Bitmap Size Calculation (e01.m6018c)

**Severity: MEDIUM**  
**Location:** `p000/e01.java:60`

### Description

```java
public static final int m6018c(int i, int i2, Bitmap.Config config) {
    int iM6017b = m6017b(config);  // returns 4 for ARGB_8888
    int i3 = i * i2 * iM6017b;     // INTEGER OVERFLOW possible
    if (i3 > 0) {
        return i3;
    }
    // throws exception
}
```

The multiplication `width * height * bytesPerPixel` is performed as 32-bit integer arithmetic. For example:
- width=32768, height=32768, bpp=4 → `32768 * 32768 * 4 = 0` (overflow to 0, caught)
- width=46341, height=46341, bpp=4 → overflows to a small positive number, **passes the > 0 check**

This is used in `C1131sw.java:26` to validate bitmap reuse:
```java
if (bitmap.getAllocationByteCount() < e01.m6017b(config) * i * i2) {
    throw new IllegalStateException("Check failed.");
}
```

If the multiplication overflows to a small positive value, the check passes, and a too-small bitmap is reused for a large image — causing native OOB write when pixels are rendered.

---

## Finding 4: PGS Subtitle Parser — Unvalidated Dimensions from Stream

**Severity: MEDIUM**  
**Location:** `p000/oed.java:123-124, 160, 177`

### Description

The PGS subtitle parser (`oed`, handling `application/pgs` streams) reads bitmap dimensions directly from the media stream:

```java
nedVar.f43371g = shjVar.m21296x();  // width from stream
nedVar.f43372h = shjVar.m21296x();  // height from stream
// ...
int i12 = nedVar.f43371g * nedVar.f43372h;  // INTEGER OVERFLOW
int[] iArr4 = new int[i12];                  // undersized array
// ... fills array from stream data ...
Bitmap bitmapCreateBitmap = Bitmap.createBitmap(iArr4, nedVar.f43371g, nedVar.f43372h, Bitmap.Config.ARGB_8888);
```

**Attack vector:** A crafted PGS subtitle stream in a video file can specify arbitrary width/height values. The multiplication `width * height` can overflow int32, causing:
1. Undersized `int[]` allocation
2. Array index out of bounds during fill loop
3. Or if it wraps to a valid size, `Bitmap.createBitmap` with mismatched array/dimensions

No validation exists on `f43371g` or `f43372h` before use.

---

## Finding 5: cropAndScale Native — No Bounds Validation on Crop Region

**Severity: MEDIUM**  
**Location:** `org/webrtc/NV12Buffer.java:29`, `org/webrtc/JavaI420Buffer.java:55`

### Description

The `nativeCropAndScale` functions receive crop coordinates (cropX, cropY, cropWidth, cropHeight) and source buffer dimensions, but the Java layer performs **no validation** that the crop region fits within the source:

```java
// NV12Buffer.java:29
nativeCropAndScale(i, i2, i3, i4, i5, i6, this.buffer, this.width, this.height, 
    this.stride, this.sliceHeight, ...);
```

The `NativeCapturerObserver.java:31` passes `frameAdaptationParametersAdaptFrame.cropX/cropY/cropWidth/cropHeight` directly from `adaptFrame()` which is a native call returning parameters. If the native adaptation logic has a bug or the frame dimensions change between adaptation and crop (TOCTOU), the crop region can exceed source bounds.

**VideoFileRenderer.java:120** calculates crop offsets:
```java
buffer.cropAndScale((buffer.getWidth() - width2) / 2, (buffer.getHeight() - height) / 2, width2, height, i, i2);
```

If integer truncation causes `width2 > buffer.getWidth()`, the cropX becomes negative (wraps to large positive in native), causing OOB read from the source buffer.

---

## Finding 6: AnimatedFileDrawable Dimension Cap Bypass via Scale Factor

**Severity: MEDIUM**  
**Location:** `one/me/sdk/media/ffmpeg/AnimatedFileDrawable.java:488-492, 888`

### Description

The decoder validates dimensions at creation:
```java
if (iArr[0] > 3840 || iArr[1] > 3840) {
    destroyDecoder(this.nativePtr);
    this.nativePtr = 0L;
}
```

However, `getNextFrame()` at line 888 creates a bitmap with **scaled** dimensions:
```java
this.f50188A0 = Bitmap.createBitmap((int)(f * f2), (int)(r0[1] * f2), Bitmap.Config.ARGB_8888);
```

Where `f = this.f50236d[0]` (up to 3840) and `f2 = this.f50247n1` (scale factor). The scale factor `f50247n1` is not bounded. If set > 1.0 (e.g., via display scaling), the bitmap dimensions can exceed what native expects, or conversely if < 1.0, the bitmap is smaller than what native writes (based on unscaled `f50236d` values).

---

## Finding 7: RLottieDrawable getFrame — Bitmap/Dimension Desync via Network Reload

**Severity: MEDIUM**  
**Location:** `one/me/rlottie/RLottieDrawable.java:689`, `RLottieDrawableUtils.java`

### Description

`RLottieDrawable` stores width/height as `final int f50005a, f50007b` but the native handle (`f50018j1`) can be destroyed and recreated via `restartDownloadFromUrl()` (network re-fetch). The new Lottie file from the network may have different internal dimensions than what `f50005a/f50007b` specify.

The native `getFrame()` is called with the Java-side width/height:
```java
getFrame(j, i, bitmap, this.f50005a, this.f50007b, bitmap.getRowBytes(), true);
```

If the native Lottie renderer's internal canvas size differs from the passed width/height, it may render beyond the bitmap bounds. The bitmap is allocated as `f50005a × f50007b`, but native may use its own internal dimensions for rendering calculations.

---

## Recommendations

1. **Validate bitmap dimensions match native expectations** — Before every native call, assert `bitmap.getWidth() >= passedWidth && bitmap.getHeight() >= passedHeight`.

2. **Use long arithmetic for size calculations** — Replace `width * height * bpp` with `(long)width * height * bpp` and check against `Integer.MAX_VALUE`.

3. **Bound PGS subtitle dimensions** — Add maximum dimension checks (e.g., 4096×4096) before allocating pixel arrays.

4. **Validate crop regions** — Assert `cropX + cropWidth <= bufferWidth && cropY + cropHeight <= bufferHeight` before native cropAndScale calls.

5. **Synchronize bitmap access** — The bitmap rotation pattern (`Q0 → O0 → P0`) with volatile fields is insufficient; native writes and Java-side bitmap swaps can race.

6. **Cap scale factor** — Ensure `f50247n1` is bounded such that `dimension * scale <= 3840`.
