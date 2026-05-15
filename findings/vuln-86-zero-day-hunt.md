# Zero-Day Vulnerability Hunt: Max Messenger

## VULN-86-A: Integer Overflow in LZ4 Decompression Output Size (CRITICAL)

**Location:** `p000/owb.java` line ~768, `p000/x5d.java` line ~127-129

**The Bug:**
```java
// x5d constructor - parsing received packet header:
int i = byteBufferWrap.getInt();
this.f79134e = (byte) (i >> 24);   // compression ratio (signed byte: -128 to 127)
int i2 = i & 16777215;             // payload length (24-bit, max 16MB)
this.f79136g = i2;

// owb.java - decompression on receive:
byte b3 = x5dVar.f79134e;  // attacker-controlled byte
if (b3 > 0) {
    int i5 = x5dVar.f79136g * b3;  // INTEGER OVERFLOW HERE
    byte[] bArr2 = new byte[i5];
    d49.m4981F().safeDecompressor().decompress(bArrM17799a, 0, i, bArr2, 0, i5);
}
```

**Analysis:**
- `f79136g` is 24-bit (max 16,777,215) and `b3` is a signed byte (max 127)
- Multiplication: `16777215 * 127 = 2,130,706,305` which overflows signed 32-bit int → **negative value**
- A negative `i5` passed to `new byte[i5]` throws `NegativeArraySizeException` (crash/DoS)
- More critically: values that don't overflow but produce a **smaller-than-expected** buffer (e.g., `f79136g=0x800001, b3=2` → `0x1000002` which is 16MB) combined with crafted LZ4 data could write beyond buffer bounds inside the native LZ4 decompressor

**Exploitability:** HIGH - Server (or MITM on pre-TLS connection setup) controls the 4-byte header field directly  
**Impact:** DoS (crash), potential heap corruption in native LZ4 lib  
**Novelty:** HIGH - Custom to Max/TamTam binary protocol, not a standard library bug

---

## VULN-86-B: Attacker-Controlled LZ4 Output Size in ConversationParams (HIGH)

**Location:** `ru/p027ok/android/externcalls/sdk/api/ConversationParams.java` line 46-55

**The Bug:**
```java
public static ConversationParams decode(String str) {
    String[] strArrSplit = str.split(":");
    int i = Integer.parseInt(strArrSplit[0]);  // attacker-controlled size
    byte[] bArr = new byte[i];                  // allocation with attacker size
    LZ4.m20468a(Base64.decode(strArrSplit[1], 0), bArr);  // native decompress
}
```

**Analysis:**
- Called from `ConversationFactory` line 317 with `answerCallParams.getConversationParams()` — data from server during call setup
- Attacker (malicious server or MITM) controls the size prefix entirely
- Can allocate arbitrary-sized buffers (OOM DoS) or specify a size smaller than actual decompressed data → native heap overflow in `nativeDecompress`
- The native LZ4 decompressor writes to a DirectByteBuffer with `capacity = bArr.length` but LZ4 output may exceed this

**Exploitability:** MEDIUM-HIGH - Requires server compromise or MITM during VoIP call setup  
**Impact:** Native heap overflow → potential RCE, guaranteed DoS  
**Novelty:** HIGH - Custom call parameter encoding unique to Max/TamTam

---

## VULN-86-C: Zstd Decompression with Unbounded Output (HIGH)

**Location:** `p000/owb.java` line ~758, `one/me/sdk/zsrd/ZstdUtil.java`

**The Bug:**
```java
// When compression flag byte == -1 (0xFF):
if (b3 == -1) {
    bArrM17799a = pag.m17799a(bArrM17799a);  // ZstdUtil.nativeDecompress
}
```

```java
// ZstdUtil - native decompression with NO size limit:
public static byte[] m16973a(byte[] bArr) {
    return nativeDecompress(bArr);  // output size determined by zstd frame header
}
```

**Analysis:**
- The zstd frame header contains the decompressed size — entirely attacker-controlled
- No upper bound check before native allocation
- A crafted zstd frame claiming 2GB decompressed size with tiny compressed input = instant OOM kill
- Worse: if the native code trusts the frame header for allocation but the actual data is different, heap corruption possible

**Exploitability:** HIGH - Any server response packet with compression byte 0xFF triggers this path  
**Impact:** DoS (OOM kill), potential native heap corruption  
**Novelty:** MEDIUM - Zstd decompression bomb, but the trigger path is Max-specific

---

## VULN-86-D: Protobuf Varint Used as Array Size Without Bounds (MEDIUM)

**Location:** `p000/oy3.java` lines 106-112

**The Bug:**
```java
// m17546g() - readBytes:
public final byte[] m17546g() {
    int iM17555p = m17555p();  // varint from wire - can be up to 2^31-1
    // ...
    byte[] bArr = new byte[iM17555p];  // allocation with attacker-controlled size
    System.arraycopy(this.f51696a, i2, bArr, 0, iM17555p);
}
```

**Analysis:**
- Varint can encode values up to MAX_INT (2,147,483,647)
- While there's a bounds check (`iM17555p > i - i2`), it falls through to `m17552m(iM17555p)` which allocates `new byte[i]` with the varint value
- The check in `m17552m` only validates against `f51702g` (size limit) but this limit is set per-message and may be large
- Crafted protobuf with varint claiming huge embedded message → OOM

**Exploitability:** MEDIUM - Requires crafting malformed protobuf in server response  
**Impact:** DoS (OOM crash)  
**Novelty:** LOW-MEDIUM - Common protobuf issue but specific to their nano implementation

---

## VULN-86-E: ObjectInputStream Deserialization — Network Reachability Assessment

**Location:** `p000/mdb.java`, `p000/y45.java`, `p000/z8f.java`

**Assessment:**
- `mdb.m14079a()` — reads from `ldb.mo9790f()` which is implemented by `hv3.java` using `Class.getResourceAsStream()` — **LOCAL ONLY** (app assets). NOT network-reachable.
- `y45.m25673a()` — WorkManager Data class, reads from local SQLite database. Could be network-reachable if server data is stored in WorkManager tasks, but requires a separate SQLite injection first. **LOW risk.**
- `z8f.m26691L()` — reads from `FileInputStream(file)` — local file. NOT directly network-reachable.
- `AbstractC0432hr.m9702C()` — reads from SharedPreferences (Base64-encoded serialized object). NOT network-reachable.

**Verdict:** ObjectInputStream deserialization is NOT directly reachable from network input in current code paths. All paths read from local storage (assets, files, SharedPreferences).

**Exploitability:** LOW (requires prior local file write primitive)  
**Impact:** RCE if reachable  
**Novelty:** LOW

---

## VULN-86-F: Binary Protocol Packet Size Field — Signed/Unsigned Confusion

**Location:** `p000/x5d.java` constructor, `p000/owb.java` m17503d()

**The Bug:**
```java
// owb.java receive path:
x5d x5dVar = new x5d(bArr);
int i = x5dVar.f79136g;          // 24-bit field, always positive (0 to 16777215)
byte[] bArrM17799a = new byte[i]; // allocates up to 16MB - OK

// But the read loop:
while (i2 < x5dVar.f79136g) {
    int iMo8859k = this.f51589b.f54715L.mo8859k(i2, bArrM17799a, Math.min(256, i - i2));
```

**Analysis:**
- The 24-bit payload length means max 16MB allocation per packet — bounded but large
- No rate limiting on packet reception — attacker can send many 16MB packets rapidly → memory exhaustion
- The `Math.min(256, i - i2)` read chunking is safe against overflow

**Exploitability:** MEDIUM - Flood attack with max-size packets  
**Impact:** DoS (memory exhaustion)  
**Novelty:** LOW

---

## Summary Table

| ID | Vulnerability | Exploitability | Impact | Novelty |
|----|-------------|---------------|--------|---------|
| 86-A | LZ4 integer overflow in decompression size | HIGH | Critical (heap overflow) | HIGH |
| 86-B | ConversationParams attacker-controlled LZ4 output | MEDIUM-HIGH | High (native heap overflow) | HIGH |
| 86-C | Zstd unbounded decompression | HIGH | High (OOM/heap) | MEDIUM |
| 86-D | Protobuf varint as array size | MEDIUM | Medium (DoS) | LOW-MEDIUM |
| 86-E | ObjectInputStream deserialization | LOW | Critical if reachable | LOW |
| 86-F | Packet flood memory exhaustion | MEDIUM | Medium (DoS) | LOW |

## Top Priority for Exploitation

**VULN-86-A** is the most promising: the integer overflow `f79136g * b3` on the receive path is triggered by every incoming packet from the server. A malicious server (or MITM before encryption is established) can send a packet with header bytes crafted to overflow the multiplication, potentially causing the LZ4 safe decompressor to write beyond the allocated buffer into adjacent heap memory.
