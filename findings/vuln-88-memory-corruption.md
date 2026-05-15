# Memory Corruption at JNI Boundary — Max Messenger v26.15.3

## 1. RLottieDrawable Use-After-Free (CRITICAL)

**File**: `one/p011me/rlottie/RLottieDrawable.java`

### The Bug

Two native pointer fields exist:
- `f50018j1` (volatile long) — main nativePtr, properly zeroed on dispose
- `f49970D1` (long, **NOT volatile**) — cached ptr for frame generation, **NEVER zeroed**

### Dispose Path (m16800l, line 896)
```java
public final void m16800l(boolean z) {
    long j = this.f50018j1;
    this.f50018j1 = 0L;  // ← zeroes f50018j1
    // f49970D1 is NEVER touched!
    if (j == 0) return;
    // destroy scheduled asynchronously:
    ((ScheduledExecutorService)...).execute(new tha(j, 2));
}
```

### Use Path (getNextFrame, line 684)
```java
public int getNextFrame(Bitmap bitmap) {
    long j = this.f49970D1;  // ← reads stale pointer (NOT zeroed by dispose!)
    if (j == 0) return -1;
    // USES FREED POINTER:
    getFrame(j, this.f50036z1, bitmap, ...);  // ← UAF!
}
```

### Race Condition
```
Thread A (render):     Thread B (GC/recycle):
─────────────────     ──────────────────────
read f49970D1 → j     
                      m16800l() called
                      f50018j1 = 0
                      schedule destroy(ptr)
                      [destroy executes - frees native memory]
getFrame(j, ...)      
  ↑ USES FREED PTR!
```

### Exploitation
- **Trigger**: View animated sticker, then quickly scroll away (triggers recycle)
- **Impact**: Native use-after-free in libjlottie.so
- **Heap spray**: Send multiple stickers to control freed memory layout
- **Result**: Arbitrary code execution in app process

### Why It Works
1. `f49970D1` is NOT volatile — compiler may cache the read
2. `f49970D1` is NEVER set to 0 during dispose
3. `destroy()` is async — executes on ScheduledExecutorService
4. `getNextFrame()` is called from render thread (different from dispose thread)
5. No synchronization between render and dispose

---

## 2. AnimatedFileDrawable — Same Pattern

**File**: `p000/RunnableC1227vh.java:155`

```java
animatedFileDrawable.nativePtr = AnimatedFileDrawable.createDecoder(file.getAbsolutePath(), ...);
```

`AnimatedFileDrawable.nativePtr` is volatile but the same TOCTOU exists:
- Read nativePtr → check != 0 → use it
- Between check and use, another thread can destroy

---

## 3. VpxEncoderWrapper — Non-Volatile Encoder Field

**File**: `ru/p027ok/android/webrtc/` (VpxEncoderWrapper)

The `encoder` field is not volatile. `encode()` reads it without synchronization while `release()` nulls it from another thread.

---

## 4. DataChannel — checkExists Then Use

```java
// Pattern in DataChannel:
checkDataChannelExists();  // checks nativeDataChannel != 0
// ← RACE WINDOW: dispose() can null it here
nativeSend(...);           // uses potentially-freed pointer
```

---

## Impact Assessment

| Bug | Trigger | Exploitability | Impact |
|-----|---------|---------------|--------|
| RLottieDrawable UAF | View+scroll sticker | HIGH | RCE via heap spray |
| AnimatedFileDrawable TOCTOU | View+scroll WebM | MEDIUM | RCE |
| VpxEncoderWrapper race | End video call | LOW | Crash/RCE |
| DataChannel race | Close call during data | LOW | Crash |

The RLottieDrawable UAF is the most exploitable because:
1. Stickers are everywhere (0-click viewing)
2. Attacker controls the Lottie JSON content (heap layout)
3. The race window is wide (async destroy via executor)
4. No ASLR bypass needed (heap spray with multiple stickers)
