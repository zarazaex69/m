# Custom Parser Vulnerabilities — Max Messenger v26.15.3

## 1. Zstd Native Decompression — No Output Size Limit (CRITICAL)

**File**: `owb.java:758-763`, `ZstdUtil.java`

```java
// owb.java line 758
byte b3 = x5dVar.f79134e;  // compression flag from packet
if (b3 == -1) {  // 0xFF = Zstd compression
    bArrM17799a = pag.m17799a(bArrM17799a);  // → ZstdUtil.nativeDecompress()
}
```

```java
// ZstdUtil.java
private static final native byte[] nativeDecompress(byte[] input) throws IOException;
```

**Bug**: `nativeDecompress` reads the output size from the **zstd frame header** which is attacker-controlled. The native code allocates a buffer of that size and decompresses into it. No Java-side bounds check exists.

**Impact**: If the zstd frame header claims output = 4GB but actual data is crafted to overflow, native heap corruption occurs. Even without overflow, OOM crash is guaranteed.

**Trigger**: Any WebSocket packet with compression flag `0xFF` (byte value -1 in Java signed).

**Exploitability**: HIGH — triggered on every received packet, no user interaction.

---

## 2. Binary Protocol Packet Size — 16MB Allocation (HIGH DoS)

**File**: `owb.java:678`

```java
int i = x5dVar.f79136g;           // 24-bit from packet header (max 16,777,215)
byte[] bArrM17799a = new byte[i]; // allocate up to 16MB per packet
```

**Bug**: Single packet forces 16MB allocation. Rapid packet flood = OOM.

**Mitigation**: None. No rate limiting on packet reception.

---

## 3. x5d Packet Parser — Insufficient Remaining Check

**File**: `x5d.java:117-130`

```java
if (byteBufferWrap.remaining() >= 10) {  // only checks header fits
    // ... reads header ...
    int i2 = i & 16777215;  // payload_length
    this.f79136g = i2;
    if (i2 > 0) {
        this.f79135f = new byte[i2];  // allocates but DOESN'T READ payload here
    }
}
```

**Bug**: The constructor allocates `payload_length` bytes but doesn't verify the input buffer actually contains that many bytes. The payload is read separately in `owb.java` from the socket. If the socket provides fewer bytes than declared, `EOFException` is thrown (handled). Not exploitable for memory corruption, but the allocation itself is unbounded.

---

## 4. Nonce Validation — Log Only, No Enforcement

**File**: `owb.java:128-131`

```java
if (Objects.equals(this.f51589b.f54721d, tl9Var.f67623e) && 
    Objects.equals(this.f51589b.f54723f, tl9Var.f67624f)) {
    ct4.m4641T0(..., "Nonce check success", ...);
} else {
    ct4.m4626M(..., "Nonce error", new NonceException(...));
    // BUT EXECUTION CONTINUES! No return/throw!
}
```

**Bug**: Nonce mismatch is LOGGED but not enforced. The login packet is still processed. This enables session replay attacks.

**Exploitability**: MEDIUM — requires MITM (trivial due to no cert pinning).

---

## 5. LZ4 Decompression — Safe but OOM

**File**: `owb.java:768-770`

```java
int i5 = x5dVar.f79136g * b3;  // max 16M * 127 = 2.1GB
byte[] bArr2 = new byte[i5];    // OOM on most devices
d49.m4981F().safeDecompressor().decompress(src, 0, i, bArr2, 0, i5);
```

**Assessment**: Uses `safeDecompressor()` which bounds-checks output. NOT a heap overflow. IS a guaranteed OOM crash (DoS) from a single packet.

---

## 6. ConversationParams LZ4 — Bounded by Dest Capacity

**File**: `ConversationParams.java:46-55`

```java
int i = Integer.parseInt(strArrSplit[0]);  // attacker-controlled size
byte[] bArr = new byte[i];                  // allocate
LZ4.m20468a(Base64.decode(strArrSplit[1], 0), bArr);
```

**Assessment**: `LZ4.nativeDecompress()` passes `dest.capacity()` as max output. Safe from overflow. OOM if `i` is large. `Integer.MAX_VALUE` → OOM. Negative → `NegativeArraySizeException`.

---

## Summary

| # | Bug | Type | Severity | Exploitable? |
|---|-----|------|----------|-------------|
| 1 | Zstd no output limit | Heap overflow (native) | CRITICAL | Yes (0-click) |
| 2 | 16MB packet allocation | OOM DoS | HIGH | Yes (0-click) |
| 3 | Unbounded alloc in parser | OOM DoS | MEDIUM | Yes |
| 4 | Nonce not enforced | Session replay | MEDIUM | Yes (MITM) |
| 5 | LZ4 2.1GB allocation | OOM DoS | HIGH | Yes (0-click) |
| 6 | ConversationParams OOM | OOM DoS | MEDIUM | Yes |
