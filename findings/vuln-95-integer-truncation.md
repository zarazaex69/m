# Vuln-95: Integer Truncation Vulnerabilities in Max Messenger

## Summary

Multiple integer truncation issues in Max messenger's protocol, file handling, and media code that can lead to response confusion, buffer miscalculations, and undersized allocations.

---

## 1. Sequence Number: AtomicInteger → short (32-bit → 16-bit)

**File:** `p000/owb.java:116`

```java
AtomicInteger atomicInteger2 = this.f51589b.f54719b;
atomicInteger2.incrementAndGet();
short sShortValue = atomicInteger2.shortValue();  // TRUNCATION: 32-bit → 16-bit
```

**Field declaration** (`p000/pwb.java:95`):
```java
public final AtomicInteger f54719b = new AtomicInteger(0);
```

**Response dispatch** (`p000/owb.java:691`):
```java
a6d a6dVar = (a6d) pwbVar.f54741x.get(Short.valueOf(x5dVar.f79132c));
```

### Impact: Response Confusion / Handler Mismatch

The sequence counter is a monotonically incrementing `AtomicInteger` but is truncated to `short` (16-bit, range 0–65535) before being used as the key in the pending-request map (`f54741x`, a `ConcurrentHashMap<Short, a6d>`).

**Collision scenario:**
- After 65,536 requests, the sequence wraps around (request #65537 gets seq=1)
- If a prior request with seq=1 is still pending (slow server, timeout not yet fired), the new request **overwrites** the old entry in `f54741x`
- When the response for the OLD request arrives with seq=1, it is dispatched to the NEW request's handler
- The new request's response is then orphaned (no handler found)

**Consequences:**
- Wrong handler receives wrong response data → potential auth token leakage between handlers
- A login response could be delivered to a file-upload handler or vice versa
- Attacker on the network could delay responses to force collisions deliberately

**Severity:** Medium-High (information disclosure between request contexts)

---

## 2. LZ4 Payload Size: 24-bit Field from 32-bit int

**File:** `p000/x5d.java:127-128` (parsing) and `x5d.java:93` (encoding)

### Encoding (sender side):
```java
// Original uncompressed size = i (int, 32-bit)
// Compressed size = iCompress
byteBufferAllocate.putInt((((i / iCompress) + 1) << 24) | iCompress);
//                         ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  ^^^^^^^^
//                         ratio in top 8 bits               compressed size in lower 24 bits
```

### Decoding (receiver side):
```java
int i = byteBufferWrap.getInt();
this.f79134e = (byte) (i >> 24);   // compression ratio (top 8 bits)
int i2 = i & 16777215;             // payload length = lower 24 bits (0xFFFFFF max)
this.f79136g = i2;
```

### Decompression buffer allocation (`owb.java:769-770`):
```java
int i5 = x5dVar.f79136g * b3;  // decompressed size = compressed_size * ratio
byte[] bArr2 = new byte[i5];
d49.m4981F().safeDecompressor().decompress(bArrM17799a, 0, i, bArr2, 0, i5);
```

### Truncation Vulnerabilities:

1. **Compressed size limited to 24 bits (16,777,215 bytes max):** If `iCompress > 0xFFFFFF`, the lower 24 bits silently truncate the size. The receiver allocates a buffer based on the truncated value → **undersized buffer** → LZ4 decompressor writes out of bounds.

2. **Ratio limited to 8 bits (max 255):** The ratio `(i / iCompress) + 1` is stored in a single byte. If the actual compression ratio exceeds 255 (highly compressible data), the ratio is truncated → decompression buffer is undersized → **heap buffer overflow**.

3. **Integer overflow in decompression:** `x5dVar.f79136g * b3` can overflow a 32-bit int if a malicious server sends `f79136g = 0xFFFFFF` and `b3 = 127` → product = 2,130,706,177 which wraps negative → `new byte[negative]` throws or allocates wrong size.

**Severity:** High (remote heap corruption via crafted server response)

---

## 3. File Size: long → int Truncation

### 3a. RandomAccessFile.length() → int (`p000/j01.java:410,465`)
```java
int length = (int) randomAccessFile32.length();  // Files > 2GB truncate silently
i01Var.f26091c = (int) randomAccessFile4.length();
```

### 3b. Native library unpacking (`p000/w8j.java:161`)
```java
int length = (int) randomAccessFile2.length();
byte[] bArr = new byte[length];
```

### 3c. File read with guard (`p000/nv6.java:87`)
```java
long length = file.length();
if (length > 2147483647L) {
    throw new OutOfMemoryError(...);  // At least has a check
}
int i = (int) length;
```

### 3d. Upload size comparison (`UploadFileAttachWorker.java:2157`)
```java
if (j > ((int) r13.m20170m(PmsKey.f88741maxdownloadedsizefornotifykb, ...))) {
```
The config value (long) is truncated to int before comparison → files larger than 2GB always pass the size check.

**Impact:** For j01.java, a file > 2GB causes the int to wrap negative → `new byte[negative]` or incorrect seek offsets → data corruption in the emoji/sticker cache. For w8j.java, corrupted native library loading.

**Severity:** Medium (local DoS, potential code execution via corrupted .so)

---

## 4. Bitmap Dimension Truncation

### 4a. Float → int in AnimatedFileDrawable (`AnimatedFileDrawable.java:888`)
```java
this.f50188A0 = Bitmap.createBitmap((int) (f * f2), (int) (r0[1] * f2), Bitmap.Config.ARGB_8888);
```

If `f * f2` produces a value > `Integer.MAX_VALUE` or a negative float (due to corrupted metadata), the cast to `(int)` produces 0 or a negative value. `Bitmap.createBitmap` with dimensions ≤ 0 throws `IllegalArgumentException` (DoS), or with a very small truncated value creates an undersized bitmap that native code writes beyond.

### 4b. RectF dimensions (`p000/w75.java:173-174`)
```java
int iWidth3 = (int) rectF.width();
int iHeight3 = (int) rectF.height();
```

Float-to-int truncation loses the fractional part. If these dimensions are used to allocate a bitmap that native drawing code fills based on the original float dimensions, the bitmap is 1 pixel too small in each dimension → off-by-one heap write.

### 4c. Sticker size from server config (`ActPhotoEditor.java:87`)
```java
bitmapM11419b = trb.m22189n0(
    (int) rtdVar.m20170m(PmsKey.f88767minstickersize, 432),
    (int) rtdVar.m20170m(PmsKey.f88748maxstickersize, 512),
    bitmapM11419b);
```
Server-controlled long values truncated to int for bitmap operations. A malicious server config could supply values that truncate to small positive ints, causing undersized bitmap allocations.

**Severity:** Medium (DoS via crash, potential heap corruption in native bitmap operations)

---

## 5. Protocol Frame Field Summary

| Field | Wire Size | Source Type | Truncation Risk |
|-------|-----------|-------------|-----------------|
| seq (sequence) | 16-bit short | AtomicInteger (32-bit) | Wraps every 65536 packets |
| payload length | 24-bit (lower bits of int) | int | Max 16MB per frame |
| compression ratio | 8-bit (upper byte of int) | int division result | Max ratio 255x |
| cmd | 8-bit byte | byte | None |
| opcode | 16-bit short | short | None |

---

## Exploitation Scenarios

### Scenario A: Response Hijacking via Sequence Collision
1. Attacker performs MITM, delays server responses
2. Client sends >65536 requests, sequence wraps
3. Delayed response arrives with colliding seq number
4. Auth token from login response delivered to attacker-controlled handler

### Scenario B: Heap Overflow via Crafted LZ4 Frame
1. Malicious/compromised server sends packet with:
   - `ratio = 0x01` (byte field)
   - `compressed_size = 0xFFFFFF` (24-bit max)
   - Actual decompressed data is much larger
2. Client allocates `0xFFFFFF * 1 = 16MB` buffer
3. LZ4 decompressor writes beyond buffer → heap corruption

### Scenario C: Integer Overflow in Decompression Size
1. Server sends `ratio = 200`, `compressed_size = 0x800000` (8MB)
2. Decompression buffer = `0x800000 * 200 = 0x64000000` (1.6GB)
3. On 32-bit devices or memory-constrained environments → OOM crash (DoS)

---

## Recommendations

1. **Sequence number:** Use full 32-bit int for the pending-request map key, or detect and reject collisions before overwriting entries
2. **LZ4 size field:** Validate `compressed_size * ratio` doesn't overflow; cap decompressed size to a sane maximum (e.g., 64MB)
3. **File sizes:** Use `long` throughout file handling; reject files > `Integer.MAX_VALUE` explicitly where int is required
4. **Bitmap dimensions:** Validate dimensions are positive and within sane bounds before allocation; use `Math.max(1, ...)` pattern consistently
5. **Protocol parsing:** Add bounds checks on the 24-bit payload length field against available socket data before allocating buffers
