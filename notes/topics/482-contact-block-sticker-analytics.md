---
tags: [contact-block, telemetry, surveillance, contact-analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/xi4.java
  - work/jadx_base/sources/defpackage/n8b.java
related:
  - "[[472-log-controller-event-types]]"
  - "[[06-contacts]]"
---

# CONTACT_OR_BLOCK + sticker.send_sticker — аналитика контактов и стикеров

## CONTACT_OR_BLOCK.clicked

```json
{
  "screen": 400,
  "clickType": "to_contacts" | "block"
}
```

или

```json
{
  "screen": 350,
  "UIElementType": "add_or_block_infobar",
  "clickType": "to_contacts" | "block"
}
```

## CONTACT_OR_BLOCK.showed

```json
{
  "screen": 350,
  "UIElementType": "add_or_block_infobar"
}
```

## sticker.send_sticker

```json
{
  "screen": "first_message"
}
```

Логируется при отправке первого стикера в чате.

## Что важно

1. **`clickType: "block"`** — каждый клик "Заблокировать" логируется.

2. **`clickType: "to_contacts"`** — каждый клик "Добавить в контакты" логируется.

3. **`sticker.send_sticker`** — отправка первого стикера в чате логируется.

## Сводка

`CONTACT_OR_BLOCK.clicked {screen, clickType: to_contacts|block}`. `CONTACT_OR_BLOCK.showed {screen, UIElementType}`. `sticker.send_sticker {screen: first_message}`.
