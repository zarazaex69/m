# Vuln-70: App Initialization & Startup Lifecycle Analysis

## Summary

Analysis of Max messenger (package: `ru.oneme.app`) application initialization for injection opportunities during early lifecycle. The app uses a multi-provider ContentProvider initialization pattern with a custom tracer framework that loads classes dynamically from manifest metadata.

## Application Class

**File:** `one/p011me/android/OneMeApplication.java`  
**Extends:** `android.app.Application` implements `bb4, kdi, fy7, gy7`

### Initialization Order (ContentProviders run before Application.onCreate):

1. `ru.p027ok.android.commons.app.ApplicationProvider` — stores Application reference in static field
2. `androidx.startup.InitializationProvider` (authority: `ru.oneme.app.androidx-startup`) — loads `ProcessLifecycleInitializer`
3. `ru.p027ok.tracer.startup.InitializationProvider` (authority: `ru.oneme.app.tracer-startup`) — loads tracer initializers from manifest metadata
4. `com.google.firebase.provider.FirebaseInitProvider` — Firebase init (disabled by default in manifest)
5. `ru.trace_flow.dps.internal.DpsInitProvider` — DPS analytics (disabled by default in manifest)
6. `OneMeApplication.attachBaseContext()` — locale, watchdog config, AccountInitializer
7. `OneMeApplication.onCreate()` — AccountInitializer completion, multi-account setup

## Finding 1: Tracer Startup Loads Classes from Manifest Metadata

**Severity:** Medium (requires APK modification or rooted device)

The `ru.p027ok.tracer.startup.InitializationProvider.onCreate()` calls `kti.m12738a(context)` which:

1. Reads `ApplicationInfo.metaData` from the manifest
2. Iterates all keys matching prefix `ru.ok.tracer.startup.Initializer@`
3. Calls `Class.forName(string)` on the metadata value
4. Instantiates the class via `cls.newInstance()`
5. Calls `mo14186b(context)` (the initializer's init method)

**Manifest entries loaded:**
- `ru.ok.tracer.utils.LoggerInitializer`
- `ru.ok.tracer.TracerInitializer`
- `ru.ok.tracer.disk.usage.DiskUsageInitializer`
- `ru.ok.tracer.heap.dumps.HeapDumpInitializer`
- `ru.ok.tracer.nativebridge.NativeBridgeInitializer`
- `ru.ok.tracer.crash.report.CrashReportInitializer`
- `ru.ok.tracer.performance.metrics.PerformanceMetricsInitializer`

**Attack vector:** If an attacker can modify the APK's manifest metadata (repackaging attack) or inject a class into the classpath, they can add a new `ru.ok.tracer.startup.Initializer@<uuid>` entry pointing to their malicious class implementing `ml8` interface. This class would be instantiated and executed before `Application.onCreate()`.

## Finding 2: Native Library Loading During Init

**Severity:** Low-Medium (requires library path manipulation)

`NativeBridgeInstaller` calls `System.loadLibrary("tracernative")` in a static initializer. On rooted devices, the library search path could be manipulated to load a malicious `libtracernative.so` before the legitimate one.

**File:** `ru/p027ok/tracer/nativebridge/NativeBridgeInstaller.java`

## Finding 3: Locale File-Based Configuration in attachBaseContext

**Severity:** Low

In `OneMeApplication.attachBaseContext()`:
```java
fileArrListFiles = context.getFilesDir().listFiles(new dj9(0));
// Reads files matching "locale_*" from internal storage
String strM15465F0 = o0i.m15465F0(name, "locale_");
```

This reads from **internal** storage (`getFilesDir()`), not external. Not directly injectable without root access. However, the locale string is used to create a localized context — a malformed locale value could potentially cause unexpected behavior.

## Finding 4: SharedPreferences Read During Init

**Severity:** Low

`attachBaseContext()` reads from `user.prefs`:
- `user.lang` (default: "ru") — language setting
- `app.lang.multilang` — multi-language flag

These are in internal storage SharedPreferences. Not injectable without root or backup restore attack.

## Finding 5: No External Storage Reads During Initialization

**Confirmed:** No initialization code reads from external storage (`getExternalFilesDir`, `Environment.getExternalStorageDirectory`, etc.) during the startup sequence. The only external storage reference in tracer code is in `DiskUsageWorker` (post-init worker, not startup).

## Finding 6: Intent Extras in MainActivity

**Severity:** Low

`MainActivity.m16127U()` reads `intent.getStringExtra("push_action")` and checks for value `"push_action_open_chat"`. This is post-initialization and only affects navigation routing, not code execution. The intent is validated against known action strings.

## Finding 7: DpsInitProvider Uses Obfuscated Manifest Keys

**Severity:** Low (provider is disabled)

`DpsInitProvider` reads obfuscated keys from manifest metadata via `z8f.m26706a()` (string deobfuscation). It's marked `android:enabled="false"` in the manifest, so it doesn't run by default. If enabled, it reads configuration strings from metadata and initializes a DPS SDK with user ID and version callbacks.

## Finding 8: No First-Run Flow Manipulation

No dedicated "first run" detection was found in the initialization code. The app uses `AccountInitializer` which manages account state, but there's no file-based or preference-based "first launch" flag that could be pre-seeded before installation.

## Finding 9: androidx.startup InitializationProvider

The standard `androidx.startup.InitializationProvider` loads initializers from its own provider's `metaData` bundle. Only `ProcessLifecycleInitializer` is registered. This follows the standard pattern and doesn't load external code.

## Attack Surface Summary

| Vector | Feasibility | Impact |
|--------|-------------|--------|
| Manifest metadata injection (tracer classes) | Requires APK repackaging | Code execution before onCreate |
| Native library path hijacking | Requires root | Full code execution |
| Locale file manipulation | Requires root/backup | Context misconfiguration |
| SharedPreferences pre-seeding | Requires root/backup | Language change only |
| Intent extras to MainActivity | Any app can send | Navigation only, no code exec |
| DpsInitProvider activation | Requires APK modification | Analytics SDK init |

## Conclusion

The most significant finding is the tracer startup framework's dynamic class loading from manifest metadata (Finding 1). While all providers are marked `android:exported="false"`, the pattern of reading class names from metadata and instantiating them via reflection creates a code injection point for APK repackaging attacks. The app does NOT read from external storage during initialization, eliminating pre-first-run file injection as an attack vector.
