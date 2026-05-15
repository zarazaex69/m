# VULN-09: SQL Injection Analysis — Max Messenger

**Date:** 2026-05-15  
**Severity:** Low-Medium (mitigated by non-exported providers)  
**Source:** `~/max/decompiled/jadx/base/sources`

---

## 1. ContentProvider Implementations

Seven ContentProvider subclasses found:

| Class | Purpose |
|-------|---------|
| `one.p011me.android.notifications.NotificationsImagesProvider` | Notification image serving |
| `ru.p027ok.tracer.startup.InitializationProvider` | Tracer init |
| `ru.p027ok.android.commons.app.ApplicationProvider` | App init |
| `ru.trace_flow.dps.internal.DpsInitProvider` | DPS init |
| `com.google.firebase.provider.FirebaseInitProvider` | Firebase init |
| `androidx.startup.InitializationProvider` | AndroidX startup |
| `androidx.core.content.FileProvider` | File sharing |

### ContentProvider Query/Insert/Update/Delete Analysis

**NotificationsImagesProvider** — The only app-specific provider with meaningful logic:
- `query()` returns `null` (no-op)
- `insert()` returns `null` (no-op)
- `update()` returns `0` (no-op)
- `delete()` returns `0` (no-op)
- `openFile()` validates URI via `UriMatcher`, checks for internal URI injection

**Finding:** No SQL injection risk in ContentProvider implementations — they don't perform database operations.

---

## 2. Raw SQL with String Concatenation (High-Risk Patterns)

### 2.1 `DatabaseHelper.java` — LIMIT concatenation
**File:** `ru/p027ok/android/externcalls/analytics/internal/storage/DatabaseHelper.java:136`
```java
this.cursor = db.rawQuery("select item, c, id from call_events order by id limit " + this.limit, null, null);
```
**Risk:** LOW — `this.limit` is an integer field, not user-controlled string. No injection vector.

### 2.2 `lih.java` — DELETE with dynamic IN clause
**File:** `p000/lih.java:69`
```java
s0gVar.m20814l().compileStatement("DELETE FROM events WHERE _id in " + s0g.m20808D0(iterable)).execute();
```
**Risk:** LOW-MEDIUM — `m20808D0()` builds `(id1,id2,...)` from `kj0.f34148a` fields (Long IDs). Values come from internal event processing, not direct user input.

### 2.3 `zj5.java` — UPDATE with dynamic IN clause
**File:** `p000/zj5.java:44`
```java
String str = "UPDATE events SET num_attempts = num_attempts + 1 WHERE _id in " + s0g.m20808D0(iterable);
```
**Risk:** LOW-MEDIUM — Same pattern as above. IDs from internal transport layer.

### 2.4 `qvh.java` — UPDATE with timestamp concatenation
**File:** `p000/qvh.java:118`
```java
sQLiteDatabaseM20814l.compileStatement("UPDATE global_log_event_state SET last_metrics_upload_ms=" + s0gVar3.f62828b.mo4596n()).execute();
```
**Risk:** LOW — `mo4596n()` returns a system clock value (Long). Not user-controllable.

### 2.5 `eu2.java` — UPDATE with numeric concatenation
**File:** `p000/eu2.java:159`
```java
sQLiteDatabase.execSQL(hjj.m9559c(j, "UPDATE log_event_dropped SET events_dropped_count = events_dropped_count + ", " WHERE log_source = ? AND reason = ?"), new String[]{str, Integer.toString(i)});
```
**Risk:** LOW — `j` is a long value (count). The WHERE clause uses parameterized binding.

### 2.6 `lcc.java` / `wk5.java` — DROP TABLE with concatenation
**File:** `p000/lcc.java:217,322`
```java
writableDatabase.execSQL("DROP TABLE IF EXISTS " + strConcat);
writableDatabase.execSQL("CREATE TABLE " + ((String) this.f36610o) + " (...)");
```
**File:** `p000/wk5.java:144-145`
```java
writableDatabase.execSQL("DROP TABLE IF EXISTS " + ((String) this.f77303c));
writableDatabase.execSQL("CREATE TABLE " + ((String) this.f77303c) + " (...)");
```
**Risk:** LOW — Table names derived from hex-encoded cache UIDs (`Long.toHexString(j)`), not user input.

### 2.7 `qv3.java` — WorkManager DELETE with timestamp
**File:** `p000/qv3.java:23`
```java
t5iVar.mo12383A("DELETE FROM workspec WHERE state IN (2, 3, 5) AND (last_enqueue_time + minimum_retention_duration) < " + (System.currentTimeMillis() - fuk.f19730a) + " AND ...");
```
**Risk:** LOW — System timestamp concatenation. AndroidX WorkManager internal code.

### 2.8 `wuk.java` — SELECT with dynamic WHERE clause
**File:** `p000/wuk.java:196`
```java
.mo12391a0("SELECT COUNT(*) FROM workspec WHERE " + strM18490L0 + " AND state=0 AND schedule_requested_at<>-1");
```
**Risk:** LOW — `strM18490L0` is built from work spec tag names (internal constants), not user input.

### 2.9 `nzi.java` — Room invalidation tracker INSERT
**File:** `p000/nzi.java:275`
```java
String str2 = "INSERT OR IGNORE INTO room_table_modification_log VALUES(" + i5 + ", 0)";
```
**Risk:** NONE — `i5` is an integer table index from Room's internal schema. Standard Room framework code.

---

## 3. Parameterized Queries (Properly Secured)

The majority of rawQuery calls use proper parameterization:

| File | Query | Parameterized? |
|------|-------|----------------|
| `C0215l0.java` | `SELECT COUNT(*) FROM table_events WHERE major=1` | Static (no params needed) |
| `uej.java` | `SELECT 1 FROM events WHERE context_id = ? LIMIT 1` | ✅ Yes |
| `C0217m0.java` | `SELECT eid, ts FROM table_events_timestamps WHERE eid=?` | ✅ Yes |
| `C0227r0.java` | `SELECT sid, ts_start, ts_end FROM table_sessions_timestamps WHERE sid=?` | ✅ Yes |
| `C0221o0.java` | `SELECT value FROM table_params WHERE param_key=?` | ✅ Yes |
| `n0g.java` | `SELECT distinct t._id...` | Static |
| `cv8.java` | `SELECT next_request_ms FROM transport_contexts WHERE backend_name = ? and priority = ?` | ✅ Yes |
| `eu2.java` | `SELECT 1 FROM log_event_dropped WHERE log_source = ? AND reason = ?` | ✅ Yes |
| `C0225q0.java` | `SELECT id, name, ts_start, ts_skipped FROM table_sessions WHERE name=?` | ✅ Yes |
| `neb.java` | `SELECT SUBSTR(attaches, ?, ?) as chunk FROM messages WHERE id = ?` | ✅ Yes |

---

## 4. Room Database DAO Analysis

- **OneMeRoomDatabase** (`one.p011me.sdk.database.OneMeRoomDatabase_Impl`) is the main app database
- Extends `kuf` (obfuscated `RoomDatabase`)
- **No `@RawQuery` annotations found** in the codebase
- **No `SimpleSQLiteQuery` or `SupportSQLiteQuery` usage found**
- Room-generated DAOs use the standard Room query compilation (type-safe, parameterized)
- Room's internal invalidation tracker uses integer concatenation (safe)

**Finding:** Room DAOs appear to use only compile-time verified `@Query` annotations with proper parameterization.

---

## 5. LIKE Queries

**No LIKE queries with string concatenation found** in the codebase. No unescaped LIKE patterns detected.

---

## 6. Exported ContentProviders (Manifest Analysis)

All ContentProviders are explicitly **`android:exported="false"`**:

```xml
<provider android:name="androidx.startup.InitializationProvider" android:exported="false" .../>
<provider android:name="androidx.core.content.FileProvider" android:exported="false" .../>
<provider android:name="com.google.firebase.provider.FirebaseInitProvider" android:exported="false" .../>
<provider android:name="one.p011me.android.notifications.NotificationsImagesProvider" android:exported="false" .../>
<provider android:name="ru.trace_flow.dps.internal.DpsInitProvider" android:enabled="false" .../>
```

**Finding:** No exported ContentProviders. External apps cannot interact with any provider, eliminating the primary SQL injection attack surface on Android.

---

## 7. Database File Storage

### MyTracker Database
**File:** `com/p006my/tracker/core/p008o/C0195b0.java:53`
```java
SQLiteDatabase sQLiteDatabaseOpenOrCreateDatabase = application.openOrCreateDatabase(str2, 0, null);
```
- Mode `0` = `MODE_PRIVATE` — file is accessible only to the app
- Database name: `mytracker_<id>.db` — stored in app's private `/data/data/` directory

### Other Databases
- Transport/analytics databases use `SQLiteOpenHelper` (default private storage)
- ExoPlayer cache databases use `SQLiteOpenHelper` (default private storage)
- Room database (`OneMeRoomDatabase`) uses standard Room builder (private storage)

**Finding:** No databases stored on external storage. No `MODE_WORLD_READABLE` usage. All databases are in the app's private directory.

---

## 8. Summary of Findings

| # | Issue | Severity | Exploitable? |
|---|-------|----------|--------------|
| 1 | String concatenation in SQL (IN clauses, timestamps) | Low | No — values are internal integers/longs |
| 2 | `DatabaseHelper` LIMIT concatenation | Low | No — integer field |
| 3 | DROP/CREATE TABLE with concatenated names | Low | No — hex-encoded internal IDs |
| 4 | No exported ContentProviders | N/A | Attack surface eliminated |
| 5 | No world-readable databases | N/A | No file-level exposure |
| 6 | No @RawQuery in Room DAOs | N/A | Best practice followed |
| 7 | No LIKE with unescaped input | N/A | Best practice followed |

---

## 9. Risk Assessment

**Overall SQL Injection Risk: LOW**

The application demonstrates generally good SQL hygiene:
1. All ContentProviders are non-exported, eliminating external SQL injection vectors
2. Room database DAOs use compile-time verified queries
3. Most raw queries use proper parameterization (`?` placeholders)
4. String concatenation exists but only with internally-derived numeric values

**Residual Risks:**
- If server-controlled data ever flows into the `kj0.f34148a` field used in `m20808D0()`, the IN-clause construction could be exploited. However, these appear to be locally-generated database row IDs.
- The `DatabaseHelper.limit` field concatenation is safe as an integer but represents a code smell — should use parameterized query.
- WorkManager's dynamic WHERE clause construction (`wuk.java`) relies on tag names being safe strings — if tags could be influenced by server data, this could be a vector.

**Recommendations:**
1. Replace all string concatenation in SQL with parameterized queries, even for numeric values (defense in depth)
2. Validate that `m20808D0()` input IDs are always locally-generated integers
3. Audit data flow from server responses to any values used in SQL construction
