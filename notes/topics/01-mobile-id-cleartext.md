---
tags: [auth, cleartext, header-enrichment, surveillance, mobile-id]
status: confirmed
sources:
  - work/apktool_base/res/xml/network_security_config.xml
  - work/apktool_base/AndroidManifest.xml
related:
  - "[[10-webapp-jsbridge]]"
  - "[[13-deeplinks-idp]]"
  - "[[04-telemetry-endpoints]]"
---

# Mobile ID: вход по номеру в чистом HTTP

## Где это лежит

`apktool_base/res/xml/network_security_config.xml`:

```xml
<network-security-config>
    <domain-config cleartextTrafficPermitted="true">
        <domain includeSubdomains="false">mobileid.megafon.ru</domain>
        <domain includeSubdomains="false">idgw.mobileid.mts.ru</domain>
        <domain includeSubdomains="false">hhe.mts.ru</domain>
        <domain includeSubdomains="false">he-mc.tele2.ru</domain>
        <domain includeSubdomains="false">he-mc.t2.ru</domain>
        <domain includeSubdomains="false">balance.beeline.ru</domain>
    </domain-config>
</network-security-config>
```

Все остальные домены под общим запретом cleartext, эти шесть — единственные исключения. Это не баг, это отдельный whitelist.

## Что это за схема

Это так называемый **Header Enrichment / Mobile ID** у мобильных операторов РФ:

- `mobileid.megafon.ru` — Mobile ID Мегафона
- `idgw.mobileid.mts.ru`, `hhe.mts.ru` — MTS («Mobile ID» и Header Enrichment)
- `he-mc.tele2.ru`, `he-mc.t2.ru` — Tele2 (после перехода на бренд T2)
- `balance.beeline.ru` — Beeline (используется в том числе для авто-определения номера)

Когда пользователь сидит через мобильную сеть оператора, оператор сам вписывает в HTTP-заголовки ответа MSISDN/IMSI/связанные идентификаторы абонента (header enrichment). Сервер MAX, видя эти заголовки, понимает, **кто** это, БЕЗ ввода SMS-кода.

Чтобы это работало, запросы обязаны идти именно в **открытом HTTP**: TLS-соединение оператор не может «вскрыть» и подставить заголовки в полезную нагрузку (он видит только Server Name Indication и адрес). Поэтому в MAX этим шести доменам прямо разрешён cleartext в `network-security-config`.

## Что это значит на практике

1. **«Бесшовный вход» по номеру телефона** — рекламная фича для пользователя. Оператор делает HE, MAX без SMS даёт сессию.
2. **Канал прослушивается** — между телефоном и оператором (Wi-Fi, прокси, шпион в сети) и на самом операторе всё видно в чистом виде. По сути это намеренная регрессия безопасности ради удобства интеграции с операторами.
3. **Cleartext HTTP с SIM-идентификатором** — это и есть тот самый «вход по номеру», ради которого и существует Mobile ID. И это же — wiretap-friendly канал: на пути виден сам факт авто-регистрации в MAX, и часто видно сами параметры.
4. **Включает Tele2 «двойной» бренд** — `he-mc.tele2.ru` и `he-mc.t2.ru` рядом. Это просто две точки входа одного оператора (миграция бренда), не самостоятельный сервис.

## Где это используется в коде

В коде ищется по строкам `mobileid`/`Mobile ID`. Реализация заходит через стандартные эндпоинты HE (специфические для каждого оператора) и парсит заголовки/payload. Mobile ID завязан на стандартный ETSI-MEC-style API; основной поставщик SDK — сами операторы (MTS-DTPP, Megafon DPN, Beeline OpenId, Tele2 ID). MAX встраивает их как cleartext-исключения, чтобы Android начиная с 9.0 не блокировал.

## Кратко для статьи

> «Внутри MAX в `network_security_config.xml` шесть доменов — мобильных операторов РФ — добавлены в white-list для HTTP без TLS. Это нужно, чтобы оператор поверх запроса смог дописать в заголовки идентификатор абонента (Mobile ID/Header Enrichment) и сервер MAX опознал владельца SIM без SMS. Цена этого — авторизационный канал в открытом HTTP: оператор и любой посередине видит, что вы залогинились, и часто — какой именно номер.»
