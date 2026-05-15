# Vulnerability 60: Custom URI Scheme Handlers Analysis

## Summary

Max messenger registers multiple URI schemes and has an extensive internal deeplink routing system with potential scheme confusion and injection vectors.

## 1. Registered URI Schemes (AndroidManifest.xml)

### Externally-Handled Schemes (in `<queries>`)
- `yandexmaps://` - Yandex Maps
- `yandexnavi://` - Yandex Navigator
- `dgis://` - 2GIS Maps
- `petalmaps://` - Petal Maps
- `https://` - Web browsing

### App-Owned Schemes (in `<intent-filter>` on `LinkInterceptorActivity`)

**Filter 1 - Web links (autoVerify=true):**
- Schemes: `http`, `https` (via `@string/web_scheme`)
- Host: `max.ru` (via `@string/app_host`)
- Path: `/..*` (any path with content)

**Filter 2 - Custom scheme:**
- Scheme: `max` (via `@string/app_scheme`)
- Host: `max.ru` (via `@string/app_host`)

### String Resource Values
- `app_scheme` = `max`
- `web_scheme` = `https`
- `app_host` = `max.ru`

## 2. Internal URI Scheme (Deeplink Routes)

Internal routes use colon-prefixed paths (e.g., `:chats`, `:settings`). The `qcc.m18814h()` method converts these to `max://` URIs with null authority.

### Complete Route Registry (from `ps0.m18196A` calls):

| Route | Parameters |
|-------|-----------|
| `:webapp:root` | bot_id, entry_point |
| `:contact-list` | - |
| `:call-list` | - |
| `:chat-list` | - |
| `:settings` | - |
| `:settings/privacy` | - |
| `:settings/blacklist` | - |
| `:settings/privacy/onboarding` | - |
| `:settings/privacy/pincode` | mode |
| `:settings/folder-list` | - |
| `:settings/folder` | id |
| `:settings/folder/edit` | - |
| `:settings/folder/create` | - |
| `:settings/folder/by-chat` | ids |
| `:settings/folder/settings` | - |
| `:settings/folder/members-picker` | - |
| `:settings/messages` | - |
| `:settings/appearance` | - |
| `:settings/aboutapp` | - |
| `:settings/ringtone` | - |
| `:settings/locale` | - |
| `:settings/notifications` | - |
| `:settings/devices` | - |
| `:settings/media` | - |
| `:settings/caching` | - |
| `:settings/webapp` | - |
| `:settings/webapps` | - |
| `:chats` | id, type |
| `:chats/callshare` | - |
| `:chats/forward` | messages_ids |
| `:chats-search` | - |
| `:chats/share` | - |
| `:profile` | id, type |
| `:profile/avatars` | id, type |
| `:profile/attaches` | id |
| `:profile/members` | id, type |
| `:profile/add-admins` | - |
| `:profile/add-members` | - |
| `:profile/change-owner` | chat_id |
| `:profile/edit` | - |
| `:profile/invite` | id |
| `:profile/join-requests` | - |
| `:stickers/showcase` | chat_id |
| `:stickers/set` | set_id |
| `:stickers/favorite` | - |
| `:stickers/preview` | - |
| `:stickers/recent` | - |
| `:stickers/search` | - |
| `:stickers/settings` | - |
| `:polls/create` | chat_id, request_code |
| `:polls/result` | chat_id, message_id, poll_id |
| `:polls/result/voters` | chat_id, message_id, poll_id, answer_id |
| `:calls-history` | - |
| `:call-history-info` | - |
| `:call-presettings` | chat_id |
| `:share` | text |
| `:start-conversation` | - |
| `:story-publish` | - |
| `:link-intercept` | link (bundle) |
| `:photo-editor` | image_uri (bundle) |
| `:media-editor` | initial_id (bundle) |
| `:qr-scanner` | - |
| `:inAppReview/fake` | - |

### Special Routes in gb9 handler:
- `max://max.ru` / `max://max.ru/` → opens home screen
- `https://max.ru/:share-self-out` → share profile
- `https://max.ru/:auth` → authentication flow
- `:current` → current screen context
- `:folder` → folder navigation
- `join/<hash>` → join group chat
- `joincall/<hash>` → join call
- `stickerset/<id>` → sticker set
- `@username` → profile lookup
- `?startapp=<param>` → mini-app launch

## 3. External Trigger Analysis

**YES - Internal routes CAN be triggered externally:**

The `LinkInterceptorActivity` is `exported="true"` and handles:
- `max://max.ru/<any_path>` - directly triggers deeplink routing
- `https://max.ru/<any_path>` - triggers deeplink routing after normalization

The `yc9.m25902e()` URI normalizer in the deeplink handler:
1. If URI starts with `:` or `max://:` → converts to internal route
2. If URI starts with `@` → treated as username lookup
3. If no scheme present → prepends `https://`
4. Otherwise → passes through as-is

**Attack vector:** An attacker can craft `max://max.ru/:settings/privacy` or `https://max.ru/:chats?id=<target_id>&type=local` to navigate the victim to arbitrary internal screens.

## 4. Scheme Confusion Vulnerabilities

### Finding: HTTP/HTTPS/Max Equivalence
The `gb9` handler treats all three schemes equivalently for `max.ru` host:
```java
// All resolve to the same home screen:
"max.ru"
"http://max.ru"
"https://max.ru"
"max://max.ru"
```

### Finding: No autoVerify on max:// scheme filter
The `max://` intent-filter does NOT have `android:autoVerify="true"`, meaning any app can register to handle `max://` URIs and intercept them.

### Finding: Fallback to https:// for unknown URIs
In `yc9.m25902e()`:
```java
if (!string.contains("://") && TextUtils.isEmpty(uri.getScheme())) {
    return Uri.parse("https://".concat(string));
}
```
A bare string without scheme gets `https://` prepended, potentially redirecting to attacker-controlled domains.

## 5. WebView Scheme Injection Analysis

### Finding: WebApp URL Interceptor (ppk.java) - Intent Launch for Non-HTTP Schemes
```java
public boolean mo6702y(Uri uri) {
    if (zm0.m27177c(uri.getScheme(), "http") || zm0.m27177c(uri.getScheme(), "https")) {
        return false;  // Allow http/https to load in WebView
    }
    // ANY other scheme → launch as Intent
    r16Var.f58701a.startActivity(new Intent("android.intent.action.VIEW", uri));
    return true;
}
```
**Vulnerability:** No filtering of dangerous schemes. A malicious page in the WebApp WebView can trigger `intent://`, `file://`, or other dangerous scheme URIs via navigation. The only check is "not http/https → launch intent".

### Finding: FaqWebViewWidget (nk6.java) - Limited Scheme Handling
- Handles `mailto:` → opens email
- Handles `max:` scheme → calls `startActivity(ACTION_VIEW, uri)` 
- Handles `https:` with file extensions → opens externally
- **No explicit blocking of `javascript:` or `data:` schemes**

The `shouldOverrideUrlLoading` in nk6 only checks for `mailto` and `max` schemes explicitly. If a `javascript:` or `data:` URI is navigated to and doesn't match these checks, it falls through to `return false` which allows the WebView to load it.

### Finding: data: URI Processing (f55.java)
The image loading system processes `data:` URIs with base64 decoding without origin validation, used in image rendering contexts.

## 6. Additional Internal Schemes

- `ok://api` - Internal OK API scheme (from `agl.java`)
- `content://ru.oneme.app.provider` - FileProvider (not exported)
- `content://ru.oneme.app.notifications` - Notification images (not exported)
- `content://downloads/public_downloads` - System downloads access

## 7. Risk Assessment

| Issue | Severity | Impact |
|-------|----------|--------|
| External deeplink to internal screens | Medium | Navigate victim to settings/profile/chat screens |
| WebApp intent launch without scheme filter | High | Arbitrary intent launch from WebView content |
| No javascript:/data: blocking in FAQ WebView | Medium | Potential XSS in FAQ WebView context |
| HTTP scheme accepted alongside HTTPS | Low | Downgrade possible but autoVerify mitigates |
| max:// not autoVerified | Medium | Other apps can intercept max:// links |
| URI normalization adds https:// to bare strings | Low | Open redirect potential |

## 8. Proof of Concept

### Navigate victim to arbitrary internal screen:
```
adb shell am start -a android.intent.action.VIEW -d "max://max.ru/:settings/privacy"
```

### Trigger chat open with specific ID:
```
adb shell am start -a android.intent.action.VIEW -d "max://max.ru/:chats?id=12345&type=local"
```

### WebApp arbitrary intent (from within WebView):
```html
<a href="intent://evil.com#Intent;scheme=http;component=com.example/.Evil;end">Click</a>
```
