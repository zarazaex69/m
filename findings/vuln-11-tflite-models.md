# VULN-11: TFLite Model Loading — Model Replacement Attack Surface

## Summary

Max messenger uses TensorFlow Lite models for audio processing during calls (keyword spotting and noise suppression). Models are **downloaded from a remote server** at runtime and stored in the app's private directory. While MD5 checksum verification exists, the use of MD5 (a broken hash algorithm) and the server-controlled config create a viable model replacement attack surface.

## Severity: Medium

## ML Model Purposes

Two TFLite model types identified in the calls SDK (`calls-sdk_release`):

| Feature Type | Delegate Class | Purpose | Version |
|---|---|---|---|
| **WS** (Word Spotter) | `KwsFeatureDelegate` | Keyword spotting / wake word detection during calls | `ws_0` |
| **NS** (Noise Suppression) | `NSFeatureDelegate` | Audio noise suppression during calls | `ns_1` |

These are **NOT** content moderation models. They are audio processing models for the VoIP calling feature. A compromised model could:
- Disable keyword detection (privacy implications)
- Degrade call quality (noise suppression bypass)
- Potentially exfiltrate audio patterns via crafted model outputs

## Model Loading Architecture

### 1. Download Mechanism

**Source:** `MLFeatureDelegate.java` → `downloadModel()`

```java
// Models downloaded from server-provided URL
cgh cghVarDownload = this.downloadService.download(
    config.getUrl(),    // URL from remote config
    file,              // destination file
    new FileValidationConfig(config.getChecksum(), hy7.MD5)  // MD5 validation
);
```

**Config source:** `MLFeatureConfigProviderBase` fetches JSON from `RemoteSettings`:
```java
// Remote config keys: "android.mlfeatures.%s"
JSONObject jSONObject = new JSONObject(config);
return new MLFeatureConfig(
    jSONObject.getString("url"),       // Download URL
    jSONObject.getString("cs"),        // Checksum
    JsonExtKt.getBooleanOrDefault(jSONObject, "use", false)  // Enabled flag
);
```

### 2. Storage Location

```java
private final String downloadDir() {
    return this.context.getFilesDir() + "/ml_features/" + this.type.getSubDirName();
}
```

**Storage path:** `getFilesDir()/ml_features/{ws|ns}/`

✅ Uses `getFilesDir()` (app-private, mode 0700) — NOT external storage.

### 3. Integrity Verification

**Download validation (`DownloadService.Impl`):**
```java
MessageDigest messageDigest = MessageDigest.getInstance(
    fileValidationConfig.getHashAlgorithm().f25958a  // "MD5"
);
// ... downloads and computes hash ...
if (!sb.toString().equals(fileValidationConfig.getExpectedChecksum())) {
    throw new RuntimeException("Downloaded model is corrupted");
}
```

**Post-download validation (`isModelValid`):**
```java
public boolean isModelValid(File modelDir) {
    // Only checks:
    // 1. Directory exists
    // 2. Files meet minimum size
    // 3. Required extensions present (.tflite, .cfg)
    // NO cryptographic verification of model content!
}
```

### 4. Model Update Flow

```
Server Config → fetchConfig() → validateCurrentModel() → downloadModel() → unzipModel() → saveNewModelInfo()
```

The entire update is server-driven. If the server is compromised or MITM'd, a malicious model + matching MD5 can be pushed.

## TFLite Runtime

- **No bundled `libtensorflowlite_jni.so`** found in the APK native libs
- TFLite loaded via **Google Play Services Dynamite** module (`com.google.android.gms.tflite_dynamite`)
- Also references `System.loadLibrary("tensorflowlite")` via `qrb.TENSORFLOW` enum with SoLoader fallback
- Barcode scanning uses ML Kit with tflite_dynamite backend
- Face detection/mesh logging present (Google ML Kit, not custom models)

## Vulnerabilities

### VULN-11a: MD5 Checksum for Model Integrity (Medium)

**Issue:** Model downloads are validated using MD5 (`hy7.MD5`), which is cryptographically broken. MD5 collision attacks are practical — an attacker who can MITM the download or compromise the config server can craft a malicious model with a matching MD5 hash.

**Evidence:**
```java
new FileValidationConfig(config.getChecksum(), hy7.MD5)
```

**Impact:** Model replacement with a crafted adversarial model that passes MD5 verification.

### VULN-11b: Server-Controlled Model URL Without Certificate Pinning Verification (Medium)

**Issue:** The model download URL comes entirely from remote server config. If the config endpoint lacks certificate pinning or the server is compromised, an attacker controls both the URL and the expected checksum.

**Evidence:**
```java
// Both URL and checksum come from same server config
new MLFeatureConfig(url, checksum, enabled)
```

**Impact:** Complete model replacement — attacker provides malicious URL + its MD5 hash.

### VULN-11c: No Post-Load Model Integrity Check (Low)

**Issue:** After initial download, `isModelValid()` only checks file existence, size, and extension — no re-verification of the hash. A model file modified on disk after download would still be loaded.

**Evidence:**
```java
// isModelValid only checks:
// - modelDir.exists() && modelDir.isDirectory()
// - file.length() >= this.modelSpec.getMinFileSize()
// - extensions contain "tflite" and "cfg"
```

### VULN-11d: Hash Algorithm Enum Supports SHA-256 But Not Used (Informational)

The `hy7` enum defines `MD5`, `SHA1`, and `SHA256`, but the ML model download hardcodes MD5:
```java
public enum hy7 {
    MD5("MD5"),
    SHA1("SHA-1"),
    SHA256("SHA-256");
}
```

SHA-256 is available but not utilized for model validation.

## Attack Scenarios

1. **MITM on model download:** Intercept the model ZIP download and replace with adversarial model + matching MD5
2. **Config server compromise:** Push malicious config with attacker-controlled URL and pre-computed MD5
3. **Local file replacement (root):** Replace model files in `getFilesDir()/ml_features/` — passes `isModelValid()` check since only extension/size is verified

## Mitigations Present

- ✅ Models stored in app-private directory (`getFilesDir()`)
- ✅ Download checksum verification exists (though MD5 is weak)
- ✅ URL validation via `Patterns.WEB_URL` matcher
- ✅ Models are for audio processing only (not content moderation/safety)

## Recommendations

1. **Replace MD5 with SHA-256** for model integrity verification
2. **Implement certificate pinning** on model download endpoints
3. **Add code-signing/signature verification** for model files (asymmetric crypto)
4. **Re-verify model hash on load**, not just on download
5. **Pin expected model versions** in the app binary rather than relying solely on server config

## Files Analyzed

- `ru/p027ok/android/externcalls/sdk/p030ml/delegate/MLFeatureDelegate.java`
- `ru/p027ok/android/externcalls/sdk/p030ml/delegate/KwsFeatureDelegate.java`
- `ru/p027ok/android/externcalls/sdk/p030ml/delegate/NSFeatureDelegate.java`
- `ru/p027ok/android/externcalls/sdk/net/DownloadService.java`
- `ru/p027ok/android/externcalls/sdk/net/FileValidationConfig.java`
- `ru/p027ok/android/externcalls/sdk/p030ml/config/MLFeatureConfig.java`
- `ru/p027ok/android/externcalls/sdk/p030ml/config/MLFeatureConfigProviderBase.java`
- `ru/p027ok/android/externcalls/sdk/p030ml/model/MLFeatureType.java`
- `ru/p027ok/android/externcalls/sdk/p030ml/model/ModelSpec.java`
- `ru/p027ok/android/externcalls/sdk/api/RemoteSettings.java`
- `p000/qrb.java` (native lib enum)
- `p000/rrb.java` (native lib loader)
- `p000/hy7.java` (hash algorithm enum)
- `p000/gul.java` (ML Kit barcode scanner)
