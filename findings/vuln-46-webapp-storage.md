# VULN-46: WebApp (Mini-App) Data Storage and Isolation Analysis

## Summary

Max messenger implements a dual-layer webapp storage system with per-bot isolation using SharedPreferences. The "device storage" (ds) layer stores data in plaintext, while the "secure storage" (ss) layer encrypts values using AES-CBC with Android Keystore-backed keys. Biometry tokens are stored in an unencrypted SQLite table.

## Architecture

### Storage Types

1. **Device Storage (DS)** — class `yok` (implements `gpk` interface)
   - SharedPreferences file: `webapp_ds_{user_id}_{bot_id}`
   - Stores key-value pairs as **plaintext strings**
   - Limit: `webapp-ds-keys-count` PmsKey, default **100 keys**

2. **Secure Storage (SS)** — class `bpk` (implements `gpk` interface)
   - SharedPreferences file: `webapp_ss_{user_id}_{bot_id}`
   - Encryption key alias: `webapp_s_key_{user_id}_{bot_id}`
   - Values **encrypted with AES/CBC/PKCS7Padding** via Android Keystore
   - Limit: `webapp-ss-keys-count` PmsKey, default **10 keys**

3. **Biometry Storage** — SQLite table `webapp_biometry`
   - Schema: `(id, user_id, bot_id, token, access_requested, access_granted)`
   - Token stored as **plaintext TEXT** in the Room database
   - Indexed on `user_id` and `bot_id`

### Storage Dispatch (class `ijk`)

Operations dispatched via JS bridge delegates:
- `WebAppStorageSaveKeyRequest` → `gpk.mo2914a(key, value)`
- `WebAppStorageGetKeyRequest` → `gpk.get(key)`
- `WebAppStorageClearRequest` → `gpk.clear()`
- Remove operation → `gpk.remove(key)`

The `p70` class holds storage context:
- `f52478a` = bot_id (from `WebAppRootScreen.m17028q1()`)
- `f52479b` = user_id (from `ipk.f28403a`)
- Boolean flag selects DS (`yok`) vs SS (`bpk`)

## Findings

### 1. Per-Bot Isolation (POSITIVE)

Storage IS isolated per-bot. The SharedPreferences filename includes both `user_id` and `bot_id`:
```
webapp_ds_{user_id}_{bot_id}   // Device storage
webapp_ss_{user_id}_{bot_id}   // Secure storage
```

Each bot gets its own SharedPreferences file. The encryption key for SS is also per-bot:
```
webapp_s_key_{user_id}_{bot_id}
```

**One bot CANNOT access another bot's stored data** through the JS bridge API — the storage instance is bound to the bot_id at construction time in `bhk.java`.

### 2. Origin Validation

The `ijk.m10449K()` method validates the webapp origin (`f27777f1`) before allowing storage operations. If the origin doesn't match, the operation is rejected with an error response (`amk`/`zlk`).

### 3. Key Count Limits — What Happens When Exceeded

**Device Storage (yok.mo2914a):**
```java
if (!sharedPreferencesM26089b.contains(str) && sharedPreferencesM26089b.getAll().size() == this.f84024d) {
    return Boolean.FALSE;  // Silently rejects new keys
}
```

**Secure Storage (bpk.mo2914a):**
```java
if (!sharedPreferencesM2915b.contains(str) && sharedPreferencesM2915b.getAll().size() == this.f6343f) {
    return Boolean.FALSE;  // Silently rejects new keys
}
```

**Behavior:** When the limit is reached, new keys are **silently rejected** (returns `false`). Existing keys CAN still be updated. The limits are server-configurable via PMS:
- DS: default 100, configurable via `webapp-ds-keys-count`
- SS: default 10, configurable via `webapp-ss-keys-count`

### 4. Encryption at Rest

| Storage Layer | Encrypted | Method |
|---|---|---|
| Device Storage (DS) | ❌ NO | Plaintext SharedPreferences |
| Secure Storage (SS) | ✅ YES | AES/CBC/PKCS7Padding, Android Keystore |
| Biometry tokens | ❌ NO | Plaintext in SQLite |

**SS Encryption details (vbk class):**
- Key: AES generated in AndroidKeyStore with alias `webapp_s_key_{user_id}_{bot_id}`
- `setUserAuthenticationRequired(false)` — no biometric auth needed to decrypt
- `setRandomizedEncryptionRequired(true)` — random IV per encryption
- IV prepended to ciphertext, Base64 encoded for storage

### 5. Cloud Backup / Reinstall Survival

**AndroidManifest.xml:** `android:allowBackup="false"`

- Webapp data does **NOT survive app reinstall** via Android backup
- No cloud sync mechanism found for webapp storage (no server-side CloudStorage API)
- Data is purely local (SharedPreferences + SQLite)

### 6. Sensitive Data in Webapp Storage

**Biometry tokens** are stored in the `webapp_biometry` SQLite table:
- `token` field: plaintext TEXT, nullable
- Updated via: `UPDATE webapp_biometry SET token = ? WHERE user_id = ? AND bot_id = ?`
- This token is used for biometric authentication flows with bots

**No payment tokens or auth tokens** are stored in the webapp DS/SS storage itself — those are in separate app-level SharedPreferences (`auth.token`).

## Security Concerns

### LOW — DS Plaintext Storage
Device Storage values are stored as plaintext SharedPreferences. On rooted devices, any app with root access can read `/data/data/one.me/shared_prefs/webapp_ds_*`. However, this is by design (non-sensitive data).

### LOW — Biometry Token Plaintext in SQLite
The `webapp_biometry.token` field is stored unencrypted in the Room database. On rooted devices this is accessible. The token is per-bot isolated (keyed by `user_id + bot_id`).

### INFO — No Key/Value Size Validation
No maximum length validation found for individual key names or values in the storage API. A malicious webapp could store large values (limited only by SharedPreferences/disk constraints). The only limit is key count (100 DS / 10 SS).

### INFO — Server-Configurable Limits
The key count limits are fetched from PMS (server config). A compromised server could raise limits to allow excessive local storage consumption.

## Key Source Files

| File | Role |
|---|---|
| `p000/yok.java` | Device Storage implementation (plaintext) |
| `p000/bpk.java` | Secure Storage implementation (encrypted) |
| `p000/vbk.java` | AES encryption/decryption with Android Keystore |
| `p000/gpk.java` | Storage interface (save/get/remove/clear) |
| `p000/ijk.java` | JS bridge storage dispatcher + origin validation |
| `p000/p70.java` | Storage context holder (bot_id, user_id) |
| `p000/bhk.java` | Storage construction with bot_id binding |
| `p000/e8k.java` | WebAppBiometryEntity data class |
| `p000/geb.java` | Database migration creating webapp_biometry table |
| `ru/p027ok/tamtam/android/prefs/PmsKey.java` | PMS key definitions |

## Conclusion

The webapp storage implementation has **proper per-bot isolation** — each bot's data is stored in a separate SharedPreferences file keyed by `{user_id}_{bot_id}`. Cross-bot data access is not possible through the JS bridge API. The secure storage layer provides AES encryption via Android Keystore. The main weakness is that biometry tokens and device storage values are stored in plaintext, accessible on rooted devices.
