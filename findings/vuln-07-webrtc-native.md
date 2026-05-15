# VULN-07: WebRTC Native Library Deep Analysis

## Library Overview

- **File**: `libjingle_peerconnection_so.so` (13MB, arm64-v8a)
- **WebRTC Version**: Cannot determine exact version; contains modern features (corruption_detection, SFrame, AI opus BWE, DRED) suggesting recent build (M120+)
- **Custom Code**: VK namespace (`vk::enh::*`) with audio enhancement, Animoji, KWS (keyword spotting), TFLite integration
- **JNI Exports**: 226 Java_* functions
- **Crypto**: Uses BoringSSL/OpenSSL for SRTP (AES-128/192/256 CTR, AES-256-GCM, ChaCha20)

## 1. Memory Corruption Indicators

Strings referencing memory safety issues in native code:

```
__stack_chk_fail
Buffer too small (
vp9_pack_bitstream: output buffer full
record overflow
Truncated packet or corrupt header length
Truncated packet or corrupt partition %d length
Failed to protect SRTCP packet: The buffer capacity
bytes_written <= buffer.size()
buffer_usage.id >= 0
direct_buffer_capacity_in_bytes_ == frames_per_buffer_ * bytes_per_frame
assertion failed: _bits>0
assertion failed: ret==packet_frame_size
```

**Key finding**: Stack canaries present (`__stack_chk_fail`) but numerous assertion-based bounds checks suggest the code relies on runtime assertions rather than safe-by-design patterns.

## 2. Codec Attack Surface

Supported codecs (all software-decoded in native):
- **Video**: H.264, VP8, VP9 (software encoder/decoder factories)
- **Audio**: Opus (with DRED, AI BWE, multichannel), SILK
- **Custom**: VK audio enhancement pipeline, Animoji processing

JNI codec functions:
```
Java_org_webrtc_LibvpxVp8Decoder_nativeCreateDecoder
Java_org_webrtc_LibvpxVp8Encoder_nativeCreate
Java_org_webrtc_H264Utils_nativeIsSameH264Profile
Java_org_webrtc_SoftwareVideoDecoderFactory_nativeCreate
Java_org_webrtc_SoftwareVideoEncoderFactory_nativeCreate
```

## 3. DataChannel Message Handling (Critical Attack Surface)

**DataChannels created**:
- `producerCommand` — command channel
- `producerNotification` — notification channel  
- `consumerScreenShare` / `producerScreenShare` — screen sharing
- `asr` — speech recognition

**Message flow** (peer → native → Java):
1. Native calls `DataChannel.Observer.onMessage(Buffer)` via JNI
2. `jx2.onMessage()` extracts raw bytes from `ByteBuffer` with NO size validation
3. Bytes dispatched to `xyf.mo8006a(a55, byte[], int)` listeners
4. **No length checks, no schema validation, no sanitization** before dispatch

```java
// jx2.onMessage - no validation whatsoever
public void onMessage(DataChannel.Buffer buffer) {
    ByteBuffer byteBuffer = buffer.data;
    byte[] bArr = new byte[byteBuffer.remaining()];  // arbitrary size from peer
    int i = buffer.binary ? 2 : 1;
    byteBuffer.get(bArr);
    // Direct dispatch to all listeners with raw bytes
    ((xyf) it.next()).mo8006a(a55Var, bArr, i);
}
```

**Vulnerability**: Arbitrary binary data from remote peer is passed directly to command handlers without any validation. A malicious peer can send crafted binary payloads to `producerCommand` channel.

## 4. SRTP/DTLS Implementation

- Uses **libsrtp** (standard) via BoringSSL for DTLS-SRTP key exchange
- AES-128/192/256 CTR and AES-256-GCM cipher suites
- ChaCha20 ARMv8 optimized (CRYPTOGAMS)
- **SFrame** support present (end-to-end encryption option)
- **FrameEncryptor/FrameDecryptor** interfaces exist for insertable streams
- **NOT a custom implementation** — standard WebRTC SRTP stack

Error strings confirm standard libsrtp error handling:
```
Failed to create SRTP session, err=
Failed to protect SRTP packet, seqnum=
Failed to unprotect SRTP packet, err=
DTLS-SRTP key export failed
```

## 5. Video Frame Buffer Handling

**Native pixel buffer operations** (JNI boundary):

```
Java_org_webrtc_JavaI420Buffer_nativeCropAndScaleI420
Java_org_webrtc_NV12Buffer_nativeCropAndScale
Java_org_webrtc_NV21Buffer_nativeCropAndScale
Java_org_webrtc_JniCommon_nativeAllocateByteBuffer
Java_org_webrtc_JniCommon_nativeFreeByteBuffer
```

**JavaI420Buffer.wrap()** has `checkCapacity()` validation:
```java
private static void checkCapacity(ByteBuffer buf, int width, int height, int stride) {
    int required = ((height - 1) * stride) + width;
    if (buf.capacity() < required) throw IllegalArgumentException;
}
```

**NV12Buffer/NV21Buffer** — NO capacity checks in Java before calling native `nativeCropAndScale`. Parameters (cropX, cropY, cropWidth, cropHeight) passed directly to native with no bounds validation at Java layer.

**VK Custom Processing**:
```
Java_org_webrtc_NativeAndroidVideoTrackSource_nativeOnFrameCaptured
Java_org_webrtc_PeerConnectionFactory_nativeSetAnimojiParams
```
Animoji processing operates on raw video frames in native code (`animoji_impl.cc`).

## 6. ICE Candidate / SDP Injection

**No input validation at Java layer**:

`IceCandidate` constructor accepts raw strings without any sanitization:
```java
public IceCandidate(String sdpMid, int sdpMLineIndex, String sdp) {
    this.sdpMid = str;        // no validation
    this.sdpMLineIndex = i;   // no range check
    this.sdp = str2;          // no sanitization
}
```

**SDP handling in PeerConnectionClient** (`ead.java`):
- Remote SDP string is passed through `m15731e()` (SDP munging/modification)
- Then directly to `peerConnection.setRemoteDescription()`
- **No SDP sanitization or validation before native parsing**
- Native has SDP munging detection: `"ReadyToUseRemoteCandidate: Candidate not valid because of SDP munging"`

**Mitigation**: Native WebRTC has its own SDP parser with some validation, but malformed SDP can still trigger parser bugs in native code.

## 7. VK Custom Native Extensions (Unique Attack Surface)

VK added custom native code in `vk::enh` namespace:
- `vk::enh::EnhancementFactory::createAlgorithm`
- `vk::enh::PipelineFactory::createAlgorithm`
- `vk::enh::AnimojiFactory::createAlgorithm`
- `vk::enh::KWSFactory::createAlgorithm` (keyword spotting)
- `vk::enh::AudioBuffer` — custom audio buffer handling
- `vk::enh::initTfLite` — TensorFlow Lite initialization

These are **non-upstream custom additions** that haven't undergone Chromium's security review process.

## Risk Assessment

| Attack Vector | Severity | Exploitability |
|---|---|---|
| DataChannel binary injection (no validation) | HIGH | Remote, peer-to-peer |
| NV12/NV21 cropAndScale without bounds check | MEDIUM | Requires media session |
| SDP injection (no Java-side sanitization) | MEDIUM | Requires signaling access |
| VK custom audio/animoji native code | MEDIUM | Requires call session |
| Opus decoder assertion failures | LOW | Requires crafted audio |

## Recommendations

1. **DataChannel**: Add message size limits and schema validation before dispatching to handlers
2. **Video buffers**: Add bounds checking in NV12Buffer.cropAndScale() before native call
3. **SDP**: Validate/sanitize SDP strings before passing to native setRemoteDescription
4. **VK extensions**: Audit `vk::enh::*` code independently — not covered by upstream WebRTC security
5. **Fuzzing**: Target `nativeCropAndScale` JNI functions with malformed dimensions
