---
tags: [dns, doh, google-dns, api-discovery, network]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/xl2.java
related:
  - "[[24-host-reachability-checker]]"
  - "[[04-telemetry-endpoints]]"
---

# DNS over HTTPS — обнаружение API endpoint через Google DNS

`xl2.java` содержит метод `u()` — запрос к Google DNS over HTTPS для обнаружения API endpoint.

## Запрос

```
GET https://dns.google.com/resolve?name=api._endpoint.ok.ru.&type=16
```

- `type=16` — TXT-запись DNS
- Таймаут: 3000 мс (connect + read)
- Парсит `Answer[0].data` из JSON-ответа

## Что важно

1. **`api._endpoint.ok.ru.`** — TXT-запись DNS используется для динамического обнаружения API endpoint. Сервер может изменить endpoint через DNS без обновления приложения.

2. **Google DNS** (`dns.google.com`) — используется вместо системного DNS. Это обход возможной блокировки DNS на уровне провайдера.

3. **TTL** — время жизни записи учитывается (`jSONObject.getInt("TTL")`).

4. Это механизм failover/discovery — если основной API недоступен, приложение находит новый через DNS TXT.

## Сводка

`https://dns.google.com/resolve?name=api._endpoint.ok.ru.&type=16` — DoH запрос для обнаружения API endpoint. Обход DNS-блокировок через Google DNS.
