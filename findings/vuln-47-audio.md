# VULN-47: Audio Recording & Voice Message Handling Vulnerabilities

## Summary

Analysis of audio recording, voice messaging, and related features in Max messenger reveals multiple server-controlled parameters that can alter recording behavior without user awareness, a "Fake Boss" call feature abusable for social engineering, and server-side transcription that sends audio content to backend servers.

---

## 1. Audio Recording Infrastructure

### Key Files
- `one/video/calls/audio/opus/FileWriter.java` — Native Opus file writer with `startRecord(path, bitrate, sampleRate)`
- `p000/AbstractC0154co.java` — Android `AudioRecord` / `AudioRecordingConfiguration` wrapper
- `ru/ok/android/externcalls/sdk/asr/` — ASR (Automatic Speech Recognition) recording in calls
- `ru/ok/android/externcalls/sdk/record/internal/RecordManagerImpl.java` — Call recording manager

### Recording Mechanism
- Uses native JNI: `nativeAudioStartRecord(String path, int bitrate, int sampleRate)`
- Opus codec for voice messages
- `FileWriter.startRecord()` accepts bitrate and sample rate as parameters
- Call recording uses signaling-based `record-start` command via JSON

---

## 2. Remote Triggering of Audio Recording

### Finding: ASR Recording Can Be Triggered via Signaling

**Location:** `ru/ok/android/externcalls/sdk/asr/internal/commands/AsrCommandsExecutorImpl.java:76`

The `AsrCommandsExecutor.startRecord(fileName, sessionRoomId, onSuccess, onError)` method initiates audio recording during calls. This is triggered via the signaling layer (`SignalingProvider`), meaning the **server or call peer can initiate ASR recording** during an active call session.

**Location:** `ru/ok/android/externcalls/sdk/record/internal/RecordManagerImpl.java:292`

`RecordManagerImpl.startRecord()` sends a `record-start` signaling command with parameters including `movieId`, `name`, `description`, `privacy`, `groupId`, `albumId`, `streamMovie`. This is a server-mediated call recording feature.

### Risk: No evidence of push/deeplink/bot triggering voice message recording outside of calls. Recording in messaging context appears to require UI interaction (SEND_AUDIO_MESSAGE event at `cg8.java:46`). However, in-call recording is signaling-controlled.

---

## 3. Voice Message Encryption Before Upload

### Finding: NO ENCRYPTION DETECTED BEFORE UPLOAD

**Evidence:** 
- No grep matches for `encrypt|cipher|AES|Cipher` in combination with audio/voice/opus/upload paths
- Voice messages are recorded as Opus files and uploaded via the standard media upload pipeline
- The `speedy-voice-messages` PmsKey (`f88854`) suggests optimized upload without additional processing
- No client-side encryption layer found between recording and upload

### Risk: **HIGH** — Voice messages appear to be uploaded as plaintext Opus files. If TLS is stripped or compromised, audio content is exposed. No end-to-end encryption for voice messages was identified.

---

## 4. "Fake Boss" Call Feature

### Finding: Social Engineering Attack Surface

**PmsKey:** `calls-fakeboss-incoming-call-enabled` (index 320, field `f88626callsfakebossincomingcallenabled`)

**Accessor:** `qp6.callFakeBossesEnabled` → `getCallFakeBossesEnabled()Z`

**UI Components:**
- `gj6.java` — `FakeBossListItem` with fields: `contactServerId`, `phoneNumber`, `country`, `registrationDate`, `mutualChatsState`, `organizationInfoTextRes`
- `lj6.java:136` — Shows `fake_boss_registration` and `fake_boss_show_mutual_chats` UI
- `xnc.java:127` — `messages_list_fake_boss_view_type` for message list rendering
- `w15.java:126` / `vm1.java:115` — `isFakeBossEnabled` flag in call state

**Purpose:** This feature simulates a fake incoming call from a "boss" contact — likely intended as a social excuse feature ("I need to take this call"). However:

### Abuse Scenarios:
1. **Server can enable/disable remotely** via PmsKey — could be weaponized to create fake call notifications
2. **Social engineering** — if an attacker controls the server config, they can enable fake boss calls to manipulate user behavior
3. **The feature shows organization info and mutual chats** — leaks organizational context

---

## 5. Audio Transcription — Server-Side Processing

### Finding: Transcription is SERVER-SIDE (audio sent to server)

**Evidence:**
- `C1089c.java:136` — `audio.transcription.enabled` preference (default: `true`)
- `bd7.java:583` — API endpoint `"transcribes-dialog"` used for transcription requests
- `AbstractC1090a.java:372-382` — `audio.transcriptionStatus` and `audio.transcription` fields in nano protobuf messages
- `AbstractC1090a.java:318-348` — Same for video messages: `video.transcriptionStatus`, `video.transcription`

**PmsKeys controlling transcription:**
- `enable-audio-messages-transcription` (index 294)
- `enable-video-messages-transcription` (index 295)
- `audio-transcription-locales` (index 47)
- `retry-transcribe-attempt` (index 296)
- `retry-transcribe-timeout` (index 297)

**ASR in Calls:**
- `ru/ok/android/externcalls/sdk/asr_online/` — Real-time ASR during calls
- `AsrOnlineChunk` contains `participantId` and `text` — live transcription chunks received from server
- Audio is streamed to server during calls for real-time speech-to-text

### Risk: **HIGH** — Audio content from voice/video messages is sent to server for transcription. The `transcribes-dialog` API endpoint processes audio server-side. This means:
1. Server has access to raw audio content for transcription
2. Feature is enabled by default (`true`)
3. Server controls which locales are supported and retry behavior
4. In-call ASR streams audio in real-time to server

---

## 6. Opus Recorder Settings — Remotely Configurable

### Finding: BITRATE AND SAMPLE RATE CONTROLLED BY SERVER

**PmsKeys:**
- `opus-recorder` (index 308) — `isOpusRecorderEnabled()Z` — enables/disables Opus recorder
- `opus-recorder-bitrate` (index 309) — `getOpusRecorderBitrate()I` — integer bitrate value
- `opus-recorder-sample-rate` (index 310) — `getOpusRecorderSampleRate()I` — integer sample rate

**Configuration loading:** `qp6.java:808-810`
```java
this.f57525t2 = new ko6(this, PmsKey.f88796opusrecorder, i26);      // boolean
this.f57529u2 = new no6(this, PmsKey.f88797opusrecorderbitrate, i26); // int
this.f57533v2 = new oo6(this, PmsKey.f88798opusrecordersamplerate, i26); // int
```

These values are fetched from the PMS (Parameter Management System) — a server-side configuration system. The server can push new values at any time.

### Risk: **CRITICAL** — The server can silently increase recording quality by:
1. Raising `opus-recorder-bitrate` (e.g., from 16kbps to 128kbps)
2. Raising `opus-recorder-sample-rate` (e.g., from 16kHz to 48kHz)
3. This produces higher-fidelity recordings without any user notification
4. Combined with server-side transcription, this enables better voice analysis
5. No client-side validation or cap on these values was found
6. User has no visibility into current recording parameters

---

## 7. Additional Audio-Related Remote Controls

| PmsKey | Purpose | Risk |
|--------|---------|------|
| `calls-sdk-log-audio` | Enable audio logging in calls | Potential call recording |
| `calls-sdk-disable-pipeline` | Disable audio pipeline | Could bypass processing |
| `calls-sdk-dnt-disable-audio` | DNT audio disable | Privacy bypass |
| `calls-sdk-opus-adapt` | Adaptive Opus complexity | Quality manipulation |
| `calls-sdk-ai-opus-bwe` | AI-based Opus BWE | Bandwidth/quality control |
| `max-audio-length` | Max recording duration | Server-controlled limit |
| `isEarlyAudioRecordingEnabled` | Start recording before call connects | Pre-call audio capture |

---

## Risk Assessment

| Vulnerability | Severity | CVSS Est. |
|--------------|----------|-----------|
| Server-controlled Opus bitrate/sample rate | Critical | 8.1 |
| No E2E encryption for voice messages | High | 7.5 |
| Server-side transcription (audio sent to server) | High | 7.0 |
| In-call ASR recording via signaling | Medium | 6.5 |
| Fake Boss feature social engineering | Medium | 5.5 |
| Early audio recording in calls | Medium | 6.0 |

---

## Recommendations

1. **Cap opus-recorder-bitrate and opus-recorder-sample-rate** client-side with hardcoded maximums
2. **Implement E2E encryption** for voice messages before upload
3. **Require explicit user consent** for transcription with clear indicator when audio is being sent to server
4. **Show recording quality indicator** in UI when bitrate/sample rate exceed normal thresholds
5. **Audit the Fake Boss feature** for potential abuse via server-side enablement
6. **Restrict ASR startRecord** to require local user confirmation, not just signaling commands
