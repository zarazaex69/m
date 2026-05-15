# Vulnerability 94: Type Confusion in Message Dispatch System

## Summary

The Max messenger (TamTam) message dispatch system in `owb.java` and `pb8.java` contains multiple type confusion vulnerabilities where received `jei` response objects are cast to specific subtypes based solely on the wire-protocol opcode (`x5d.f79133d`) without `instanceof` validation. An attacker controlling the server response or performing a MITM attack can send a packet with opcode X but payload structured as type Y, causing ClassCastException or, in specific cases, field access on the wrong object type.

## Architecture Overview

### Message Flow
1. **Wire packet** (`x5d`): Contains `f79133d` (opcode short) and `f79135f` (payload bytes)
2. **Deserialization** (`owb.m17502c`): Reads opcode from packet header, deserializes payload into a `jei` subclass based on opcode via a giant if-else chain
3. **Dispatch**: The deserialized `jei` object is passed to either:
   - `pb8.mo8661b()` — notification handler (server-push, opcode from wire `x5d.f79133d`)
   - `a6dVar.f594a.mo8661b()` — request-response handler (task callbacks like `dfi.mo459b()`)

### Key Classes
- `x5d` — Wire packet (header: version, cmd, seq, opcode, length)
- `jei` — Abstract base class for all response/notification types
- `AbstractC0830q2` — Abstract base for request types, `mo4586q()` returns opcode
- `u0d` — Opcode enum mapping names to short values
- `owb.m17502c()` — Central deserialization dispatcher
- `pb8.mo8661b()` — Notification handler with direct casts
- `dfi.mo459b()` — Task response callback interface

## Vulnerability Details

### Finding 1: Critical Logic Bug in Session Init Handling (owb.java:590)

```java
z = jeiVar instanceof ksg;
if (z) {
    ksg ksgVar = (ksg) jeiVar;  // Safe: guarded by instanceof
    this.f51589b.f54721d = ksgVar.f34984z0;
    this.f51589b.f54723f = ksgVar.f34975A0;
}
if (!z && ((ksg) jeiVar).f34983o != 1) {  // BUG: casts to ksg when NOT instanceof ksg
    this.f51589b.f54728k.set(true);
} else if (z && ((ksg) jeiVar).f34983o == 1) {
    idiVar.mo8661b(jeiVar);
    this.f51589b.m18342g(true);
    return;
}
```

**Issue**: When `z = false` (jeiVar is NOT a `ksg`), the code still casts `jeiVar` to `ksg` and accesses `f34983o`. This is a decompilation artifact of a likely bytecode-level optimization, but in the runtime it means:
- If the deserialization produces a non-`ksg` object for opcode 6 (SESSION_INIT), the cast will throw `ClassCastException`
- If the JVM doesn't throw (e.g., due to type erasure in certain scenarios), field `f34983o` at the wrong offset could be read from a different object type

### Finding 2: Unchecked Casts in Notification Handler (pb8.mo8661b)

The `pb8.mo8661b()` method reads the opcode from the wire packet header (`((x5d) this.f52860b).f79133d`) and directly casts `jeiVar` to the expected type:

```java
short s = ((x5d) this.f52860b).f79133d;  // opcode from wire
if (s == 2) {
    yzbVar2.m26338d(new n77(yzbVar2, 21, (v75) jeiVar));  // Direct cast, no instanceof
}
if (s == 3) {
    fefVar = (fef) jeiVar;  // Direct cast, no instanceof
}
if (s == u0d.f69054B2.f69214a) {
    d0c d0cVar = (d0c) jeiVar;  // Direct cast, no instanceof
}
// ... 30+ more direct casts without instanceof checks
```

**Attack vector**: The opcode in the wire packet header (`x5d.f79133d`) is read independently from the payload. The deserialization in `owb.m17502c` uses the SAME opcode to select which constructor to call. However, if an attacker can:
1. Craft a valid packet header with opcode A
2. But include payload bytes that are valid for a different message type B

The deserialization will create type A (matching the opcode), so the cast in `pb8` will succeed. The real risk is in the **request-response path** where the response opcode could differ from what the client expects.

### Finding 3: Request-Response Opcode Mismatch (Task Callbacks)

In the request-response flow:
```java
// owb.java line 800
a6d a6dVar2 = (a6d) this.f51589b.f54741x.get(Short.valueOf(x5dVar.f79132c));
// ...
m17502c(bArrM17799a, x5dVar, a6dVar2.f594a);  // passes response to original requester
```

The response is deserialized based on the **response packet's opcode** (`x5dVar.f79133d`), then delivered to the original task's callback. The task callbacks perform direct casts:

| File | Cast | Expected Type |
|------|------|---------------|
| `xaj.java:37` | `(vjb) jeiVar` | vjb (contact verify response) |
| `gi3.java:65` | `(hi3) jeiVar` | hi3 (chat update response) |
| `or6.java:39` | `(qr6) jeiVar` | qr6 (generic response) |
| `m54.java:40` | `(n54) jeiVar` | n54 |
| `uj3.java:29` | `(ak3) jeiVar` | ak3 (contact search) |
| `ck9.java:30` | `(ek9) jeiVar` | ek9 |
| `u7e.java:46` | `(aae) jeiVar` | aae (profile response) |

**Attack scenario**: A malicious server (or MITM) responds to a request with sequence number N using a different opcode than expected. The client deserializes based on the response opcode, creating type X, but the task callback expects type Y and casts without checking.

### Finding 4: No Opcode Validation Between Request and Response

The protocol uses sequence numbers (`x5d.f79132c`) to match responses to requests. The pending request map (`f54741x`) stores `a6d` objects keyed by sequence number. When a response arrives:

1. Sequence number is used to find the original request
2. Response opcode is used to deserialize the payload
3. The deserialized object is passed to the task callback

**There is no validation that the response opcode matches the request opcode.** The client trusts that the server will respond with the correct opcode for the given sequence number.

### Finding 5: Factory Method Without Type Safety (dyb.mo1994n)

The `dyb.f14192Y.mo1994n(kxaVar)` factory (used for LOGIN opcode 19) creates response objects from raw bytes. The factory at `dyb.java:871` parses fields from the `kxa` unpacker without validating that the field structure matches the expected type. A malformed payload could cause:
- Null pointer dereferences on missing fields
- Integer overflow on unexpected field types
- String read from non-string field positions

## Exploitation Potential

### Scenario 1: Server-Side Attack (Malicious/Compromised Server)
- Server sends response with mismatched opcode for a pending request
- Client deserializes as type A, task callback casts to type B → ClassCastException → session crash
- Repeated crashes = denial of service

### Scenario 2: MITM with Packet Manipulation
- Attacker intercepts response, changes opcode in 10-byte header (bytes 4-5)
- Payload remains valid for original type, but client now deserializes as different type
- Cast succeeds but field access reads wrong data → information confusion

### Scenario 3: Session Init Confusion (owb.java:590)
- If a non-ksg response reaches the session init handler path
- The `((ksg) jeiVar).f34983o` access reads an arbitrary int from the wrong object
- If value != 1, `f54728k` is set to true (session marked as initialized)
- Could bypass session initialization checks

## Risk Assessment

| Factor | Rating |
|--------|--------|
| Severity | Medium-High |
| Exploitability | Medium (requires MITM or compromised server) |
| Impact | DoS (crash), potential session state corruption |
| Scope | All message types in the dispatch system |

## Mitigations Observed

1. The deserialization and cast use the SAME opcode source in the notification path (both from `x5d.f79133d`), which limits type confusion to the request-response path
2. `ClassCastException` would crash the handler but is caught at higher levels in some paths
3. The `UnknownOpcodeException` handler (owb.java:630) catches truly unknown opcodes but not mismatched ones

## Recommendations

1. Add `instanceof` checks before all casts in `pb8.mo8661b()` and `dfi.mo459b()` implementations
2. Validate that response opcode matches the expected opcode for the pending request
3. Store expected response type in `a6d` and verify before delivering to callback
4. Fix the logic bug at owb.java:590 where `!z` path casts to `ksg`

## Files Analyzed

- `p000/owb.java` — Packet reader/writer, central dispatch (906 lines)
- `p000/pwb.java` — Session management (707 lines)
- `p000/pb8.java` — Notification listener with 30+ direct casts
- `p000/x5d.java` — Wire packet format (10-byte header)
- `p000/u0d.java` — Opcode enum definitions
- `p000/jei.java` — Response base class
- `p000/AbstractC0830q2.java` — Request base class
- `p000/dfi.java` — Task callback interface
- `p000/qei.java` — Async task executor
- `p000/gi3.java`, `p000/xaj.java`, `p000/uj3.java`, `p000/ck9.java`, `p000/u7e.java`, `p000/or6.java`, `p000/m54.java` — Task callbacks with unchecked casts
