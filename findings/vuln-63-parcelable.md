# VULN-63: Parcelable/Bundle Deserialization Analysis

## Summary

Analysis of Parcelable/Bundle deserialization patterns in Max messenger for type confusion, unsafe deserialization, and potential code execution vectors.

## Severity: MEDIUM (ObjectInputStream without whitelist) / LOW (Parcelable patterns)

---

## 1. Exported Components Receiving External Intents

### Exported Activities:
- **`one.p011me.android.MainActivity`** — exported, handles `SEND`, `SEND_MULTIPLE`, `VIEW`
- **`one.p011me.android.deeplink.LinkInterceptorActivity`** — exported, handles `VIEW` with deep links

### Exported Services/Receivers:
- `one.p011me.webapp.util.WebAppNfcService` — exported, NFC HCE service
- `ru.p027ok.tamtam.android.services.BootCompletedReceiver` — exported, boot receiver
- `one.p011me.background.wake.BackgroundWakeBootReceiver` — exported, boot receiver

---

## 2. LinkInterceptorActivity — Bundle Handling

**File:** `one/p011me/android/deeplink/LinkInterceptorActivity.java`

LinkInterceptorActivity does **NOT** directly deserialize Bundle extras from external intents. Its `onCreate()`:
1. Calls `ct4.m4617H0(getIntent())` — a **defensive unparcel check** (see below)
2. Reads only `intent.getAction()` and `intent.getData()` (URI)
3. Does not call `getExtras()`, `getParcelableExtra()`, or `getSerializableExtra()`

### Defensive Pattern in `ct4.m4617H0()`:
```java
public static final void m4617H0(Intent intent) {
    Bundle extras;
    if (intent == null || (extras = intent.getExtras()) == null) return;
    try {
        extras.size(); // Forces unparcel to detect bad data early
    } catch (BadParcelableException | RuntimeException e) {
        intent.replaceExtras(Bundle.EMPTY); // Sanitizes on failure
    }
}
```
This is a **mitigation** against malformed Parcelable attacks — it forces early unparceling and replaces extras on failure. However, it does NOT prevent type confusion if the Parcelable is well-formed but semantically wrong.

---

## 3. Parcelable CREATOR Analysis

### `spf` — Multi-type Parcelable.Creator (switch-based factory)
**File:** `p000/spf.java`

A single Creator handles 29+ different Parcelable types via a switch on an integer discriminator. Key observations:
- Case 2: `readParcelable(odg.class.getClassLoader())` for `ForegroundColorSpan`/`BackgroundColorSpan`
- Case 3: Chains `yh9.CREATOR.createFromParcel(parcel)` then multiple `readParcelable()` calls
- Case 12 (ShareData): Reads nested Parcelable lists with `readParcelable(ShareData.class.getClassLoader())`
- Case 20: Reads `readArrayList(omh.class.getClassLoader())` — uses raw ArrayList deserialization

**Risk:** The multi-type factory pattern is complex but uses proper ClassLoaders. No dangerous operations (file I/O, reflection, network) during deserialization.

### `C0405h0` — ClassLoaderCreator
**File:** `p000/C0405h0.java`

- Case 0: `readParcelable(null)` — **uses null ClassLoader**, which defaults to the boot classloader. This limits the types that can be deserialized but is a known anti-pattern.
- Default case: `n1kVar.f42310c = parcel.readParcelable(null)` — stores arbitrary Parcelable with null classloader.

**Risk:** Using `null` ClassLoader in `readParcelable()` is a minor issue — it prevents loading app-specific classes but is not directly exploitable.

---

## 4. Bundle Mismatch Vulnerability (CVE-2017-13288 Pattern)

### `n1k` class analysis:
**writeToParcel:**
```java
super.writeToParcel(parcel, i);
parcel.writeInt(this.f42308a);
parcel.writeInt(this.f42309b);
parcel.writeParcelable(this.f42310c, i);
```

**createFromParcel (in C0405h0, case 10):**
```java
n1k n1kVar = new n1k(parcel, null);  // reads super state
n1kVar.f42308a = parcel.readInt();
n1kVar.f42309b = parcel.readInt();
n1kVar.f42310c = parcel.readParcelable(null);
```

The read/write order is **consistent** (int, int, Parcelable). No size mismatch detected. The CVE-2017-13288 pattern (where `writeToParcel` writes more/fewer bytes than `createFromParcel` reads) is **not present** in the examined classes.

---

## 5. ObjectInputStream Without Class Whitelist (HIGH RISK)

### `AbstractC0432hr.m9702C()` — SharedPreferences Deserialization
**File:** `p000/AbstractC0432hr.java`

```java
public Serializable m9702C(String str) {
    String string = ((SharedPreferences) this.f25188b).getString(str, null);
    if (string != null) {
        ByteArrayInputStream bais = new ByteArrayInputStream(Base64.decode(string, 0));
        ObjectInputStream ois = new ObjectInputStream(bais);
        Serializable serializable = (Serializable) ois.readObject(); // NO WHITELIST
        ...
    }
}
```

**No class whitelist.** No `resolveClass()` override. No `ObjectInputFilter`. Deserializes arbitrary `Serializable` objects from Base64-encoded SharedPreferences values.

**Attack scenario:** If an attacker can write to SharedPreferences (via backup restore, content provider, or path traversal), they can inject a malicious serialized object that triggers code execution via gadget chains during `readObject()`.

### `z8f.m26691L()` — File-based Deserialization
**File:** `p000/z8f.java`

```java
ObjectInputStream objectInputStream = new ObjectInputStream(new FileInputStream(file));
Object object = objectInputStream.readObject(); // NO WHITELIST
```

Reads arbitrary objects from files. No class filtering.

### `y45.m25673a()` — Byte Array Deserialization
**File:** `p000/y45.java`

```java
ObjectInputStream objectInputStream = new ObjectInputStream(byteArrayInputStream);
for (int i = objectInputStream.readInt(); i > 0; i--) {
    map.put(objectInputStream.readUTF(), objectInputStream.readObject()); // NO WHITELIST
}
```

Has a 10KB size limit but **no class whitelist**. Deserializes arbitrary objects into a HashMap.

### `mdb` — Stream-based Deserialization
**File:** `p000/mdb.java`

Reads objects from an InputStream with `readObject()` — no filtering.

### `ilb.m10521f()` — Byte Array to LinkedHashSet
**File:** `p000/ilb.java`

Uses `ObjectInputStream` but only calls `readUTF()` and `readBoolean()` — **safe** (no `readObject()`).

---

## 6. Serialized Objects from SharedPreferences Without Type Checking

The `AbstractC0432hr.m9702C()` method:
- Reads a Base64 string from SharedPreferences
- Decodes and deserializes it via `ObjectInputStream.readObject()`
- Casts result to `Serializable` (no specific type check)
- **No validation of the deserialized class type**

The companion method `m9716V()` serializes objects TO SharedPreferences:
```java
objectOutputStream.writeObject(serializable); // Writes any Serializable
```

---

## 7. Global Search Results Confirmed

No usage of:
- `resolveClass()` override
- `ObjectInputFilter` / `setObjectInputFilter()`
- `ValidatingObjectInputStream`
- Any custom deserialization filtering

**Conclusion:** All `ObjectInputStream` usage in the app is unfiltered.

---

## Risk Assessment

| Finding | Severity | Exploitability |
|---------|----------|----------------|
| ObjectInputStream without whitelist (SharedPrefs) | **HIGH** | Requires write access to SharedPreferences |
| ObjectInputStream without whitelist (files) | **HIGH** | Requires write access to app files |
| ObjectInputStream without whitelist (byte arrays) | **MEDIUM** | Depends on data source |
| readParcelable(null) in C0405h0 | **LOW** | Limited by boot classloader |
| LinkInterceptorActivity Bundle handling | **LOW** | Defensive check present, no extras deserialized |
| Multi-type CREATOR factory (spf) | **INFO** | Complex but no dangerous ops |

---

## Exploitation Conditions

1. **ObjectInputStream attacks** require the attacker to control the serialized data source:
   - SharedPreferences: Possible via `android:allowBackup="true"`, ADB backup/restore, or content provider vulnerabilities
   - Files: Possible via path traversal or symlink attacks
   - Byte arrays: Depends on where `y45` data originates

2. **Gadget chains** needed: The app includes many libraries (WebRTC, protobuf, OkHttp, etc.) that may contain usable deserialization gadgets.

3. **Parcelable attacks** are largely mitigated by the `ct4.m4617H0()` defensive pattern and the fact that exported activities don't deeply deserialize Bundle extras.

---

## Recommendations

1. Replace all `ObjectInputStream.readObject()` with a filtered variant using `ObjectInputFilter` (API 26+) or a custom `resolveClass()` whitelist
2. Migrate SharedPreferences serialization from Java serialization to JSON/protobuf
3. Add type-specific deserialization (e.g., expect only specific classes from SharedPreferences)
4. Audit `android:allowBackup` setting — if true, SharedPreferences can be tampered via ADB backup
