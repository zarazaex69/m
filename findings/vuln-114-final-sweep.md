# VULN-114: Final Vulnerability Sweep — Max Messenger

**Date:** 2026-05-15  
**Scope:** ~/max/decompiled/jadx/base/sources  
**Analyst:** final-sweep (automated)

---

## 1. WebSocket Text Message Handlers (Command Injection)

**File:** `p000/owb.java` — Protocol message processor (binary opcode-based, not raw text WS)

**File:** `ru/ok/android/externcalls/sdk/wt/WTSignaling.java:154`
```java
public void onMessage(String text) {
    ((tch) ((C0530kg) listener).f33821c).m21759b(text);
}
```

**Finding:** The WebSocket text handler in `WTSignaling` passes raw text directly to `tch.m21759b()` without visible sanitization. This is the VoIP/calls signaling channel. The text is JSON-based signaling commands (join, offer, answer, ICE candidates). No evidence of shell/exec injection from WS text — the handler delegates to a typed signaling state machine (`tch`). **Risk: LOW** — signaling is structured JSON parsed into typed objects, not evaluated as commands.

**Interface:** `p000/yqb.java:9` defines `void onMessage(String str)` — the WebSocket listener contract.

---

## 2. Remote Shell / Debug Interface

**Finding: NONE DETECTED**

No matches for `RemoteDebug`, `debugServer`, `debugPort`, `adb.*connect`, or `shell.*exec` in the codebase.

The `isDebug` flag in `DefaultCallAnalyticsLogger` only gates log verbosity — no remote access surface. The `debug-mode` PMS key is a server-pushed preference for log level, not a remote shell.

**Risk: NONE**

---

## 3. Token/Session in URL Parameters (Referer Leakage)

**CRITICAL FINDING:**

**File:** `p000/och.java:42`
```java
builderBuildUpon.appendQueryParameter(ApiProtocol.KEY_TOKEN, l86Var.f36253b)
    .appendQueryParameter(ApiProtocol.PARAM_CONVERSATION_ID, l86Var.f36252a);
```

The **call/conversation token** is passed as a URL query parameter when connecting to the signaling WebSocket endpoint. This token is the authentication credential for the VoIP session.

**Impact:** If the WebSocket connection traverses any HTTP intermediary that logs URLs, or if the URL is exposed via `Referer` headers on subsequent requests, the call token leaks. An attacker with the token could potentially join/hijack the call session.

**Additional:** `ru/ok/tracer/upload/SampleUploadWorker.java:140,188` — `sampleToken` and `uploadToken` also passed as query parameters to `sdk-api.apptracer.ru`. Lower risk (crash reporting tokens), but still leakable via server logs.

**Risk: MEDIUM-HIGH** — Call session tokens in URL query strings are a well-known anti-pattern (RFC 6750 §2.3 explicitly discourages bearer tokens in URIs).

---

## 4. Backdoor Commands

**Finding: NONE DETECTED**

No matches for `backdoor`, `master.*key`, `override.*auth`, `bypass.*auth`, or `admin.*mode`.

The `skeleton` matches are all UI placeholder/shimmer resources (`skeleton_bubble_primary_static_background`, etc.) — purely cosmetic, not authentication bypasses.

**Risk: NONE**

---

## 5. Timing Side Channels

**Finding: POTENTIAL CONCERN**

**File:** `p000/zb5.java:35` and `p000/yb5.java:33`
```java
if (!Arrays.equals((byte[]) entry.getValue(), (byte[]) map2.get(entry.getKey()))) {
```

These use `Arrays.equals()` on byte arrays that appear to be cryptographic key material (key maps). `Arrays.equals()` is **not constant-time** — it short-circuits on first mismatch.

**No `MessageDigest.isEqual()` or constant-time comparison** found anywhere in the codebase.

However, practical exploitation requires:
- Local attacker with precise timing measurement
- Repeated comparison attempts against the same key material

**Risk: LOW-MEDIUM** — Theoretical timing oracle on key comparison. Unlikely to be exploitable remotely over network jitter, but violates cryptographic best practices.

---

## 6. Disappearing Messages — Are They Truly Deleted?

**Finding: CONCERNING DESIGN**

**Protobuf field:** `Protos.java:6104` — `public int messagesTtlSec;` (per-chat TTL setting)

**Delete task:** `Tasks.java` — `MsgDelete` protobuf with field:
```java
public boolean notDeleteMessageFromDb;  // line 5149
```

**Analysis:** The `notDeleteMessageFromDb` flag in the `MsgDelete` task indicates that message deletion can be requested **without actually removing from the local database**. This is a server-controlled flag — when set to `true`, the message is removed from the UI but **persists in the local SQLite database**.

This means:
1. "Disappearing" messages with `messagesTtlSec` rely on client-side enforcement
2. The `notDeleteMessageFromDb` flag allows the server to instruct clients to keep messages even after "deletion"
3. No evidence of secure memory wiping or file shredding for expired media

**Risk: MEDIUM** — Users expecting disappearing messages to be irrecoverable may be misled. Messages can persist in the local DB, recoverable via forensic tools or root access. Server can override deletion behavior.

---

## Summary Table

| # | Check | Risk | Exploitable? |
|---|-------|------|-------------|
| 1 | WebSocket text injection | LOW | No — typed JSON signaling |
| 2 | Remote shell/debug | NONE | N/A |
| 3 | Token in URL params | **MEDIUM-HIGH** | Yes — Referer/log leakage |
| 4 | Backdoor commands | NONE | N/A |
| 5 | Timing side channels | LOW-MEDIUM | Theoretical only |
| 6 | Disappearing messages | MEDIUM | Yes — forensic recovery possible |

---

## Recommendations

1. **Token in URL (Critical):** Move call tokens to WebSocket subprotocol headers or the first WS frame after connection. Never pass bearer tokens as query parameters.
2. **Timing:** Replace `Arrays.equals()` with `MessageDigest.isEqual()` for all cryptographic key comparisons in `zb5.java` and `yb5.java`.
3. **Disappearing messages:** Implement secure deletion (overwrite + `VACUUM` on SQLite) and remove the `notDeleteMessageFromDb` escape hatch, or clearly document to users that deletion is best-effort.
