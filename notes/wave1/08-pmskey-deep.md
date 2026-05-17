# PmsKey Deep Analysis — Серверно-управляемые бэкдоры

> **Версия**: 26.15.3 (подтверждено в 26.16.0 — переименовано в PmsProperty, поведение идентично)
> **Класс**: `ru.ok.tamtam.android.prefs.PmsKey` (334 enum-значения)
> **Хранение**: `settings.prefs` (SharedPreferences)
> **Доставка**: WS-опкод + login response → `cb4` → `ozb.b()` → SharedPreferences.Editor.putAll()

---

## КРИТИЧЕСКОЕ ОТКРЫТИЕ: Механизм динамической доставки PmsKey

### Как сервер пушит конфигурацию

1. **При логине** (`om9` → `ozb.b(cb4Var, true)`): сервер отдаёт полный `cb4` (Configuration) объект
2. **В любой момент через WebSocket** (`fx2` class, opcode 55/58/59/77/86/68/75/53/36/39/34/87`): сервер может отправить `cb4` с новыми значениями
3. **Через FCM push** (wake → reconnect → получение новой конфигурации)

### Код доставки (`ozb.b()`)

```java
// ozb.java — NotifConfigLogic.onConfiguration
Map map = (Map) e4Var.b;  // serverSettings — произвольная Map<String, Object>
SharedPreferences.Editor editor = rtdVar.e.edit();
for (Map.Entry entry : map.entrySet()) {
    ij9.O(editor, (String) entry.getKey(), entry.getValue());  // ЛЮБОЙ ключ, ЛЮБОЕ значение
}
editor.apply();  // Мгновенно применяется
```

**Вывод**: Сервер может в ЛЮБОЙ момент изменить ЛЮБОЙ из 334 PmsKey-флагов для ЛЮБОГО пользователя. Нет проверки подписи, нет whitelist допустимых значений, нет уведомления пользователя.

---

## ТОП-30 ОПАСНЫХ PmsKey (не задокументированных ранее)

### 🔴 КРИТИЧЕСКИЙ УРОВЕНЬ (surveillance / remote control)

#### 1. `debug-mode` (серверный int, default=0)
- **Что делает**: При значении `3` (DEV_OPTIONS_MENU) — **удалённо включает DevMenu** в production-сборке
- **Код**: `le3.java:62` — `if (y75.a(rtdVar.m(PmsKey.f92debugmode, 0)) == y75.DEV_OPTIONS_MENU)`
- **Также**: `d4.java:37` — при `debug-mode == 3` переключает HTTP-клиент на debug-режим
- **Также**: `ak1.java:45` — при `debug-mode == 3` использует `zj1()` вместо `z6f()` (другой сетевой стек)
- **Угроза**: Сервер может удалённо включить DevMenu конкретному пользователю → переключение API-серверов, фича-флагов, дамп данных

#### 2. `calls-sdk-log-audio` (boolean, default=false)
- **Что делает**: Включает **логирование аудио-потока** во время звонков
- **Код**: `ubi.java:80` — передаётся в calls SDK как параметр
- **Код**: `nd1.java:81` — читается в конфигурации звонков
- **Угроза**: Сервер может включить запись аудио звонка для конкретного пользователя

#### 3. `calls-sdk-dnt-disable-audio` (boolean, default=false)
- **Что делает**: **Отключает DNT (Do Not Track/Transmit) для аудио** — позволяет передавать аудио-данные без ограничений
- **Код**: `ubi.java:82` — передаётся в SDK звонков
- **Код**: `kd1.java:40` — конфигурация звонков
- **Угроза**: Снимает ограничения на передачу аудио-данных, потенциально позволяя серверу получать raw audio

#### 4. `calls-sdk-disable-pipeline` (boolean, default=false)
- **Что делает**: **Отключает pipeline обработки** в calls SDK
- **Код**: `ubi.java:77` — передаётся как параметр в SDK
- **Угроза**: Отключение pipeline может означать отключение шифрования/обработки аудио, передачу raw-потока

#### 5. `calls-use-p2p-relay` + `calls-use-p2p-relay-caps` (boolean/string)
- **Что делает**: **Принудительно маршрутизирует P2P-звонки через relay-сервер**
- **Код**: `kd1.java:119,125` — конфигурация ICE/TURN
- **Угроза**: Сервер может принудительно направить звонок через свой relay → MITM-позиция для прослушки

#### 6. `calls-android-signaling-ip` (string)
- **Что делает**: **Задаёт IP-адрес signaling-сервера для звонков**
- **Код**: `kd1.java:34` — используется в конфигурации звонков
- **Угроза**: Сервер может перенаправить signaling на произвольный IP → полный контроль над установкой звонка

#### 7. `calls-endpoint` (string, JSON)
- **Что делает**: **Задаёт адреса серверов звонков** (getCallServers)
- **Код**: `rtd.java:77` — `new kbg(this, PmsKey.f47callsendpoint, 2)`
- **Угроза**: Полная подмена инфраструктуры звонков

#### 8. `hide-incoming-call-notif` (boolean/int, default=false)
- **Что делает**: **Скрывает уведомления о входящих звонках**
- **Код**: `nd1.java:55`, `qp6.java:314`
- **Угроза**: Сервер может скрыть факт входящего звонка (silent call для активации микрофона?)

#### 9. `outgoing-call-uri` (string, default="")
- **Что делает**: **Задаёт URI по умолчанию для исходящих звонков** (defaultOutgoingCalleeUri)
- **Код**: `qp6.java:214` — `new po6(this, PmsKey.f223outgoingcalluri, i5)`
- **Угроза**: Сервер может перенаправить исходящие звонки на другой номер/аккаунт

#### 10. `android-use-logcat-logger` (boolean, default=false)
- **Что делает**: **Включает запись в logcat** (который потом может быть собран Apptracer)
- **Код**: `rtd.java:84` — `new jbg(this, PmsKey.f4androiduselogcatlogger, i2)`
- **Угроза**: В связке с Apptracer logcat capture (topic 535) — удалённое включение полного логирования → exfiltration

#### 11. `user-debug-report` (long, chatId)
- **Что делает**: **Задаёт chatId куда отправляются debug-отчёты** пользователя
- **Код**: `ozb.java:108-112` — парсится из серверного push, сохраняется как Long
- **Код**: `rtd.java:82` — `userLogReportChatId`
- **Угроза**: Сервер может направить debug-отчёты (включая логи, состояние приложения) в произвольный чат, контролируемый оператором

### 🟠 ВЫСОКИЙ УРОВЕНЬ (data collection / privacy bypass)

#### 12. `keep-background-socket` (JSON → BackgroundWakeConfig)
- **Что делает**: Конфигурирует **постоянное фоновое соединение** с сервером
- **Код**: `qp6.java:416` — парсится как `BackgroundWakeConfig`
- **Код**: `jn0.java:39` — логирует изменения: "PMS keepBackgroundSocket changed"
- **Угроза**: Сервер контролирует, когда и как приложение поддерживает фоновое соединение → постоянный C2-канал

#### 13. `spin-lock-enabled` (boolean, default=false)
- **Что делает**: Включает **spin-lock** — busy-wait цикл для предотвращения засыпания процесса
- **Код**: `qp6.java:418`, `ezd.java:106`
- **Угроза**: Гарантирует, что приложение не будет убито системой → persistent background execution

#### 14. `host-reachability` (JSON config)
- **Что делает**: Конфигурирует **HostReachabilityTask** — периодическую проверку доступности хостов
- **Код**: `AccountInitializer.java:479` — запускается при инициализации аккаунта
- **Код**: `mel.java:301,1015` — регистрируется как background task
- **Угроза**: Сервер задаёт список хостов для проверки → network fingerprinting, обнаружение VPN/proxy

#### 15. `net-stat-config` (JSON → NetStatConfig)
- **Что делает**: Конфигурирует **сбор сетевой статистики** по конкретным WS-опкодам
- **Код**: `etb.java:34` — `NetStatConfig(loggableOpcodes=...)`
- **Угроза**: Сервер выбирает, какие опкоды логировать → таргетированный мониторинг конкретных действий пользователя

#### 16. `opcode-stat-config` (JSON config)
- **Что делает**: Конфигурирует **статистику по WS-опкодам** — какие опкоды отслеживать
- **Код**: `qp6.java:309`
- **Угроза**: Аналогично net-stat-config — серверный выбор что мониторить

#### 17. `log-messages-meta` + `log-chat-meta` (boolean, default=false)
- **Что делает**: Включает **логирование метаданных сообщений и чатов**
- **Код**: `qp6.java:403-404`, `nd1.java:129`
- **Угроза**: Сервер может включить сбор метаданных (кто, когда, кому, размер) для конкретного пользователя

#### 18. `presence-external` (boolean/config)
- **Что делает**: Включает **внешний presence-трекинг** — отслеживание онлайн-статуса
- **Код**: `qp6.java:385`, `pl5.java:98`
- **Угроза**: Экспорт данных о присутствии пользователя во внешние системы

#### 19. `memory-slice-interval` (long, ms)
- **Что делает**: Задаёт **интервал снятия memory-слайсов** (heap snapshots)
- **Код**: `rtd.java:121` — `memorySliceIntervalMs`
- **Угроза**: В связке с Apptracer heap-dump → сервер контролирует частоту снятия дампов памяти

#### 20. `invalidate-db-force` (JSON/boolean)
- **Что делает**: **Принудительно инвалидирует локальную БД** сообщений
- **Код**: `jr8.java:83,140` — `rtdVar2.d(PmsKey.f141invalidatedbforce)`
- **Угроза**: Сервер может удалить локальную историю сообщений пользователя (anti-forensics)

### 🟡 СРЕДНИЙ УРОВЕНЬ (manipulation / censorship)

#### 21. `show-warning-links` (boolean, default=false)
- **Что делает**: Контролирует **показ предупреждений при переходе по ссылкам**
- **Код**: `e.java:71` — default false (предупреждения ВЫКЛЮЧЕНЫ по умолчанию)
- **Угроза**: Сервер может отключить предупреждения о фишинговых ссылках для конкретного пользователя

#### 22. `white-list-links` (list)
- **Что делает**: Задаёт **белый список ссылок** — какие ссылки не показывают предупреждение
- **Код**: `v7b.java:530` — `rtdVar.t(PmsKey.f314whitelistlinks, p56.a)`
- **Угроза**: Сервер может добавить фишинговый домен в whitelist

#### 23. `migrate-unsafe-warn` (boolean/config)
- **Что делает**: Контролирует **предупреждения о небезопасной миграции**
- **Код**: `qp6.java:270`
- **Угроза**: Сервер может отключить предупреждения безопасности при миграции аккаунта

#### 24. `phone-privacy-config` (JSON config)
- **Что делает**: Конфигурирует **приватность номера телефона**
- **Код**: `qp6.java:432`, `eag.java:87`
- **Угроза**: Сервер может изменить настройки приватности номера телефона пользователя

#### 25. `creation-2fa-config` (string/JSON)
- **Что делает**: Конфигурирует **поведение 2FA** при создании
- **Код**: `g1g.java:228,240` — парсится как конфиг
- **Угроза**: Сервер может ослабить или отключить 2FA для конкретного пользователя

#### 26. `calls-sdk-traffic-markup` (int/config)
- **Что делает**: Конфигурирует **маркировку трафика звонков**
- **Код**: `qp6.java:224`
- **Угроза**: Позволяет серверу маркировать/классифицировать трафик звонков (DPI-friendly)

#### 27. `net-ssl-session-validate` (boolean)
- **Что делает**: Контролирует **валидацию SSL-сессий**
- **Код**: `qp6.java:340`, `ubi.java:64`
- **Угроза**: Сервер может ОТКЛЮЧИТЬ валидацию SSL → MITM без предупреждений

#### 28. `watchdog-config` (JSON → WatchdogConfig)
- **Что делает**: Конфигурирует **watchdog** — мониторинг потоков и процессов
- **Код**: `qp6.java:392`, `jkc.java:28` — `WatchdogConfig(isEnabled=...)`
- **Угроза**: Сервер контролирует мониторинг приложения, может использовать для обнаружения отладки

#### 29. `perf-registrar-config` (JSON)
- **Что делает**: Конфигурирует **сбор performance-данных** (PerfRegistrar)
- **Код**: `qp6.java:284`, `rbd.java:22` — `isLazy, isPersistent`
- **Угроза**: Сервер контролирует что и как часто собирается из performance-метрик

#### 30. `cancel-stale-notifications` (boolean)
- **Что делает**: **Отменяет "устаревшие" уведомления**
- **Код**: `qp6.java:441`, `om5.java:79`
- **Угроза**: Сервер может удалить уведомления о сообщениях (скрыть факт получения/удаления сообщения)

---

## Дополнительные опасные ключи (краткий список)

| Ключ | Семантика | Угроза |
|------|-----------|--------|
| `push-delivery` | Контроль доставки push | Сервер может отключить push для пользователя |
| `push-check` | Проверка push-канала | Мониторинг доступности push |
| `dps` | Data Processing Service | Фоновая обработка данных |
| `webapp-exc` | WebApp exceptions (long[]) | Список webapp-ID с особыми привилегиями |
| `webapp-pr` | WebApp permissions | Серверный контроль разрешений мини-приложений |
| `conn-timeouts` | Таймауты соединений | Сервер может увеличить таймауты для persistent connection |
| `min-log-level` | Минимальный уровень логирования | Сервер может включить verbose logging |
| `anr-config` | ANR timeout config | Сервер контролирует когда собирать ANR-дампы |
| `calls-sdk-incall-stat` | In-call statistics | Сбор статистики во время звонка |
| `calls-sdk-webrtc-logs` | WebRTC logs | Включение логирования WebRTC |

---

## Цепочки атак

### Цепочка 1: Удалённая прослушка звонка
```
debug-mode=3 → DevMenu enabled
calls-sdk-log-audio=true → аудио логируется
calls-sdk-dnt-disable-audio=true → ограничения сняты
calls-use-p2p-relay=true → трафик через relay
calls-android-signaling-ip=<attacker> → signaling перенаправлен
```

### Цепочка 2: Полный дамп данных пользователя
```
android-use-logcat-logger=true → logcat включён
log-full=true → полное логирование
log-messages-meta=true → метаданные сообщений
memory-slice-interval=1000 → частые heap-dumps
user-debug-report=<operator-chat-id> → отчёты в чат оператора
```

### Цепочка 3: Anti-forensics
```
invalidate-db-force=<trigger> → удаление локальной БД
cancel-stale-notifications=true → удаление уведомлений
show-warning-links=false → скрытие предупреждений
```

### Цепочка 4: Скрытый перехват
```
hide-incoming-call-notif=true → скрытый входящий звонок
calls-sdk-disable-pipeline=true → отключение обработки
net-ssl-session-validate=false → отключение SSL-валидации
keep-background-socket=<aggressive-config> → постоянное соединение
spin-lock-enabled=true → процесс не засыпает
```

---

## Выводы

1. **Нет защиты от серверного злоупотребления**: Все 334 PmsKey применяются мгновенно без проверки подписи, без consent пользователя, без аудит-лога.

2. **Динамическая доставка**: Сервер может изменить любой флаг в любой момент через WebSocket (не только при логине).

3. **Таргетирование**: Конфигурация доставляется per-user — можно включить surveillance для конкретного пользователя.

4. **Комбинаторная атака**: Комбинация нескольких "безобидных" флагов создаёт полноценную surveillance-цепочку.

5. **Отсутствие E2E**: Без end-to-end шифрования серверный оператор имеет полный доступ к контенту; PmsKey-флаги дают дополнительный контроль над клиентским поведением.

---

## Связанные темы

- [[03-pms-server-flags]] — базовый обзор PmsKey
- [[05-dev-menu-in-prod]] — DevMenu в release
- [[535-logcat-capture]] — Apptracer logcat capture
- [[540-webrtc-encryption]] — WebRTC отключение шифрования
- [[531-wiretap-chain]] — цепочка прослушки
