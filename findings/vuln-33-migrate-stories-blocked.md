# VULN-33: Migrate-Unsafe, Stories Privacy, Blocked Users Bypass

## Task 1: migrate-unsafe-warn

### Key Findings

**PmsKey:** `migrate-unsafe-warn` (index 155) — a server-controlled boolean flag  
**Field:** `f88760migrateunsafewarn` in `PmsKey.java:489`  
**Accessor:** `getMigrateUnsafeWarn()Z` — returns boolean (qp6.java, index [46])

**What is "unsafe"?**  
The "unsafe-files migration" (`AccountInitializer.java:507`) migrates file storage settings during TamTam→Max transition. The privacy setting `app.privacy.unsafe.files.default` (default: `true`) controls whether files from unknown contacts are auto-downloaded.

**Migration logic (C0596m6.java, case 2):**
```java
if (!getBoolean("app.privacy.unsafe.files.default", true)) {
    if (getMigrateUnsafeWarn().booleanValue()) {
        // Show warning dialog to user (C0597m7)
    }
}
```

**What the warning says:**  
When `migrate-unsafe-warn` is enabled server-side AND the user had disabled unsafe file downloads, a warning dialog is shown during migration. The flag is server-controlled — meaning the server can suppress the warning.

**Data at risk:**
- `unsafeFiles` field in user privacy settings (sgj.java:190) — part of the full privacy config alongside `safeMode`, `safeModeNoPin`, `searchByPhone`, `contentLevelAccess`, `familyProtection`, `phoneNumberPrivacy`
- The `UpdateUnsafeFilesUseCase` (mel.java:67) can update this setting
- Setting parsed from server response: `map.get("unsafeFiles")` (ma4.java:120-121)

**Vulnerability:** The `app.privacy.unsafe.files.default` defaults to `true` (unsafe). During TamTam→Max migration, if the server sets `migrate-unsafe-warn` to false, users who previously disabled unsafe file downloads will NOT be warned that their setting may be reset. Files from unknown contacts could auto-download without consent.

---

## Task 2: Stories Privacy

### Key Findings

**Story classes found:**
- `one.me.stories.publish.PublishStoryBottomSheet` (stories_release module)
- `one.me.chats.picker.stories.PickStoryPresetScreen` (chats-list_release module)
- `one.me.stories.publish.PublishStoryViewModelFactory` (mel.java:105)
- ViewModel: `ppe.java` — PublishStory ViewModel

**Story TTL (Time-to-Live):**
- `selectStoryTtlButton` — UI button for selecting story duration
- Resource: `oneme_stories_story_ttl_hours` (R.java:11330)
- TTL options in ViewModel (ppe.java): `{6, 12, 24, 48}` hours
- Default TTL: 24 hours (index [2] of array)

**Story Audience/Preset Selection:**
- `PickStoryPresetScreen` allows selecting who sees stories via `preselectedIds` (long[] of user IDs)
- `selectedIds` field exposes the viewer list as raw long array
- PmsKey `story` (index 307) — server-controlled story feature config

**Viewer list / tracking:**
- No explicit `StoryViewer`, `storyWatcher`, `storySeen`, or `storyRead` classes found in decompiled sources
- No evidence of view-count reporting back to poster in client code
- Story feature appears early-stage (minimal client implementation)

**Screenshot/save detection:**
- `app.pinLock.screenshotEnabled` setting exists (WebAppRootScreen.java:661) but applies to WebApps, NOT stories
- `mediasaves-context` and `mediasaves-menu` PmsKeys control media save options
- **No screenshot detection for stories found** — no notification mechanism when stories are saved/screenshotted

**Location/time metadata:**
- No story-specific location or geotag fields found
- Story only exposes TTL (hours) — creation timestamp implicit from server

**Vulnerabilities:**
1. **No screenshot notification** — stories can be saved without alerting the poster
2. **Viewer preset IDs exposed as raw longs** — the `preselectedIds` array leaks user IDs to the client, potentially enumerable
3. **No client-side view tracking** — if server doesn't enforce it, views may not be reported
4. **TTL client-controlled** — the TTL selection is sent from client; if not validated server-side, could be manipulated

---

## Task 3: Blocked Users Bypass

### Key Findings

**PmsKey:** `blocked-users` (index 306) — `f88594blockedusers`  
**Proto:** `Protos.java:6024` — `BLOCKED = 7` (participant status enum)  
**Account status:** `AccountStatus_BLOCKED = 1`, `PortalStatus_BLOCKED = 0` (Protos.java:10134-10145)

**Blocked user UI:**
- `BlockedGhostAvatarDrawable` — shows ghost avatar for blocked users (tamtam-android-sdk_release)
- `SettingsBlacklistScreen` — blacklist management UI (settings-privacy_release)
- Unblock via `user_unblock_id` Bundle key
- `blocked_ghost_avatar` resource (R.java:4992)

**Profile data model (v7e.java):**
- Fields: `isBlocked` (f73074j), `isPortalBlocked` (f73075k), `isVerified` (f73076l)
- Also exposes: `lowResAvatarUrl`, `title`, `abbreviation`, `link` — **these fields are populated regardless of block status**
- The `isBlocked` flag is a display hint, not an access control

**Call blocking:**
- `ApiErrorUserBlocked` (one.video.calls.sdk.api.error) — thrown when calling blocked user
- Error code: 77992 (ConversationImpl.java:1787)
- Call errors: `privacy.violation` OR `call.blocked` → same error state (jll.java:363)
- **Calls are blocked server-side** — proper enforcement

**Chat blocking:**
- `unblock_contact_controller_tag` in ChatScreen.java — unblock UI in chat
- `isBlockedLabelVisible` / `isSendMessageAvailable` (f7j.java:67) — UI state model
- `portal_blocked_chat` / `portal_blocked_chat_with_reason` resources

**Can a blocked user still:**

| Action | Status | Evidence |
|--------|--------|----------|
| See online status | **LIKELY YES** | `presence-external` PmsKey requests presence for external users; no block check found in presence logic |
| See profile photo | **LIKELY YES** | `lowResAvatarUrl` populated in v7e regardless of block; `BlockedGhostAvatarDrawable` only shown to blocker |
| Send via group chat | **PARTIALLY** | `blockedParticipantsCount` tracked (Protos.java:6051) but no evidence of message filtering in groups |
| See stories | **UNKNOWN** | No block-check found in story preset/publish flow |

**Block detection:**
- The `isBlocked` field in profile model (v7e.java) is visible to the blocker only
- `BlockedGhostAvatarDrawable` replaces avatar — visible indicator for the person who blocked
- **No evidence the blocked user receives explicit "you are blocked" signal** — they get `privacy.violation` errors on calls

**Bot/deeplink bypass:**
- `bot-start-param` PmsKey exists — bots can be started with parameters
- `bots-channel-adding` PmsKey — bots can be added to channels
- **No evidence of block-check in bot message routing or deeplink handling**
- `webapp-phone-hash` — WebApps can access phone hash, potentially bypassing contact blocks

**Vulnerabilities:**
1. **Profile data leaks to blocked users** — avatar URL, title, abbreviation still populated in data model
2. **Online status likely visible** — presence system (`presence-external`, `presence-ttl`) has no block filtering found
3. **Group chat bypass** — `blockedParticipantsCount` is tracked but no message filtering evidence; blocked users may still see messages in shared groups
4. **No bot/deeplink block enforcement** — no evidence that bots or deeplinks check block status before delivering content
5. **Block detection possible** — behavioral differences (call failures with `privacy.violation`, ghost avatar not shown to blocked party) allow inferring block status

---

## Summary

| Feature | Risk Level | Key Issue |
|---------|-----------|-----------|
| migrate-unsafe-warn | MEDIUM | Server can suppress privacy warning during TamTam→Max migration; unsafe file download defaults ON |
| Stories privacy | MEDIUM | No screenshot detection, viewer IDs exposed as raw longs, no view tracking in client |
| Blocked users | HIGH | Profile data leaks, presence visible, group chat bypass likely, no bot/deeplink enforcement |
