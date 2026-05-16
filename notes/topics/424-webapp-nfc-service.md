---
tags: [webapp, nfc, hce, host-apdu, jsbridge, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/webapp/util/WebAppNfcService.java
related:
  - "[[07-nfc-hce-webapp]]"
  - "[[419-webapp-js-events]]"
---

# WebAppNfcService — NFC HCE для мини-приложений

`WebAppNfcService extends HostApduService` — NFC Host Card Emulation для мини-приложений.

## Методы

| Метод | Что |
|---|---|
| `processCommandApdu(byte[], Bundle)` | **обработка APDU команды** |
| `onDeactivated(int)` | деактивация NFC |
| `a()` | получить данные для ответа |

## Логика

1. Получает APDU команду от NFC-ридера
2. Логирует: `"APDU received: " + hex(apdu)`
3. Проверяет размер (минимум 4 байта)
4. Проверяет INS байт
5. Возвращает данные из `jxb.d` (данные от мини-приложения)

## Что важно

1. **`HostApduService`** — MAX эмулирует NFC-карту. Мини-приложение предоставляет данные для ответа на APDU.

2. **`WebAppNfcEmulateNfcTag`** — JS-событие для передачи данных NFC-тега мини-приложением.

3. **Логирование APDU** — все APDU команды логируются в hex.

4. Это позволяет мини-приложениям эмулировать банковские карты, транспортные карты и другие NFC-устройства.

## Сводка

`WebAppNfcService`: `processCommandApdu(apdu)` → данные от мини-приложения. Логирует все APDU в hex. Эмулирует NFC-карту через HCE.
