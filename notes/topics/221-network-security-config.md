---
tags: [tls, cleartext, mobile-id, operators, security, network-security-config]
status: confirmed
sources:
  - work/apktool_base/res/xml/network_security_config.xml
related:
  - "[[01-mobile-id-cleartext]]"
  - "[[220-quic-tls-disabled]]"
---

# network_security_config.xml — явное разрешение cleartext HTTP

`res/xml/network_security_config.xml` — явно разрешает cleartext (HTTP без TLS) для 6 доменов операторов РФ.

## Домены с разрешённым cleartext HTTP

| Домен | Оператор |
|---|---|
| `mobileid.megafon.ru` | Мегафон — Mobile ID |
| `idgw.mobileid.mts.ru` | МТС — Mobile ID gateway |
| `hhe.mts.ru` | МТС — Header Enrichment |
| `he-mc.tele2.ru` | Tele2 — Header Enrichment |
| `he-mc.t2.ru` | T2 (Tele2) — Header Enrichment |
| `balance.beeline.ru` | Билайн — баланс/Header Enrichment |

`includeSubdomains="false"` — только точные домены, без поддоменов.

## Что важно

1. Это **явное** разрешение в `network_security_config` — не случайность. Разработчики намеренно разрешили HTTP для этих доменов.

2. Подтверждает [[01-mobile-id-cleartext]] — запросы к операторам идут в открытом HTTP.

3. Header Enrichment работает только через HTTP — операторы не могут добавить заголовки в TLS-соединение.

4. `balance.beeline.ru` — интересно: это не только Mobile ID, но и баланс. Возможно, запрашивается баланс счёта.

## Сводка

`network_security_config.xml`: cleartext HTTP явно разрешён для 6 доменов операторов РФ (Мегафон/МТС/Tele2/T2/Билайн). Подтверждение [[01-mobile-id-cleartext]].
