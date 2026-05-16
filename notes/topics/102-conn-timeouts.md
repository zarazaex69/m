---
tags: [network, timeouts, server-control, pms]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/o2.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[52-network-session-tls-flags]]"
---

# conn-timeouts — server-controlled таймауты соединений

`PmsKey.f84conntimeouts` — JSON-объект с таймаутами для разных типов соединений.

## Структура

JSON-объект, где ключи — числовые коды типов соединений (`eg4` enum), значения — JSON-массивы таймаутов (long[], default 10000ms).

Пример: `{"1": [5000, 10000, 30000], "2": [3000, 7000, 15000]}` — для типа соединения 1 три таймаута (вероятно: connect, read, write).

## Что важно

1. **Сервер контролирует таймауты всех типов соединений** — HTTP, WS, и т.п. При низких таймаутах — соединения быстро обрываются. При высоких — клиент долго ждёт ответа.

2. **Default 10000ms** — если сервер не задал конфиг, используется 10 секунд.

3. **EnumMap по типу соединения** — разные таймауты для разных типов. Сервер может задать разные таймауты для WS, HTTP, и других соединений.

## Сводка

`conn-timeouts` — server-pushed JSON с таймаутами для разных типов соединений. Сервер контролирует, как долго клиент ждёт ответа от сервера.
