---
tags: [presence, privacy, server-control, surveillance, user-settings]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
  - work/jadx_base/sources/defpackage/sgj.java
  - work/jadx_base/sources/defpackage/ma4.java
  - work/jadx_base/sources/defpackage/ubi.java
related:
  - "[[03-pms-server-flags]]"
  - "[[11-state-bots-and-content-policy]]"
  - "[[20-ws-protocol-opcodes]]"
---

# Presence: серверно-управляемая видимость онлайн-статуса

В UserSettings (`defpackage/sgj.java`) есть поле `hiddenOnline`. Оно приходит с сервера и парсится из map в `defpackage/ma4.java` (ключ `"hiddenOnline"`). Само значение — `Boolean`.

Параллельно есть набор серверно-управляемых PmsKey (см. [[03-pms-server-flags]]):

| PmsKey | Что |
|---|---|
| `presence-ttl` | TTL кеша presence (как долго клиент считает чужой статус актуальным) |
| `presence-view-port` | размер «окна» presence — сколько контактов клиент следит за status |
| `presence-external` | управление видимостью статуса вовне |
| `presence-seen-eq` | условие равенства seen (для read-marks) |
| `presence-stat` | статистика presence в OneLog |
| `presence-keep-bg-cache` | держать ли кеш presence между перезапусками |
| `presence-offline-move-timeout` | через сколько после ухода из foreground считать «offline» |
| `presence-offline-log` | логирование переходов offline |
| `notif-typing-presence` | пушить ли уведомление о typing-presence |

Также есть WS-опкод `CONTACT_PRESENCE(35)` (см. [[20-ws-protocol-opcodes]]) — серверный канал получения чужих presence.

## Что важно

1. **`hiddenOnline` управляется сервером**, не локальным переключателем. UI-чекбокс «скрыть онлайн» в настройках профиля действительно есть (см. строки `hiddenOnline=` в toString sgj.java), но фактическое состояние приходит с сервера в общем UserSettings-объекте. Если сервер пушит `hiddenOnline: false`, локальная попытка «скрыть» отменяется при следующей синхронизации (механика та же, что и `familyProtection`/`audioTranscriptionEnabled` — см. [[11-state-bots-and-content-policy]]).
2. **`presence-external`** — отдельный флаг для «показать ли мой статус сторонним сервисам». Это значит, что presence пользователя через `OK_TOKEN`/`EXTERNAL_CALLBACK` flow (см. [[29-external-callback-idp-flow]]) может транслироваться внешним приложениям из VK/OK-экосистемы.
3. **`presence-stat`** — отдельный server-pushed флаг, включающий отчёт по presence в OneLog. То есть статистика «когда онлайн / когда офлайн / частота переходов» **пишется на сервер MAX**, плюс отдельно по требованию сервера.
4. **`presence-view-port`** ограничивает, сколько чужих presence клиент мониторит одновременно. По умолчанию — небольшое окно (типично десятки), но сервер может запросить расширение. Это влияет на скрытое наблюдение «кто-то из контактов недавно был онлайн».
5. **`presence-keep-bg-cache`** — кеш presence сохраняется между запусками клиента. После рестарта клиент знает, кто «был» онлайн в момент закрытия.

## Сводка

Онлайн-статус в MAX — **не локальный privacy-control**: видимость управляется серверной стороной (`hiddenOnline` в UserSettings + `presence-external`), статистика по своему online-поведению идёт в OneLog (`presence-stat`), и список контактов, чей presence отслеживается, ограничен серверным `presence-view-port`.

Скептический разбор: presence — стандартная фича мессенджера, но в MAX оно специфически server-controlled, что отличается от Telegram/WhatsApp, где «hidden online» — клиентская настройка. Здесь — пара клиентского чекбокса и серверного UserSettings-поля; сервер всегда побеждает.
