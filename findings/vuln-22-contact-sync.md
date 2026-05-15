# VULN-22: Contact Sync Protocol Analysis

## Summary
Max messenger (ru.ok.tamtam) uploads device contacts to VK/OK servers in **plaintext** (phone numbers + names), triggered on every app foreground and on any contact change.

## 1. Contact Sync Code Location

Key classes:
- `p000/a40.java` — Main phonebook controller, registers ContentObserver, triggers sync
- `p000/kfd.java` — PhonebookSyncService, batches and sends contacts to server
- `p000/k5l.java` — Reads contacts from Android ContactsContract
- `p000/gfd.java` — Coroutine-based contact reader (newer implementation)
- `p000/s32.java` — Network request builder for contact sync
- `p000/ndi.java` — Sync adapter for writing Max contacts back to device
- Service registered as: `ru.ok.tamtam.services.PhonebookSyncService`

## 2. Contact Fields Read

From `k5l.java` and `gfd.java`, the app queries `ContactsContract.Data.CONTENT_URI` with:
```
mimetype IN ('vnd.android.cursor.item/phone_v2', 'vnd.android.cursor.item/name')
```

**Fields extracted:**
| Field | Column | Sent to Server |
|-------|--------|----------------|
| Phone number | `data1` | ✅ YES (as map key) |
| First name | `data2` | ✅ YES (`firstName`) |
| Last name | `data3` | ✅ YES (`lastName`) |
| Display name | `display_name` | Used as fallback |
| Middle name | `data5` | Used locally for display |
| Photo URI | `photo_uri` / `photo_thumb_uri` | Stored locally only |
| Contact ID | `contact_id` | Internal use |

**NOT read:** email (stored in DB but not queried from device), address, birthday, notes.

## 3. Data Transmission Format — PLAINTEXT

From `s32.java` (case 3, opcode 28) and `uj4.java` (case 5):

```java
// s32.mo464l() - builds the request
dk9 dk9Var2 = new dk9(u0d.f69069G0, 28);  // SYNC opcode, type 28
HashMap map2 = new HashMap();
map.forEach(new uj4(5, map2));  // transforms contacts
dk9Var2.m18552i("contactList", map2);

// uj4.accept() - serializes each contact
ws9 ws9Var = new ws9();
ws9Var.put("firstName", to4Var.f67836a);  // PLAINTEXT first name
if (str2 != null && str2.length() != 0) {
    ws9Var.put("lastName", str2);           // PLAINTEXT last name
}
map.put(str, ws9Var.m24628b());            // key = PLAINTEXT phone number
```

**The phone number is the HashMap key, sent as plaintext string. Names are sent as plaintext values.**

The request uses:
- Protocol type: `u0d.f69069G0` = "SYNC" (opcode 21)
- Request subtype: 28 (contact sync specific)
- Batch size: max 100 contacts per request

The `phone_key` field in the local DB is just a normalized phone string (not a hash) — used as a unique index.

## 4. Sync Frequency

**Triggers:**
1. **Every app foreground** — `g3k.m8114b(true)` calls `a40.m170b()` (checkUpdates)
2. **On ContentObserver change** — `u30.onChange()` triggers `a40.f382h.mo3253h()` immediately
3. **Periodic timer** — `a40` constructor sets up a 5-second debounce flow: `ilb.m10508V(5, cz5.SECONDS)`
4. **On login/session init** — `om9.java` calls `phonebookSyncService.sync()` during session setup
5. **After successful sync** — `kfd.m12319b()` schedules retry after 1 second: `TimeUnit.SECONDS`

The 5-second debounce means after any contact change is detected, the system waits 5 seconds then runs a full diff check.

## 5. Contact Change Tracking

**YES — full change tracking implemented.**

From `a40.m169a()` and `dfd.java`:
- Registers `ContentObserver` on `ContactsContract.Contacts.CONTENT_URI` (notifyForDescendants=true)
- Computes diff producing: `Result(updated=[], inserted=[], deleted=[], collectedDevicePhones=[])`
- `updated` — contacts whose name/phone changed
- `inserted` — new contacts added to device
- `deleted` — contacts removed from device
- Logs: `"updatePhones=X,deletedPhones=Y,newPhones=Z. phonesInDb=A,phonesInPhonebook=B"`

The `kfd.syncInternal()` selects unsynced phones (type != synced) and sends them in batches of 100.

## 6. Non-Contact Sync

PmsKey configuration values:
- `non-contact-sync-time` (PmsKey ordinal 83) — controls sync interval for non-contacts
- `non-contact-max-chunk-size` (PmsKey ordinal 84) — default 10, max items per batch
- `non-contact-collection-interval` (PmsKey ordinal 85) — collection interval between batches

From `myb.java`:
- Collects data about users who are NOT in the device phonebook
- Batches them when chunk size >= `non-contact-max-chunk-size` (default 10)
- Respects `non-contact-collection-interval` between sends
- Runs on a dedicated coroutine scope: `limitedParallelism(1, "non-contacts")`
- Classes: `myb`, `ap4`, `sp7`, `d92`

This appears to track interactions with non-contact users (people you message who aren't in your phonebook) and syncs metadata about them.

## 7. Reading Contacts from Other Apps

**NO evidence found.** The app only queries standard Android ContactsContract with mimetypes:
- `vnd.android.cursor.item/phone_v2`
- `vnd.android.cursor.item/name`

No references to WhatsApp, Telegram, Viber, or Signal package names. No queries for app-specific contact mimetypes. The app reads the unified Android contacts database, which may include contacts synced by other apps, but does not specifically target them.

## 8. Opt-Out Mechanism

**Limited — permission-based only:**

1. **Deny READ_CONTACTS permission** — `a40.m171c()` checks permission before registering observer:
   ```java
   if (!aed.m690d(aed.f1229g)) {  // "android.permission.READ_CONTACTS"
       ct4.m4610E("subscribeOnSystemChanges: no permissions, return");
   }
   ```
2. **No in-app toggle** to disable contact sync specifically
3. **Phone privacy settings** exist (`app.privacy.phone.number.privacy` = CONTACTS/ALL/NO_ONE) but these control who can find YOU by phone, not whether your contacts are uploaded

**There is no way to use the app with contacts permission granted but opt out of uploading contacts to VK servers.**

## Risk Assessment

| Risk | Level |
|------|-------|
| Data exposure | **HIGH** — plaintext phone numbers + names sent to VK |
| Frequency | **HIGH** — every foreground + real-time change detection |
| Scope | All device contacts (up to 100 per batch, iterates until complete) |
| User control | **LOW** — only OS permission denial prevents sync |
| Third-party data | Contacts of people who never consented to VK having their data |

## Network Protocol Summary

```
Direction: Client → Server
Transport: Custom binary protocol over TLS
Request type: SYNC (u0d opcode 21)
Sub-opcode: 28 (dk9 type)
Payload: JSON map "contactList" = { "phone_number": {"firstName": "...", "lastName": "..."} }
Batch size: 100 contacts max
Response: List of ul4 (server contact objects) with server IDs
```
