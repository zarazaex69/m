# vuln-106: Account Takeover Vectors + Cleartext Mobile ID Auth

## Severity: HIGH-CRITICAL

## Finding 1: Cleartext Mobile ID Authentication (HIGH)

**File**: `res/xml/network_security_config.xml`

```xml
<domain-config cleartextTrafficPermitted="true">
    <domain includeSubdomains="false">mobileid.megafon.ru</domain>
    <domain includeSubdomains="false">idgw.mobileid.mts.ru</domain>
    <domain includeSubdomains="false">hhe.mts.ru</domain>
    <domain includeSubdomains="false">he-mc.tele2.ru</domain>
    <domain includeSubdomains="false">he-mc.t2.ru</domain>
    <domain includeSubdomains="false">balance.beeline.ru</domain>
</domain-config>
```

Mobile ID verification (carrier-based auth) uses **cleartext HTTP** to all major Russian carriers.
Any network observer (ISP, WiFi, MITM) can intercept the Mobile ID auth flow.

**JS Bridge trigger**: `WebAppVerifyMobileId` event from bot WebView sends request to carrier URL.
The URL comes from the bot (attacker-controlled): `unk.java` has `requestId` + `url` fields.

**Attack**: Malicious bot sends `WebAppVerifyMobileId` with attacker-controlled URL → app makes HTTP request → auth tokens leaked.

## Finding 2: ObjectInputStream Deserialization from SharedPreferences (CRITICAL)

**File**: `p000/AbstractC0432hr.java:47`

```java
public Serializable m9702C(String str) {
    String string = ((SharedPreferences) this.f25188b).getString(str, null);
    if (string != null) {
        ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(Base64.decode(string, 0));
        ObjectInputStream objectInputStream = new ObjectInputStream(byteArrayInputStream);
        Serializable serializable = (Serializable) objectInputStream.readObject();
        // NO CLASS FILTER - arbitrary deserialization!
    }
}
```

Java ObjectInputStream deserialization from SharedPreferences with NO class filtering.
If attacker can write to SharedPreferences (via content provider, backup, or root), this is **instant RCE** via gadget chains.

**Note**: `allowBackup="false"` mitigates backup vector, but root/ADB still works.

## Finding 3: File Deserialization (HIGH)

**File**: `p000/z8f.java:266`

```java
public static Object m26691L(File file) throws Throwable {
    FileInputStream fileInputStream = new FileInputStream(file);
    ObjectInputStream objectInputStream = new ObjectInputStream(fileInputStream);
    Object object = objectInputStream.readObject(); // No class filter!
}
```

Reads arbitrary serialized objects from files. If attacker can write to app's file directory, RCE.

## Finding 4: Session Storage (MEDIUM)

**File**: `p000/dw8.java:310`, `p000/lg0.java:44`

Session data structure:
- `uid` — user ID
- `session_key` — session identifier
- `session_secret_key` — used for API signing (MD5)
- `auth_token` — authentication token
- `auth_hash` — hash value
- `api_server` — server URL

Stored as JSON in `session_data` parameter. The `session_secret_key` is the API signing key.
Combined with MD5 signing (vuln-71), stealing this key = full account takeover.

**No device binding**: `device_id` is just a string identifier, not cryptographically bound.
Session can be replayed from any device if `session_key` + `session_secret_key` are known.

## Finding 5: Mutable PendingIntents (MEDIUM)

Multiple PendingIntents created with `FLAG_MUTABLE | FLAG_UPDATE_CURRENT` (0x0C000000 = 201326592):
- `ip1.java:47` — Activity PendingIntent
- `ip1.java:52` — Broadcast PendingIntent  
- `iyf.java:89` — Broadcast PendingIntent
- `soc.java:437` — Service PendingIntent (notifications)

Mutable PendingIntents can be modified by receiving app to change the target intent.

## Impact

- Mobile ID auth interceptable on any shared network
- Bot can trigger auth requests to attacker-controlled URLs
- ObjectInputStream RCE if SharedPrefs writable (root/ADB)
- Session replay from any device (no binding)
- Full account takeover with stolen session_key + session_secret_key
