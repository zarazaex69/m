# VULN-23: Family Protection Bot Analysis

## Summary

Max Messenger (TamTam/OneMe) includes a "Family Protection" feature that operates via a server-configured bot ID and account-level privacy status. The feature is a **parental control mechanism** that places a child's account into a restricted "MANAGEABLE" state controlled by a parent ("ADMIN").

## Bot ID Configuration

- **PmsKey**: `family-protection-botid` (enum ordinal 187)
- **Field**: `PmsKey.f88692familyprotectionbotid`
- **Accessor**: `qp6.getFamilyProtectionBotId()` returns `long` (bot user ID)
- **Storage**: `qp6.f57535w0` — initialized from PMS (Platform Management Service) remote config
- **Feature gate**: `z1h.m26439A()` — returns `true` if `familyProtectionBotId != 0`

The bot ID is **server-provisioned** — the actual numeric ID is delivered via remote configuration, not hardcoded in the APK.

## Account States (enum `qgj`)

| State | Meaning |
|-------|---------|
| `OFF` | Family protection disabled (default) |
| `ADMIN` | Parent/guardian account — controls the managed account |
| `MANAGEABLE` | Child/managed account — subject to restrictions |

## Server Protocol

- **Privacy settings field**: `FAMILY_PROTECTION` (parsed in `apl.java` from server JSON/protobuf)
- **Local storage key**: `app.family.protection.status` (SharedPreferences string: "OFF"/"ADMIN"/"MANAGEABLE")
- **Write path**: `ugj.m7043i("app.family.protection.status", qgjVar.f56625a)` — persists server-sent status locally

## UI Integration

- Located in: **Settings → Privacy → Family Protection** section
- Navigation ID: `oneme_settings_privacy_family_protection` (layout resource `0x7f0a07f5`)
- Icon: `icon_family_protection` (drawable `0x7f080751`)
- Title string: `oneme_settings_privacy_screen_family_protection_title`
- Status strings:
  - `oneme_settings_privacy_family_protection_admin` — shown to parent
  - `oneme_settings_privacy_family_protection_manageable` — shown to child
  - `oneme_settings_privacy_family_protection_off` — disabled state

## Privacy Restrictions on MANAGEABLE Accounts

When `status == MANAGEABLE`, the privacy settings UI **disables user control** over:
- **Who can see online status** (`app.privacy.online.show`)
- **Who can call** (`app.privacy.incoming.call`)
- **Who can add to chats** (`app.privacy.chats.invite`)
- **Search by phone number** (`app.privacy.search_by_phone`)

The code in `u1h.java` sets `z2 = !(qgjVar != MANAGEABLE)` — when MANAGEABLE, privacy setting items are rendered as **non-interactive** (disabled).

## Capability Assessment

### Can it read child's messages?
**NOT CONFIRMED in client code.** No client-side message interception logic found. The bot operates server-side — the client only stores the status flag. Message access would be a server-side bot capability not visible in the APK.

### Can it track location?
**NOT CONFIRMED.** No location-sharing code tied to family protection found in client.

### Can it monitor screen time?
**NOT CONFIRMED.** No screen time tracking code found in client.

### Can it block contacts?
**PARTIALLY — INDIRECT.** The MANAGEABLE state locks privacy settings (who can call, who can add to chats) so the child cannot change them. The ADMIN presumably sets these via the bot conversation server-side.

### Can it see call history?
**NOT CONFIRMED in client.** No call history export tied to family protection.

### Can it silently install on a child's device?
**NO.** The feature is an account-level server flag (`FAMILY_PROTECTION` status). It does not install anything — it's activated by the server setting the account status to MANAGEABLE. The child's app simply reads this status and restricts the UI accordingly.

## Elevated Permissions

The family protection bot has a **special status** compared to normal bots:
1. **Server-provisioned ID via PMS** — alongside `money-transfer-botid`, `channel-statistics-botid`, `stickers-botid`, `digitalid-botid` — these are platform-level system bots
2. **`availableBotForPrBridge`** (`getAvailableBotForPrBridge()` returns `LongSet`) — suggests certain bots have bridge/proxy access to privacy-related operations
3. **Account state modification** — the bot (server-side) can set a user's `FAMILY_PROTECTION` status, which is a **privacy-level account attribute** — normal bots cannot modify account privacy settings

## API Calls

The family protection feature uses:
1. **Privacy settings sync** — `FAMILY_PROTECTION` field in the privacy settings response (parsed in `apl.java`)
2. **Status persistence** — `app.family.protection.status` written to local prefs
3. **Bot ID resolution** — `getFamilyProtectionBotId()` used to identify the bot for chat/interaction

No dedicated REST/RPC endpoints for family protection were found in the client — the logic is:
- Server sets account status via privacy settings sync
- Client reads status and restricts UI
- Parent interacts with the bot via normal chat to manage settings

## Supervision/Monitoring Features

**No active surveillance found in client code.** The feature is a **restriction mechanism**, not a monitoring tool:
- It locks privacy settings on the child's account
- It does NOT intercept messages, track location, or report activity
- The "monitoring" is limited to the parent being able to control who can contact the child

## Risk Assessment

| Risk | Level | Notes |
|------|-------|-------|
| Silent activation | **MEDIUM** | Server can set any account to MANAGEABLE without client consent UI |
| Privacy lockout | **MEDIUM** | Child cannot modify their own privacy settings |
| Message surveillance | **UNKNOWN** | Server-side bot capabilities not visible in client |
| Abuse potential | **MEDIUM** | Could be used for non-parental control if server-side validation is weak |

## Key Source Files

| File | Role |
|------|------|
| `ru/p027ok/tamtam/android/prefs/PmsKey.java:580-581` | Bot ID PMS key definition |
| `p000/qgj.java` | Family protection status enum (OFF/ADMIN/MANAGEABLE) |
| `p000/qp6.java:696` | Bot ID field initialization from PMS |
| `p000/apl.java:348-361` | Server response parsing of FAMILY_PROTECTION field |
| `p000/ugj.java:214` | Writing status to local preferences |
| `p000/u1h.java:150-270` | Privacy settings UI — family protection section rendering |
| `p000/z1h.java:293-296` | `isFamilyProtectionEnabled()` check (botId != 0) |
| `p000/sgj.java:194` | Privacy settings model includes familyProtection field |

## Conclusion

The Family Protection bot is a **server-controlled parental restriction system**, not a full surveillance tool (from the client perspective). Its primary client-side effect is locking privacy settings on managed accounts. The actual bot capabilities (message reading, activity reporting) are entirely server-side and cannot be determined from the APK alone. The main concern is that account status can be set server-side without visible client-side consent flow in the decompiled code.
