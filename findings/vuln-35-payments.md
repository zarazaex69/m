# VULN-35: Payment/Money Transfer System Analysis

## Overview

Max messenger implements money transfers via a WebApp bot integration. The bot ID is configured server-side via PmsKey `money-transfer-botid` (stored as `f88768moneytransferbotid` in `PmsKey.java:272`). The payment UI is a WebView-based webapp launched with `entry_point=money_button`.

## Architecture

- **PmsKey**: `money-transfer-botid` → Long value (bot ID), fetched via `rtd.f61509E` (case 2 in `mbg.java` = Long type, default 0L)
- **Entry point**: `de7.java:279-287` — builds URL `:webapp:root?bot_id={moneyBotId}&entry_point=money_button&source_id={chatId}&request_code=1010`
- **WebView bridge**: `upk.java` (WebViewHandler) and `i6e.java` (PrivateWebViewHandler) expose `postEvent()` to JavaScript
- **Event dispatcher**: `ijk.m10440A()` processes JS bridge events
- **Biometry auth**: `WebAppRootScreen.java:968` — BiometricPrompt integration for payment confirmation

## Key Files

| File | Role |
|------|------|
| `p000/rtd.java:204` | PmsKey money-transfer-botid registration |
| `p000/de7.java:279-287` | Money button click handler, webapp launch |
| `p000/aca.java:749` | Money button visibility logic |
| `p000/ijk.java` | WebApp JS bridge event handler |
| `p000/kka.java` | MONEY enum (ordinal 4) in attachment picker |
| `p000/b6k.java:50` | MONEY_BUTTON entry point enum |
| `p000/ahk.java:102-105` | WebViewHandler/PrivateWebViewHandler injection |

## Vulnerability Analysis

### 1. Bot ID Manipulation (Medium Risk)

**Finding**: The money transfer bot ID is fetched from PMS (server config) as a Long with default 0L. The client passes it directly into the webapp URL without validation.

```java
// de7.java:279-287
long jLongValue2 = ((Number) rtdVar.f61509E.mo677x(rtdVar, rtd.f61504e0[20])).longValue();
StringBuilder sb = new StringBuilder(":webapp:root?bot_id=");
sb.append(jLongValue2);
sb.append("&entry_point=money_button&source_id=");
```

**Issue**: If an attacker can intercept/modify PMS responses (MITM on config delivery), they could redirect money transfers to a malicious bot. The bot_id is not pinned or validated client-side.

### 2. Private Bridge Access (Medium-High Risk)

**Finding**: The PrivateWebViewHandler is added based on a server-side whitelist (`webapp-exc` PmsKey → `f57403L0`):

```java
// ahk.java:104-105
if (webAppRootScreen4.m17030s1().f27771b1) {
    v9gVar.addJavascriptInterface(new i6e(...), "PrivateWebViewHandler");
}
```

The whitelist check (`ijk.java:290`): `((ynb) qp6Var.f57388H1.mo677x(...)).m26061d(j)` determines if a bot gets private bridge access. The money transfer bot likely has this privilege, giving it elevated JS bridge capabilities.

**Issue**: If the bot ID is spoofed (see #1), a malicious webapp could gain private bridge access intended only for the legitimate payment bot.

### 3. User Click Timing Bypass (Low-Medium Risk)

**Finding**: Certain sensitive events require a user click within 3000ms (`ijk.java:447-451`):

```java
if (!AbstractC0585lw.m13761s0(this.f27770b, ...) && f27730L1.contains(str) 
    && System.currentTimeMillis() - this.f27748I1 >= 3000) {
    // "Did not execute js bridge method: no user click in the last 3000 ms"
    return;
}
```

Protected events: `WebAppMaxShare`, `WebAppShare`, `WebAppDownloadFile`, `WebAppOpenLink`, `WebAppOpenMaxLink`.

**Issue**: Payment-related events are NOT in this protected set. The money transfer webapp can invoke bridge methods without requiring a recent user click, potentially allowing automated actions within the payment flow.

### 4. No Client-Side Amount Validation (By Design, but Notable)

**Finding**: No payment amount, currency, or transaction parameters are visible in the client code. The entire payment flow is handled within the WebView webapp. The client only:
- Launches the webapp with bot_id + source_id (chat context)
- Provides biometric authentication when requested
- Receives a result via request_code 1010

**Assessment**: This is actually a reasonable design — amounts should be validated server-side. However, it means the client cannot detect if the webapp has been compromised to show different amounts than what's processed.

### 5. Double-Spend / Race Condition (Low Risk - Insufficient Evidence)

**Finding**: No client-side payment confirmation logic, transaction IDs, or idempotency tokens were found in the decompiled code. The payment flow is entirely within the webapp/server. Without server-side code, a double-spend vulnerability cannot be confirmed or denied from client analysis alone.

### 6. Bot-Initiated Payments Without Consent (Medium Risk)

**Finding**: The money button visibility in `aca.java:749` requires:
- `moneyBotId != 0` (bot configured)
- `z3` = chat is writable and not a bot chat (`fr2.m7782Z() && !fr2.m7776T()`)
- `z2` = chat has participants (`fr2.mo7818u() != 0`)
- `!zM24560e` = not a scheduled message context

However, the webapp can be opened programmatically via deeplink `:webapp:root?bot_id=X&entry_point=money_button`. If a malicious message contains a crafted internal link, it could open the payment webapp without the user explicitly pressing the money button.

### 7. Biometry Token Storage (Medium Risk)

**Finding**: Webapp biometry tokens are stored in SQLite:
```sql
CREATE TABLE webapp_biometry (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 
  user_id INTEGER NOT NULL, bot_id INTEGER NOT NULL, token TEXT, 
  access_requested INTEGER NOT NULL, access_granted INTEGER NOT NULL)
```

The token is updatable: `UPDATE webapp_biometry SET token = ? WHERE user_id = ? AND bot_id = ?`

**Issue**: If a device is rooted or the database is accessible, biometry tokens for the payment bot could be extracted and replayed. The token appears to be a simple text value without hardware-backed key binding.

### 8. No Subscription/Auto-Payment Mechanism Found

No evidence of recurring payment, subscription, or auto-debit functionality was found in the client code. The payment system appears to be single-transaction only, triggered by explicit user action (money button press).

## Summary of Risks

| # | Vulnerability | Severity | Exploitability |
|---|---|---|---|
| 1 | Bot ID not pinned client-side | Medium | Requires MITM on PMS |
| 2 | Private bridge escalation via bot ID spoof | Medium-High | Chained with #1 |
| 3 | Payment events bypass click timing check | Low-Medium | Requires webapp compromise |
| 4 | No client-side amount verification | Informational | By design |
| 5 | Double-spend | Unknown | Server-side only |
| 6 | Deeplink can open payment webapp | Medium | Social engineering |
| 7 | Biometry token in plaintext SQLite | Medium | Requires root/physical access |
| 8 | Auto-payment abuse | N/A | Not implemented |

## Recommendations

1. Pin the money-transfer-botid value or validate it against a hardcoded allowlist
2. Add payment-related webapp events to the user-click-required set (`f27730L1`)
3. Use hardware-backed keystore for biometry tokens instead of plaintext SQLite
4. Validate deeplink-triggered payment webapp launches require explicit user confirmation
5. Consider certificate pinning for PMS config delivery to prevent bot ID MITM
