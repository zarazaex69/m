# VULN-68: Emoji/Unicode Handling Analysis

## Summary

Analysis of emoji and Unicode handling in Max messenger (decompiled from `~/max/decompiled/jadx/base/sources`) reveals several potential vulnerabilities related to message length enforcement, RTL override characters, and lack of Unicode normalization for homograph protection.

---

## 1. Grep Results — Emoji/Unicode References

Key files identified:
```
one/p011me/sdk/emoji/sprite/IllegalWidthSpriteException.java
one/p011me/keyboardmedia/emoji/KeyboardEmojiWidget.java
ru/p027ok/tamtam/animoji/views/AnimojiTextView.java
androidx/emoji2/text/EmojiCompatInitializer.java
androidx/emoji2/widget/EmojiExtractEditText.java
p000/z26.java  (emoji codepoint classification)
p000/f36.java  (EmojiCompat processing)
p000/cx0.java  (surrogate pair iteration)
p000/dx0.java  (bidi directionality detection)
```

Relevant patterns found:
- `charSequence2.codePoints().allMatch(new x26(1))` — emoji-only message detection (C1089c.java:401)
- `Character.codePointBefore`, `Character.charCount` — surrogate pair handling (cx0.java)
- `Character.isSurrogatePair`, `Character.toCodePoint` — UTF-8 encoding (py3.java)
- `Character.getDirectionality` — bidi detection (dx0.java)
- `IDN.toASCII` — domain normalization (xh8.java)

---

## 2. Custom Emoji Rendering

**Finding: YES — Custom emoji rendering via AnimojiTextView and sprite system**

- `AnimojiTextView` (ru/p027ok/tamtam/animoji/views/) extends `AppCompatTextView` with custom `onDraw()` override
- Uses `f36` (EmojiCompat wrapper) to process text with `m6996f(charSequence)` which calls `m6995e()` with `Integer.MAX_VALUE` as maxEmojiCount
- Custom sprite system at `one/p011me/sdk/emoji/sprite/` with `IllegalWidthSpriteException`
- `AnimojiTextView.setText()` processes spans via `f36.m6991a().m6996f(charSequence)` and applies animoji spans

**Risk**: The `f36.m6996f()` method passes `Integer.MAX_VALUE` as the emoji count limit, meaning there is no practical cap on how many emoji are processed in a single text view render pass.

---

## 3. Unicode Normalization / Homograph Attacks

**Finding: VULNERABLE — No Unicode normalization for usernames/display names**

- Domain names use `IDN.toASCII()` (xh8.java:1486) which provides basic punycode conversion
- Chat search uses `normalizedTitle` and `normalizedTitleWithoutEmoji` columns (bj3.java) — but this is for search indexing, not display
- **No evidence of `java.text.Normalizer` (NFC/NFKC) being applied to usernames or display names**
- No confusable character detection found
- The `show-warning-links` PmsKey and `white-list-links` provide some link protection, but no homograph-specific defense

**Attack vector**: A user could register with a visually identical username using Cyrillic/Greek lookalike characters (e.g., "а" U+0430 vs "a" U+0061) to impersonate another user.

---

## 4. Long Emoji Sequence DoS

**Finding: POTENTIALLY VULNERABLE**

- `EmojiExtractEditText` defaults `maxEmojiCount` to `Integer.MAX_VALUE` (EmojiExtractEditText.java:37)
- `f36.m6996f()` processes emoji with `Integer.MAX_VALUE` limit
- `AnimojiTextView.setText()` iterates through all spans with no visible limit on animoji span count
- The emoji-only detection (`codePoints().allMatch()`) processes the entire message stream without early termination bounds
- `z26.m26459b()` iterates through a blocked emoji set comparing codepoint arrays — O(n*m) for each message

**Attack vector**: A message consisting of thousands of ZWJ emoji sequences could cause excessive CPU usage during rendering, especially in `AnimojiTextView` which processes each emoji for custom animoji replacement.

---

## 5. RTL Override Character Spoofing

**Finding: PARTIALLY VULNERABLE**

- `dx0.java` implements bidi directionality detection using `Character.getDirectionality()` — this is used for text layout direction detection, not filtering
- `oqh.java:75` sets text direction based on a boolean flag: `TextDirectionHeuristics.RTL` or `TextDirectionHeuristics.LTR`
- **No evidence of RTL override character (U+202E) stripping or sanitization in message text**
- No InputFilter found that removes bidi control characters (U+200F, U+200E, U+202A-U+202E, U+2066-U+2069)
- The directionality code in `dx0.java` handles embedding levels (cases 14-17 increment `i3` counter) but this appears to be for layout detection, not security filtering

**Attack vector**: An attacker can embed U+202E (RTL Override) in messages to reverse the visual order of subsequent text, potentially making malicious URLs appear as trusted domains or reversing the meaning of messages. Example: `"Click here: ‮moc.elgoog‬.evil.com"` would visually render the domain in reverse.

---

## 6. Message Length Bypass with Multi-byte Characters

**Finding: VULNERABLE — Length enforced by Java char count, not codepoints or bytes**

The `max-msg-length` enforcement uses `String.length()` (Java char count = UTF-16 code units):

```java
// o0i.m15479T0 (o0i.java:314):
public static String m15479T0(int i, String str) {
    int length = str.length();
    if (i > length) { i = length; }
    return str.substring(0, i);
}

// vqg.java:222:
int iM20170m = (int) rtdVar.m20170m(PmsKey.f88745maxmsglength, 4000);
if (ssaVarMo2939x.f65214g.length() > iM20170m) { ... }
```

- Default max message length: **4000** (from `tch.CLOSE_SOCKET_CODE_TIMEOUT = 4000`)
- Enforcement: `String.length()` counts UTF-16 code units
- Supplementary characters (emoji, CJK Extension B, etc.) count as **2** toward the limit
- Basic multilingual plane characters count as **1**

**Attack vector**: If the server enforces length in bytes (UTF-8), a client-side limit of 4000 chars could produce up to 16000 bytes of UTF-8 data (4 bytes per supplementary codepoint × 2 code units × 4000/2 = 8000 codepoints × 4 bytes). However, since supplementary chars count as 2 in `.length()`, the actual maximum is 2000 supplementary codepoints = 8000 UTF-8 bytes. This is a moderate discrepancy if the server expects 4000 bytes.

Additionally, `str.substring(0, i)` can split a surrogate pair if `i` falls between a high and low surrogate, producing malformed UTF-16 that could cause issues downstream.

---

## 7. `max-msg-length` PmsKey Enforcement

**PmsKey Definition** (PmsKey.java:61):
```java
public static final PmsKey f88745maxmsglength = new PmsKey("max-msg-length", 11);
```

**Default value**: 4000 (from `tch.CLOSE_SOCKET_CODE_TIMEOUT`)

**Enforcement locations**:

1. **gv5.java:198** — Message truncation before send:
   ```java
   strM15479T0 = o0i.m15479T0(
       (int) rtdVar.m20170m(PmsKey.f88745maxmsglength, 4000), strM11931b2);
   ```
   Uses `o0i.m15479T0()` which calls `str.substring(0, i)` — **enforced in Java chars (UTF-16 code units)**.

2. **vqg.java:221** — Message splitting for long messages:
   ```java
   int iM20170m = (int) rtdVar.m20170m(PmsKey.f88745maxmsglength, 4000);
   if (ssaVarMo2939x.f65214g.length() > iM20170m) { ... }
   ```
   Uses `String.length()` comparison — **enforced in Java chars (UTF-16 code units)**.
   Splits at newline boundaries (delta 300) or space boundaries (delta 50).

**Critical issue**: The enforcement is purely client-side and uses `.length()` (UTF-16 code units), not:
- Codepoints (`codePointCount()`)
- Bytes (`getBytes("UTF-8").length`)

This means:
- A modified client can bypass the limit entirely
- The char-based truncation in `o0i.m15479T0()` can split surrogate pairs, producing invalid UTF-16
- Server-side enforcement (if any) may use a different metric, creating inconsistencies

---

## Risk Assessment

| Issue | Severity | Exploitability |
|-------|----------|----------------|
| RTL override spoofing | Medium | Easy — no filtering of bidi control chars |
| Homograph attacks (usernames) | Medium | Easy — no normalization on display names |
| Message length surrogate split | Low-Medium | Easy — truncation can produce malformed strings |
| Emoji DoS (rendering) | Low-Medium | Moderate — requires long emoji sequences |
| Client-side length bypass | Low | Easy — modified client, but server may also enforce |
| maxEmojiCount=MAX_VALUE | Low | Moderate — requires many emoji in single view |

---

## Recommendations

1. **Strip or escape bidi override characters** (U+202A-U+202E, U+2066-U+2069) from message text and usernames
2. **Apply Unicode normalization** (NFKC) to usernames/display names and implement confusable detection
3. **Use `codePointCount()` instead of `length()`** for message length enforcement to prevent surrogate pair splitting
4. **Set a reasonable `maxEmojiCount`** (e.g., 100-200) instead of `Integer.MAX_VALUE`
5. **Add server-side length enforcement** that matches the client metric
6. **Validate surrogate pair integrity** before truncation in `o0i.m15479T0()`
