---
tags: [protocol, api, exceptions, session, nonce, opcode, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/api/
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[245-login-ws-response]]"
---

# ru.ok.tamtam.api — исключения протокола

Исключения WS-протокола MAX.

## Исключения

| Исключение | Что |
|---|---|
| `CorruptedInputDataException` | повреждённые входные данные |
| `SessionSendLimitException` | **превышен лимит отправки сессии** |
| `UnknownOpcodeException` | **неизвестный опкод** |
| `SessionSenderUnexpectedException` | неожиданный отправитель сессии |
| `NonceException` | **ошибка nonce** |
| `SessionTamErrorException` | ошибка TamTam сессии |
| `MaxRetryCountExceededException` | превышено максимальное количество попыток |
| `InvalidParsePresenceException` | ошибка парсинга presence |

## Что важно

1. **`NonceException`** — nonce используется в протоколе. Это защита от replay-атак.

2. **`SessionSendLimitException`** — сервер ограничивает количество сообщений в сессии.

3. **`UnknownOpcodeException`** — клиент получил неизвестный опкод. Это может быть при обновлении протокола.

## Сводка

8 исключений протокола: NonceException/SessionSendLimitException/UnknownOpcodeException/MaxRetryCountExceededException/InvalidParsePresenceException.
