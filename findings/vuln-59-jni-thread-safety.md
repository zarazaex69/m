# VULN-59: JNI Thread Safety — Use-After-Free and Race Conditions

## Summary

Multiple JNI-backed classes in Max messenger lack proper thread synchronization around native pointer lifecycle management. This creates exploitable use-after-free, double-free, and TOCTOU (Time-of-Check-Time-of-Use) race conditions that can lead to native code execution.

## Severity: HIGH

## Affected Components

### 1. Native Method Declarations Found

Key classes with native methods:
- `org/webrtc/PeerConnection.java` — 30+ native methods (nativeClose, nativeAddIceCandidate, nativeCreateOffer, etc.)
- `org/webrtc/DataChannel.java` — 8 native instance methods (nativeSend, nativeClose, nativeState, etc.)
- `org/webrtc/RtpReceiver.java` — 6 native methods
- `org/webrtc/RtpSender.java` — 10 native methods
- `org/webrtc/VpxEncoderWrapper.java` — 5 native methods (nativeCreate, nativeEncode, nativeRelease)
- `org/webrtc/PeerConnectionFactory.java` — 15+ native methods
- `one/p011me/rlottie/RLottieDrawable.java` — create, destroy, getFrame, replaceColors
- `one/p011me/sdk/media/ffmpeg/AnimatedFileDrawable.java` — createDecoder, destroyDecoder, getVideoFrame
- `one/video/calls/audio/opus/FileWriter.java` — nativeAudioWriteFrame, nativeRelease
- `one/p011me/sdk/p018gl/effects/VideoMessageStencilHolder.java` — 7 native methods

### 2. Thread Safety Analysis

#### Classes with NO synchronization on native pointer access:
- **PeerConnection**: `nativePeerConnection` is `final long` — no null check before native calls, no synchronization
- **DataChannel**: `nativeDataChannel` checked via `checkDataChannelExists()` but NOT synchronized
- **RtpReceiver**: `nativeRtpReceiver` checked but NOT synchronized
- **RtpSender**: `nativeRtpSender` checked but NOT synchronized
- **VpxEncoderWrapper**: `encoder` field — check-then-use with NO synchronization
- **VideoMessageStencilHolder**: `nativeInstance` is `final` — no dispose guard at all, `release()` can be called while `render()` is active

#### Classes with partial/broken synchronization:
- **AnimatedFileDrawable**: `nativePtr` is `volatile long` — provides visibility but NOT atomicity for check-then-use
- **RLottieDrawable**: `f50018j1` is `volatile long` — same issue

### 3. PeerConnection Lifecycle — Dispose While Callbacks Fire

**Critical Finding**: `PeerConnection.dispose()` calls `nativeClose()` then iterates and disposes all senders, receivers, and transceivers. However:

1. The native `Observer` interface has callbacks (`onIceCandidate`, `onDataChannel`, `onAddTrack`, etc.) annotated with `@CalledByNative` — these fire from native threads
2. `dispose()` does NOT unregister the observer before freeing resources
3. No synchronization prevents a callback from firing during or after `dispose()`
4. The observer pointer created via `nativeCreatePeerConnectionObserver()` is never explicitly freed in the Java layer

**Race window**: Native thread delivers callback → Java observer accesses PeerConnection state → main thread calls `dispose()` → native memory freed → callback accesses freed memory.

### 4. Ref-Counting Mechanism

`JniCommon` provides ref-counting:
```java
public class JniCommon {
    public static native void nativeAddRef(long j);
    public static native void nativeReleaseRef(long j);
    // ...
}
```

Used by: `RtpReceiver.dispose()`, `RtpSender.dispose()`, `RtpTransceiver.dispose()`, `DataChannel.dispose()`, `MediaStreamTrack.dispose()`.

**NOT used by**: `PeerConnection` (uses `nativeFreeOwnedPeerConnection`), `VpxEncoderWrapper`, `AnimatedFileDrawable`, `RLottieDrawable`, `VideoMessageStencilHolder`, `FileWriter`.

The ref-counting itself is not thread-safe at the Java level — nothing prevents `nativeReleaseRef` from being called concurrently with a native method that uses the same pointer.

### 5. Native Pointer Validation Before Use

Pattern found in multiple classes — a simple null check with no synchronization:

```java
// DataChannel.java
private void checkDataChannelExists() {
    if (this.nativeDataChannel == 0) {
        throw new IllegalStateException("DataChannel has been disposed.");
    }
}
```

Same pattern in: `RtpReceiver`, `RtpSender`, `RtpTransceiver`, `PeerConnectionFactory`, `MediaStreamTrack`.

**PeerConnection has NO validation at all** — `nativePeerConnection` is `final` and native methods are called directly without any check.

### 6. Double-Free Patterns

#### DataChannel — No double-free protection:
```java
public void dispose() {
    checkDataChannelExists();  // non-atomic check
    JniCommon.nativeReleaseRef(this.nativeDataChannel);  // free
    this.nativeDataChannel = 0L;  // set to null AFTER free
}
```
Two threads calling `dispose()` simultaneously: both pass the check, both call `nativeReleaseRef` on the same pointer.

#### RLottieDrawable — Partial protection:
```java
public final void m16800l(boolean z) {
    long j = this.f50018j1;    // read volatile
    this.f50018j1 = 0L;        // clear
    if (j == 0) return;        // check
    // ... destroy(j) via async task
}
```
The read-then-clear is NOT atomic. Two threads can both read a non-zero value before either clears it.

#### AnimatedFileDrawable — Same pattern:
```java
// In recycle():
if (this.nativePtr != 0) {
    destroyDecoder(this.nativePtr);
    this.nativePtr = 0L;
}
```
`volatile` does not make the check-and-destroy atomic.

#### VpxEncoderWrapper — No protection:
```java
public void release() {
    nativeRelease(this.encoder);  // free unconditionally
    this.encoder = 0L;
}
```
No check, no synchronization. Calling `release()` twice = double-free.

### 7. TOCTOU Race Conditions on Native Pointers

#### VpxEncoderWrapper (most exploitable):
```java
public void encode(VideoFrame videoFrame, boolean z) {
    long j = this.encoder;       // T1: read non-zero value
    if (j == 0) {                // T1: check passes
        throw new IllegalStateException("encoder already released");
    }
    // T2: release() sets encoder = 0 and frees native memory
    nativeEncode(j, videoFrame, z);  // T1: USE-AFTER-FREE
}
```
No `volatile`, no `synchronized` — classic TOCTOU. The encoder is used from the encoding thread while `release()` can be called from any thread.

#### AnimatedFileDrawable.getNextFrame():
```java
public Bitmap getNextFrame(boolean z) {
    if (this.nativePtr == 0) {       // check (volatile read)
        return this.f50188A0;
    }
    // ... 
    long j = this.nativePtr;         // second read — may now be 0!
    getVideoFrame(j, bitmap, ...);   // USE-AFTER-FREE if recycle() ran between reads
}
```
Even though `nativePtr` is `volatile`, the two reads are not atomic. `recycle()` can run between the check and the use.

#### RLottieDrawable.cacheFrame():
```java
public void cacheFrame(int i) {
    // ...
    long j = this.f50018j1;          // volatile read
    // No null check at all!
    getFrame(j, i, bitmap, ...);     // If j == 0, passes NULL to native
}
```
This is called from background thread pool (`ScheduledThreadPoolExecutor`). If `recycle()` zeroes the pointer concurrently, this passes 0 to native `getFrame()` — likely a null pointer dereference in native code, or worse if the memory has been reallocated.

#### DataChannel — all operations:
```java
public boolean send(Buffer buffer) {
    checkDataChannelExists();        // T1: nativeDataChannel != 0
    // T2: dispose() → nativeReleaseRef → nativeDataChannel = 0
    byte[] bArr = ...;
    return nativeSend(bArr, buffer.binary);  // T1: uses freed pointer via `this`
}
```
Instance native methods use `this.nativeDataChannel` implicitly — the JNI layer reads the field. If `dispose()` runs between check and native call, the native side accesses freed memory.

## Exploitation Scenarios

### Scenario 1: WebRTC Call Teardown Race
1. User is on a call, receiving ICE candidates (native thread fires `onIceCandidate`)
2. User hangs up → `PeerConnection.dispose()` called on main thread
3. Race: native callback fires, accesses disposed RtpReceiver/RtpSender objects
4. Result: Use-after-free in native WebRTC code → potential RCE

### Scenario 2: Animated Sticker Race
1. Animated sticker is rendering frames on background thread via `getNextFrame()`
2. User scrolls away → `recycle()` called, `destroyDecoder(nativePtr)` frees decoder
3. Background thread already passed the `nativePtr == 0` check, calls `getVideoFrame()` with freed pointer
4. Result: Use-after-free in FFmpeg decoder → potential RCE via crafted sticker

### Scenario 3: VpxEncoder Double-Free
1. Video call encoding thread calls `encode()` — reads `this.encoder`
2. Call teardown calls `release()` — frees native encoder, sets field to 0
3. Another `release()` call (e.g., from error handler) — no protection against double-free
4. Result: Double-free in libvpx → heap corruption → potential RCE

### Scenario 4: DataChannel Message During Dispose
1. Attacker sends rapid data channel messages
2. App decides to close the data channel
3. `send()` or `onMessage` callback races with `dispose()`
4. Result: Use-after-free on native DataChannel object

## Root Cause

The Java WebRTC wrapper and custom media classes use a fundamentally unsafe pattern:
1. Store native pointer as a plain `long` field
2. Check if pointer is non-zero (sometimes not even that)
3. Use the pointer in a native call
4. Steps 2 and 3 are NOT atomic and NOT synchronized

The `volatile` keyword (used in AnimatedFileDrawable, RLottieDrawable) only provides visibility guarantees, NOT atomicity of the check-then-use sequence.

## Recommendations

1. All native pointer access must be protected by `synchronized` blocks or use `AtomicLong` with `compareAndSet` for the dispose pattern
2. PeerConnection must unregister its observer BEFORE disposing child objects
3. VpxEncoderWrapper needs synchronization between `encode()` and `release()`
4. AnimatedFileDrawable/RLottieDrawable need a lock protecting the check-and-use of nativePtr
5. DataChannel/RtpSender/RtpReceiver dispose should use CAS to prevent double-free

## Files Analyzed

- `org/webrtc/PeerConnection.java`
- `org/webrtc/DataChannel.java`
- `org/webrtc/RtpReceiver.java`
- `org/webrtc/RtpSender.java`
- `org/webrtc/RtpTransceiver.java`
- `org/webrtc/VpxEncoderWrapper.java`
- `org/webrtc/PeerConnectionFactory.java`
- `org/webrtc/JniCommon.java`
- `org/webrtc/MediaStreamTrack.java`
- `org/webrtc/audio/WebRtcAudioRecord.java`
- `one/p011me/rlottie/RLottieDrawable.java`
- `one/p011me/sdk/media/ffmpeg/AnimatedFileDrawable.java`
- `one/p011me/sdk/p018gl/effects/VideoMessageStencilHolder.java`
- `one/video/calls/audio/opus/FileWriter.java`
