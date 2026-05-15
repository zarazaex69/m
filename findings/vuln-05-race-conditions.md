# Race Conditions & TOCTOU Vulnerabilities - Max Messenger

## 1. Token Refresh Race Condition

**Files:** `p000/sb2.java`, `p000/e2m.java`

The token refresh logic in `sb2.java` checks token expiry (`m25806j() < m25813q()`) then triggers an async coroutine refresh (`bgcVar.m2542a()`). No mutex or atomic flag prevents multiple threads from simultaneously detecting an expired token and triggering parallel refresh requests.

```java
// sb2.java - m21133a()
long jM25806j = ((yag)((vw3) v49Var.getValue())).m25806j(); // get current time
long jM25813q = ((yag)((vw3) v49Var.getValue())).m25813q(); // get expiry
if (jM25806j < jM25813q) {
    // trigger refresh - NO LOCK preventing concurrent refresh
    bgcVar.m2542a(qb2Var);
}
```

**Impact:** Multiple concurrent API calls detecting token expiry can trigger simultaneous refresh requests. The server may invalidate the first refresh token after issuing a new one, causing subsequent refresh attempts to fail and potentially logging the user out.

**Severity:** HIGH

## 2. Firebase Installation Persistence - TOCTOU File Race

**File:** `p000/ura.java` (method `m23078B`, `m23080D`, `m23085x`)

Token persistence uses temp file + rename pattern but the read path (`m23080D`) has no synchronization with the write path (`m23078B`). The file path getter `m23085x()` uses double-checked locking correctly (with synchronized), but the actual read/write of token data is unprotected.

```java
// Write path - m23078B
File fileCreateTempFile = File.createTempFile("PersistedInstallation", "tmp", ...);
FileOutputStream fileOutputStream = new FileOutputStream(fileCreateTempFile);
fileOutputStream.write(jSONObject.toString().getBytes("UTF-8"));
fileOutputStream.close();
fileCreateTempFile.renameTo(m23085x()); // atomic on most filesystems

// Read path - m23080D - no lock, can read partial/stale data during rename
FileInputStream fileInputStream = new FileInputStream(m23085x());
```

**Impact:** On filesystems where rename isn't atomic, or during the brief window between temp write and rename, a concurrent read could get empty/corrupt data, causing auth token loss.

**Severity:** MEDIUM

## 3. SharedPreferences Custom Implementation - Partial Synchronization

**Files:** `p000/ts6.java`, `p000/ss6.java`

The custom SharedPreferences implementation (`ts6`) uses `synchronized(f68214f)` for commits but reads from `f68211c` (the data map) without holding the same lock:

```java
// Read - no lock on f68214f
public final Object mo22228a(String str) {
    return this.f68211c.m13532f(str);
}

// Write - holds lock
synchronized (ts6Var.f68214f) {
    // ... modify f68211c ...
    ts6Var.f68211c = lobVar;
}
```

The reference assignment is atomic in Java, but readers may see stale data. Multiple `edit().commit()` calls from different threads could interleave, with the last writer winning and silently dropping changes from concurrent editors.

**Impact:** Settings corruption, lost preference updates. If auth tokens or session data stored here, could cause auth failures.

**Severity:** MEDIUM

## 4. File Operations - TOCTOU Patterns

**Files:** `p000/kv6.java`, `p000/nv6.java`, `p000/lv6.java`, `p000/n81.java`

Widespread check-then-act patterns with no locking:

```java
// kv6.java - file append logic
if (!file.exists()) {
    m12801f(file.getParentFile()); // create dirs
    if (file2.renameTo(file)) { return; } // TOCTOU: file could be created between check and rename
}
// Falls through to append mode if file appeared between check and rename

// nv6.java - file copy
if (!file.exists()) { throw new NoSuchFileException(file); }
if (file2.exists() && !file2.delete()) { throw ... }
// Between delete and the copy below, another thread could recreate file2
FileInputStream fileInputStream = new FileInputStream(file);
```

**Impact:** Log file corruption, cache file overwrites, potential data loss. In crash reporting (`CrashReportInitializer.java`), could lose crash data.

**Severity:** LOW-MEDIUM

## 5. WebSocket Message Queue Race

**File:** `p000/tcf.java`

The WebSocket implementation uses `synchronized(this)` for queue operations but the connection state check and message enqueue are separate operations:

```java
// m21741b - close
synchronized (this) {
    if (!this.f66933u && !this.f66930r) {
        this.f66930r = true;
        this.f66928p.add(new pcf(i, e81Var));
        m21745f(); // trigger send
    }
}

// m21743d - connection established
synchronized (this) {
    this.f66926n = kcfVar;
    if (!this.f66928p.isEmpty()) {
        m21745f(); // flush pending
    }
}
```

While individual operations are synchronized, the `epk` reader thread (`m21744e`) runs outside the lock, creating a window where messages could be processed after close is initiated.

**Impact:** Messages sent after close initiated, potential duplicate delivery or message loss during reconnection.

**Severity:** MEDIUM

## 6. Message Sending Queue - Duplicate Send Risk

**Files:** `p000/xqg.java`, `p000/wsj.java`

The `ServiceTaskSendMessageQueue` (`xqg`) processes messages sequentially from a Queue, and on success (`mo1763w`) creates a new `xqg` for remaining messages. However, the task submission to `uvk.m23175a()` has no deduplication:

```java
// wsj.java - multiple paths can submit the same message
((uvk) xsjVar.f80963b.getValue()).m23175a(new xqg(pqgVar));
// ... later in same flow ...
((uvk) xsjVar.f80963b.getValue()).m23175a(new xqg(pqgVar2));
```

If a user rapidly taps send, or if a network timeout triggers retry while the original request is still in-flight, duplicate `xqg` tasks can be submitted for the same message content.

**Impact:** Duplicate messages sent to recipients.

**Severity:** MEDIUM

## 7. Session/Reconnect Race in Signaling Transport

**File:** `p000/tch.java`

The signaling transport uses `ReentrantLock` for reconnect context but `volatile` fields for connection state (`f66945B`, `f66969s`). The endpoint URL can be modified from the failure handler while a new connection attempt reads it:

```java
// Failure handler - modifies URL under sync
synchronized (tchVar.f66975y) {
    tchVar.f66969s = och.m15801a(...); // switch endpoint
}

// But connection attempts read f66969s outside this lock in some paths
access$validateEndpoint(tchVar); // reads f66969s
```

**Impact:** Connection attempt to stale/invalid endpoint after failover, causing unnecessary reconnection delays.

**Severity:** LOW

## 8. Database Transaction Locking

**File:** `p000/s0g.java`

The database layer uses a busy-wait loop with `SQLiteDatabaseLockedException`:

```java
public final SQLiteDatabase m20814l() {
    long jMo4596n = px3Var.mo4596n();
    while (true) {
        try {
            return p6gVar.getWritableDatabase();
        } catch (SQLiteDatabaseLockedException e) {
            if (px3Var.mo4596n() >= ((long) this.f62830d.f43840c) + jMo4596n) {
                throw new SynchronizationException("Timed out while trying to open db.", e);
            }
            SystemClock.sleep(50L);
        }
    }
}
```

The `PRAGMA busy_timeout=0` in `onConfigure` disables SQLite's built-in busy handler, forcing the app to handle contention manually. The `m20815u0` method has a similar retry loop for transactions. Read-modify-write patterns in event logging could lose events under contention if the timeout fires.

**Impact:** Event/analytics data loss under high contention; potential `SynchronizationException` crashes.

**Severity:** LOW

## 9. Purchase Tracking - No Client-Side Deduplication

**File:** `com/p006my/tracker/core/p008o/C0195b0.java`

Purchase data is inserted into `table_raw_purchases` with auto-increment IDs and no unique constraint on purchase content:

```sql
CREATE TABLE IF NOT EXISTS table_raw_purchases(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    data TEXT NOT NULL,
    signature TEXT NOT NULL,
    ts INTEGER NOT NULL
)
```

If the purchase callback fires multiple times (common on Android with `BillingClient`), duplicate entries are created with no deduplication logic visible.

**Impact:** Duplicate purchase tracking events; if server trusts client-reported purchases, potential double-credit.

**Severity:** MEDIUM (tracking only - actual billing is server-validated via Google Play)

## Summary

| # | Vulnerability | Severity | File(s) |
|---|---|---|---|
| 1 | Token refresh race - concurrent refresh | HIGH | sb2.java |
| 2 | Firebase token file TOCTOU | MEDIUM | ura.java |
| 3 | SharedPreferences partial sync | MEDIUM | ts6.java, ss6.java |
| 4 | File exists() TOCTOU patterns | LOW-MEDIUM | kv6.java, nv6.java |
| 5 | WebSocket message queue race | MEDIUM | tcf.java |
| 6 | Duplicate message send | MEDIUM | xqg.java, wsj.java |
| 7 | Signaling endpoint race | LOW | tch.java |
| 8 | Database busy-wait timeout | LOW | s0g.java |
| 9 | Purchase tracking duplicates | MEDIUM | C0195b0.java |
