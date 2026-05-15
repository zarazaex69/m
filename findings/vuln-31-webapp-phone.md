# VULN-31: WebApp Phone Hash System Analysis

## Summary

Max messenger's WebApp (mini-app) platform allows bots to request user phone numbers via the `WebAppRequestPhone` JS bridge event. A feature flag `webapp-phone-hash` (PmsKey) controls whether the phone is sent as a hash or in plaintext. When enabled, the server computes the hash; when disabled, the **actual phone number is sent directly to the bot**.

## Architecture

### Key Classes (Decompiled)

| Class | Role |
|-------|------|
| `ogk` | Enum `REQUEST_PHONE` (opcode 55), JS bridge event name "WebAppRequestPhone" |
| `sgk` | JS bridge delegate handler for `WebAppRequestPhone` |
| `vgk` | `WebAppRequestPhoneRequest(requestId)` — bot's request payload |
| `ygk` | `WebAppRequestPhoneResponse(requestId, phone, hash, authDate)` — response sent to bot |
| `zgk` | `WebAppRequestPhoneSuccess(phone, hash, authDate)` — internal success model |
| `klk` | Server `Response(phone, hash, authDate)` — server API response |
| `ijk` | Main WebApp ViewModel, method `m10445F` = `onRequestPhoneResult(boolean)` |
| `uik` | Coroutine that fetches phone data from server or local profile |
| `hik` | Singleton event `ShowPhoneConfirmationDialog` |
| `ghk` | UI handler that shows the ConfirmationBottomSheet |
| `qgk` | Callback that serializes `ygk` response and sends it to the WebView |

### Flow

```
Bot JS → "WebAppRequestPhone" → sgk.m21249f() → npf (consent trigger)
  → xjd (event processor) detects npf → shows hik (ShowPhoneConfirmationDialog)
  → ghk shows ConfirmationBottomSheet (dialog_id=2)
  → User accepts (result=1) → ijk.m10445F(true)
  → uik coroutine executes:
      if (useWebAppPhoneHash == true):
          server request via ucc.m22790F() with rkj(PHONE_WEBAPP_SHARE, botId)
          → server returns klk(phone, hash, authDate)
      else:
          local profile lookup via tje.m21962a(userId)
          → returns q7e profile, extracts phone as String.valueOf(profile.phone)
          → zgk(null, phoneString, null) — NO hash, NO authDate
  → qgk serializes ygk(requestId, phone, hash, authDate) → sends to WebView JS
```

## Findings

### 1. Hash Algorithm

**The hash is computed SERVER-SIDE, not on the client.** When `webapp-phone-hash` is enabled:
- Client sends `rkj` request with opcode `PHONE_WEBAPP_SHARE` (u0d.f69181p2, type 106) and `botId` parameter
- Server returns `klk` containing `phone`, `hash`, and `authDate`
- The client does NOT compute the hash locally

The client-side code contains `HmacSHA256` references (in `kcb.java`, `xr0.java`) but these are for TLS/HKDF operations, not phone hashing. The phone hash algorithm is opaque to the client — it's whatever the server implements.

### 2. Can Bots Get the Actual Phone Number?

**YES — in TWO scenarios:**

1. **When `webapp-phone-hash` feature flag is DISABLED (default unclear):** The `uik` coroutine takes the `else` branch, fetches the user's local profile via `tje.m21962a()`, and returns the **raw phone number** as a string with **no hash and no authDate**:
   ```java
   // uik.java line ~91
   yy8Var.m26324a(new zgk(null, String.valueOf(((q7e) obj).f55801d.m5499s()), null));
   ```
   This sends `zgk(authDate=null, phone=PLAINTEXT, hash=null)`.

2. **When `webapp-phone-hash` is ENABLED:** The server response (`klk`) contains BOTH `phone` AND `hash` fields. The `ygk` response sent to the bot WebView includes the phone field:
   ```java
   // qgk.java
   ygk ygkVar = new ygk(requestId, zgkVar.phone, zgkVar.hash, String.valueOf(zgkVar.authDate));
   ```
   The response structure `WebAppRequestPhoneResponse(requestId, phone, hash, authDate)` sends the **actual phone number alongside the hash**.

### 3. User Consent

**YES — consent is required.** The flow is:
1. Bot triggers `WebAppRequestPhone` via JS bridge
2. `sgk` handler creates an `npf` (consent action) and suspends on it
3. `xjd` event processor detects `npf` and emits `hik` (ShowPhoneConfirmationDialog)
4. `ghk` shows a `ConfirmationBottomSheet` with:
   - Title: `phone_request_dialog_title`
   - Accept button: resource `pzf.f55084S2`
   - Decline button: `phone_request_dialog_decline`
5. User must tap Accept (result=1) for `m10445F(true)` to proceed
6. If user declines (result=2), `m10445F(false)` is called → `lgk` (extends `ngk` extends `Throwable`) is sent as error

### 4. WebAppRequestPhone Event Handler

Located in `sgk.java` (package `p000`):
- Original class: `one.me.webapp.domain.jsbridge.delegates.phone.WebAppRequestPhoneRequest`
- Implements `lz8` (JsDelegate interface)
- Method `m21249f()` handles the coroutine flow:
  1. Deserializes `vgk` (request with requestId)
  2. Creates `npf` consent action and suspends
  3. On consent, creates `qgk` callback that fetches phone data
  4. Sends response via `zy8("WebAppRequestPhone", serialized_ygk)`

### 5. Phone Number Enumeration via Hash

**LOW RISK if server-side hash is properly salted.** However:
- Phone numbers have limited entropy (~10-12 digits, with country code constraints)
- If the hash is unsalted (or uses a global salt), a rainbow table of all valid phone numbers could be precomputed
- The `authDate` field suggests time-binding but doesn't prevent precomputation if the hash input is just `phone + botId`
- **The client code provides no evidence of per-user salt** — the only parameter sent to the server is `botId`

### 6. Salt Analysis

**No per-user salt visible in client code.** The request to the server (`rkj`) only includes:
- `botId` (via `m18551h(j, "botId")`)
- The request type `PHONE_WEBAPP_SHARE`

The server MAY use a per-bot secret or per-user salt internally, but the client has no visibility into this. The response contains only `phone`, `hash`, and `authDate` — no salt field is returned.

### 7. Feature Flag Behavior

The `webapp-phone-hash` PmsKey (index 329) is a **boolean** feature flag:
```java
// qp6.java
io6.m10623e(qp6.class, "useWebAppPhoneHash", "getUseWebAppPhoneHash()Z", 0)
```

When `false`, the phone is sent in **plaintext with no hash**. This is a server-controlled A/B flag — the server decides per-user whether hashing is active.

## Vulnerabilities

### CRITICAL: Phone Number Exposed in Both Modes

Even when `webapp-phone-hash` is enabled, the `ygk` response object contains BOTH `phone` (plaintext) AND `hash`. The bot receives the actual phone number regardless of the hash feature flag. The hash appears to be for **verification/integrity** (similar to Telegram's `data_check_string` pattern), NOT for privacy.

### HIGH: No Hash When Feature Flag Disabled

When `useWebAppPhoneHash == false`, the phone is fetched from the local profile and sent with no hash and no authDate. Any bot can receive the raw phone number after user consent.

### MEDIUM: Consent Bypass Not Observed

The consent dialog IS enforced — the `npf` action suspends the coroutine until user interaction. However, the dialog text (`phone_request_dialog_title`) may not clearly communicate that the **actual phone number** (not just a hash) is being shared.

### LOW: No Rate Limiting Visible

No client-side rate limiting on `WebAppRequestPhone` requests is visible. A bot could repeatedly prompt the user for phone sharing. The only protection is the user declining.

## Conclusion

The `webapp-phone-hash` system is **NOT a privacy-preserving mechanism**. It's a data integrity/verification feature (like HMAC signing). In ALL cases where the user consents, the **actual phone number is transmitted to the bot**. The hash serves as proof that the phone number was legitimately provided by the platform (preventing bot-side forgery), not as a way to hide the phone from the bot.

This is architecturally similar to Telegram's `requestContact` where the bot receives the full phone number with an HMAC for verification.
