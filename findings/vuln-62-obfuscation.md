# VULN-62: ProGuard/R8 Obfuscation Analysis — Max Messenger

## Summary

Max messenger uses R8 obfuscation (AGP 8.11.1) but has significant gaps: **1,116 app-specific classes** retain original names across `one.me`, `ru.ok`, and `tech.kwik` packages, exposing internal architecture, security flows, and API structure. No string encryption or control flow obfuscation is applied. A git commit hash and module names are embedded in the APK.

**Severity: Medium** — Facilitates reverse engineering of security-critical components.

---

## 1. Non-Obfuscated Classes (Kept Original Names)

**Statistics:**
- Obfuscated classes (in `p000/`): **21,064**
- Non-obfuscated classes: **2,731** total
  - `one.me` (app core): **381 classes**
  - `ru.ok` (OK.ru SDK/API): **702 classes**
  - `tech.kwik` (QUIC/TLS): **33 classes**
  - Third-party libs (kotlin, androidx, rxjava, etc.): ~1,615 classes

**Key exposed app packages:**
- `one.me.login/` — Full login flow (InputPhoneScreen, ConfirmPhoneScreen, LoginScreen)
- `one.me.settings.twofa/` — 2FA implementation (TwoFACreationScreen, TwoFACheckPassScreen)
- `one.me.net.ssl.api/` — SSL session/integrity exceptions
- `one.me.android.secure/` — Security module
- `one.me.sdk.tasks.login/` — Login task internals
- `one.me.devmenu/` — Developer menu (server config, memory debugger, threads viewer)
- `one.me.calls.impl.service/` — Call service implementation
- `ru.ok.android.api.session/` — Session management exceptions
- `ru.ok.android.externcalls.sdk.api/` — Full VoIP API (TURN/STUN parsing, join links, conversation management)

---

## 2. Security-Critical Code in Non-Obfuscated Packages

| Component | Package | Risk |
|-----------|---------|------|
| Login flow | `one.me.login.*` | Full authentication UI flow readable |
| 2FA implementation | `one.me.settings.twofa.*` | 2FA creation/verification logic exposed |
| SSL validation | `one.me.net.ssl.api.*` | SSL session/integrity error handling visible |
| Session management | `ru.ok.android.api.session.*` | Session recreation logic exposed |
| Push token handling | `one.me.sdk.vendor.SystemServicesManager$PushTokenGeneratedListener` | Token generation interface exposed |
| Signature generation | `one.me.sdk.android.tools.SignatureGenerateException` | Reveals signature verification exists |
| TURN/STUN parsing | `ru.ok.android.externcalls.sdk.api.TurnStunParser` | WebRTC ICE server parsing fully readable |
| API login | `ru.ok.android.api.core.ApiLoginException` | API auth error hierarchy exposed |

---

## 3. @Keep Rules Exposing Internal APIs

**67 @Keep annotations** found across the codebase. Notable kept classes:

- `one.me.sdk.vendor.SystemServicesManager$PushTokenGeneratedListener` — Push token interface with method `onPushTokenGenerated(String token)`
- `one.me.sdk.android.tools.SignatureGenerateException` — Extends `SecurityException`, reveals app signature verification
- `one.me.sdk.gl.effects.VideoMessageStencilHolder` — Native JNI methods kept (required for JNI binding)
- `one.me.sdk.concurrent.LinkedTransferQueue34` — Uses `java.lang.invoke.VarHandle`
- `one.me.sdk.richvector.internal.element.*` — Multiple internal vector rendering classes
- `one.me.devmenu.tools.TestCrash` — Debug crash tool kept in production
- `ru.ok.tamtam.login.LoginEventsByBus` — Login event bus kept

**Reason for keeps:** Primarily JNI native method binding, reflection-based instantiation, and serialization requirements. However, some keeps (devmenu, internal exceptions) appear unnecessary.

---

## 4. Reflection-Based Code Revealing Names

Reflection usage found in non-obfuscated code:
- `CallScreen.java:872` — Accesses `RecyclerView` private field `"o1"` via `getDeclaredField`
- `MediaBarPreviewLayout.java:579` — Accesses `TextView.mCursorDrawableRes`, `mEditor` fields
- `ConversationFactory.java:659` — Accesses `Build.VERSION.SEM_PLATFORM_INT` (Samsung detection)
- `kotlinx.coroutines.internal.FastServiceLoader` — `Class.forName("kotlinx.coroutines.android.AndroidDispatcherFactory")`

**Kotlin @Metadata annotations** throughout non-obfuscated classes leak:
- Original fully-qualified class names (e.g., `Lone/me/sdk/tasks/login/LoginException;`)
- Method signatures and parameter names
- Module names (e.g., `tamtam-android-sdk_release`, `calls-sdk_release`, `odnoklassniki-android-api_release`)
- Internal package references (reveals "TamTam" is the internal project name, "Odnoklassniki" is the parent platform)

---

## 5. Predictable Obfuscated Class Names

Obfuscated names in `p000/` follow a **highly predictable sequential pattern**:
- Format: `[a-z][a-z0-9][a-z0-9]` (3-character base-36 encoding)
- Examples: `a00, a01, a02, ..., a0z, a10, a11, ..., zz0, zzl`
- Total: 21,064 unique names

This is standard R8 dictionary-based naming. While not directly exploitable, the sequential nature means:
- Class ordering correlates with compilation/package order
- Related classes often have adjacent names (e.g., inner classes)
- Easier to map relationships between obfuscated classes

---

## 6. Anti-Decompilation Techniques

**None detected.** The app does NOT use:
- ❌ String encryption (no DexGuard/iXGuard patterns found)
- ❌ Control flow obfuscation
- ❌ Class encryption
- ❌ Anti-tampering checks (no integrity verification in decompiled code)
- ❌ Debugger detection
- ❌ Root/emulator detection (in non-obfuscated code)
- ❌ Custom classloaders

**Native libraries present** (but not for protection):
- `librtmp-jni.so` — RTMP streaming
- `libzstd.so` — Zstandard compression
- FFmpeg-based video decoding
- GL effects rendering

The app relies solely on R8 name obfuscation as its only reverse-engineering countermeasure.

---

## 7. Debug Symbols and Build Information Leaked

**Found in APK:**

| Artifact | Content | Risk |
|----------|---------|------|
| `META-INF/version-control-info.textproto` | Git commit: `2524a0588e12f6264d1097cfc1d3479c79fc6e9a` | Identifies exact source version |
| `META-INF/app-metadata.properties` | `androidGradlePluginVersion=8.11.1` | Build toolchain info |
| `.kotlin_module` files | `calls-sdk_release`, `calls-audiomanager_release`, `calls-sdk-analytics_release`, `calls-sdk-common_release`, `rlottie_release` | Internal module architecture |
| `.properties` files | Firebase, Play Services, OpenTelemetry version info | Dependency versions for CVE targeting |
| Kotlin `@Metadata` annotations | Full class/method names, module names | Defeats obfuscation for annotated classes |
| Hardcoded URL | `https://sdk-api.apptracer.ru` (tracer upload endpoint) | Internal infrastructure exposed |

**No mapping.txt found** — not shipped in APK (correct practice).

---

## 8. Recommendations

1. **Enable R8 full mode** with aggressive keep rule auditing — remove unnecessary keeps on devmenu, internal exceptions
2. **Strip Kotlin @Metadata** annotations or obfuscate their content (R8 supports `-dontnote kotlin.Metadata`)
3. **Remove devmenu code** from production builds entirely (use build flavors)
4. **Remove version-control-info.textproto** from release builds
5. **Apply string encryption** for sensitive constants (API endpoints, error messages that reveal architecture)
6. **Consider control flow obfuscation** for security-critical paths (login, 2FA, SSL validation)
7. **Audit @Keep annotations** — 67 keeps is excessive; many appear to be legacy or overly broad
8. **Remove .kotlin_module files** that reveal internal module structure
9. **Obfuscate the externcalls SDK** — 429 fully-readable classes expose the entire VoIP/calling API
