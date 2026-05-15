# C2C (Client-to-Client) RCE Attack Paths — Max Messenger v26.15.3

## Overview

These are attack paths where User A sends content to User B, and User B's
device executes attacker-controlled code without any interaction beyond
viewing the message.

---

## Path 1: Animated Sticker → rlottie Heap Overflow (0-click)

### Attack Flow
```
Attacker → sends crafted Lottie JSON sticker → Server stores it →
Victim opens chat → sticker auto-downloads → RLottieDrawable.create() →
native libjlottie.so parses JSON → HEAP OVERFLOW → code execution
```

### Code Path (verified)
1. `maf.java:64` / `lcb.java:117` — `RLottieFactory.createByUrl(url, width, height, ...)`
2. `RLottieFactory.java:726` — `createByUrl(url.getUrl(), way.getWidth(), ...)`
3. File downloaded to disk cache
4. `RLottieDrawable.java:999` — `create(file.getAbsolutePath(), null, w, h, ...)`
5. **Native**: `libjlottie.so` parses Lottie JSON

### Validation: NONE
- No file size limit before native parse
- No JSON schema validation
- No sandboxing (same process)
- No content-type verification
- File goes directly from network → disk → native parser

### Known CVEs in rlottie
- CVE-2021-28021 (stb_image heap overflow)
- CVE-2021-37620 (heap buffer overflow in VBitmap)
- CVE-2021-37621 (heap buffer overflow in VDasher)
- CVE-2021-37622 (heap buffer overflow in VGradientCache)
- CVE-2021-37623 (heap buffer overflow in LOTCompLayerItem)

### Library Version
Built with clang 11.0.5 (old Telegram fork, ~2021 era). Likely unpatched.

### Exploitability: HIGH
- 0-click (auto-renders on chat open)
- No ASLR bypass needed (heap spray via multiple stickers)
- Attacker controls full JSON content
- No sandbox between native and Java

---

## Path 2: WebM Video Sticker → FFmpeg Heap Overflow (0-click)

### Attack Flow
```
Attacker → sends crafted WebM sticker → Server stores →
Victim opens chat → auto-download → AnimatedFileDrawable.createDecoder() →
native libffmpg.so (FFmpeg n4.4.3) → HEAP OVERFLOW → code execution
```

### Code Path
1. `dqk.java:82` — `WebmFactory.create(url, size, networkFetchEnabled=true)`
2. File downloaded to cache
3. `RunnableC1227vh.java:155` — `AnimatedFileDrawable.createDecoder(file.getAbsolutePath(), ...)`
4. **Native**: `libffmpg.so` Matroska demuxer + VP8/VP9 decoder

### Validation: NONE
- `isWebmSticker` flag determines routing to native FFmpeg
- No format validation before native call
- No dimension/duration limits enforced

### Known CVEs (FFmpeg n4.4.3 CONFIRMED)
- CVE-2024-22860 (RCE via crafted media)
- CVE-2024-22862 (RCE via crafted media)
- CVE-2023-50009 (heap buffer overflow)

### Exploitability: HIGH
- 0-click (auto-renders)
- FFmpeg n4.4.3 is 2+ years behind patches
- Matroska/WebM parser is historically vulnerable

---

## Path 3: SVG Image → Integer Overflow → Heap Corruption (0-click)

### Attack Flow
```
Attacker → sends message with SVG content (QR code, avatar) →
Victim views → ij7.java parses dimensions via regex →
nativeRenderSvg(svg, width, height) → width*height*4 overflows int32 →
small buffer allocated → large write → HEAP OVERFLOW
```

### Code Path
1. `ij7.java` — regex extracts width/height from SVG
2. `QrCodeGenerator.nativeRenderSvg(svgString, width, height)`
3. Native allocates `width * height * 4` bytes (ARGB)
4. If `width=65536, height=65536`: `65536*65536*4 = 0` (int32 overflow)
5. Allocates 0 or small buffer, writes full image data → heap overflow

### Validation
- Only 4-byte magic check (`<svg` or `<?xm`)
- No dimension cap
- No total pixel limit

### Exploitability: MEDIUM-HIGH
- Requires SVG to reach nativeRenderSvg (QR code generation path)
- Integer overflow is deterministic
- Heap layout may be predictable

---

## Path 4: JPEG Image → libjpeg-turbo Buffer Overflow (0-click)

### Attack Flow
```
Attacker → sends crafted JPEG → Victim views in chat →
Fresco pipeline → NativeJpegTranscoder.nativeTranscodeJpeg() →
libnative-imagetranscoder.so (libjpeg-turbo 2.1.5.1) → potential overflow
```

### Code Path
1. Image downloaded via Fresco HTTP pipeline
2. `NativeJpegTranscoderFactory` creates transcoder
3. `nativeTranscodeJpeg()` processes raw JPEG stream

### Validation
- File size limit: 40MB (PmsKey image-size)
- No decoded dimension validation
- No pixel count limit

### Exploitability: LOW-MEDIUM
- libjpeg-turbo 2.1.5.1 is relatively current
- No known unpatched CVEs for this version
- But: no dimension validation means OOM/DoS is trivial

---

## Path 5: DataChannel Binary → WebRTC Native (during call)

### Attack Flow
```
Attacker in call → sends crafted binary via DataChannel →
Victim's WebRTC → onMessage(ByteBuffer) → new byte[remaining()] →
dispatched to animoji/asr/screenShare handlers → native processing
```

### Code Path
1. Remote peer sends DataChannel message
2. `jx2.onMessage(DataChannel.Buffer)` — `new byte[buffer.data.remaining()]`
3. Dispatched to registered listeners (6+ channels)
4. Data reaches native vk::enh:: processors

### Validation: NONE
- No size limit on DataChannel messages
- No type/schema validation
- Direct dispatch to native handlers

### Exploitability: MEDIUM
- Requires active call with victim
- Attacker controls full binary content
- Native vk::enh code processes raw buffers

---

## Path 6: LZ4 Decompression → Heap Overflow (server-to-client)

### Attack Flow (from vuln-86-zero-day-hunt)
```
Compromised server/MITM → sends WebSocket packet with crafted size_cof →
owb.java: output_size = payload_length * compression_factor →
integer overflow → small buffer allocated → LZ4 decompresses into it →
HEAP OVERFLOW in native LZ4 decompressor
```

### Specifics
- `payload_length`: 24-bit (max 16,777,215)
- `compression_factor`: signed byte (max 127)
- `16777215 * 127 = 2,130,706,305` — overflows int32 to negative
- Native LZ4 allocates based on overflowed size → small buffer
- Decompresses full data into undersized buffer → heap overflow

### Exploitability: HIGH
- Triggered on every received packet
- No user interaction needed
- Requires MITM (trivial due to no cert pinning) or compromised server

---

## Summary Table

| # | Vector | Trigger | Validation | CVEs | Exploitability |
|---|--------|---------|-----------|------|----------------|
| 1 | Lottie sticker | 0-click (view chat) | NONE | 5 CVEs | HIGH |
| 2 | WebM sticker | 0-click (view chat) | NONE | 3 CVEs | HIGH |
| 3 | SVG image | 0-click (view) | Magic only | Novel | MEDIUM-HIGH |
| 4 | JPEG image | 0-click (view) | Size only | None known | LOW-MEDIUM |
| 5 | DataChannel | Active call | NONE | Novel | MEDIUM |
| 6 | LZ4 protocol | 0-click (connected) | NONE | Novel | HIGH |
