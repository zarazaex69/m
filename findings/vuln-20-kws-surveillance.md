# VULN-20: Keyword Spotting (KWS) System — In-Call Audio Surveillance

## Summary

Max messenger contains a Keyword Spotting (KWS) system that **actively listens to call audio for specific keywords during VoIP calls**. The system uses a TFLite ML model downloaded from a remote server, processes audio locally via native code, and reports detection events (with confidence scores) to analytics servers.

## Architecture

```
[Call Audio Stream] → [libjingle_peerconnection_so.so (native KWS)] → [NativeDoubleArrayConsumer callback]
                                                                              ↓
                                                                    [KeywordSpotterManagerImpl]
                                                                              ↓
                                                                    [ConversationKwsStat.onKeyword()]
                                                                              ↓
                                                                    [Server-side analytics event]
```

## Key Findings

### 1. KWS-Related Java Code

Primary classes in `ru.ok.android.externcalls.sdk`:
- `audio/KeywordSpotterManager` — Interface defining `setKeywordSpotterParams(isEnabled, filePath)`
- `audio/KeywordSpotterManagerImpl` — Implementation managing KWS lifecycle
- `audio/internal/KeywordSpotterConfigProvider` — Remote config provider
- `audio/internal/KeywordSpotterConfigProviderImpl` — Parses `android.wordspotter.config`
- `ml/delegate/KwsFeatureDelegate` — Downloads TFLite model
- `ml/config/kws/KwsFeatureConfigProvider` — Remote ML feature config
- `ml/MLFeaturesManagerImpl` — Orchestrates KWS during calls
- `stat/kws/ConversationKwsStat` — Reports keyword detections to server

### 2. JNI Bridge

```java
// org/webrtc/PeerConnectionFactory.java:412
private static native void nativeSetKeywordSpotterParams(long j, boolean z, String str, NativeDoubleArrayConsumer nativeDoubleArrayConsumer);

// org/webrtc/PeerConnectionFactory.java:579
public void setKeywordSpotterParams(boolean z, String str, NativeDoubleArrayConsumer.Consumer consumer) {
    nativeSetKeywordSpotterParams(this.nativeFactory, z, str, new NativeDoubleArrayConsumer(consumer));
}
```

Native callback interface (`@CalledByNative`):
```java
// org/webrtc/NativeDoubleArrayConsumer.java
public class NativeDoubleArrayConsumer {
    public interface Consumer { void consume(Double[] dArr); }
    @CalledByNative
    public void consume(Double[] dArr) { this.consumer.consume(dArr); }
}
```

### 3. WHEN KWS is Active

**During VoIP calls ONLY**, not in background:
- `MLFeaturesManagerImpl` is created inside `ConversationImpl` (line 1490) — the call session object
- `mlFeaturesManager.start()` called at line 2041 (call connected)
- `mlFeaturesManager.dispose()` called at line 3368 (call ended)
- KWS has a configurable auto-shutoff timer (`turnOffInMs`) from remote config
- Waits until user leaves "waiting hall" before activating

### 4. Keywords Spotted

The system detects the Russian phrase **"не слышу"** (meaning "I can't hear you"):

```java
// ConversationKwsStat.java:24
((uj1) tj1Var).m22973c("bad_call_detected_by_audio_spotter",
    EventItemValueKt.toEventItemValue(z5l.m26557g0(confidence * 100)),
    new EventItemsMap().set(SdkMetricStatEvent.STRING_VALUE_KEY, "не слышу"));
```

This is a **call quality detection** keyword — spotting when users say they can't hear the other party.

### 5. What Happens on Keyword Detection

1. Native code calls `NativeDoubleArrayConsumer.consume(Double[])` with confidence scores
2. `consumer$lambda$3` logs "Keyword detected: {confidence}" and posts to main thread
3. `consumer$lambda$3$lambda$2` finds max confidence from array
4. `ConversationKwsStat.onKeyword(confidence)` fires analytics event `"bad_call_detected_by_audio_spotter"` with confidence percentage and keyword label

**No audio recording or audio upload occurs on detection** — only a statistical event with confidence score is sent.

### 6. Audio Processing: Local vs Server

**Audio is processed LOCALLY only:**
- TFLite model runs inside `libjingle_peerconnection_so.so` (native C++ code)
- Native implementation at: `../../modules/audio_processing/enhancer_ns/kws_impl.cc`
- C++ classes: `vk::enh::KWSFactory`, `vk::enh::KwsBufferizator<float>`, `KeywordSpotterImpl`
- Only the detection **event** (confidence float) is sent to server, not audio data

### 7. Remote Enablement via Server Config

**YES — KWS can be enabled/configured remotely** via two remote keys:

| Remote Key | Purpose |
|---|---|
| `android.wordspotter.config` | Controls KWS behavior (turnOffInMs timer) |
| `android.mlfeatures.ws_0` | Controls model download (URL, checksum, enabled flag) |

The ML config (`MLFeatureConfigProviderBase`) parses:
```json
{"url": "<model_download_url>", "cs": "<md5_checksum>", "use": true/false}
```

The `"use"` field enables/disables KWS remotely. The server can push a new model URL at any time.

### 8. TFLite Model Path

- **Model version**: `ws_0` (constant `CURRENT_WS_MODEL_VERSION`)
- **File extension**: `.tflite` (model) + `.cfg` (config)
- **Download directory**: `{context.filesDir}/ml_features/ws/`
- **Full model path**: `/data/data/ru.ok.android/files/ml_features/ws/<downloaded_model>.tflite`
- **Config file**: `/data/data/ru.ok.android/files/ml_features/ws/<downloaded_model>.cfg`
- **TFLite runtime**: loaded via `PeerConnectionFactory.setTFLiteLibraryPath()` pointing to `libtensorflowlite.so`

Native strings confirm: `"SetTFLiteLibraryPath: tensorflowlite (KWS) done"`

## Risk Assessment

| Factor | Assessment |
|---|---|
| Severity | **Medium** — listens during calls only, not background surveillance |
| Privacy Impact | Moderate — processes speech locally, sends only detection events |
| Remote Control | **High concern** — server can enable/disable and push new models |
| Transparency | Low — no user consent UI found for keyword spotting |
| Scope | Limited to detecting "не слышу" for call quality metrics |
| Data Exfil | Confidence score only, no audio sent to server |

## Potential Abuse Vectors

1. **Model swap attack**: Server pushes new `.tflite` model trained on different keywords (political speech, names, etc.) — detection events would reveal conversation content
2. **Remote activation**: `"use": true` in remote config enables KWS without user knowledge
3. **Confidence as side-channel**: High-frequency confidence reporting could leak speech patterns
4. **No integrity verification beyond MD5**: Model download uses MD5 checksum only — vulnerable to collision attacks

## Conclusion

The KWS system is **primarily a call quality feature** (detecting "I can't hear you"), but its architecture enables **silent remote reconfiguration** to spot arbitrary keywords during calls. The server controls both the model and the enable flag, meaning the keyword vocabulary can be changed without app updates or user consent.
