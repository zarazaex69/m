# VULN-89: GIF Native Decoder Heap Overflow

## Summary

The Max messenger app uses Facebook's `libgifimage.so` native library (based on GIFLIB/DGifLib) to decode GIF images. The Java-side code passes raw GIF data to native parsing with **no pre-validation** of dimensions, frame count, or file structure. Frame dimensions from the GIF file are trusted directly for buffer allocation, creating a heap overflow attack surface exploitable via crafted GIF images sent in chat.

## Severity: HIGH

- **Type**: Heap Buffer Overflow (Native)
- **Attack Vector**: Network (C2C - attacker sends GIF to victim in chat)
- **User Interaction**: Victim views/scrolls past the message (auto-decode on display)
- **Impact**: Remote Code Execution potential, Denial of Service

## Native Library Details

- **Library**: `libgifimage.so` (arm64-v8a, 318,992 bytes)
- **Based on**: GIFLIB (DGifLib functions: DGifOpen, DGifSlurp, DGifGetImageDesc, DGifGetLine, etc.)
- **Compiler**: Android NDK clang 18.0.2 (r522817b) with PGO+BOLT+LTO+MLGO
- **Key functions**: `nativeCreateFromDirectByteBuffer`, `nativeCreateFromNativeMemory`, `nativeRenderFrame`
- **Allocators used**: `malloc`, `calloc`, `realloc`, `openbsd_reallocarray`

## Vulnerability Analysis

### 1. No Pre-Validation Before Native Parsing

The Java entry point in `GifImage.java` passes data directly to native with **Integer.MAX_VALUE as the frame count limit**:

```java
// GifImage.m3862a() - called for ByteBuffer input
GifImage gifImageNativeCreateFromDirectByteBuffer = nativeCreateFromDirectByteBuffer(byteBuffer, Integer.MAX_VALUE, false);

// GifImage.m3863b() - called for native memory input  
GifImage gifImageNativeCreateFromNativeMemory = nativeCreateFromNativeMemory(j, i, Integer.MAX_VALUE, false);
```

**No validation performed before native call:**
- ❌ No magic bytes verification (GIF87a/GIF89a) before native entry
- ❌ No file size limit check
- ❌ No frame count limit (Integer.MAX_VALUE = 2,147,483,647)
- ❌ No dimension bounds checking
- ❌ No structure validation

Note: `ke5.java` contains GIF magic byte constants (`GIF87a`, `GIF89a`) but these are used only for **content-type detection** (routing to the GIF decoder), NOT for validation before parsing.

### 2. Frame Dimensions Trusted for Buffer Allocation

In `uf4.java`, the frame rendering path creates bitmaps using dimensions returned from native:

```java
// m22863g() - creates bitmap with dimensions from GIF frame
public synchronized Bitmap m22863g(int i, int i2) {
    if (((Bitmap) this.f70524j) == null) {
        this.f70524j = Bitmap.createBitmap(i, i2, Bitmap.Config.ARGB_8888);
    }
    return (Bitmap) this.f70524j;
}
```

The dimensions come from `GifFrame.nativeGetWidth()` and `GifFrame.nativeGetHeight()` which read directly from parsed GIF data. The native `nativeRenderFrame` then writes pixel data into this bitmap:

```java
// m22866j() - renders frame with native-reported dimensions
gifFrame.m3861g(iM3858d, iM3857c, bitmapM22863g);  // calls nativeRenderFrame
```

**Critical issue**: If the GIF file declares small frame dimensions in the Image Descriptor but contains more pixel data (via LZW stream), the native `DGifGetLine`/`DGifGetPixel` functions may write beyond the allocated buffer.

### 3. Native Library Error Strings Confirm Minimal Validation

```
Width or height is too small       -- only checks for zero/negative, not overflow
Invalid dimensions                 -- generic error, no upper bound enforcement
Index exceeds GIF file image count -- frame index check only
St14overflow_error                 -- C++ overflow_error exception (present but unclear usage)
St15underflow_error                -- C++ underflow_error exception
```

The library checks for "too small" dimensions but there is **no evidence of maximum dimension validation** or integer overflow protection in the allocation path.

### 4. Integer Overflow in Allocation

The native lib uses `calloc` and `malloc` for frame buffers. A GIF with crafted Logical Screen Descriptor or Image Descriptor can specify:
- Width: 65535 (max uint16)
- Height: 65535 (max uint16)
- Resulting allocation: 65535 × 65535 × 4 bytes = ~16 GB (will fail)

More dangerous: dimensions that cause integer overflow in `width * height * bytes_per_pixel` calculation, resulting in a small allocation but large write.

## Attack Path (C2C)

### Code Flow: Network → Native Parsing

```
1. Attacker sends crafted GIF in chat message
   └─ Server accepts image/gif content type (s60.java, peb.java)

2. Victim's client downloads GIF to memory pool
   └─ ooa.java (PooledByteBuffer) - stores raw bytes, no validation

3. Content-type detection routes to GIF decoder
   └─ ke5.java checks GIF87a/GIF89a magic → identifies as GIF
   └─ C1338yh.m25960a() called for animated GIF decoding

4. Raw ByteBuffer passed to native
   └─ GifImage.m3862a(byteBuffer, qc8Var)
   └─ nativeCreateFromDirectByteBuffer(byteBuffer, Integer.MAX_VALUE, false)
   └─ Native: DGifOpen → DGifSlurp (parses entire GIF structure)

5. Frame rendering triggered on display
   └─ GifViewerWidget (one.me.chatmedia.viewer.photo.GifViewerWidget)
   └─ uf4.m22865i() → m22866j() → GifFrame.m3861g(width, height, bitmap)
   └─ Native: nativeRenderFrame writes pixels to bitmap buffer

6. OVERFLOW: If crafted GIF has mismatched dimensions between:
   - Image Descriptor (used for allocation size)
   - Actual LZW pixel data stream (amount of data written)
   → Heap overflow in native context
```

### C2C Attack Surface Confirmed

The `one.me.chatmedia.viewer.photo.GifViewerWidget` class:
- Takes `messageId` and `attachId` parameters (from received chat messages)
- Extends `BasePhotoViewerWidget` (standard media viewer)
- GIF auto-plays when scrolled into view in chat

Any user can send a GIF to any other user (1:1 chat, group chat). The GIF is decoded natively when the recipient views the conversation.

## Specific Vulnerability Vectors

### Vector 1: LZW Stream Overflow
Craft GIF with small Image Descriptor dimensions but oversized LZW data stream. `DGifGetLine` reads more pixels than the allocated row buffer.

### Vector 2: Sub-frame Overflow  
GIF frames can have offsets (xOffset, yOffset) and dimensions independent of the Logical Screen. A frame with `xOffset + frameWidth > screenWidth` causes out-of-bounds write during compositing.

### Vector 3: Color Map Overflow
`GifMakeMapObject` allocates based on color count from GIF header. Malformed color table size can cause heap corruption.

### Vector 4: Frame Count Resource Exhaustion
With `Integer.MAX_VALUE` as frame limit, a GIF declaring millions of frames causes massive memory allocation in `DGifSlurp` → OOM or heap spray.

## Affected Components

| Component | File | Role |
|-----------|------|------|
| GifImage | `com/facebook/animated/gif/GifImage.java` | JNI bridge to native |
| GifFrame | `com/facebook/animated/gif/GifFrame.java` | Frame data + render |
| C1338yh | `p000/C1338yh.java` | Animated image factory |
| uf4 | `p000/uf4.java` | Frame rendering/bitmap mgmt |
| GifViewerWidget | `one/me/chatmedia/viewer/photo/GifViewerWidget.java` | Chat media display |
| ooa | `p000/ooa.java` | Pooled byte buffer (no validation) |
| libgifimage.so | `native/lib/arm64-v8a/` | Native GIF parser (GIFLIB) |

## Recommendations

1. **Add dimension validation before native call**: Max width/height (e.g., 4096×4096)
2. **Add file size limit**: Reject GIFs over reasonable size before native parsing
3. **Set realistic frame count limit**: Replace `Integer.MAX_VALUE` with sane limit (e.g., 10000)
4. **Validate frame bounds**: Ensure `xOffset + frameWidth <= screenWidth` in native code
5. **Update GIFLIB**: Check if the bundled version has known CVEs patched
6. **Add AddressSanitizer testing**: Fuzz the native decoder with malformed GIFs

## References

- GIFLIB known vulnerabilities: CVE-2022-28506, CVE-2021-40633, CVE-2020-23922
- Facebook Fresco animated-gif module (open source base)
- GIF89a specification: frame dimensions independent of logical screen
