# VULN-30: DevNull PmsKey & Informer Banner System

## Summary

Max messenger contains two server-controlled systems: **DevNull** (a statistics sink controlled by server config) and **Informer** (a server-push banner system that can display arbitrary messages and open arbitrary URLs on user devices).

---

## 1. DevNull System

### PmsKey Registration
- **PmsKey**: `devnull` (ordinal 174)
- **Source**: `ru/ok/tamtam/android/prefs/PmsKey.java:544`
- **Config class**: `ru.ok.tamtam.models.pms.DevNullServerConfig` (decompiled as `p000/mm5.java`)

### What DevNullServerConfig Configures

```java
// mm5.java - DevNullServerConfig
public final class mm5 {
    public final Map f40810a;  // events map: Map<String, Boolean>

    public final boolean m14194a(String str) {
        return ((Boolean) this.f40810a.getOrDefault(str, Boolean.FALSE)).booleanValue();
    }

    public final String toString() {
        return "DevNullServerConfig(events=" + this.f40810a + ")";
    }
}
```

**Purpose**: DevNullServerConfig is a server-controlled feature flag map that determines which statistics/telemetry events are **discarded** (sent to /dev/null). The server sends a JSON map of event names → boolean values. When an event is checked via `m14194a(eventName)`, if the value is `true`, the event data is effectively silenced/discarded.

**Controlled events** (from `mm5.f40808c` properties):
- `isAbEventEnabled` - A/B test events
- `isOpcodeStatEnabled` - Protocol opcode statistics
- `isChatHistoryStatEnabled` - Chat history statistics
- `isUploadHangCheckEnabled` - Upload hang detection
- `isUploadErrorEventEnabled` - Upload error events
- `isMemoryStatEnabled` - Memory statistics
- `isBatteryStatEnabled` - Battery statistics
- `isVideoTranscodeSizeRegressionEnabled` - Video transcode regression stats

**Security implication**: The server can selectively disable client-side telemetry/monitoring, potentially hiding malicious activity from detection systems.

### Module Registration
- `one.me.statistics.devnull.DevNull` (component 0)
- `one.me.statistics.devnull.DevNullStatsDependenciesProvider` (component 1)

---

## 2. Informer Banner System

### PmsKey Flags
| PmsKey | Ordinal | Purpose |
|--------|---------|---------|
| `informer-enabled` | 185 | Master toggle for informer banners |
| `informer-divider-can-hidden` | 312 | Controls if divider above banner can be hidden |
| `informer-icon-themed` | 313 | Controls if banner icon uses app theme |

### Database Schema

```sql
CREATE TABLE IF NOT EXISTS informer_banner (
    id TEXT NOT NULL,
    title TEXT NOT NULL,
    settings INTEGER NOT NULL DEFAULT 0,
    description TEXT,
    priority INTEGER NOT NULL,
    repeat INTEGER NOT NULL,
    rerun INTEGER NOT NULL,
    animoji_id INTEGER,
    url TEXT,
    type INTEGER NOT NULL,
    click_time INTEGER NOT NULL DEFAULT 0,
    show_time INTEGER NOT NULL DEFAULT 0,
    close_time INTEGER NOT NULL DEFAULT 0,
    show_count INTEGER NOT NULL DEFAULT 0,
    PRIMARY KEY(id)
)
```

### What the Server Can Push

The informer system allows the server to push **arbitrary banners** to users containing:

1. **Arbitrary title text** (`title TEXT NOT NULL`) - displayed as banner heading
2. **Arbitrary description text** (`description TEXT`) - displayed as banner body
3. **Arbitrary URL** (`url TEXT`) - opened when banner is clicked
4. **Animated emoji/icon** (`animoji_id`) - loaded from Lottie URL
5. **Priority ordering** (`priority`) - controls display order
6. **Repeat/rerun logic** - controls how often banner reappears
7. **Settings bitmask** - controls visual behavior:
   - Bit 0: `useTextShimmer` - animated text effect
   - Bit 1: `hideCloseButton` - user CANNOT dismiss
   - Bit 2: `hideOnClick` - disappears after click
   - Bit 3: `informerIconThemed` - themed icon

### Banner Types

```java
// gk8.java - Type.SoftUpdate (opens URL on click)
public final class gk8 extends jk8 {
    public final String toString() { return "Type.SoftUpdate"; }
}

// ik8.java - Type.Unsupported (logged and skipped)
public final class ik8 extends jk8 {
    public final String toString() { return "Type.Unsupported(" + byte + ")"; }
}

// lk8.java - AppUpdate action (triggers app update flow)
public final class lk8 implements nk8 {
    public final String toString() { return "AppUpdate"; }
}
```

### URL Opening on Click (CRITICAL)

From `ni3.java` (click handler):
```java
// When banner is clicked:
ek8 ek8Var = tmdVarM16716g1.f67707P0;
if (ek8Var != null) {
    String str = ek8Var.f16051l;  // URL from banner
    if (str != null) {
        ek8Var.f16054o.mo3253h(new mk8(Uri.parse(str)));  // OpenLink event
    }
}
```

From `pnd.java` (link handler in PinBarsWidget):
```java
if (nk8Var instanceof mk8) {
    ynd.f83907c.m26079j0(((mk8) nk8Var).m14171a());  // Opens arbitrary URI
} else if (nk8Var instanceof lk8) {
    // Triggers app update flow
    ((C0250cu) pinBarsWidget.f49639K0.getValue()).m4733a(activity);
}
```

**The URL is stored only for `Type.SoftUpdate` banners** (from `ck8.java`):
```java
ek8Var.f16051l = kk8Var.f34235j instanceof gk8 ? kk8Var.f34234i : null;
```

### Statistics Tracking

From `rk8.java`:
```java
ws9Var.put("informer_id", str2);
ws9Var.put("informer_type", Byte.valueOf(b));
```

Events tracked: `informer_show` (when banner is displayed to user).

### Sync Mechanism

- Banners are synced from server periodically
- Sync timestamp stored in: `app.informer_banners.sync`
- Show duration stored in: `app.informer_banners.show_duration`
- DAO: `ru.ok.tamtam.android.informer.InformerBannerDao`
- Stats: `one.me.sdk.statistics.informer.InformerStats`

---

## 3. Security Analysis

### Can the Server Push Fake System Messages?

**YES** - The informer system can:
1. Display banners with **arbitrary title and description** text
2. Banners appear in the **PinBarsWidget** which is embedded in ChatScreen and ChatsTabWidget
3. The `hideCloseButton` setting (bit 1) can make banners **non-dismissable**
4. Banners can include animated icons (Lottie from URL) to mimic system UI
5. The `useTextShimmer` effect can make text appear "official"

### Can Informer Banners Open URLs?

**YES** - When type is `SoftUpdate` (gk8):
- The `url` field from the database is parsed as `Uri.parse(str)`
- Opened via `ynd.f83907c.m26079j0(uri)` - which opens the URI in the system
- **No URL validation or whitelist check** is visible in the click handler
- Any URI scheme could potentially be opened (http, https, intent://, market://, etc.)

### Can Informer Banners Execute Code?

**Indirectly YES**:
- Opens arbitrary URIs which could trigger intent:// scheme handlers
- Loads Lottie animations from arbitrary URLs (animoji_id → URL)
- The `AppUpdate` type triggers app update flow which could redirect to malicious APK source

### Attack Scenarios

1. **Phishing**: Server pushes banner with "Security Alert: Verify your account" + phishing URL
2. **Forced URL opening**: Non-dismissable banner (hideCloseButton=true) with hideOnClick=true forces user to click → opens malicious URL
3. **Fake system notification**: Banner mimics system message with official-looking animated icon
4. **Silent telemetry suppression**: DevNull config disables monitoring while attack is in progress
5. **Social engineering**: Repeated banners (repeat/rerun) with urgent messaging

---

## 4. Affected Components

| Component | File | Role |
|-----------|------|------|
| PmsKey.devnull | PmsKey.java:544 | Server config key |
| DevNullServerConfig | mm5.java | Telemetry suppression config |
| PmsKey.informer-enabled | PmsKey.java:575 | Feature flag |
| PmsKey.informer-divider-can-hidden | PmsKey.java:950 | UI flag |
| PmsKey.informer-icon-themed | PmsKey.java:953 | UI flag |
| InformerBannerEntity | kk8.java | Banner data model |
| InformerBanner (display) | oj8.java | Banner display model |
| Content (UI model) | ok8.java | What's shown to user |
| InformerBannerManager | ek8.java | Banner selection/display logic |
| Click handler | ni3.java | Opens URL on click |
| Link handler | pnd.java | Executes URI opening |
| OpenLink event | mk8.java | URI wrapper |
| AppUpdate action | lk8.java | App update trigger |
| Banner DAO | InformerBannerDao | Database operations |
| PinBarsWidget | PinBarsWidget.java | UI container |

---

## 5. Risk Assessment

| Factor | Rating |
|--------|--------|
| Server-controlled content | **CRITICAL** - arbitrary text + URLs |
| No URL validation | **HIGH** - any URI scheme accepted |
| Non-dismissable banners | **HIGH** - forced user interaction |
| Telemetry suppression | **MEDIUM** - can hide attack evidence |
| Appears in chat UI | **HIGH** - trusted context |
| Combined attack potential | **CRITICAL** |

**Overall**: The combination of DevNull (suppress monitoring) + Informer (push fake messages with malicious URLs) creates a potent server-side attack vector against users. A compromised or malicious server operator can phish users, redirect them to malicious sites, or social-engineer them into dangerous actions — all while suppressing the telemetry that might detect the attack.
