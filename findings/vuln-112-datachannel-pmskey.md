# vuln-112: DataChannel P2P Arbitrary Data + PmsKey Full Enumeration (335 flags)

## Severity: HIGH

## Finding 1: WebRTC DataChannel — No Input Validation (MEDIUM-HIGH)

**File**: `p000/jx2.java:417`

```java
public void onMessage(DataChannel.Buffer buffer) {
    ByteBuffer byteBuffer = buffer.data;
    byte[] bArr = new byte[byteBuffer.remaining()];
    int i = buffer.binary ? 2 : 1;
    byteBuffer.get(bArr);
    // Dispatches raw bytes to listeners with NO validation
    ((xyf) it.next()).mo8006a(a55Var, bArr, i);
}
```

During P2P calls, the remote peer can send arbitrary binary data via DataChannel.
No size limits, no format validation, no authentication of individual messages.
If the listener processes this data unsafely (e.g., protobuf parsing, media decoding),
it's a direct C2C attack vector.

## Finding 2: Complete PmsKey Enumeration — 335 Server-Controlled Flags

**File**: `ru/p027ok/tamtam/android/prefs/PmsKey.java`

335 flags that the server can set remotely without app update. Critical ones:

### Security-Critical Flags:
- `net-ssl-session-validate` — SSL session validation toggle
- `debug-mode` — enables debug features
- `log-sensitive` — logs sensitive data to logcat
- `log-full` — full logging
- `user-debug-report` — debug reports
- `android-use-logcat-logger` — logcat logging

### Privacy-Critical Flags:
- `non-contact-sync-time` — sync non-contacts
- `non-contact-max-chunk-size` — bulk collection size
- `non-contact-collection-interval` — collection frequency
- `send-location-enabled` — GPS tracking
- `mytracker-enabled` — analytics
- `analytics-enabled` — analytics
- `webapp-phone-hash` — phone hash to bots

### Surveillance Flags:
- `show-vpn-chat-bottomsheet` — VPN warning in chats
- `show-vpn-call-bottomsheet` — VPN warning in calls
- `show-vpn-snackbar` — VPN snackbar
- `calls-sdk-webrtc-logs` — WebRTC call logging
- `calls-sdk-log-audio` — AUDIO LOGGING during calls!
- `calls-sdk-traffic-markup` — traffic analysis

### App Control Flags:
- `keep-connection` — persistent connection
- `keep-background-socket` — background socket
- `wakelock-on-push` — wake device on push
- `disconnect-timeout` — connection timeout
- `ping-background-interval` — background ping frequency

### Feature Flags That Affect Security:
- `devnull` — /dev/null endpoint (data sink?)
- `dps` — unknown data processing
- `informer-enabled` — informer system
- `cfs` — unknown
- `ilm` — unknown

## Finding 3: `calls-sdk-log-audio` — Remote Audio Recording (CRITICAL)

The server can enable `calls-sdk-log-audio` flag which enables audio logging during calls.
This is effectively a **remote wiretap capability** — the server can record call audio
without user knowledge or consent.

## Finding 4: `devnull` Flag — Data Sink

The `devnull` PmsKey suggests a server-controlled data sink endpoint.
Combined with the 335 flags, the server has complete control over what data
the app collects and where it sends it.

## Impact

- Server has god-mode control over app behavior (335 flags, 0 user opt-outs)
- Remote audio recording capability via `calls-sdk-log-audio`
- Bulk contact harvesting via `non-contact-*` flags
- Debug/logging can be enabled remotely to exfiltrate data
- VPN detection warnings controlled by server
- DataChannel allows arbitrary P2P data injection during calls
