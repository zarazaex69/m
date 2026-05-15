# VULN-67: STUN/TURN Server Parsing and ICE Candidate Injection Analysis

## Summary

Max messenger's VoIP calling SDK (`calls-sdk`) receives STUN/TURN server configurations and ICE candidates from the server without URL validation, enabling potential media interception through malicious relay injection.

## Affected Components

- `ru.p027ok.android.externcalls.sdk.api.TurnStunParser` — parses STUN/TURN server URLs
- `ru.p027ok.android.externcalls.sdk.api.ConversationParams` — stores and processes call parameters
- `ru.p027ok.android.externcalls.sdk.api.CallInfo` — call info with TURN/STUN server lists
- `p000.lo5` — parses remote ICE candidates from signaling messages
- `p000.C0530kg` (IceCandidatesHandler) — applies ICE candidates to peer connections
- `ru.p027ok.android.externcalls.sdk.p2prelay.P2pRelaySwitchTrigger` — forces relay mode

## Finding 1: No URL Validation on STUN/TURN Server URLs

**Severity: HIGH**

### Evidence

`TurnStunParser.java` accepts any string from the server as a STUN/TURN URL without validation:

```java
// parseStun - no scheme/host/port validation
for (String str : arrayList) {
    if (str.length() != 0) {
        arrayList2.add(PeerConnection.IceServer.builder(str).createIceServer());
    }
}

// parseTurn - same issue, only checks non-empty
for (String str : arrayList) {
    if (str.length() != 0) {
        arrayList2.add(PeerConnection.IceServer.builder(str)
            .setUsername(strMo10979v02)
            .setPassword(strMo10979v0)
            .setTlsCertPolicy(PeerConnection.TlsCertPolicy.TLS_CERT_POLICY_SECURE)
            .createIceServer());
    }
}
```

`ConversationParams.parseCallParams()` similarly passes raw URLs directly:
```java
conversationParams.stunTurnServers.add(
    PeerConnection.IceServer.builder(jSONArrayOptJSONArray.getString(i))
        .setUsername(strOptString)
        .setPassword(strOptString2)
        .createIceServer());
```

**Missing validations:**
- No scheme validation (should only accept `stun:`, `stuns:`, `turn:`, `turns:`)
- No host validation (no allowlist, no domain verification)
- No port range validation
- No check against known/trusted server infrastructure

### Attack Vector

A compromised server or MITM on the API connection can inject arbitrary TURN server URLs. The client will blindly connect to attacker-controlled relay servers, routing all media through them.

## Finding 2: Server-Controlled P2P Relay Forcing (Surveillance Capability)

**Severity: HIGH**

### Evidence

The server can force all media through relay servers via multiple mechanisms:

1. **`isP2PForbidden` flag** — Server sets `p2p_forbidden: true` in call parameters:
```java
// CallInfo.java
public final boolean isP2PForbidden;

// ApiProtocol.java
public static final String KEY_P2P_FORBIDDEN = "p2p_forbidden";
```

2. **Remote P2P relay switch config** — Server pushes config via `android.p2prelay.config` remote setting that triggers relay switching based on RTT thresholds:
```java
// P2pRelaySwitchTrigger.java
if (this.rttViolationCount >= config.getRttViolationCount()) {
    this.onSwitchTrigger.invoke();  // Forces switch to relay
}
```

3. **`USE_P2P_RELAY` capability** — Client advertises relay support; server can mandate it.

**Impact:** The server operator (or an attacker who compromises the signaling server) can force ALL media through their TURN relay servers, enabling passive surveillance of voice/video calls without the user's knowledge.

## Finding 3: No Validation of Remote ICE Candidates

**Severity: MEDIUM**

### Evidence

Remote ICE candidates received via signaling are parsed from JSON and passed directly to WebRTC without validation:

```java
// lo5.java:1045 - parsing remote ICE candidates from signaling
JSONObject jSONObjectOptJSONObject4 = jSONObject2.optJSONObject("candidate");
IceCandidate iceCandidate = jSONObjectOptJSONObject4 != null ?
    new IceCandidate(
        jSONObjectOptJSONObject4.getString("sdpMid"),
        jSONObjectOptJSONObject4.getInt("sdpMLineIndex"),
        jSONObjectOptJSONObject4.getString("candidate")
    ) : null;
```

The `IceCandidatesHandler` (`C0530kg`) pushes candidates directly to the peer connection:
```java
oadVar.m15744s((IceCandidate) it.next());  // addRemoteIceCandidate
```

The `amf` interface (`mo974a`) provides a candidate transformation hook, but the default implementation (`dig.java`) is a pass-through:
```java
public IceCandidate mo974a(IceCandidate iceCandidate) {
    return iceCandidate;  // No validation
}
```

**Missing validations:**
- No check on candidate type (host/srflx/relay/prflx)
- No IP address validation (private ranges, localhost, etc.)
- No rate limiting on candidate additions
- No verification that candidates match expected network topology

## Finding 4: TURN Credentials Exposure in toString()

**Severity: MEDIUM**

### Evidence

`ConversationParams.toString()` includes the full `stunTurnServers` list:
```java
public String toString() {
    StringBuilder sb = new StringBuilder("ConversationParams{endpoint='");
    // ...
    sb.append(", stunTurnServers=");
    sb.append(this.stunTurnServers);  // Calls IceServer.toString()
    // ...
}
```

The `PeerConnection.IceServer` class stores `username` and `password` as public fields:
```java
public final String password;
public final String username;
```

If `ConversationParams.toString()` is ever logged (which is common for debugging), TURN credentials will appear in logcat.

**Mitigating factor:** The `LoggingApiRequestDebugger` erases `credential` from API request/response logs:
```java
new RequestSecretEraser(new String[]{
    ApiProtocol.KEY_TOKEN, "auth_data", "credential", "auth_token", "session_data"
}, ERASED_SECRET);
```

However, this only covers API-level logging, not application-level `toString()` calls.

## Finding 5: Compact Params Encoding Lacks Integrity Protection

**Severity: MEDIUM**

### Evidence

`ConversationParams.decode()` accepts Base64+LZ4 encoded parameters that include TURN server URLs and credentials:
```java
public static ConversationParams decode(String str) {
    String[] strArrSplit = str.split(":");
    // ... LZ4 decompress + Base64 decode
    return parseCallParamsCompact(new JSONObject(new String(bArr)));
}
```

The compact format (`parseCallParamsCompact`) contains:
- `trne` — TURN endpoints (comma-separated)
- `trnu` — TURN username
- `trnp` — TURN password
- `stne` — STUN endpoints

No signature or MAC verification is performed on this data. If this encoded blob is transmitted through an insecure channel (e.g., push notification, intent), it can be tampered with.

## Attack Scenarios

### Scenario 1: Malicious TURN Server Injection
1. Attacker compromises the signaling server or performs MITM on API calls
2. Injects a TURN server URL pointing to attacker infrastructure: `turn:evil.example.com:3478`
3. Provides valid credentials for the malicious TURN server
4. Sets `p2p_forbidden: true` to force relay mode
5. All media flows through attacker's server — voice/video intercepted

### Scenario 2: State-Level Surveillance via Relay Forcing
1. Server operator sets `p2p_forbidden: true` for targeted users
2. Provides TURN servers under operator's control
3. All call media is relayed through monitored infrastructure
4. Users have no indication that P2P was disabled

### Scenario 3: ICE Candidate Injection
1. Attacker with signaling server access injects crafted ICE candidates
2. Candidates point to attacker-controlled IP addresses
3. WebRTC connectivity checks may establish connection to attacker
4. Enables media interception or call disruption

## Recommendations

1. **Validate STUN/TURN URLs** — Enforce scheme allowlist (`stun:`, `stuns:`, `turn:`, `turns:`), validate hostname against known infrastructure domains, validate port ranges
2. **Certificate pinning for TURN/TLS** — Use `TLS_CERT_POLICY_SECURE` consistently (already done in `parseTurn` but not in `parseCallParams`)
3. **Integrity-protect call parameters** — Sign the compact params blob to prevent tampering
4. **ICE candidate validation** — Validate candidate format, restrict IP ranges, implement rate limiting
5. **User notification for relay mode** — Inform users when P2P is disabled and media is relayed
6. **Redact credentials from toString()** — Override `toString()` to mask passwords in `ConversationParams`
