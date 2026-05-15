# vuln-115: Zero Anti-Tampering / Anti-Reversing Protection

## Severity: MEDIUM-HIGH (enables all other attacks)

## Summary

Max messenger has absolutely NO protection against:
- Reverse engineering
- Runtime instrumentation (Frida)
- Code modification (repackaging)
- Root detection bypass
- Debugging

## Evidence

### Not Present:
- ❌ No Frida detection (no `frida-server` port check, no `/proc/self/maps` scanning)
- ❌ No root detection (no `su` binary check, no Magisk detection)
- ❌ No Xposed detection (no XposedBridge check)
- ❌ No SafetyNet / Play Integrity API
- ❌ No debugger detection (`Debug.isDebuggerConnected()` not called)
- ❌ No signature verification of own APK
- ❌ No code integrity checks
- ❌ No native anti-debug (no `ptrace(PTRACE_TRACEME)`)
- ❌ No obfuscation of critical paths (1116 unobfuscated classes)
- ❌ No string encryption for sensitive constants

### Minimal Detection Present:
- ✓ Basic emulator detection (`bhj.java:386`) — checks Build.DEVICE for "emulator"/"generic"
  Used only for analytics/telemetry, NOT for security blocking
- ✓ VPN detection — but only shows a dismissible warning

### Signature Checks (NOT self-integrity):
- `sxl.java:40` — checks `com.android.vending` (Play Store) signature
- `til.java:36` — same, for GMS availability
- `wb7.java:77` — checks content provider package signature
- **None of these verify the app's own signature**

## Impact

1. **Trivial Frida hooking** — All API calls, crypto operations, and auth flows can be intercepted
2. **Easy repackaging** — APK can be modified and redistributed (phishing clones)
3. **No protection for credentials** — session_key, session_secret_key extractable via Frida
4. **Bypass any client-side check** — VPN detection, pin lock, etc. trivially bypassed
5. **Full protocol analysis** — No obstacles to reverse engineering the binary protocol
6. **Credential theft at scale** — Malware can hook Max and steal all messages/tokens

## Comparison

| Feature | Max | Telegram | Signal | WhatsApp |
|---------|-----|----------|--------|----------|
| Root detection | ❌ | ❌ | ❌ | ✓ |
| Frida detection | ❌ | ❌ | ❌ | ✓ |
| Cert pinning | ❌ | ✓ | ✓ | ✓ |
| Code obfuscation | Partial | ✓ | ✓ | ✓ |
| Native protection | ❌ | ❌ | ❌ | ✓ |
| Integrity check | ❌ | ❌ | ❌ | ✓ |

Note: While Telegram and Signal also lack some protections, they compensate with
strong E2E encryption and cert pinning. Max has NEITHER.
