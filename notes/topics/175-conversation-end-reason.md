---
tags: [calls, end-reason, stats, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/events/end/ConversationEndReason.java
related:
  - "[[173-call-finish-init-stat]]"
  - "[[71-call-onelog-events]]"
---

# ConversationEndReason — причины завершения звонка

Полный список причин завершения звонка (отправляется в `call_finish.reason`):

| Ключ | Что |
|---|---|
| `another_device` | принято на другом устройстве |
| `banned` | **заблокирован** |
| `busy` | занято |
| `call_timeout` | таймаут звонка |
| `canceled` | отменён |
| `error` | ошибка |
| `failed` | сбой |
| `hangup` | завершён пользователем |
| `initially_closed` | изначально закрыт |
| `killed` | принудительно завершён |
| `killed_without_delete` | принудительно завершён без удаления |
| `missed` | пропущен |
| `obsolete_client` | устаревший клиент |
| `rejected` | отклонён |
| `removed` | удалён |
| `socket_closed` | соединение закрыто |
| `timeout` | таймаут |

## Что важно

1. **`banned`** — звонок завершён из-за блокировки. Сервер знает, когда пользователь был заблокирован во время звонка.

2. **`killed`** и **`killed_without_delete`** — принудительное завершение. Это server-initiated завершение звонка.

3. **`obsolete_client`** — устаревший клиент. Сервер может завершить звонок из-за устаревшей версии клиента.

4. **`another_device`** — принято на другом устройстве. Подтверждение multi-device функциональности.

## Сводка

17 причин завершения звонка. Ключевые: `banned` (блокировка), `killed` (принудительное завершение сервером), `obsolete_client` (устаревший клиент), `another_device` (multi-device).
