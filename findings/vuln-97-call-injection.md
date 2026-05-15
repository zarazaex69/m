# VULN-97: Call Setup Injection Analysis

## Summary

Analysis of the Max messenger (based on OK/TamTam calls SDK) call setup flow reveals multiple attack surfaces related to push-triggered call initiation, STUN/TURN server injection, and early media start before user consent.

## 1. Call Initiation Flow

The call system uses the following key components:
- `StartCallParams` / `StartCallApiParams` — outgoing call parameters
- `ConversationStart` — initiates a conversation/call via API
- `CallInfo` — server response containing endpoint, token, STUN/TURN servers
- `ConversationParams` — parsed call parameters including ICE servers
- `u92` (CallEngine) — manages call state, incoming/outgoing logic
- `fei` — processes incoming call push notifications (`m7420c`)
- `cei` — handles raw push data parsing for "InboundCall" type

**Flow**: Push notification → `cei` (parse) → `fei.m7420c` → `u92.m22712M` (present call) → `o92` (early UI if enabled)

## 2. STUN/TURN Server Reception

STUN/TURN servers are received in two ways:

### a) Via API response (CallInfo / ConversationParams)
- `TurnStunParser.parseTurn()` and `TurnStunParser.parseStun()` parse server URLs, username, and credential from JSON
- Servers are stored in `CallInfo.turnServer` / `CallInfo.stunServer` (type: `List<PeerConnection.IceServer>`)
- `ConversationParams.parseCallParams()` parses `turnServers` from JSONObject with `urls`, `username`, `credential` fields

### b) Via signaling/push (izb.java)
- The `izb` class (incoming call signaling message) contains a `turnServer` field parsed directly from the signaling payload
- Fields parsed: `conversationId`, `chatId`, `callerId`, `vcp`, `type`, `turnServer`, `isContact`, `sdpOffer`, `country`
- **No validation of TURN server URLs** is performed — they are parsed and used as-is

**Risk**: If an attacker can inject or modify signaling messages, they can supply a malicious TURN server that would relay all media through attacker-controlled infrastructure.

## 3. Call Injection with Crafted Parameters

### Push notification parameters (cei.java, line 169+):
The "InboundCall" push type extracts the following from the push payload map:
- `trid` — transaction ID (parsed as Long)
- `eKey` — encryption key
- `suid` — sender user ID
- `userName` — caller display name
- `vcId` — conversation/call ID
- `chatId` — chat identifier
- `vcp` — video call parameters
- `iv` — is video (boolean)
- `isContact` — whether caller is a contact
- `country` — caller country
- `rt` — ring time
- `phn` — phone number

**Critical Finding**: The caller identity (`suid`, `userName`) comes directly from the push payload. The `isContact` flag is also push-supplied and only partially validated:
```java
boolean z2 = Boolean.parseBoolean(str17) || !(di4VarM25981h == null || ...) || (di4VarM25981h != null && di4VarM25981h.m5487c());
```
The app checks local contacts DB as a fallback, but the initial `isContact` value from the push is trusted.

### Fake Caller ID Risk
- `userName` from push is displayed directly to the user
- `suid` (sender UID) determines the caller identity
- No cryptographic signature verification of the push payload is visible in the decompiled code
- The `eKey` field appears to be an encryption key for the call, not a verification token for the push itself

## 4. VoIP Push Handler

The push handler in `cei.java` processes "InboundCall" pushes:
1. Checks `map.get("type")` equals `"InboundCall"` (line 169)
2. Extracts all call parameters from the push map
3. Calls `fei.m6393d().m7420c(...)` to process the incoming call
4. On failure, calls `m7421d(map)` as error handling

**The push handler does NOT perform**:
- Signature verification of the push payload
- Server-side validation of the caller's identity before presenting the call
- Rate limiting on incoming call pushes

The push triggers immediate call UI presentation via `fei.m7420c` → `u92.m22712M`.

## 5. InboundCall Push Caller Identity Validation

**Validation is minimal**:
- The `suid` (sender user ID) is extracted from the push and used to look up local contact info: `((yj4) eeiVar3.f15551g.getValue()).m25981h(jLongValue, false)`
- If the user is found in local DB, their stored name/avatar may be used
- If NOT found, the push-supplied `userName` is displayed directly
- No server-side challenge/response to verify the caller actually initiated the call
- The `eKey` field is passed through but not used for push authentication

**Attack scenario**: An attacker who can send push notifications (e.g., via compromised push infrastructure or server-side vulnerability) could spoof any caller ID.

## 6. Forced Call Without User Consent

### Early Call Start (PmsKey: `early-call-start`)
From `o92.java` (line 129-134):
```java
"Early check: isEarlyCallStartEnabled=" + zM19049M + ", canShowEarly=" + z2 + ", hasCall="
...
"Early incoming: setting up early UI"
```

When `isEarlyCallStartEnabled` is true AND `canShowEarly` conditions are met:
- The call UI is presented immediately
- Call state is set to incoming (`ji8VarM22733w.f30974a = 2`)
- `presentIncomingCall` is triggered
- Listeners are notified (`c32.mo3226l()`)

### Early Media Start
From `rj5.java`:
- `CallsSDK-Audio-EarlyStartPlayout/Enabled/` — starts audio playout before answer
- `CallsSDK-Audio-EarlyStartRecording/Enabled/` — starts audio recording before answer

These are WebRTC field trials that, when enabled, begin media capture/playback before the user explicitly answers.

### Fake Boss Feature
- PmsKey `calls-fakeboss-incoming-call-enabled` enables a "fake boss" incoming call feature
- This appears to be a legitimate feature for simulating incoming calls, but its existence confirms the app has infrastructure to present calls without a real remote party

### No Auto-Answer Found
No explicit "auto-answer" mechanism was found that would answer a call without user interaction. However, the early-call-start feature combined with early media recording effectively captures audio before the user answers.

## 7. Early-Call-Start PmsKey Analysis

**PmsKey**: `early-call-start` (index 92)
- Controlled by server-side configuration (`PmsKey` values are fetched from server)
- When enabled, `isEarlyCallStartEnabled()` returns true
- Used in `qp6` (feature flags class) at index position matching `isEarlyCallStartEnabled`
- Related PmsKeys:
  - `calls-android-early-create-pc` — early PeerConnection creation
  - `calls-android-early-set-offer` — early SDP offer setting

**Impact**: When `early-call-start` is enabled:
1. Incoming call UI is shown immediately from push data (before SDK/server validation completes)
2. Combined with `EarlyStartRecording`, the microphone may be activated before the user answers
3. The call is set up using push-supplied parameters which may not be fully validated

## Risk Assessment

| Attack Vector | Severity | Feasibility |
|---|---|---|
| TURN server injection via signaling | HIGH | Medium (requires signaling access) |
| Caller ID spoofing via push | HIGH | Medium (requires push infra access) |
| Early media capture before answer | MEDIUM | Low (requires server config change) |
| Forced call UI presentation | MEDIUM | Medium (via crafted push) |
| Full call injection (fake caller + media) | HIGH | Medium-High (combined attack) |

## Recommendations

1. **Cryptographically sign push payloads** — verify that InboundCall pushes originate from the legitimate server
2. **Validate TURN server URLs** against an allowlist of known infrastructure
3. **Server-side caller verification** — require a server round-trip to confirm the call before presenting UI
4. **Disable early media recording** by default — require explicit user consent before microphone activation
5. **Rate-limit incoming call pushes** to prevent call flooding attacks
6. **Do not trust push-supplied display names** — always resolve from server-verified user data

## Files Analyzed

- `p000/cei.java` — InboundCall push parser
- `p000/fei.java` — Incoming call processor (`m7420c`)
- `p000/u92.java` — Call engine (state management)
- `p000/o92.java` — Early call start logic
- `p000/izb.java` — Signaling message with turnServer field
- `p000/rj5.java` — WebRTC field trials (early media)
- `p000/qp6.java` — Feature flags (isEarlyCallStartEnabled)
- `p000/bt1.java` — Push notification reporter
- `ru/ok/android/externcalls/sdk/api/CallInfo.java` — Call info with STUN/TURN
- `ru/ok/android/externcalls/sdk/api/TurnStunParser.java` — ICE server parser
- `ru/ok/android/externcalls/sdk/api/ConversationParams.java` — Call params parser
- `ru/ok/tamtam/android/prefs/PmsKey.java` — Feature flag keys
