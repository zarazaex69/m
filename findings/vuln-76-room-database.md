# VULN-76: Room Database Schema — Data Exposure & Injection Analysis

## Summary

Max messenger (OneMeRoomDatabase) uses an **unencrypted** Room database with WAL mode enabled, storing extensive sensitive user data across 42 tables. No SQLCipher or at-rest encryption is applied.

## 1. Database Class

- **Class**: `one.p011me.sdk.database.OneMeRoomDatabase` (extends `kuf` = RoomDatabase)
- **Implementation**: `OneMeRoomDatabase_Impl`
- **Open Helper**: `p000.crc` (extends `r3d`)
- **Schema Version**: 47
- **Identity Hash**: `2c31abd041757f103622f3650abc5682`
- **Database Name**: Generated via `lg9.m13329a("cache", "db")` → produces `cache_<accountId>.db`

## 2. All Database Tables (42 tables)

| Table | Sensitive Data Level |
|-------|---------------------|
| `messages` | **CRITICAL** — full message text, sender, attachments (BLOB), timestamps |
| `comments` | **CRITICAL** — comment text, sender, attachments |
| `contacts` | **HIGH** — server_id + serialized data BLOB |
| `phones` | **HIGH** — phone numbers, emails, first/last names, avatar paths |
| `chats` | **HIGH** — chat data BLOB, server IDs |
| `profile` | **HIGH** — serialized profile BLOB |
| `webapp_biometry` | **HIGH** — biometric tokens per user/bot |
| `fcm_notifications` | **MEDIUM** — push text, sender names, chat titles, URLs |
| `uploads` | **MEDIUM** — file paths, upload URLs, photo tokens |
| `message_uploads` | **MEDIUM** — file paths linked to messages |
| `draft_uploads` | **MEDIUM** — draft file paths |
| `video_conversions` | **MEDIUM** — source URIs, result paths |
| `presence` | **MEDIUM** — contact online/seen status |
| `organizations` | **MEDIUM** — org names, descriptions |
| `chat_title` | LOW — FTS4 virtual table for search |
| `contact_title` | LOW — FTS4 virtual table for search |
| `tasks` | LOW — task data BLOBs |
| `WorkerQueueItem` | LOW — work specs |
| `sticker_sets` | LOW |
| `favorite_sticker_sets` | LOW |
| `favorite_stickers` | LOW |
| `recent` | LOW |
| `fcm_notifications_history` | LOW |
| `fcm_notifications_analytics` | LOW |
| `notifications_read_marks` | LOW |
| `notifications_tracker_messages` | LOW |
| `chat_folder` | LOW |
| `folder_and_chats` | LOW |
| `selected_mentions` | LOW |
| `stat_events` | LOW — analytics BLOBs |
| `stickers` | LOW |
| `message_comments` | LOW |
| `animoji` | LOW |
| `animoji_set` | LOW |
| `reactions_section` | LOW |
| `saved_msg_chat` | LOW |
| `complain_reasons` | LOW |
| `informer_banner` | LOW |
| `metrics` | LOW |
| `battery` | LOW |

## 3. Sensitive Data Stored in Plaintext

### Messages Table (CRITICAL)
```sql
CREATE TABLE messages (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  server_id INTEGER, time INTEGER, sender INTEGER,
  text TEXT,              -- plaintext message content
  attaches BLOB,         -- serialized attachments (images, files, etc.)
  elements BLOB,         -- rich content elements
  chat_id INTEGER,       -- links to chat
  ...
)
```

### Phones Table (HIGH)
```sql
CREATE TABLE phones (
  phone TEXT NOT NULL,        -- phone number in plaintext
  phone_key TEXT NOT NULL,    -- phone identifier
  email TEXT,                 -- email in plaintext
  first_name TEXT NOT NULL,   -- contact name
  last_name TEXT,
  avatar_path TEXT,           -- local file path to avatar
  ...
)
```

### WebApp Biometry Table (HIGH)
```sql
CREATE TABLE webapp_biometry (
  user_id INTEGER, bot_id INTEGER,
  token TEXT,                 -- biometric token stored plaintext
  access_requested INTEGER, access_granted INTEGER
)
```

### Uploads Table (MEDIUM)
```sql
CREATE TABLE uploads (
  prepared_path TEXT, file_name TEXT,
  upload_url TEXT,            -- server upload endpoint
  photo_token TEXT,           -- upload auth token
  path TEXT NOT NULL,         -- local file path
  ...
)
```

## 4. Encryption Status: ❌ NONE

- **No SQLCipher** dependency found in the codebase
- **No SupportFactory** or SafeHelperFactory usage
- **No PRAGMA key** statements
- **No at-rest encryption** of any kind
- Database file is stored as **plaintext SQLite** on the filesystem
- Any app with root access or backup access can read all data

## 5. WAL Mode: ✅ ENABLED (Data Leakage Risk)

From `p000/xk5.java`:
```java
// On API 30+, journal mode is WAL
if (g65Var.f20470g == 3) {
    lj7.m13402m(o0gVar, "PRAGMA journal_mode = WAL");
    lj7.m13402m(o0gVar, "PRAGMA synchronous = NORMAL");
} else {
    lj7.m13402m(o0gVar, "PRAGMA journal_mode = TRUNCATE");
    lj7.m13402m(o0gVar, "PRAGMA synchronous = FULL");
}
```

**Risk**: WAL journal files (`cache_N.db-wal`, `cache_N.db-shm`) may contain:
- Recently deleted messages still in WAL pages
- Partial transaction data
- Deleted contact information recoverable via forensic tools

## 6. Database Export/Dump Feature

- **No explicit export/dump feature** found in the database layer
- However, `android:allowBackup` may expose the database via ADB backup
- The database path is in the app's private data directory

## 7. Migration Behavior

From `OneMeRoomDatabase_Impl.mo12777d()` — **35 migrations** registered:
- Uses `geb` (standard migration), `zqc` (schema migration), `oeb` (destructive), `neb` (destructive fallback), `yqc` (auto migration)
- **Destructive migrations exist**: `oeb(0)`, `oeb(1)`, `oeb(2)`, `oeb(3)`, `oeb(4)` — these DROP and recreate tables
- Migration from version 30→31 recreates the `messages` table entirely:
  ```java
  "CREATE TABLE IF NOT EXISTS `_new_messages` (...)"
  "DROP TABLE `messages`"
  "ALTER TABLE `_new_messages` RENAME TO `messages`"
  ```
- **Data preserved during migrations** — messages are copied to `_new_messages` before drop
- `clearAllTables` method (`mo4572c`) drops ALL 40 tables — used on destructive fallback

## 8. PRAGMA Settings

| Setting | Value | Source |
|---------|-------|--------|
| `busy_timeout` | **3000ms** (minimum) | `xk5.java` — sets to 3000 if current value < 3000 |
| `journal_mode` | **WAL** (API 30+) / TRUNCATE (older) | `xk5.java` |
| `synchronous` | **NORMAL** (WAL) / FULL (TRUNCATE) | `xk5.java` |
| `foreign_keys` | **ON** | `crc.mo4574t()` |

From `p000/p6g.java` (Firebase transport DB):
```java
sQLiteDatabase.rawQuery("PRAGMA busy_timeout=0;", new String[0]).close();
```

## 9. SQL Injection Analysis

### Dynamic Query Construction (Potential Risk)
Several DAOs build queries with string concatenation for IN clauses:
```java
// sm6.java - favorite_stickers
"DELETE FROM favorite_stickers WHERE id IN (" + list + ")"

// ztf.java - chat_folder
"DELETE FROM chat_folder WHERE id IN (" + list + ")"

// ydb.java - metrics
sb.append("DELETE FROM metrics WHERE traceId IN (");
```

These use `hjj.m9563g()` (StringBuilder) + `hjj.m9561e()` to build IN-clause lists. While the values appear to be internally generated IDs (not user input), the pattern is unsafe if any user-controlled data reaches these paths.

### Parameterized Queries (Safe)
Most DAO operations use parameterized queries via Room's generated code:
```java
"DELETE FROM messages WHERE chat_id = ?"  // fj3.java
"DELETE FROM fcm_notifications WHERE chat_id = ?"
```

## 10. Attack Vectors

1. **Physical device access**: Database is unencrypted — full data extraction with root/ADB
2. **Backup extraction**: If `android:allowBackup=true`, database extractable via `adb backup`
3. **WAL forensics**: Deleted messages recoverable from WAL/SHM files
4. **Content Provider exposure**: If any ContentProvider exposes the database (needs further check)
5. **Shared preferences leak**: `ri9` class stores `phoneCode`, `phoneNumber`, `serverHost`, `serverPort` in preferences

## Risk Rating

| Category | Rating |
|----------|--------|
| Data Exposure (no encryption) | **CRITICAL** |
| WAL data remnants | **HIGH** |
| SQL Injection (IN-clause construction) | **MEDIUM** |
| Migration data handling | **LOW** |

## Recommendations

1. Implement SQLCipher for at-rest encryption
2. Use secure delete (`PRAGMA secure_delete = ON`) to prevent WAL data recovery
3. Replace string-concatenated IN clauses with parameterized queries
4. Disable `android:allowBackup` or use encrypted backups
5. Clear WAL checkpoint on app close/logout
