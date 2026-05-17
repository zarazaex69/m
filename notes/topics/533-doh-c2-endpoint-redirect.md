---
tags: [critical, dns, doh, c2, server-control, network, api-discovery, censorship-bypass]
status: verified
severity: high
sources:
  - work/jadx_base/sources/defpackage/xl2.java
  - findings/raw/all_urls_and_apis.txt
related:
  - "[[227-dns-over-https-endpoint-discovery]]"
  - "[[03-pms-server-flags]]"
  - "[[12-force-update-killswitch]]"
  - "[[495-server-host-selector]]"
---

# 533. DNS-over-HTTPS как скрытый C2-редирект

## Суть

Приложение резолвит TXT-запись `api._endpoint.ok.ru.` через Google DoH (`https://dns.google.com/resolve`). Результат используется как **альтернативный API-endpoint**. Это позволяет серверной инфраструктуре динамически перенаправить всех клиентов на произвольный сервер — без обновления приложения, без изменения кода, без уведомления пользователя.

## Механизм

```
Клиент MAX
    │
    │  GET https://dns.google.com/resolve?name=api._endpoint.ok.ru.&type=16
    ▼
Google DoH возвращает TXT-запись
    │
    │  TXT: "api-new.oneme.ru" (или любой другой хост)
    ▼
Клиент переключается на новый API-endpoint
```

## Зачем это

1. **Обход блокировок** — если `api.oneme.ru` заблокирован провайдером, клиент автоматически найдёт новый endpoint через Google DoH (который сам не заблокирован)
2. **Скрытая миграция** — перевести всех пользователей на другой сервер без релиза
3. **C2-паттерн** — классическая техника malware для обнаружения C&C-сервера через DNS

## Что делает это опасным

- Google DoH (`dns.google.com`) практически невозможно заблокировать без поломки интернета
- TXT-запись может содержать **любой** URL — включая IP-адрес или .onion
- Переключение происходит **прозрачно** для пользователя
- Включается серверным PmsKey `net-client-dns-enabled`
- В сочетании с отсутствием certificate pinning на QUIC (topic 220) — новый endpoint может быть чем угодно

## Контекст

Это не уникально для MAX — Telegram использует похожий механизм. Но в сочетании с:
- Отсутствием E2E шифрования
- QUIC TLS bypass (null TrustManager)
- 334 серверными флагами
- Wiretap chain

...это означает, что даже если пользователь контролирует DNS на своём роутере, приложение обойдёт это через Google DoH и подключится к серверу, который укажет OK.ru-инфраструктура.

## Статус в 26.16.0

Без изменений.
