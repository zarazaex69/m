# WebRTC & Calls Implementation — Max Messenger

## 1. Architecture Overview

The calls system has three main layers:
- **`ru.p027ok.android.externcalls.sdk`** — External calls SDK (core logic)
- **`ru.p027ok.android.webrtc`** — WebRTC protocol layer
- **`one.video.calls.sdk`** — Video calls SDK (higher-level)

## 2. Signaling Transport

### Dual Transport: WebSocket + WebTransport (QUIC)

The signaling uses **two transport options** with automatic fallback:

1. **WebSocket (WS)** — default, via `j4k` builder → `tch` base class
2. **WebTransport (WT)** — QUIC-based, via `WTSignaling` class (requires API 26+)

```java
// ConversationImpl.java:1889
private boolean isWebTransportEnabled() {
    return this.callParams.f22700u.f16980s && WTSignaling.isAvailable();
}
```

**Transport selection** in `SignalingTransportBuilder.build()`:
```java
boolean useWt = (!preferWs && wtEndpointBaseUrl != null && wtEndpointBaseUrl.length() > 0);
log.log(TAG, "Build signaling transport. wt=" + useWt + ", prefer_ws=" + preferWs);
if (useWt) {
    endpointParametersBuilder.f33127e = this.wtEndpointBaseUrl;  // wt endpoint
    endpointParametersBuilder.f33128f = this.wtIps;              // wt IPs
} else {
    endpointParametersBuilder.f33127e = this.wsEndpointBaseUrl;  // ws endpoint
    endpointParametersBuilder.f33128f = this.wsIps;              // ws IPs
}
```

WebTransport uses **deflate-raw compression** by default and the `tech.kwik.flupke` QUIC library.

### PmsKey Feature Flags (Remote Config)

| Key | Purpose |
|-----|---------|
| `calls-endpoint` | Signaling endpoint URL |
| `calls-sdk-wt-enabled` | WebTransport toggle |
| `calls-android-wtp` | WebTransport params |
| `calls-android-signaling-ip` | Signaling by IP feature |
| `calls-android-signaling-to` | Signaling timeouts |
| `calls-android-direct-ice-restart` | Direct ICE restart |
| `calls-android-no-ice-restart` | Disable ICE restart on fail |
| `calls-android-ice-cps` | ICE candidate pool size |
| `calls-use-p2p-relay` | P2P relay switch |
| `calls-use-p2p-relay-caps` | Consider P2P relay capability |
| `calls-android-fast-join` | Fast join optimization |
| `calls-android-early-create-pc` | Early PeerConnection creation |
| `calls-android-early-set-offer` | Early apply remote offer |
| `calls-android-gen-peerid` | Self-generated peer ID |
| `calls-sdk-h265-prioritized` | H.265 codec priority |
| `calls-android-h265-s` | H.265 bitrate scale |
| `calls-android-simulcast-sw-vp8` | SW VP8 simulcast |
| `calls-sdk-ai-opus-bwe` | AI-based Opus BWE |
| `calls-sdk-linear-opus-bwe` | Linear Opus BWE |
| `calls-android-ns` | Noise suppressor |
| `calls-android-lla` | Low-latency audio |

## 3. Signaling URL Construction

The endpoint URL is built with these query parameters:

```
{endpointBaseUrl}?userId=...&token=...&conversationId=...&deviceIdx=...
  &peerId=...&locale=...&version=...&capabilities=...
  &device={manufacturer}/{model}&platform=android&clientType=...
  &appVersion=...&osVersion=...&ispAsOrg=...&locCc=...&locReg=...
  &ispAsNo=...&compression=deflate-raw&recoverTs=...
```

### Signaling by IP Feature

When `calls-android-signaling-ip` is enabled (`isUseOfIPEnabled`), the transport:
1. Parses the host and port from the endpoint URL
2. Builds a list of IP addresses from `wsIps` / `wtIps` (server-provided)
3. Appends the port to each IP
4. Rotates through IPs on reconnection for failover

```java
// tch.java:815 - m21752a builds IP list
public final List m21752a(l86 l86Var) {
    ArrayList arrayList = new ArrayList();
    int port = this.f66947D != null ? this.f66947D.port : -1;
    List<String> ips = l86Var.f36257f;  // endpointIPs
    if (ips != null) {
        for (String ip : ips) {
            arrayList.add(port > 0 ? ip + ":" + port : ip);
        }
    }
    // Also add original hostname as fallback
    String host = this.f66947D != null ? this.f66947D.host : null;
    if (host != null) {
        arrayList.add(port > 0 ? host + ":" + port : host);
    }
    return arrayList;
}
```

## 4. STUN/TURN Server Configuration

Servers are **dynamically provided by the backend** in API responses, not hardcoded.

### API Response Fields

| JSON Key | Short Key | Description |
|----------|-----------|-------------|
| `stun_server` | `stne` | STUN server URLs (comma-separated) |
| `turn_server` | (JSON object) | TURN server with urls/username/credential |
| `endpoint` | `wse` | WebSocket signaling endpoint |
| `wt_endpoint` | `wte` | WebTransport signaling endpoint |
| — | `wsip` | WebSocket endpoint IPs (JSON array) |
| — | `wtip` | WebTransport endpoint IPs (JSON array) |
| — | `trnu` | TURN username |
| — | `trnp` | TURN password |

### TurnStunParser

```java
// STUN: just URLs, no auth
PeerConnection.IceServer.builder(url).createIceServer();

// TURN: URLs + username + credential, TLS_CERT_POLICY_SECURE
PeerConnection.IceServer.builder(url)
    .setUsername(username)
    .setPassword(credential)
    .setTlsCertPolicy(PeerConnection.TlsCertPolicy.TLS_CERT_POLICY_SECURE)
    .createIceServer();
```

### CallInfo Structure

```java
public final class CallInfo {
    public final String endpoint;       // WS signaling URL
    public final List<String> wsIps;    // WS endpoint IPs
    public final String wtEndpoint;     // WebTransport URL
    public final List<String> wtIps;    // WT endpoint IPs
    public final String id;             // conversation ID
    public final String token;          // auth token
    public final String clientType;
    public final boolean isConcurrent;
    public final boolean isP2PForbidden;
    public final List<PeerConnection.IceServer> turnServer;
    public final List<PeerConnection.IceServer> stunServer;
    public final int deviceIndex;
}
```

## 5. P2P Relay Configuration

### Server-Side Control
- `isP2PForbidden` flag in `CallInfo` / `ConversationParams` — server can forbid P2P
- Remote config key: `android.p2prelay.config` with RTT threshold

### P2P Relay Switch Logic

Triggered after call is accepted, based on:
1. `calls-use-p2p-relay` PmsKey enabled
2. `calls-use-p2p-relay-caps` — all participants must have `USE_P2P_RELAY` capability
3. RTT violation threshold from `P2PRelaySwitchConfig`:
   - `rtt` — RTT threshold in ms
   - `rtt_violation_count` — number of violations before switching (default: 1)

```java
// ConversationImpl.java:770
private void maybeInitP2PRelaySwitchTrigger() {
    if ((!experiments.f16984w || canOpponentsUseP2PRelay()) 
        && isCaller 
        && call.peerConnectionState == CONNECTED) {
        P2PRelaySwitchConfigProviderImpl configProvider = 
            new P2PRelaySwitchConfigProviderImpl(remoteSettings, log);
        // Creates P2pRelaySwitchTrigger with stat monitor
    }
}
```

Stat event: `client_requested_p2p_relay` with reason (trigger, threshold, violationsCount).

### ICE Servers Resolver

```java
private t98 chooseIceServersResolver() {
    return this.experiments.f16966e0 == 3 ? new vsl() : new t3m(22);
}
// Used as: chooseIceServersResolver().mo21619m(conversationParams.stunTurnServers)
```

## 6. WebTransport Implementation

Uses **tech.kwik.flupke** (Java QUIC/HTTP3 library):
- `WTSignaling` extends `tch` (base signaling transport)
- NAL (Network Abstraction Layer) socket: `lpk` class
- Bidirectional streams for signaling messages
- Custom hostname verification against SAN certificates

```java
// WTSignaling stats events:
"webtransport_restart"
"webtransport_connected"
"webtransport_reconnected"
"webtransport_failed_pings"
"webtransport_failed_exception"
"webtransport_timeout"
```

**Fallback**: WT → WS fallback is supported (`isFallbackSupported() = true`), controlled by `callsWtToWsFallbackParams`.

## 7. vchat API Methods

| Method | Purpose |
|--------|---------|
| `vchat.startConversation` | Initiate a call |
| `vchat.joinConversation` | Join existing call |
| `vchat.joinConversationByLink` | Join via invite link |
| `vchat.getConversationParams` | Get call parameters (ICE servers, endpoints) |
| `vchat.hangupConversation` | End call |
| `vchat.createJoinLink` | Create invite link |
| `vchat.removeJoinLink` | Remove invite link |
| `vchat.getAnonymTokenByLink` | Anonymous join token |
| `vchat.getOkIdByExternalId` | ID resolution |
| `vchat.getOkIdsByExternalIds` | Batch ID resolution |
| `vchat.getExternalIdsByOkIds` | Reverse ID resolution |
| `vchat.clientSupportedCodecs` | Report codec support |

## 8. ICE Handling Features

- **Direct ICE restart** (`calls-android-direct-ice-restart`)
- **No ICE restart on fail** (`calls-android-no-ice-restart`)
- **ICE candidate pool size** (`calls-android-ice-cps`)
- **ICE candidate pair changed stat** tracking
- **ICE candidate gathering failed stat** tracking
- **Server topology requested stat** — tracks SFU/relay requests

## 9. Codec & Media Configuration

- **H.265** support with bitrate scale and prioritization flags
- **VP8 simulcast** (software encoder option)
- **Opus** with adaptive complexity and AI-based BWE
- **Low-latency audio** mode
- **Noise suppressor** (ML-based keyword spotter integration)
- **Video transform v2** for effects

## 10. Connection Parameters

- Signaling timeout: **30,000 ms** (connection)
- Server ping timeout: **20,000 ms**
- Fast recover: configurable per call
- Protocol version: **≥6** (includes deviceIdx parameter)
- Platform identifier: `android`

## 11. Key Observations

1. **No hardcoded STUN/TURN servers** — all provided dynamically by backend
2. **Dual-transport architecture** (WS + WebTransport/QUIC) with automatic fallback
3. **IP-based signaling** for DNS bypass / faster connection in restricted networks
4. **P2P relay** is an adaptive quality feature triggered by RTT degradation
5. **Server controls P2P** via `isP2PForbidden` flag
6. **Certificate pinning** in WebTransport via custom hostname verifier checking SANs
7. **Signaling endpoint** is provided per-call by the `vchat.startConversation` API response
