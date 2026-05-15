# Vuln-51: Complete Deeplink Route Map — Max Messenger

## Summary

Max messenger exposes **109+ deeplink routes** via an exported `LinkInterceptorActivity`. Deeplinks use schemes `max://` and `https://` with host `max.ru`. The router (`gb9.java` / `o95.java`) dispatches URIs to internal screens. Critical finding: the auth check (`mec.m14097a()`) only gates a subset of routes, and several high-risk routes accept URL/file parameters from untrusted input.

## Entry Points

| Component | Exported | Schemes |
|-----------|----------|---------|
| `LinkInterceptorActivity` | **YES** | `http://max.ru`, `https://max.ru`, `max://max.ru` |
| `NewWidgetActivity` | No | Internal only |

## Architecture

- **Router**: `o95.java` — main navigation dispatcher
- **Link Handler**: `gb9.java` — parses URI, resolves to internal route
- **URL Normalizer**: `yc9.java` — converts various URI formats to canonical form
- **Route Definition**: `ps0.m18196A()` (standard) / `ps0.m18199z()` (constrained)
- **Route Model**: `m95.java` — `DeepLinkRoute(deepLinkUri, constraints, requiredParams, supportRoot, bundleRequiredParams)`
- **Constraint**: `h95(0)` = no auth needed, `h95(1)` = `mel.f40304d` = requires auth, `h95(2)` = dev/special

## Auth Check Flow

```
gb9.m8366a() → if mec.m14097a() [NOT logged in] → emit w99(null) [block]
             → else → parse URI → check constraints via mob.m14259c()
             → if constraint fails → redirect to :login
```

## Routes Accessible WITHOUT Authentication

These use `ps0.m18199z()` with `mel.f40304d` (h95 value=1, pre-auth constraint):

| Route | Params | Purpose | Risk |
|-------|--------|---------|------|
| `:auth` | — | Auth/login screen | LOW |
| `:login` | — | Login flow | LOW |
| `:logout` | — | Logout | MEDIUM |
| `:app-update/force` | — | Force update screen | LOW |
| `:webview/faq` | — | FAQ webview | MEDIUM |
| `:twofa/auth/password/check` | track_id, phone | 2FA check during auth | HIGH |

## Routes with Special/Dev Constraints (h95 value=2)

| Route | Params | Purpose | Risk |
|-------|--------|---------|------|
| `:settings/dev` | — | Dev menu | HIGH |
| `:settings/dev/logsviewer` | — | Log viewer | HIGH |
| `:settings/dev/integritylogsviewer` | — | Integrity logs | HIGH |
| `:settings/dev/showroom` | — | UI showroom | LOW |
| `:settings/dev/threadsviewer` | — | Thread viewer | MEDIUM |
| `:settings/dev/memorydebugger` | — | Memory debugger | MEDIUM |
| `:settings/magic-room` | — | Hidden settings | HIGH |
| `:settings/server-host` | — | Change server host | CRITICAL |
| `:settings/server-port` | — | Change server port | CRITICAL |
| `:call-user` | opponent_id | Initiate call to user | HIGH |
| `:call-join-link` | link | Join call by link | MEDIUM |
| `:call-chat` | chat_id | Call from chat | HIGH |
| `:media-picker/select/photo` | — | Photo picker | LOW |

## Routes That Modify App State

| Route | Params | What It Modifies | Risk |
|-------|--------|-----------------|------|
| `:settings/privacy/pincode` | mode | App PIN code | CRITICAL |
| `:settings/privacy/creation-twofa` | track_id, src | 2FA setup | CRITICAL |
| `:settings/privacy/profile-deletion` | — | Account deletion | CRITICAL |
| `:profile/edit` | id, type | Profile editing | HIGH |
| `:profile/edit/link` | id, type, flow | Profile link change | HIGH |
| `:profile/edit/admin_permission` | chat_id, contact_id, permissions_type | Admin perms | CRITICAL |
| `:profile/change-owner` | chat_id | Transfer ownership | CRITICAL |
| `:profile/add-admins` | chat_id | Add admins | HIGH |
| `:profile/add-members` | chat_id, is_chat | Add members | MEDIUM |
| `:settings/folder/create` | — | Create folder | LOW |
| `:settings/folder/edit` | — | Edit folder | LOW |
| `:settings/notifications` | — | Notification settings | LOW |
| `:settings/appearance` | — | Theme settings | LOW |
| `:settings/locale` | — | Language | LOW |
| `:settings/blacklist` | — | Blocklist | MEDIUM |
| `:settings/devices` | — | Device management | HIGH |
| `:logout` | — | Force logout | HIGH |

## Routes with URL/File Parameters (Injection Risk)

| Route | Dangerous Param | Type | Risk |
|-------|----------------|------|------|
| `:link-intercept` | `link` (bundle) | Opens arbitrary URL | CRITICAL |
| `:videoweb/full` | `video_url` (bundle) | Loads video URL in player | HIGH |
| `:dialogs/file-download-warning` | `file_url` (bundle) | Triggers file download | HIGH |
| `:external_callback` | `params` (bundle) | External callback execution | HIGH |
| `:webapp:root` | bot_id, entry_point | Opens WebApp/WebView | HIGH |
| `:photo-editor` | `image_uri` (bundle) | Loads image URI | MEDIUM |
| `:media-editor` | `initial_id` (bundle) | Loads media | MEDIUM |

## All Registered Routes (Complete List)

### Chat Routes
| Route | Params | Risk |
|-------|--------|------|
| `:chats` | id, type | MEDIUM |
| `:chats/share` | — | MEDIUM |
| `:chats/forward` | messages_ids | MEDIUM |
| `:chats/callshare` | — | LOW |
| `:chats-search` | — | LOW |
| `:chat-list` | — | LOW |
| `:chat/add-icon` | — | LOW |
| `:saved-messages` | — | LOW |
| `:scheduled-messages` | id | LOW |

### Profile Routes
| Route | Params | Risk |
|-------|--------|------|
| `:profile` | id, type | MEDIUM |
| `:profile/avatars` | id, type | LOW |
| `:profile/attaches` | id | LOW |
| `:profile/members` | id, type | LOW |
| `:profile/join-requests` | id | LOW |
| `:profile/invite` | id | MEDIUM |
| `:profile/member_permissions` | id | HIGH |
| `:profile/edit/reactions` | id | LOW |

### Call Routes
| Route | Params | Risk |
|-------|--------|------|
| `:call-active` | — | LOW |
| `:call-incoming` | chat_id, call_name | MEDIUM |
| `:call-join-preview` | link | MEDIUM |
| `:call-list` | — | LOW |
| `:call-opponents-list` | — | LOW |
| `:call-pip` | — | LOW |
| `:call-presettings` | chat_id | LOW |
| `:call-rate` | call_id, is_group, is_video | LOW |
| `:call-admin-settings` | — | MEDIUM |
| `:call-admin-waiting-room` | — | LOW |
| `:call-debug-menu` | — | MEDIUM |
| `:call-history-info` | — | LOW |
| `:calls-history` | — | LOW |
| `:call-contact` | — | LOW |
| `:unknown-call` | call_id, caller_id | MEDIUM |

### Settings Routes
| Route | Params | Risk |
|-------|--------|------|
| `:settings` | — | LOW |
| `:settings/aboutapp` | — | LOW |
| `:settings/appearance` | — | LOW |
| `:settings/caching` | — | LOW |
| `:settings/media` | — | LOW |
| `:settings/messages` | — | LOW |
| `:settings/ringtone` | — | LOW |
| `:settings/privacy` | — | MEDIUM |
| `:settings/privacy/onboarding` | — | LOW |
| `:settings/privacy/onboarding-twofa` | state | MEDIUM |
| `:settings/webapps` | — | LOW |
| `:settings/webapp` | bot_id | MEDIUM |
| `:settings/folder-list` | — | LOW |
| `:settings/folder` | id | LOW |
| `:settings/folder/by-chat` | ids | LOW |
| `:settings/folder/settings` | — | LOW |
| `:settings/folder/members-picker` | — | LOW |
| `:settings/notifications/chat` | — | LOW |
| `:settings/notifications/dialog` | — | LOW |
| `:settings/notifications/other` | — | LOW |

### Sticker Routes
| Route | Params | Risk |
|-------|--------|------|
| `:stickers/showcase` | — | LOW |
| `:stickers/search` | — | LOW |
| `:stickers/preview` | sticker_id | LOW |
| `:stickers/set` | set_id | LOW |
| `:stickers/settings` | — | LOW |
| `:stickers/recent` | — | LOW |
| `:stickers/favorite` | — | LOW |

### Other Routes
| Route | Params | Risk |
|-------|--------|------|
| `:contact-list` | — | LOW |
| `:contact-list/create-contact` | — | LOW |
| `:contact-list/share-invite` | — | LOW |
| `:contacts-picker` | request_code | LOW |
| `:contact/add/dialog` | contact_id | LOW |
| `:share` | text | MEDIUM |
| `:join` | id, link | MEDIUM |
| `:invite/phone` | — | LOW |
| `:invite/qr` | — | LOW |
| `:invite/friends_to_max_bottom_sheet` | — | LOW |
| `:start-conversation` | — | LOW |
| `:start-conversation/chat` | — | LOW |
| `:start-conversation/channel` | — | LOW |
| `:start-conversation/add-subscribers` | id | LOW |
| `:story-publish` | — | LOW |
| `:story-publish/picker` | title | LOW |
| `:location/pick` | chat_id, request_code | LOW |
| `:location/show` | chat_id, lat, lon, z | LOW |
| `:polls/create` | chat_id, request_code | LOW |
| `:polls/result` | chat_id, message_id, poll_id | LOW |
| `:polls/result/voters` | chat_id, message_id, poll_id, answer_id | LOW |
| `:attach/viewer` | chat_id, attach_id, msg_id | LOW |
| `:complaint` | — | LOW |
| `:dialogs/share-media` | msg_id, attach_id, local_attach_id, cause_ordinal | LOW |
| `:inAppReview/fake` | — | LOW |
| `:neuro-avatars` | — | LOW |
| `:qr-scanner` | — | LOW |
| `:twofa/password/check` | — | MEDIUM |

## Parameter Injection Vulnerabilities

### 1. `:link-intercept` — Open Redirect / Arbitrary URL Load
- Bundle param `link` passed as Parcelable URI
- No URL validation visible in `b99.java`
- Can open arbitrary URLs in app context

### 2. `:videoweb/full` — Arbitrary Video URL Load
- `video_url` bundle param loaded into video player
- Could load malicious media or trigger SSRF

### 3. `:dialogs/file-download-warning` — Arbitrary File Download
- `file_url` bundle param triggers download dialog
- Combined with `file_name` could social-engineer user into downloading malicious files

### 4. `:webapp:root` — WebView with Bot Context
- `bot_id` + `entry_point` opens WebApp WebView
- Potential for JavaScript bridge exploitation if bot_id is attacker-controlled

### 5. `:external_callback` — External Callback Execution
- `params` bundle param triggers external callback logic
- Opaque parameter handling in `ExternalCallbackWidget`

### 6. `startapp` Query Parameter
- On any single-path-segment URL (e.g., `https://max.ru/botname?startapp=payload`)
- Payload passed to bot as `startPayload` — no sanitization visible
- Truncated at `&` character but no other validation

## Key Security Findings

1. **Exported Activity**: `LinkInterceptorActivity` is exported with broad intent filters accepting any path on max.ru
2. **Scheme Confusion**: Accepts `max://`, `http://`, `https://` — normalization in `yc9.m25902e()` could be bypassed
3. **Auth Bypass Risk**: Routes registered with `ps0.m18196A()` (flag 14) vs `ps0.m18199z()` (flag 10/2) have different constraint levels; the constraint check in `o95` falls through to `:login` redirect but the URI is already parsed
4. **No Input Validation**: Query parameters like `id`, `chat_id`, `opponent_id` are parsed via `Long.parseLong()` with only NumberFormatException catch — no bounds checking
5. **Bundle Params Bypass**: Routes using `Collections.singleton("param")` for bundle-required params receive data from the calling intent's extras — externally injectable via crafted intents
6. **Dev Routes Accessible**: If constraint check for h95(2) can be bypassed, server host/port can be changed to attacker-controlled server
