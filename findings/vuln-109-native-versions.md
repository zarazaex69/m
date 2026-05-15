# VULN-109: Native Library Version Analysis — Max Messenger

**Date:** 2026-05-15  
**Path:** `~/max/decompiled/native/lib/arm64-v8a/`  
**Package:** one.me (Max Messenger)

---

## Summary

Analysis of native shared libraries bundled with Max messenger reveals outdated third-party components with known CVEs, alongside generally adequate binary hardening. The most critical finding is an outdated FFmpeg (n4.4.3) with an embedded VPX 1.14.0 codec library.

---

## 1. Library Inventory

| Library | Size | Purpose |
|---------|------|---------|
| libjingle_peerconnection_so.so | 12.2 MB | WebRTC (VoIP/video calls) |
| libEnhancementLibShared.so | 5.5 MB | Image enhancement (TFLite-based) |
| libffmpg.so | 1.9 MB | FFmpeg media decoding |
| libc++_shared.so | 1.2 MB | C++ standard library |
| libjlottie.so | 978 KB | rlottie animation rendering |
| libnative-imagetranscoder.so | 571 KB | Image transcoding |
| libstatic-webp.so | 490 KB | WebP decoding |
| libgifimage.so | 311 KB | GIF decoding |
| libgleff.so | 296 KB | OpenGL effects |
| libqrcode.so | 135 KB | QR code generation |
| libzstd.so | 64 KB | Zstandard compression |
| libtracernative.so | 780 KB | Tracing/profiling |
| libnative-filters.so | 23 KB | Image filters |

---

## 2. Identified Versions

### 2.1 FFmpeg — n4.4.3 (CRITICAL)

**Confirmed version:** `FFmpeg version n4.4.3`  
**Embedded codecs:**
- VP9 decoder via libvpx v1.14.0 (`WebM Project VP9 Decoder v1.14.0`)
- H.264, HEVC/H.265, AV1, AAC, AAC-LATM, Opus
- Licensed under GPL v3

**Build toolchain:**
- Clang 9.0.9 (Android NDK r365631c3) — **very old compiler**
- Linker: LLD 11.0.5

**Known CVEs for FFmpeg 4.4.x (non-exhaustive):**
- CVE-2022-3109 — NULL pointer dereference in vp3_decode_frame
- CVE-2022-3341 — NULL pointer dereference in avformat
- CVE-2022-48434 — use-after-free in libavcodec/pthread_frame.c
- CVE-2023-47342 — heap buffer overflow in libavcodec
- CVE-2024-31578 — heap use-after-free in av_hwframe_ctx_init
- CVE-2024-31585 — floating point exception in libavcodec/vorbisdec.c
- Multiple additional heap overflows and OOB reads in various decoders

**Risk:** HIGH — FFmpeg 4.4.3 is EOL. Current stable is 7.x. Media parsing is a primary attack surface for messenger apps (receiving images/videos from untrusted contacts).

### 2.2 WebRTC — Recent Build (May 2025)

**Source stamp:** `WebRTC source stamp 2025-05-26T04:07:21`  
**Build toolchain:** Clang 18.0.2 (Android NDK r522817b)  
**Features detected:**
- X25519MLKEM768 (post-quantum key exchange — added in M131+)
- AV1 codec support
- corruption-detection RTP extension (added ~M125)
- BoringSSL integrated (paths: `third_party/boringssl/src/`)

**Assessment:** This is a very recent WebRTC build (May 2025), likely based on M131+ milestone. The presence of X25519MLKEM768 (ML-KEM post-quantum hybrid) confirms a modern build. **Low risk** from a version perspective.

### 2.3 GIFLIB — Version 5.1.x+

**Identified via exported symbols:**
- `DGifSavedExtensionToGCB` — added in GIFLIB 5.1
- `DGifSlurp`, `GifAddExtensionBlock` — standard 5.x API
- No `GifErrorString` export (added in 5.1.2+)

**Build toolchain:** Clang 18.0.2 (same as WebRTC — likely built together)

**Known issues:**
- CVE-2022-28506 — heap buffer overflow in DGifGetImageDesc (GIFLIB ≤ 5.2.1)
- CVE-2023-39742 — segfault in gif2rgb (GIFLIB 5.2.1)
- Older versions have multiple OOB read/write in DGifSlurp

**Risk:** MEDIUM — Exact sub-version unclear but API surface suggests 5.1.x. GIF parsing from untrusted sources (received media) is an attack vector.

### 2.4 libvpx — v1.14.0 (embedded in libffmpg.so)

**Confirmed:** `WebM Project VP9 Decoder v1.14.0`

**Known CVEs:**
- CVE-2024-5197 — integer overflow in vpx_img_alloc (libvpx < 1.14.1)
- CVE-2023-44488 — VP9 encoding crash (affects encoder, likely not applicable)
- CVE-2023-5217 — heap buffer overflow in VP8 encoding (actively exploited in wild)

**Risk:** MEDIUM-HIGH — v1.14.0 is missing the fix for CVE-2024-5197.

### 2.5 Zstandard (zstd) — Version Unknown

**No version string found in binary.**  
**Build toolchain:** Clang 11.0.5 (older NDK r399163b1)  
**No stack canary detected** (`__stack_chk_fail` absent from dynamic symbols).

**Risk:** LOW-MEDIUM — Compression library, less exposed to untrusted input directly, but missing stack protector is concerning.

### 2.6 libstatic-webp — Version Unknown

**Build toolchain:** Clang 18.0.2 (modern)  
**Exports:** Standard WebP decode API (WebPDecode, WebPDemux, etc.)

**Risk:** LOW-MEDIUM — Modern build toolchain suggests recent version. WebP has had critical CVEs (CVE-2023-4863 heap buffer overflow).

### 2.7 rlottie (libjlottie.so)

**Build toolchain:** Clang 11.0.5 (older)  
**Namespace:** `one.me.rlottie`

**Known issues with rlottie:**
- CVE-2021-36257 — buffer overflow in lottie_animation_from_data
- CVE-2021-36258 — heap buffer overflow
- Multiple OOB issues in JSON/Lottie parsing

**Risk:** MEDIUM — Lottie animations from untrusted sources (stickers, animated content) are an attack vector. Older build toolchain.

### 2.8 libEnhancementLibShared.so — TFLite-based

**Namespace:** `vk::enh` (VK Enhancement library)  
**Uses:** TensorFlow Lite for ML inference  
**Risk:** LOW — ML inference on local data, limited attack surface.

---

## 3. Binary Hardening Assessment

| Library | Type | RELRO | BIND_NOW | NX (Stack) | Stack Canary | FORTIFY_SOURCE |
|---------|------|-------|----------|------------|--------------|----------------|
| libffmpg.so | DYN | ✅ Full | ✅ | ✅ (RW) | ✅ | ✅ (partial) |
| libjingle_peerconnection_so.so | DYN | ✅ Full | ✅ | ✅ (RW) | ✅ | ✅ (partial) |
| libgifimage.so | DYN | ✅ Full | ✅ | ✅ (RW) | ✅ | — |
| libzstd.so | DYN | ✅ Full | ✅ | ✅ (RW) | ❌ | — |
| libstatic-webp.so | DYN | ✅ Full | ✅ | ✅ (RW) | ✅ | — |

**Notes:**
- All libraries are shared objects (DYN) — PIC is inherent
- GNU_STACK is RW (not RWE) on all checked libraries — NX bit is enforced
- Full RELRO + BIND_NOW on all libraries — GOT overwrite attacks mitigated
- `libzstd.so` is missing stack canary protection
- FORTIFY_SOURCE detected in libffmpg.so (`__memmove_chk`, `__strlen_chk`, `__vsnprintf_chk`) and libjingle (`__memcpy_chk`, `__FD_SET_chk`)

---

## 4. Toolchain Age Concerns

| Toolchain | Libraries Using It |
|-----------|-------------------|
| Clang 9.0.9 (NDK r365631c3) | libffmpg.so (partial — VPX component) |
| Clang 11.0.5 (NDK r399163b1) | libffmpg.so (linker), libzstd.so, libgleff.so, libjlottie.so |
| Clang 18.0.2 (NDK r522817b) | libjingle_peerconnection_so.so, libgifimage.so, libstatic-webp.so |

Older compilers lack modern exploit mitigations:
- Missing Control Flow Integrity (CFI)
- Missing Shadow Call Stack (SCS) — available since NDK r21+
- Missing Branch Target Identification (BTI) for ARM64

---

## 5. Critical Findings Summary

| # | Finding | Severity | CVE Examples |
|---|---------|----------|--------------|
| 1 | FFmpeg n4.4.3 — EOL, 30+ known CVEs | **CRITICAL** | CVE-2022-48434, CVE-2024-31578 |
| 2 | libvpx 1.14.0 — integer overflow | **HIGH** | CVE-2024-5197 |
| 3 | GIFLIB ~5.1.x — heap overflow | **MEDIUM** | CVE-2022-28506 |
| 4 | rlottie — old build, known buffer overflows | **MEDIUM** | CVE-2021-36257 |
| 5 | libzstd — missing stack canary | **LOW-MEDIUM** | — |
| 6 | Mixed old/new toolchains — inconsistent hardening | **LOW** | — |

---

## 6. Attack Scenarios

1. **Remote Code Execution via Media:** Attacker sends crafted video (VP9/H.264/HEVC) or GIF to victim. FFmpeg 4.4.3 heap overflow triggers during decode → code execution in app context.

2. **Sticker/Animation Exploit:** Malicious Lottie animation exploits rlottie buffer overflow → arbitrary code execution when user views sticker.

3. **GIF Bomb:** Crafted GIF exploits DGifGetImageDesc heap overflow → memory corruption during auto-preview in chat.

---

## 7. Recommendations

1. **Urgent:** Update FFmpeg to 6.1.x or 7.x series
2. **Urgent:** Update libvpx to ≥ 1.14.1
3. **High:** Verify GIFLIB version and update to 5.2.2+
4. **Medium:** Rebuild rlottie with modern toolchain, apply upstream patches
5. **Medium:** Enable stack canary for libzstd
6. **Low:** Standardize on modern NDK (r26+) for all native libraries to get CFI/SCS/BTI
