# Protobuf Deserialization Vulnerabilities in Max Messenger

## 1. Message Types with Nested Messages / Repeated Fields

Key file: `ru/p027ok/tamtam/nano/Protos.java` (12,352 lines)

### Critical Nested Message Hierarchy:
- `Protos.Attaches` → `Attach[]` (repeated), `InlineKeyboard`, `SendAction`, `ReplyKeyboard`
- `Protos.Attaches.Attach` → `Photo`, `Video`, `Audio`, `File`, `Call`, `Contact`, `Share`, `Sticker`, `App`, `Poll`, `Widget`, `Location`, `Present`, `Control`, `InlineKeyboard`
- `Protos.Chat` → `Chunk[]`, `Section[]`, `ChatSettings`, `ChatMedia` (x8), `VideoConversation`, `GroupChatInfo`, `ChatOptions`, `BotsInfo`, `PushMessage`, `LiveStream`, `AdminParticipant` (map)
- `Protos.Chat.VideoConversation` → `long[] previewParticipantIds`
- `Protos.Chat.Section` → `long[] stickers`
- `Protos.MessageElements` → `MessageElement[]`
- `Protos.Attaches.Attach.Poll` → `Answer[]`, `AnswerStats[]`, `Result[]`, `State`

### Repeated Fields (arrays without bounds):
- `Attach[] attach`, `Button[] button`, `Buttons[] buttons`, `Chunk[] chunk`, `Section[] sections`
- `String[] stickersOrder`, `int[] localChanges`, `int[] options`, `long[] admins`
- `long[] previewParticipantIds`, `long[] chatFoldersIds`, `byte[] wave`, `byte[] draft`

## 2. Protobuf Parsing Entry Points

### Base class: `p000/rva.java` (protobuf-nano MessageNano equivalent)
```java
public static final <T extends rva> T mergeFrom(T t, byte[] bArr, int i, int i2) {
    oy3 oy3Var = new oy3(bArr, i, i2);
    t.mergeFrom(oy3Var);
    oy3Var.m17540a(0);
    return t;
}
```

### CodedInputStream: `p000/oy3.java`
- `m17555p()` - readRawVarint32
- `m17556q()` - readRawVarint64
- `m17546g()` - readBytes
- `m17557r()` - readString
- `m17549j(rva)` - readMessage (nested)

### Network entry points:
- `p000/ss3.java` - Deserializes `Protos.Chat` from byte[] received from binary protocol
- `p000/owb.java` - Binary session protocol handler with LZ4 decompression
- `p000/eoe.java` - Message storage proto with `Protos.Attaches` and `Protos.MessageElements`
- `p000/nne.java` - Deserializes inline message elements from `el8.f16185d` byte arrays

### Binary Protocol (owb.java / pwb):
Uses custom binary framing with LZ4 compression over TCP/WebSocket. Packets are decompressed then parsed via protocol-specific deserializers (`kf0`, `lf0`, `ig0`, `ox2`, etc.).

## 3. Type Confusion Vulnerabilities

### VULN-01: Generic field type dispatch in oy3.m17550k()
```java
public final Serializable m17550k(int i) {
    switch (i) {
        case 1: return Double.valueOf(m17547h());
        case 5: return Integer.valueOf(m17555p());
        case 12: return m17546g();  // bytes
        case 13: return Integer.valueOf(m17555p());
        // ...
    }
}
```
The type parameter `i` is caller-controlled. If a field's wire type doesn't match the expected schema type, the same bytes are interpreted differently. No validation that wire type matches field type in the generic path.

### VULN-02: Enum fields accept any int value
```java
// Chat.mergeFrom - type field
int iM17555p = oy3Var2.m17555p();
if (iM17555p == 0 || iM17555p == 1 || iM17555p == 2 || iM17555p == 3 || iM17555p == 4) {
    this.type = iM17555p;
}
// Values outside range are silently ignored - field retains default
```
While not directly exploitable, downstream code may assume the field was set when it wasn't.

### VULN-03: Wire type mismatch in m17560u() skip logic
```java
public final boolean m17560u(int i) {
    int i2 = i & 7;  // wire type from tag
    if (i2 == 2) {
        m17561v(m17555p());  // skip length-delimited - size from varint
        return true;
    }
}
```
A crafted tag with wrong wire type causes the parser to skip incorrect amounts of data, potentially reading subsequent fields with wrong types.

## 4. Missing Bounds Checks - OOM / Buffer Issues

### VULN-04: Unbounded repeated field allocation via qcc.m18779D()
```java
// qcc.java:141
public static final int m18779D(oy3 oy3Var, int i) {
    int iM17542c = oy3Var.m17542c();
    oy3Var.m17560u(i);
    int i2 = 1;
    while (oy3Var.m17558s() == i) {
        oy3Var.m17560u(i);
        i2++;
    }
    oy3Var.m17559t(iM17542c);
    return i2;
}
```
This counts repeated elements by scanning forward. The count is then used to allocate arrays:
```java
// Chat.mergeFrom - chunk field
int iM18779D = qcc.m18779D(oy3Var2, 114);
Chunk[] chunkArr2 = new Chunk[iM18779D + length]; // NO MAX CHECK
```

**Exploitation:** A malicious protobuf with millions of repeated field tags (each minimal size ~2 bytes) in a ~2MB message could cause allocation of millions of objects → OOM crash.

### VULN-05: Bytes field allocation without size limit
```java
// oy3.java - m17546g() readBytes
public final byte[] m17546g() {
    int iM17555p = m17555p();  // attacker-controlled size
    if (iM17555p > i - i2 || iM17555p <= 0) {
        return iM17555p == 0 ? qcc.f56203h : m17552m(iM17555p);
    }
    byte[] bArr = new byte[iM17555p];  // allocation based on varint
    System.arraycopy(this.f51696a, i2, bArr, 0, iM17555p);
    return bArr;
}
```
The size is bounded by the buffer limit (`m17552m` checks), but a crafted message with a large declared size that matches available buffer can still allocate large byte arrays.

### VULN-06: Recursive nesting depth limit too high
```java
// oy3.java - m17549j()
public final void m17549j(rva rvaVar) {
    int iM17555p = m17555p();
    if (this.f51703h >= 64) {  // 64 levels deep!
        throw new InvalidProtocolBufferNanoException("too many levels");
    }
    this.f51703h++;
    rvaVar.mergeFrom(this);
    this.f51703h--;
}
```
64 levels of nesting with object allocation at each level can cause significant stack/heap pressure.

### VULN-07: Integer overflow in array size calculation
```java
int i = iM18779D + length;  // can overflow if both are large
Chunk[] chunkArr2 = new Chunk[i];  // negative size → NegativeArraySizeException
```
If `iM18779D` (from scanning) + existing `length` overflows int, it wraps negative → crash or unexpected behavior.

## 5. Data Flow to Native Code (JNI)

### Path 1: Protobuf → LZ4 Native Decompression
`owb.java` handles binary protocol packets that use LZ4 compression (`net.jpountz.lz4.LZ4JNI`):
```java
// LZ4JNI.java
public static native int LZ4_decompress_safe(byte[] src, ByteBuffer srcBuf, 
    int srcOff, int srcLen, byte[] dest, ByteBuffer destBuf, int destOff, int destLen);
```
Corrupted compressed data from network flows directly to native LZ4 decompressor.

### Path 2: Protobuf → Image Decoding
`nne.java` deserializes byte arrays from protobuf message elements and passes to native image decoders:
```java
wk8 wk8Var = (wk8) rva.mergeFrom(new wk8(), el8Var.f16185d);
byte[] bArr = wk8Var.f77307a;  // raw image bytes from protobuf
Bitmap bitmapM25580a = xz0.m25580a(bArr);  // → native bitmap decode
```

### Path 3: Protobuf → WebRTC Native
`org/webrtc/DataChannel.java` has native `send()` that takes byte buffers. Media attachment data parsed from protobuf can flow through to WebRTC native layer.

### Path 4: Protobuf → Lottie Animation Native
`RLottieDrawable` native methods process animation data that originates from sticker attachments in protobuf messages.

## 6. Integer Overflow in Size Calculations

### VULN-08: computeSerializedSize overflow
```java
// Protos.java - Attaches.computeSerializedSize()
public int computeSerializedSize() {
    int iM18405i = 0;
    if (attachArr != null && attachArr.length > 0) {
        while (iM18405i < attachArr2.length) {
            iM18405i = py3.m18405i(1, attach) + iM18405i;  // accumulates without overflow check
        }
    }
}
```
With enough attachments, the serialized size can overflow int, leading to undersized buffer allocation in `toByteArray()`.

### VULN-09: getSerializedSize used for buffer allocation
```java
// rva.java
public static final byte[] toByteArray(rva rvaVar) {
    int serializedSize = rvaVar.getSerializedSize();  // can overflow
    byte[] bArr = new byte[serializedSize];  // wrong size allocation
    toByteArray(rvaVar, bArr, 0, serializedSize);
    return bArr;
}
```

## Exploitation Scenarios

1. **Remote OOM DoS**: Send a crafted protobuf message via the binary protocol with thousands of repeated `Attach` elements. The `qcc.m18779D` scanner counts them all, allocates a massive array, and each element triggers nested object creation → app crashes with OOM.

2. **Stack exhaustion**: Craft deeply nested protobuf messages (up to 64 levels) where each level contains another length-delimited message. Combined with repeated fields at each level, this exhausts stack space.

3. **LZ4 buffer corruption**: Send a malformed LZ4-compressed packet where the decompressed size header claims a small size but the actual data decompresses larger → native heap corruption in LZ4 JNI.

4. **Image decoder exploitation**: Embed a malformed image in a protobuf `wk8` message element byte field. The raw bytes flow directly to native bitmap decoding without content validation → potential native heap corruption.

5. **Integer overflow → heap corruption**: Trigger `computeSerializedSize` overflow by constructing a message with enough fields to wrap the int size, then serialize it → undersized buffer allocation followed by out-of-bounds write in `writeTo()`.
