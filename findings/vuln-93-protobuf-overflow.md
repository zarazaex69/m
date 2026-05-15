# VULN-93: Integer Overflow in Protobuf Array Allocation

## Summary

The Max messenger (TamTam) Android client uses protobuf-nano for message deserialization. The `mergeFrom()` methods in `Protos.java` and `Tasks.java` allocate arrays based on counts derived from wire data without adequate bounds checking. This creates integer overflow and OOM denial-of-service vectors.

## Severity: Medium-High (DoS / Client Crash)

## Affected Code

- **Primary files**: `ru/p027ok/tamtam/nano/Protos.java`, `ru/p027ok/tamtam/nano/Tasks.java`
- **Parser class**: `p000/oy3.java` (CodedInputStream equivalent)
- **Count helper**: `p000/qcc.java` method `m18779D()` (counts repeated field occurrences)
- **Base class**: `p000/rva.java` (MessageNano equivalent)

## Vulnerable Pattern

### Pattern 1: Unpacked repeated fields (tag-counted allocation)

Found in `Protos.Attaches.Buttons.mergeFrom()` (line 636), `Protos.Attaches.Call.mergeFrom()` (line 818), `Protos.Attaches.Control.mergeFrom()` (line 1244), `Tasks.AssetsListModify.mergeFrom()` (line 238), and many others:

```java
// qcc.m18779D() scans ahead counting how many consecutive tags match
int iM18779D = qcc.m18779D(oy3Var, 40);  // count from wire data
long[] jArr = this.contactIds;
int length = jArr == null ? 0 : jArr.length;
int i = iM18779D + length;               // <-- POTENTIAL INTEGER OVERFLOW
long[] jArr2 = new long[i];              // <-- allocation with overflowed size
if (length != 0) {
    System.arraycopy(jArr, 0, jArr2, 0, length);  // <-- copy with wrong bounds
}
```

### Pattern 2: Packed repeated fields (length-delimited, pre-counted)

```java
int iM17544e = oy3Var.m17544e(oy3Var.m17555p());  // push limit from varint
int iM17542c = oy3Var.m17542c();                    // save position
int i2 = 0;
while (oy3Var.m17541b() > 0) {                     // count elements in packed data
    oy3Var.m17556q();
    i2++;
}
oy3Var.m17559t(iM17542c);                          // rewind
long[] jArr3 = this.contactIds;
int length2 = jArr3 == null ? 0 : jArr3.length;
int i3 = i2 + length2;                             // <-- POTENTIAL INTEGER OVERFLOW
long[] jArr4 = new long[i3];                       // <-- allocation
```

## Analysis

### How `qcc.m18779D()` works (the count function):

```java
public static final int m18779D(oy3 oy3Var, int i) {
    int iM17542c = oy3Var.m17542c();
    oy3Var.m17560u(i);       // skip current field
    int i2 = 1;
    while (oy3Var.m17558s() == i) {  // count consecutive matching tags
        oy3Var.m17560u(i);
        i2++;                         // unbounded increment
    }
    oy3Var.m17559t(iM17542c);        // rewind
    return i2;
}
```

This function iterates through the input buffer counting matching tags. The count is bounded only by the buffer size — there is no explicit cap on the returned value.

### Integer Overflow Scenario (Pattern 1)

If a message is parsed multiple times (e.g., mergeFrom called repeatedly on the same object), or if the buffer contains enough repeated tags:

- `iM18779D` returns a large value (up to buffer_size / min_tag_size)
- `length` (existing array) could be non-zero from a prior parse
- `iM18779D + length` can overflow `Integer.MAX_VALUE`, producing a **negative value**
- `new long[negative]` → `NegativeArraySizeException` → crash

### OOM Scenario

Even without overflow, a crafted message with many small repeated varint fields can cause:
- A buffer of ~1MB containing 1-byte varint tags + 1-byte values = ~500K elements
- `new long[500000]` = 4MB allocation per field
- Multiple repeated fields in a single message multiply this effect
- Nested messages amplify further

### Practical Constraints

The `oy3` (CodedInputStream) class operates on a **byte array already in memory**:
```java
public oy3(byte[] bArr, int i, int i2) {
    this.f51696a = bArr;  // entire message must fit in memory
    ...
}
```

This means the count from `m18779D()` is bounded by `buffer.length / 2` (minimum 2 bytes per tag+value). For a 1MB message, this caps at ~500K elements.

However, the **packed repeated field** path (Pattern 2) counts elements by reading varints, where a single byte can represent a value. A packed field with a declared length of N bytes can contain up to N elements (1-byte varints), leading to allocations of `N * element_size`.

## Maximum Message Size Enforcement

**Finding: No pre-parsing size limit was found in the protobuf layer.**

- The `oy3` class has no `setSizeLimit()` method or equivalent
- The `rva.mergeFrom(T t, byte[] bArr)` simply wraps the entire byte array — no size check
- The recursion depth is limited to 64 (`f51703h >= 64` check in `m17549j`)
- No evidence of a transport-layer message size cap was found in the decompiled protobuf handling code

The only implicit limit is that the entire message must be loaded as a `byte[]` before parsing, so JVM heap constrains the maximum message size. But on Android with typical 256MB-512MB heaps, a multi-MB protobuf message is feasible.

## Attack Vectors

1. **Crafted repeated field with many tags**: Send a protobuf message with field tag X repeated thousands of times. Each occurrence is a minimal varint (1-2 bytes), but the parser allocates an array of size = count.

2. **Packed repeated field with inflated length**: Declare a packed field with a large byte-length containing minimal 1-byte varints. The parser counts all elements, then allocates an array of that count × element size.

3. **Repeated mergeFrom on same object**: If application logic calls mergeFrom multiple times on the same proto object (e.g., incremental updates), `length + newCount` can overflow.

4. **Nested message amplification**: A message containing repeated sub-messages, each containing repeated fields, creates multiplicative allocation.

## Impact

| Scenario | Result |
|----------|--------|
| Large count, sufficient heap | OOM → app crash |
| Integer overflow (count + existing length) | NegativeArraySizeException → crash |
| Moderate count, many fields | GC pressure → UI freeze → ANR |
| Repeated parsing of same object | Gradual memory exhaustion |

## Proof of Concept Approach

Craft a protobuf binary message targeting `Protos.Attaches.Call.contactIds` (field number 5, wire type 0 = varint):
- Tag byte: `0x28` (field 5, varint) repeated N times
- Each followed by a 1-byte varint value (e.g., `0x01`)
- Total message size: 2*N bytes
- Resulting allocation: `N * 8` bytes (long[])

For N=100,000 (200KB message): allocates 800KB array
For N=10,000,000 (20MB message): allocates 80MB array (4x amplification)

## Recommendations

1. Add a maximum element count check before array allocation in all `mergeFrom()` methods
2. Enforce a maximum message size at the transport layer before passing to protobuf parsing
3. Use checked arithmetic for `count + existingLength` to detect overflow
4. Consider switching from protobuf-nano (unmaintained) to protobuf-lite which has better bounds checking
5. Implement progressive/streaming parsing for large repeated fields instead of pre-allocating full arrays

## References

- Source: `~/max/decompiled/jadx/base/sources/ru/p027ok/tamtam/nano/Protos.java`
- Source: `~/max/decompiled/jadx/base/sources/ru/p027ok/tamtam/nano/Tasks.java`
- Parser: `~/max/decompiled/jadx/base/sources/p000/oy3.java`
- Count helper: `~/max/decompiled/jadx/base/sources/p000/qcc.java`
- Base class: `~/max/decompiled/jadx/base/sources/p000/rva.java`
