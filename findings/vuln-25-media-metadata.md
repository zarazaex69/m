# VULN-25: Media Metadata Leakage in Upload Pipeline

## Summary

Max messenger (TamTam/OneMe) does **NOT** strip EXIF metadata (including GPS coordinates, device model, and creation timestamps) from images before upload. The image transcoding pipeline re-encodes pixel data but actively **preserves and re-writes** sensitive EXIF fields. Video transcoding focuses on codec transformation without metadata sanitization.

**Severity: HIGH** — User geolocation, device fingerprint, and temporal data leak to recipients.

---

## 1. Image Upload Pipeline

### Key Files
- `p000/m94.java` — `SimpleImageTranscoder`: Decodes bitmap, applies rotation matrix, re-compresses as JPEG at quality 85. **No EXIF stripping.**
- `com/facebook/imagepipeline/nativecode/NativeJpegTranscoder.java` — Native JPEG transcoder via Facebook's Fresco library. Calls `nativeTranscodeJpeg()` / `nativeTranscodeJpegWithExifOrientation()`. These native methods handle rotation but **preserve EXIF data in the output stream**.
- `p000/klb.java` — Factory that selects NativeJpegTranscoder (preferred) or falls back to SimpleImageTranscoder.

### EXIF Handling Analysis
- `p000/wd6.java` — Full ExifInterface implementation (2000+ lines). Reads/writes all EXIF IFDs including GPS.
- `p000/od6.java` — Defines EXIF tag structures including:
  - IFD0: `Make`, `Model`, `DateTime`, `Orientation`
  - GPS IFD: `GPSLatitudeRef`, `GPSLatitude`, `GPSLongitudeRef`, `GPSLongitude`, `GPSAltitudeRef`, `GPSAltitude`, `GPSTimeStamp`, `GPSSpeedRef`, `GPSTrackRef`, `GPSImgDirectionRef`
- `p000/o2m.java` — `convertToJpeg()`: On API < 28, explicitly **copies Orientation from source EXIF to output**. Only copies Orientation — other tags survive via the native transcoder.

### Critical Finding: Camera Capture Writes Full EXIF
- `p000/k2m.java` — Camera image processing explicitly writes:
  ```java
  nd6Var.m14946c("Make", Build.MANUFACTURER, arrayList);
  nd6Var.m14946c("Model", Build.MODEL, arrayList);
  // GPS data written if available:
  nd6Var.m14945b("GPSVersionID", "2300", list);
  nd6Var.m14945b("GPSLatitudeRef", ...);
  nd6Var.m14945b("GPSLatitude", ...);
  nd6Var.m14945b("GPSLongitudeRef", ...);
  nd6Var.m14945b("GPSLongitude", ...);
  ```
  This writes device manufacturer, model, and GPS coordinates directly into the JPEG EXIF before upload.

---

## 2. Video Upload Pipeline

### Key Files
- `p000/rej.java` — `UploadVideoConfig`: Controls upload parallelism per connection type (wifi/4g/3g). Fields: `isOneMeUploaderEnabled`, `ConnectionBasedValues` (parallelism, chunkSize).
- `p000/qej.java` — `ConnectionBasedValues`: `isEnabled`, `parallelism`, `parallelHeaderDisabled`, `chunkSize`.
- `p000/hi7.java` — MediaMuxer wrapper for video muxing.
- `ru/p027ok/messages/media/trim/FrgTrimVideo.java` — Video trimming uses `MediaMetadataRetriever`.

### Video Metadata Handling
- No evidence of metadata stripping in the video pipeline.
- `MediaMuxer` preserves container-level metadata (creation time, location atoms in MP4).
- The `TRANSCODE` event type (`p000/jm5.java`) is for analytics/perf tracking, not metadata removal.

---

## 3. EXIF GPS Data — NOT Stripped Before Upload

**CONFIRMED: GPS data is NOT stripped.**

Evidence:
1. No calls to `removeAttribute`, `clearExif`, `stripMetadata`, or any GPS-removal logic found in the entire codebase.
2. `k2m.java` actively **writes** GPS data (GPSLatitude, GPSLongitude, GPSAltitude, GPSTimeStamp, GPSSpeedRef, GPSTrackRef) into camera-captured images.
3. The NativeJpegTranscoder performs rotation/downscaling but passes EXIF through.
4. The SimpleImageTranscoder (m94.java) re-encodes via `Bitmap.compress()` which strips EXIF, but this is only the **fallback** path when native transcoder is unavailable.

**Attack scenario**: Any image sent in Max messenger may contain the sender's precise GPS coordinates, accessible to any recipient who downloads and inspects the file.

---

## 4. Image Creation Date / Device Model — NOT Stripped

**CONFIRMED: Device model and timestamps are preserved.**

- `k2m.java` writes `Build.MANUFACTURER` and `Build.MODEL` into EXIF `Make` and `Model` tags.
- `DateTime` tag is written by the camera subsystem and not removed.
- The native transcoder preserves all EXIF IFDs during re-encoding.

**Impact**: Recipients can determine the exact device model (e.g., "Samsung SM-S918B") and capture timestamp of every photo sent.

---

## 5. Media-Transform PmsKey Configuration

**PmsKey**: `media-transform` (index 141)
**Config class**: `p000/ija.java` — `MediaTransformModel`

Fields:
| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `isTransformAllowed` | boolean | false | Master switch for video transcoding |
| `isTransformWithHevcAllowed` | boolean | false | Allow H.265/HEVC encoding |
| `isTransformWithKeepingHdrAllowed` | boolean | false | Preserve HDR during transcode |
| `isStreamableMp4Enabled` | boolean | false | Generate streamable (moov-first) MP4 |
| `encoderConfig` | hja | default | Encoder-specific settings |
| `isSizeFixEnabled` | boolean | false | Fix output size issues |
| `bppf` | double | 0.01 | Bits-per-pixel factor for quality |

**Note**: This config controls **video codec transformation only** — no metadata stripping options exist.

---

## 6. Voice Messages — No Location Metadata

Voice messages use:
- `p000/s2d.java` — `AudioRecord` for raw PCM capture
- Opus encoder (PmsKey: `opus-recorder`, `opus-recorder-bitrate`, `opus-recorder-sample-rate`)

Opus/OGG format does not natively support GPS metadata. However, the protobuf message structure (`ru/p027ok/tamtam/nano/Protos.java`) includes `LocationInfo` with `latitude`/`longitude` fields that can be attached to any message type. No evidence that voice messages automatically attach location, but the protocol supports it if the app sends it.

---

## 7. Upload-Video-Config

**PmsKey**: `upload-video-config` (index 173)
**Config class**: `p000/rej.java` — `UploadVideoConfig`

```
UploadVideoConfig(
  isOneMeUploaderEnabled: boolean,  // Use new uploader
  wifi: ConnectionBasedValues,       // WiFi upload params
  mobile4g: ConnectionBasedValues,   // 4G upload params  
  mobile3g: ConnectionBasedValues    // 3G upload params
)

ConnectionBasedValues(
  isEnabled: boolean,
  parallelism: int (default 1),
  parallelHeaderDisabled: boolean,
  chunkSize: long (default MAX_TIME_TO_UPLOAD)
)
```

**This config controls upload transport parameters only — no metadata stripping or preservation options.**

---

## 8. Forwarded Media — Metadata Preserved

**Forward mechanism** (`p000/od7.java` — `ForwardMessagesSendData`):
- Forwards reference messages by ID (`messageIds`) or attachment ID (`attachId`)
- `shouldHideAuthor` flag hides the display name but does NOT re-process media
- No re-upload or re-encoding occurs — the same server-side media blob is referenced

**Impact**: Forwarded images retain ALL original EXIF metadata (GPS, device model, timestamps) from the original sender. The `shouldHideAuthor` flag only affects the UI attribution, not the binary media content.

The `EmptyVideoTokenException` message confirms: `"Video token is empty on forward, message=..., video=null, from chat=null"` — videos are forwarded by token reference, not re-uploaded.

---

## Vulnerability Summary

| Check | Status | Risk |
|-------|--------|------|
| GPS coordinates stripped from images | ❌ NOT STRIPPED | **HIGH** — Location tracking |
| Device model/make stripped | ❌ NOT STRIPPED | MEDIUM — Device fingerprinting |
| Creation timestamp stripped | ❌ NOT STRIPPED | MEDIUM — Temporal correlation |
| Video metadata stripped | ❌ NOT STRIPPED | **HIGH** — MP4 location atoms preserved |
| Voice message location | ⚠️ Protocol supports it | LOW — Opus format doesn't embed GPS |
| Forwarded media re-processed | ❌ NO — Original metadata preserved | **HIGH** — Metadata leaks across forward chains |

## Recommendations

1. **Critical**: Implement EXIF stripping before upload — remove GPS IFD, Make, Model, DateTime, and all identifying tags.
2. Strip MP4 location atoms (`©xyz`, `location` moov atom) from videos before upload.
3. Re-process forwarded media or strip metadata server-side before serving to recipients.
4. Add user-visible privacy setting: "Strip location from media" (default: ON).
5. The `SimpleImageTranscoder` path (Bitmap.compress) accidentally strips EXIF but is only the fallback — the primary `NativeJpegTranscoder` preserves it.
