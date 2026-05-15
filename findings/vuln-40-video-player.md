# VULN-40: Video Player Security Analysis

## Component Overview

Max messenger uses a custom video player stack called **OneVideoPlayer** built on top of ExoPlayer (androidx.media3). The implementation spans:
- `one.video.player.BaseVideoPlayer` — abstract base player (module: `one-video-player_release`)
- `one.video.exo.*` — ExoPlayer integration (module: `one-video-player-exo_release`)
- `one.p011me.chatmedia.viewer.video.*` — UI viewer widgets
- `one.p011me.sdk.media.ffmpeg.AnimatedFileDrawable` — native FFmpeg decoder for animated content
- Native library: `libffmpg.so` (arm64-v8a, 1.9MB)

## 1. Video Player Implementation

The player architecture:
- `BaseVideoPlayer` (abstract) implements `b0d` interface with thread verification
- `OneVideoExoPlayer` extends BaseVideoPlayer using ExoPlayer/Media3 as backend
- `VideoViewerWidget` / `BaseVideoViewerWidget` — UI layer with `VideoView`, `VideoPreviewView`, `PinchToZoomVideoWrapper`
- `GifViewerWidget` — GIF/short video playback via same VideoView
- `MediaPlayerController` — controls volume, looping, playWhenReady state
- Video data uses `HttpDataSource` for network streaming

## 2. Codec Vulnerability Surface (Crafted Video Files)

### Native FFmpeg Library (`libffmpg.so`)

**Supported codecs identified via strings:**
- Video: AVC/H.264 (avc1/avc2/avc3/avc4), VP8/VP9 (vpx_codec)
- Audio: AAC, AAC-LATM, Opus, MP3 (MPEG L1/L2/L3)
- Containers: WebM, Matroska, MP4

**Version functions present but no version strings embedded** — version is determined at runtime via:
- `avformat_version`, `avcodec_version`, `avutil_version`
- `vpx_codec_version`, `vpx_codec_version_str`

**Risk: HIGH** — The custom `libffmpg.so` is a stripped FFmpeg build. Without version identification, it's impossible to confirm patches for known CVEs. The library handles:
- `createDecoder(String path, int[] params)` — native file path passed directly
- `getFrameAtTime(long ptr, long time, Bitmap, int[], int)` — frame extraction
- `getVideoFrame(long ptr, Bitmap, int[], int, boolean, float, float, boolean)` — video decoding

The `AnimatedFileDrawable` class passes file paths directly to native `createDecoder()` without content-type validation, making it vulnerable to crafted files that exploit codec parsing bugs.

### ExoPlayer Layer
- Uses `HttpDataSource` for streaming — handles `InvalidResponseCodeException`
- Error types: `OneVideoSourceException`, `OneVideoRendererException`, `OneVideoPlaybackException`
- ExoPlayer itself has its own codec handling which adds another attack surface

## 3. PmsKey Configuration Analysis

### `one-video-player` (PmsKey ordinal 166)
- Controls the OneVideoPlayer feature flag/configuration
- Referenced in `qp6` settings class via `m19041E()` method
- Logged as diagnostic: `u6d("one-video-player", String.valueOf(...))`
- Likely controls: player engine selection, codec preferences, buffering params

### `one-video-failover` (PmsKey ordinal 167)
- Controls: `isOneVideoPlayerFailoverResolverEnabled()` — boolean flag
- Purpose: Enables failover host resolution for video streaming
- When enabled, video playback can switch to a `failoverHost` (field `f428k` in video model `a48`)
- Referenced in `ydc.java`: "OneVideo: обработка failover хоста" (OneVideo: processing failover host)
- **Security concern**: The failover host is a server-controlled string. If PMS (server settings) are compromised, an attacker could redirect video traffic to arbitrary hosts.

### `player-load-control` (PmsKey ordinal 161)
- Maps to: `one.me.sdk.media.player.PlayerLoadControl` (DI registration ID 168)
- Controls buffering parameters (min/max buffer sizes, playback start thresholds)
- **DoS risk**: Malicious server config could set extreme buffer sizes causing OOM

## 4. Video URL Validation (SSRF Analysis)

### Findings — NO URL VALIDATION DETECTED

**Video URL flow:**
1. Server sends `video_url` in message attachments (protobuf field in `Protos.java`)
2. `streamurl` parsed from regex-matched key-value pairs in `ba8.java` / `aa8.java` — **no scheme/host validation**
3. Video model `a48` contains: URL string, `failoverHost`, dimensions, type (`video/hls`)
4. URL passed to ExoPlayer's `HttpDataSource` for streaming
5. `videoweb/full` endpoint fetches video URL with `chat_id` and `msg_id` params

**SSRF vectors identified:**
- `video_url` from server response passed directly to player without validation
- `streamurl` extracted from content without scheme checking
- `failoverHost` — server-controlled redirect target
- `liveStreamsUrlPrefix` — configurable URL prefix for live streams
- `video-msg-download-urls-hack-enabled` — suggests URL manipulation exists

**Risk: MEDIUM-HIGH** — While URLs come from the server (not directly user-controlled), a compromised server or MITM could inject arbitrary URLs. The client makes HTTP requests to whatever URL the server provides, potentially reaching internal network resources.

## 5. Subtitle/Caption Support

**No subtitle/caption rendering found.** Search for subtitle-related classes (SubtitleView, TextTrack, .srt, .vtt, WebVTT) returned no results in the video player code.

The "subtitle" references found are all UI text labels (e.g., `subtitleView` in bottom sheets, settings items) — not video subtitle tracks.

**Risk: NONE** — Subtitle injection attacks are not applicable as the player does not support text track rendering.

## 6. Native FFmpeg Library Analysis

**Library:** `/lib/arm64-v8a/libffmpg.so` (1,954,240 bytes)

**Version identification:**
```
avformat_version
av_format_ffversion
avcodec_version
av_codec_ffversion
av_version_info
avutil_version
av_util_ffversion
vpx_codec_version
vpx_codec_version_extra_str
vpx_codec_version_str
```

Version functions are present but actual version strings are not embedded as static data — they're computed at runtime. This makes it impossible to determine the exact FFmpeg version from static analysis alone.

**Codec support confirmed:** AVC/H.264, VP8/VP9 (libvpx), AAC, Opus, MP3, WebM/Matroska/MP4 containers.

**Risk: HIGH** — Custom FFmpeg builds are notorious for lagging behind security patches. The library handles untrusted media content (received files, stickers, GIFs) through native code with potential for memory corruption vulnerabilities.

## 7. Video Thumbnail Generation from Untrusted Content

### Findings — THUMBNAILS GENERATED FROM UNTRUSTED CONTENT

**Server-side thumbnails:**
- `video.thumbnail` — URL string from server protobuf (`AbstractC1090a.java:934`)
- `video.thumbhashData` — binary thumbhash data from server (`AbstractC1090a.java:959`)
- `media-thumbhash` PmsKey controls thumbhash feature

**Client-side frame extraction:**
- `AnimatedFileDrawable.createDecoder(file.getAbsolutePath(), params)` — decodes downloaded files
- `getFrameAtTime()` / `getVideoFrame()` — extracts frames from video files for preview
- `MediaMetadataRetriever` used in `FrgTrimVideo` for video trimming thumbnails
- `VideoThumbnailView` renders thumbnails from `SimpleTransitionDraweeView` (Fresco)

**UI resource IDs confirm thumbnail views in multiple player layouts:**
- `view_auto_play_video_player__iv_thumbnail`
- `view_constructor_video_player__iv_thumbnail`
- `view_full_screen_video_player__iv_thumbnail`
- `view_small_video_player__iv_thumbnail`
- `view_video_player__iv_thumbnail`

**Risk: HIGH** — Video thumbnails are generated client-side by passing downloaded file paths directly to native FFmpeg `createDecoder()`. A crafted video file could exploit codec vulnerabilities during thumbnail generation, which happens automatically when viewing messages (before user explicitly plays the video).

## Summary of Vulnerabilities

| # | Issue | Severity | Type |
|---|-------|----------|------|
| 1 | Custom FFmpeg library with unknown version processing untrusted media | HIGH | Memory Corruption / RCE |
| 2 | No URL validation on video playback URLs (SSRF potential) | MEDIUM-HIGH | SSRF |
| 3 | Server-controlled failover host redirect | MEDIUM | Open Redirect / SSRF |
| 4 | Automatic thumbnail generation from untrusted video files via native code | HIGH | Memory Corruption / RCE |
| 5 | Server-controlled player-load-control can cause resource exhaustion | LOW | DoS |
| 6 | video-msg-download-urls-hack suggests URL manipulation workarounds | MEDIUM | Logic Bug |

## Recommendations

1. **Identify and update FFmpeg version** — determine exact build, patch against known CVEs
2. **Add URL scheme/host validation** — whitelist allowed video hosting domains before passing to player
3. **Sandbox native decoding** — isolate FFmpeg operations in separate process with restricted permissions
4. **Validate failover hosts** — ensure failover URLs match expected CDN domains
5. **Add content-type verification** — validate file magic bytes before passing to native decoder
6. **Limit player-load-control ranges** — cap buffer sizes client-side regardless of server config
