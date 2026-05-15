# Vulnerability 04: Path Traversal & Arbitrary File Write

## CRITICAL: Zip-Slip Vulnerability in ZIP Extraction

**File:** `p000/kv6.java` method `m12804i()`

The ZIP extraction code uses `ZipEntry.getName()` directly in a `File` constructor without sanitizing `../` sequences. This is a classic Zip-Slip vulnerability allowing arbitrary file overwrite.

```java
public static ArrayList m12804i(File file, File file2) {
    ArrayList arrayList = new ArrayList();
    if (!file.exists()) {
        file.mkdirs();
    }
    ZipFile zipFile = new ZipFile(file2);
    try {
        Enumeration<? extends ZipEntry> enumerationEntries = zipFile.entries();
        while (enumerationEntries.hasMoreElements()) {
            ZipEntry zipEntryNextElement = enumerationEntries.nextElement();
            if (!zipEntryNextElement.isDirectory()) {
                // VULN: getName() used directly - no sanitization of "../"
                File file3 = new File(file, zipEntryNextElement.getName());
                File parentFile = file3.getParentFile();
                if (parentFile != null && (parentFile.exists() || parentFile.mkdirs())) {
                    FileOutputStream fileOutputStream = new FileOutputStream(file3);
                    InputStream inputStream = zipFile.getInputStream(zipEntryNextElement);
                    // ... writes content to file3
                }
            }
        }
    }
}
```

**Impact:** A malicious ZIP file with entries like `../../shared_prefs/config.xml` can overwrite arbitrary files within the app's writable directories. If the app processes ZIP files from server responses or attachments, an attacker controlling the server or performing MITM can achieve code execution.

**Missing check:** No `getCanonicalPath()` validation that the resolved path stays within the target directory.

---

## HIGH: File Download with Server-Controlled Filename (Content-Disposition)

**File:** `p000/tlc.java` method `m22012d()` and `m22007e()`

The download manager extracts file extension from `Content-Disposition` header and appends it to the output filename:

```java
// m22007e - extracts extension from Content-Disposition
public static String m22007e(rqf rqfVar) {
    String strM20097G = rqf.m20097G(rqfVar, "Content-Disposition");
    // Pattern: .*filename=".*\\.(\w+)".*
    Matcher matcher = f67628h.matcher(strM20097G);
    if (matcher.matches()) {
        return matcher.group(1);  // Returns extension from server
    }
    return null;
}

// m22012d - constructs output file path
public final File m22012d(File file, File file2, String str) {
    if (str != null && str.length() != 0) {
        String name = file2.getName();
        int iM15497z0 = o0i.m15497z0(name, '.', 0, 6);
        if (iM15497z0 >= 0) {
            name = name.substring(0, iM15497z0);
        }
        // Server-provided extension appended directly
        file2 = new File(file2.getParentFile(), name + "." + str);
    }
    File parentFile = file2.getParentFile();
    File fileM14434l = parentFile != null ? mv6.m14434l(parentFile, file2.getName()) : null;
    // ... moves temp file to final location
}
```

**Mitigation note:** The regex `\w+` limits the extension to word characters only, preventing path traversal in the extension itself. However, the base filename (`file2.getName()`) comes from the download request parameters which may be server-controlled.

---

## HIGH: WebApp File Download with Bot-Provided Filename

**File:** `ru/p027ok/tamtam/upload/workers/DownloadFileFromWebAppWorker.java`

The worker receives `fileName` directly from WorkerParameters (originating from a WebApp/bot request):

```java
y45 y45Var = this.f61461b.f5397b.f3078b;
long jM25677d = y45Var.m25677d("requestId", 0L);
long jM25677d2 = y45Var.m25677d("botId", 0L);
String strM25678e = y45Var.m25678e("fileName");  // Bot-controlled filename
String str = strM25678e == null ? "" : strM25678e;
String strM25678e2 = y45Var.m25678e("fileUrl");   // Bot-controlled URL
return new rgi(jM25677d, jM25677d2, strM25678e2, str);
```

**Impact:** A malicious bot/WebApp can supply a filename containing `../` sequences. If this filename is used directly in file path construction without sanitization, it enables path traversal to write files outside the intended download directory.

---

## MEDIUM: DownloadService Downloads to Caller-Specified Path

**File:** `ru/p027ok/android/externcalls/sdk/net/DownloadService.java`

The download service accepts a URL and destination `File` object directly:

```java
public interface DownloadService {
    neh download(String url, File dest, FileValidationConfig fileValidationConfig);
}

// Implementation in Impl class:
private final void downloadInner(String url, File destination, MessageDigest md) {
    URLConnection uRLConnectionOpenConnection = new URL(url).openConnection();
    InputStream inputStream = uRLConnectionOpenConnection.getInputStream();
    FileOutputStream fileOutputStream = new FileOutputStream(destination);  // Direct write
    kv6.m12803h(inputStream, fileOutputStream, jfjVar);
}
```

The only validation is `Patterns.WEB_URL.matcher(str).matches()` on the URL. The destination path has no validation — callers control where files are written. Parent directories are created via `lzl.m13850b(file)`:

```java
public static final void m13850b(File file) throws IOException {
    File parentFile = file.getParentFile();
    parentFile.mkdirs();  // Creates arbitrary directory structure
}
```

---

## MEDIUM: FileProvider openFile() - Properly Secured

**File:** `androidx/core/content/FileProvider.java` → `p000/zs6.java`

The FileProvider's `openFile()` delegates to `zs6.m27310b()` which **does** validate paths:

```java
public final File m27310b(Uri uri) {
    String strDecode2 = Uri.decode(encodedPath.substring(iIndexOf + 1));
    File file = (File) this.f87953b.get(strDecode);
    File file2 = new File(file, strDecode2);
    File canonicalFile = file2.getCanonicalFile();
    String path = canonicalFile.getPath();
    String path2 = file.getPath();
    // Path traversal check using canonical path
    if (FileProvider.m1169a(path).startsWith(FileProvider.m1169a(path2) + '/')) {
        return canonicalFile;
    }
    throw new SecurityException("Resolved path jumped beyond configured root");
}
```

**Status:** SECURE — uses `getCanonicalFile()` and validates the resolved path stays within the configured root.

---

## MEDIUM: NotificationsImagesProvider - Partial Validation

**File:** `one/p011me/android/notifications/NotificationsImagesProvider.java`

The `openFile()` implementation has some security checks:

```java
public final ParcelFileDescriptor openFile(Uri uri, String str) {
    if (!"r".equals(str)) {
        throw new SecurityException("Only read mode is supported");
    }
    // UriMatcher validates pattern: "message_image/*/*"
    if (f47916a.match(uri) != 1 || ...) { return null; }
    
    String string = pathSegments2.get(1);  // Image URI from path
    Uri uriM22145J = trb.m22145J(string);
    
    // Internal URI check
    if (jel.m11375k(getContext(), uriM22145J)) {
        throw new SecurityException("Internal uri detected");
    }
    // ... loads from disk cache
}
```

**Status:** Partially secure — read-only mode enforced, internal URI check present. However, the image URI from path segments is used to look up cached files. If the cache lookup (`m16167c`) maps URIs to filesystem paths without sanitization, traversal may be possible.

---

## MEDIUM: mkdirs with Potentially Controlled Paths

**File:** `p000/kv6.java` method `m12801f()`

```java
public static void m12801f(File file) throws IOException {
    if (!file.exists()) {
        if (file.mkdirs()) { return; }
        throw new IOException("Cannot create directory " + file.getAbsolutePath());
    }
}
```

Called from download flows where the destination path may be influenced by server data. Combined with the Zip-Slip vulnerability, arbitrary directories can be created.

---

## LOW: File Download Command with Server-Provided fileName

**File:** `p000/or6.java`

The `FileDownloadCmd` stores a `fileName` from server that flows into the download pipeline:

```java
fileDownloadCmd.fileName = this.f51112e;  // Server-provided filename
```

This fileName is passed to `jgi` constructor and eventually to the download manager (`tlc`), where it influences the output file path.

---

## Summary

| # | Vulnerability | Severity | File |
|---|---|---|---|
| 1 | Zip-Slip: unsanitized ZipEntry.getName() | CRITICAL | `p000/kv6.java` |
| 2 | Content-Disposition filename in downloads | HIGH | `p000/tlc.java` |
| 3 | WebApp bot-controlled filename | HIGH | `DownloadFileFromWebAppWorker.java` |
| 4 | DownloadService writes to caller path | MEDIUM | `DownloadService.java` |
| 5 | NotificationsImagesProvider partial validation | MEDIUM | `NotificationsImagesProvider.java` |
| 6 | mkdirs with controlled paths | MEDIUM | `p000/kv6.java` |
| 7 | Server fileName in download commands | LOW | `p000/or6.java` |

**FileProvider (zs6.java):** Properly secured with canonical path validation.

## Recommendations

1. **Zip-Slip fix:** Validate `new File(destDir, entry.getName()).getCanonicalPath()` starts with `destDir.getCanonicalPath()`.
2. **Download filenames:** Strip path separators and `..` from all server-provided filenames before constructing File objects.
3. **WebApp downloads:** Sanitize bot-provided `fileName` parameter — reject or strip directory traversal characters.
4. **DownloadService:** Add canonical path validation to ensure destination stays within allowed directories.
