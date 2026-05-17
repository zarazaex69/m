---
tags: [critical, fcm, push, c2, command-channel, location, wake, surveillance, silent]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/cei.java
  - work/jadx_base/sources/defpackage/eei.java
  - work/jadx_base/sources/defpackage/eqe.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[203-live-location]]"
  - "[[08-background-wake]]"
  - "[[535-logcat-capture-upload]]"
---

# 539. FCM как C2-канал: silent push → wake → location tracking без UI

## Суть

FCM push-канал MAX функционирует как **command-and-control** с 7 типами команд. Критический сценарий: сервер отправляет silent push `LocationRequest` → устройство просыпается (WakeLock в Doze) → WS подключается → сервер запрашивает GPS → клиент отвечает координатами. **Пользователь ничего не видит.**

## Типы push-команд

| Команда | Действие | UI |
|---------|----------|-----|
| `LocationRequest` | Wake + WS reconnect + GPS acquisition | **Нет** |
| `MessageRemoved` | Удаление сообщения из БД | **Нет** |
| `InboundCall` | Входящий звонок (wake + foreground) | Да |
| `TamtamSpam` | Произвольный deeplink URI | Уведомление |
| `ConversationReadOnOtherDevice` | Пометить чат прочитанным | **Нет** |
| `onDeletedMessages` | Force WS reconnect + sync | **Нет** |
| Standard message | Показать уведомление | Да |

## Цепочка удалённого отслеживания

```
Сервер MAX
    │
    │  FCM data message: {type: "LocationRequest"}
    ▼
cei.java: парсит push, вызывает eqe.f(false, forceConnection=true)
    │
    │  Acquires WakeLock "ru.ok.tamtam:doze-wakelock" (10 сек)
    │  PmsKey wakelock-on-push = true (серверно-управляемый!)
    ▼
WebSocket подключается к api.oneme.ru
    │
    ▼
Сервер отправляет WS-опкод LOCATION_REQUEST
    │
    ▼
TaskLocationRequest → FusedLocationProvider → GPS fix
    │
    ▼
Координаты отправляются серверу через WS
```

## Усиливающие факторы

1. **WakeLock в Doze** — PmsKey `wakelock-on-push` (серверно-управляемый!) позволяет будить устройство даже в глубоком сне
2. **Нет rate limiting** — сервер может слать неограниченное количество silent push
3. **Подтверждение доставки** — каждый push логируется обратно на сервер с `dtime` (задержка доставки)
4. **Работает с убитым приложением** — FCM будит процесс
5. **Нет проверки получателя** — флаг проверки `consignee` гейтится PmsKey index 160 (серверно-отключаемый)

## TamtamSpam — произвольный deeplink

Сервер может отправить push с произвольным `uri`:
- Нет whitelist URI-схем
- Может открыть мини-аппу → JS-bridge → получить MSISDN
- Может открыть внутренний экран приложения
- Может открыть внешний URL через `LinkInterceptorActivity`

## Статус в 26.16.0

Без изменений.
