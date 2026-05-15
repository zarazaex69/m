# VULN-102: Signaling Injection Analysis in Max Messenger WebRTC Calls

## Summary

Max messenger uses a **server-mediated signaling architecture** over WebSocket (`tch.java`). All signaling messages flow through the OK server — peers do not communicate directly at the signaling layer. The server dispatches notifications to call participants. This architecture limits direct peer-to-peer signaling injection but introduces server-trust and notification-handling attack surfaces.

## Architecture Overview

- **Signaling transport**: `obh` interface → WebSocket implementation (`tch.java`)
- **Signaling command queue**: `pbh.java` — sends commands, processes responses/notifications
- **Notification handler**: `z91.java` — processes all incoming signaling notifications in `ra1` (call object)
- **Message types**: `"type"` field with values: `"response"`, `"notification"`, `"error"`
- **Notification dispatch**: Notifications are dispatched to listeners registered in `pbh.f52893k`

## Findings

### 1. Signaling Message Handler

**Location**: `ConversationImpl.java:1822` (`handleSignalingError`), `ConversationImpl.java:2258` (`onSignalingRefresh`)

The core notification handler is `z91.java` (registered as `ra1.f59549e`), which processes 40+ notification types via a switch statement on `jSONObject.getString("notification")`.

### 2. Remote Peer Signaling Messages

**Finding: Server-mediated, but peer-originated messages exist**

The `"transmitted-data"` notification (z91.java:429) carries SDP offers/answers and ICE candidates between peers, relayed by the server. The server includes `participantId` to identify the sender.

Key peer-influenced notifications:
- `"transmitted-data"` — SDP offer/answer, ICE candidates (peer-to-peer via server relay)
- `"mute-participant"` — can target specific participants or all (`muteAll`)
- `"switch-micro"` — remote mute toggle
- `"force-media-settings-change"` — forces media settings changes
- `"topology-changed"` — changes call topology (P2P ↔ server)

**Risk**: If the server is compromised or a participant can craft arbitrary signaling messages, they can inject any of these notification types.

### 3. ICE Restart from Remote Peer

**Finding: PARTIALLY VULNERABLE — Server/peer-triggered ICE restart possible**

In `lo5.java:531-541`, ICE restart is triggered when ICE connection fails:
```java
"Ice failed, restart with offer"
oadVar.m15750y(true); // createOffer with iceRestart=true
```

In `z91.java` `"transmitted-data"` handler, a remote SDP offer with `"p2pRelay": true` triggers:
```java
ra1Var.f59557g1 = true; // forces P2P relay mode
ra1Var.f59579o0.mo13510Q(true);
```

The remote peer can send an SDP offer that forces the local client to accept a P2P relay topology change. This effectively forces ICE renegotiation. However, STUN/TURN servers are configured locally from `conversationParams.stunTurnServers` — not from the remote SDP.

**Severity**: LOW-MEDIUM. ICE restart is triggered by connection failure detection locally, not directly by remote command. However, the `"topology-changed"` notification can force topology switches that trigger renegotiation.

### 4. Call Redirect / Transfer

**Finding: NO DIRECT REDIRECT MECHANISM FOUND**

No `"redirect"`, `"transfer"`, or `"moveCall"` signaling commands exist. The `"realloc-con"` notification is explicitly ignored:
```java
case "realloc-con":
    ra1Var.f59513O.log("OKRTCCall", "Unexpected notification... Ignore, because session id support is on");
```

The `"topology-changed"` notification can change call topology (e.g., from server-mediated to P2P), which alters the media path but doesn't redirect to a different endpoint.

**Severity**: LOW. No call redirect/transfer injection vector found.

### 5. DTMF Processing

**Finding: NO DTMF HANDLING FOUND**

No DTMF-related code (`dtmf`, `DTMF`, `DtmfTone`) was found in the calls SDK. Max messenger does not appear to implement DTMF tone processing in its WebRTC call stack.

**Severity**: N/A — attack surface does not exist.

### 6. Audio-to-Video Escalation Without Consent

**Finding: VULNERABLE — Remote can force media settings change**

The `"force-media-settings-change"` notification (z91.java:498) processes a `bob` (MediaSettings) object from the server notification:
```java
case "force-media-settings-change":
    bob bobVarM10818k2 = ivl.m10818k(jSONObject);
    if (!bobVarM10818k2.f6179e && bobVar.f6179e) { // audio being disabled
        bobVar.f6179e = false;
        bobVar.m2870a(); // notify listeners
    }
```

The `bob` class contains: `f6179e` (audio), `f6180f` (video), `f6176b` (screen capture), `f6181g` (animoji).

Additionally, the `"mute-participant"` notification can change media option states for any participant including the local user. The `kaa` enum has states:
- `UNMUTED` (0)
- `UNMUTED_BUT_MUTED_ONCE` (1)  
- `MUTED_PERMANENT` (2)
- `MUTED_PERMANENT_BUT_UNMUTED_ONCE` (3)

The `m23061j` method in `ur1.java` shows that when processing mute notifications, if the current state is `MUTED_PERMANENT`, it transitions to `MUTED_PERMANENT_BUT_UNMUTED_ONCE` — effectively allowing a remote unmute request.

**However**: The `"force-media-settings-change"` only appears to handle *disabling* audio (the code checks `!bobVarM10818k2.f6179e && bobVar.f6179e`). Enabling video remotely would require the `"mute-participant"` path with `requestedMedia` containing `"VIDEO"`.

The `requestToEnableMedia` method in `MediaMuteCommandExecutorImpl.java` sends a `"mute-participant"` command with `"requestedMedia": ["VIDEO"]` — this is a **request** to enable media for a participant. If the server relays this as a notification, the target client processes it.

**Severity**: MEDIUM. The server can force media state changes. A compromised server or MITM on the signaling WebSocket could potentially unmute video for a participant. The client does not appear to require explicit user consent before processing these state changes.

### 7. Screen Sharing Forced by Remote Peer

**Finding: PARTIALLY VULNERABLE — Same mechanism as video**

Screen sharing (`SCREEN_SHARING` / `jaa.f30259c`) is handled through the same `"mute-participant"` notification path. The `MediaMuteCommandExecutorImpl` can send `requestToEnableMediaForParticipant` with `SCREEN_SHARING` in the set.

However, `ScreenCaptureManagerImpl.java` requires explicit `setScreenCaptureEnabled(enabled, isFastScreenShareEnabled)` calls, which appear to be local-only operations requiring Android's MediaProjection permission.

**Severity**: LOW-MEDIUM. While the signaling state for screen sharing can be manipulated remotely (marking it as "requested" or "unmuted"), actually capturing the screen requires Android system-level permission that cannot be bypassed via signaling alone.

## Attack Scenarios

### Scenario A: Compromised Signaling Server
If an attacker controls or MITMs the WebSocket signaling server:
1. Inject `"mute-participant"` with `muteAll: true` to mute all participants
2. Inject `"force-media-settings-change"` to disable audio
3. Inject `"topology-changed"` to force P2P mode (potentially exposing IP addresses)
4. Inject `"transmitted-data"` with crafted SDP to force ICE renegotiation

### Scenario B: Malicious Participant (Limited)
A malicious participant can send signaling commands through the server:
- `"mute-participant"` targeting other participants (if server doesn't validate admin role)
- `"change-media-settings"` / `"update-media-modifiers"` for their own stream

## Mitigations Observed

1. **Server-mediated architecture**: Peers cannot directly inject signaling; server acts as gatekeeper
2. **Participant ID validation**: Some operations check `participantId` against known participants
3. **Role-based access**: `"roles-changed"` notification manages admin roles; mute operations check `adminId`
4. **ICE servers configured locally**: STUN/TURN servers come from initial call params, not from remote SDP

## Recommendations

1. **Validate `adminId` server-side** for `"mute-participant"` notifications — ensure only admins can mute others
2. **Require user consent** before processing `"force-media-settings-change"` that enables video/screen sharing
3. **Pin signaling WebSocket TLS certificates** to prevent MITM on the signaling channel
4. **Rate-limit topology changes** to prevent rapid ICE renegotiation attacks
5. **Validate SDP content** in `"transmitted-data"` before processing (check for malformed or oversized SDP)
