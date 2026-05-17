---
tags: [critical, ws-protocol, debug-opcode, c2-channel, contacts-exfiltration, log-exfiltration, server-command]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/v75.java
  - work/jadx_base/sources/defpackage/ay6.java
  - work/jadx_base/sources/defpackage/n77.java
  - work/jadx_base/sources/defpackage/pb8.java
  - work/jadx_base/sources/defpackage/owb.java
  - work/jadx_base/sources/defpackage/u0d.java
  - work/jadx_base/sources/defpackage/rzb.java
  - work/jadx_base/sources/defpackage/ckc.java
  - work/jadx_base/sources/defpackage/a40.java
  - work/jadx_base/sources/defpackage/xh8.java
  - work_26.16.0/apktool_base/smali_classes3/fm4.smali
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[06-contacts]]"
  - "[[17-apptracer-uplink]]"
  - "[[535-logcat-capture-upload]]"
  - "[[531-wiretap-chain-collect-debug-dump]]"
  - "[[542-traceflow-dps-deanonymization]]"
  - "[[543-reconnect-ws-server-host-takeover]]"
---

# 544. WS-опкод DEBUG (код 2) — прямой C2-канал с именованными командами

## Суть

В WS-протоколе MAX есть опкод **2** с именем `DEBUG`. Сервер отправляет JSON-payload с полем `cmd`. Клиент безусловно (без подтверждения пользователя, без UI-индикации, без opt-out) выполняет одну из именованных команд:

| `cmd` | Что делает |
|---|---|
| `SYNC_CONTACTS` | Принудительно синхронизирует ВСЮ телефонную книгу устройства на сервер (БД + ContentResolver-делта) |
| `SEND_LOG` | Бросает `IllegalStateException("onNotifDebug")` → штатный crash-pipeline собирает логи / стек-трейсы / heap-info и отправляет на `sdk-api.apptracer.ru` |
| *(любое другое или null)* | Тихо игнорируется (default `ay6.d`) |

Поле `args` (`List<String>`) в payload **парсится, но в текущей логике не читается** — это готовая площадка для расширения протокола без обновления клиента: на стороне сервера можно завести новые `cmd`-строки, и в следующем обновлении клиента просто подключить к ним обработчики, передавая параметры через `args`.

Это не «отладочный артефакт, забытый в production» — это полноценный **command-and-control-канал** с production-семантикой (выгрузка адресной книги, выгрузка логов).

## Структура сообщения

`defpackage/v75.java`:

```java
public final class v75 extends jei {
    public ay6 c;       // cmd
    public List d;      // args (List<String>)

    public final void c(kxa kxaVar, String str) {
        if (str.equals("cmd")) {
            String s = j8h.U(kxaVar);
            ay6 v = ay6.d;                              // default
            if (s != null) {
                if (s.equals("SYNC_CONTACTS")) v = ay6.X;
                else if (s.equals("SEND_LOG")) v = ay6.o;
            }
            this.c = v;
        } else if (str.equals("args")) {
            int n = j8h.J(kxaVar);
            this.d = new ArrayList(n);
            for (int i = 0; i < n; i++) this.d.add(j8h.U(kxaVar));
        } else {
            kxaVar.C();
        }
    }
}
```

Класс `ay6` — НЕ enum, а multipurpose-обфусцированный объект. Для DEBUG используются три статических синглтона:

```java
static {
    ay6.d = new ay6(0, 2);   // default — silent drop
    ay6.o = new ay6(1, 2);   // SEND_LOG
    ay6.X = new ay6(2, 2);   // SYNC_CONTACTS
}
```

## Регистрация в протоколе

`defpackage/u0d.java`:
```java
public static final u0d Y = new u0d("DEBUG", 1, 2, null);
//                                  name      ord  op   parser-factory(null — handled in owb)
```

Парсер регистрируется в `owb.java:374`:
```java
} else if (s2 == 2) {
    jeiVarN = new v75(kxaVarA);
}
```

Диспетчер в `pb8.java`:
```java
if (s == 2) {
    yzbVar2.d(new n77(yzbVar2, 21, (v75) jeiVar));
}
```

## Обработчик — `n77.java case 21`

```java
case 21:
    v75 v75Var = (v75) this.c;
    rzb rzbVar = (rzb) yzbVar2.b().a.getValue();

    ct4.E(rzb.e, "onNotifDebug, response = " + v75Var);
    // ↑ запись в внутренний log tag, НЕ в UI, НЕ в logcat по умолчанию

    ay6 cmd = v75Var.c;

    if (zm0.c(cmd, ay6.o)) {
        // === SEND_LOG ===
        ((ckc) rzbVar.a).a(new IllegalStateException("onNotifDebug"));
        // → ckc.a(Throwable) → ct4.M("OneMeExceptionHandler", ..., th)
        // → штатный Apptracer crash pipeline:
        //     - сбор stack trace
        //     - сбор thread states
        //     - захват логов приложения за последний период
        //     - sample-uploader на sdk-api.apptracer.ru
    } else if (zm0.c(cmd, ay6.X)) {
        // === SYNC_CONTACTS ===
        // Шаг 1: синхронизация внутренней БД контактов
        xh8.K(((h55) rzbVar.b.get()).d().b().a, false, true, new g4c(22));
        // Шаг 2: вызов Phonebook.checkUpdates → читает ContactsContract.Contacts.CONTENT_URI
        ((a40) rzbVar.c.get()).b();
        // → "call checkUpdates from:" + stack
        // → отправка дельты адресной книги на сервер
    }
    // (любое другое cmd — silent drop)
```

## Детали поведения

### SEND_LOG
- Вызывает `OneMeExceptionHandler.a(Throwable)` — это путь, по которому ходят **все** реальные крэши приложения
- Поэтому уходит:
  - stack trace
  - thread dump
  - last logs (если активирован `log-full` / `log-sensitive` PmsKey-флагами — см. [[535-logcat-capture-upload]])
  - метаданные устройства (через Apptracer device data — [[336-ilb-device-data]])
- Endpoint: `https://sdk-api.apptracer.ru/api/crash/...` ([[17-apptracer-uplink]])
- **На стороне сервера это выглядит как обычный crash-репорт** — то есть в потоке нормальной телеметрии его не отличить от настоящих ошибок без отдельной таблицы

### SYNC_CONTACTS
- Шаг 1: синхронизация внутренней SQLite-таблицы контактов с сервером
- Шаг 2: `Phonebook.checkUpdates` → `ContactsContract.Contacts.CONTENT_URI` → upload дельты
- Это **тот же** механизм, что описан в [[06-contacts]], но триггерится **сервером по требованию**, а не пользователем / не по таймеру

## Что отсутствует — намеренно

- ❌ UI-уведомление пользователю
- ❌ Запись в видимый журнал событий приложения
- ❌ Rate-limiting на стороне клиента
- ❌ Какая-либо опция отключения (нет PmsKey, нет настройки)
- ❌ Подтверждение выполнения серверу с timestamp (просто запускается)

## Поле `args` — заготовка для расширения

`v75.d` (`List<String>`) парсится корректно (читает массив строк произвольной длины), но в `n77 case 21` **не передаётся в обработчики**. Это значит:
- сервер уже сейчас может слать `args: [...]` без ошибок парсинга
- в любой момент в новой версии клиента можно подключить args к новой команде
- расширение протокола (новые `cmd`-строки) **не требует изменения wire format**

Принцип «forward-compatible by silence» — стандартный приём для C2-протоколов: клиент молча принимает неизвестные команды, новые команды активируются только когда клиент их «понимает».

## Почему имя «DEBUG» — маскировка

Семантика двух текущих команд `SYNC_CONTACTS` и `SEND_LOG`:
- Это **production-операции** (адресная книга — это пользовательские данные, логи через Apptracer — это штатный канал телеметрии)
- В реальной debug-сборке они уже выполняются автоматически (сбор крэшей, синхронизация контактов по таймеру)
- В прод-сборке отдельный канал «как-бы-debug» имеет смысл только для **точечной выгрузки данных конкретного пользователя**

Имя `DEBUG` создаёт plausible deniability — «это просто диагностический канал». В то время как функционально — это адресный command channel.

## Цепочка исполнения

```
[Server] WS frame opcode=2 + payload {cmd: "SYNC_CONTACTS"}
   │
   ▼
defpackage/owb.java:374 — new v75(kxaVar) — парсинг payload
   │
   ▼
defpackage/pb8.java — if (s == 2) → new n77(yzb, 21, v75)
   │
   ▼
defpackage/n77.java case 21 — диспетч по cmd
   │
   ▼
[ a40.b() ] → checkUpdates → ContactsContract.Contacts.CONTENT_URI → upload дельты
[ ckc.a(IllegalStateException) ] → Apptracer pipeline → sdk-api.apptracer.ru
```

## Какие данные реально утекают на сервер

### При `SYNC_CONTACTS`
- Все `display_name` из системной книги
- Все номера телефонов
- Привязка к существующим MAX-userId (через ранее сохранённые `tt_contact_mimetype` записи — см. [[06-contacts]])
- E-mail адреса контактов (если есть в адресной книге)
- Photo URI

В то время как [[06-contacts]] уже описывает, что такая синхронизация идёт периодически — DEBUG SYNC_CONTACTS это **on-demand** триггер, который сервер может использовать сразу после, например, RECONNECT (см. [[543-reconnect-ws-server-host-takeover]]) на свой собственный хост.

### При `SEND_LOG`
- Stack trace и thread states приложения
- Если в момент команды активны PmsKey `log-full` / `log-sensitive` / `log-violations` ([[244-log-sensitive-server-flag]]) — также:
  - URL'ы запросов
  - chat ids, user ids, message ids в логах
  - timestamps событий
- Если активно `android-use-logcat-logger` + `g85` запущен ([[535-logcat-capture-upload]]) — также **ротированный logcat за период до 32 МБ**

## Сценарий боевого использования

```
1. Сервер устанавливает PmsKey log-full=true и log-sensitive=true для userId=12345
   (опкод NOTIF_CONFIG)

2. Сервер ждёт несколько минут — клиент успевает написать содержательные логи
   (URL'ы, chatIds, активность пользователя)

3. Сервер шлёт WS opcode 2: { "cmd": "SEND_LOG" }

4. Клиент:
   - бросает IllegalStateException("onNotifDebug")
   - Apptracer собирает stack + logs + thread states + memory metrics
   - upload на sdk-api.apptracer.ru

5. Сервер шлёт WS opcode 2: { "cmd": "SYNC_CONTACTS" }

6. Клиент:
   - выгружает свежий снимок ContactsContract.Contacts на сервер MAX

7. Готово — у сервера полный снимок (логи + контакты) этого userId,
   собранный по требованию, без UI-уведомления.
```

## Сравнение с известными системами

| Система | Аналог | Прозрачность |
|---|---|---|
| iOS MDM | RemoteCommand | Пользователь видит профиль, может удалить |
| Android Enterprise | Device Admin commands | Требует регистрации устройства, видимо в настройках |
| Apple Find My | Activate / Erase / Lock | Требует Apple ID, есть UI-индикация |
| **MAX DEBUG opcode** | SYNC_CONTACTS / SEND_LOG | **Никакой прозрачности, никакой регистрации, никакого UI** |

## Что это в комбинации с другими находками

- **+ [[535-logcat-capture-upload]]:** SEND_LOG триггерит crash-репорт; PmsKey `log-full` = true + `g85` = активен → в дамп уйдёт logcat (включая сис-логи других приложений на pre-Android 11)
- **+ [[17-apptracer-uplink]]:** Apptracer штатно умеет heap-dumps процесса — SEND_LOG может триггернуть и снимок памяти, который содержит ключи, токены, сообщения в буферах
- **+ [[543-reconnect-ws-server-host-takeover]]:** после RECONNECT на свой хост и DEBUG SYNC_CONTACTS — атакующий получает контакты пользователя на свой сервер
- **+ [[542-traceflow-dps-deanonymization]]:** связка `(uid, реальный IP, vpn-флаг, оператор)` от DPS + содержимое адресной книги от DEBUG = полный профиль абонента, обходящего блокировки

## Статус в 26.16.0

Без изменений по сути. Классы переименованы (обфускация):
- `v75` → `fm4`
- `ay6` → `m96`
- `kxa` → `tw9`
- `j8h` → `tf3`

Те же две `cmd`-строки (`SYNC_CONTACTS`, `SEND_LOG`), та же логика, те же поля. Никаких новых команд, никаких ограничений не добавлено.

Файл-доказательство: `work_26.16.0/apktool_base/smali_classes3/fm4.smali`:
```
.field public c:Lm96;        # cmd (бывший ay6)
.field public d:Ljava/util/List;   # args
const-string v0, "SYNC_CONTACTS"
const-string v0, "SEND_LOG"
```

## Файлы

- `defpackage/v75.java` — парсер payload
- `defpackage/ay6.java` — статические маркеры команд
- `defpackage/n77.java:235-252` — обработчик
- `defpackage/pb8.java:348-353` — диспетчер
- `defpackage/owb.java:374` — регистрация парсера
- `defpackage/u0d.java:32` — enum (`DEBUG=2`)
- `defpackage/rzb.java` — контейнер зависимостей (`ckc` + `Database` + `Phonebook`)
- `defpackage/ckc.java` — `OneMeExceptionHandler` (path для SEND_LOG)
- `defpackage/a40.java:329-340` — `Phonebook.checkUpdates` (path для SYNC_CONTACTS)
- `notes/wave2/01-debug-opcode-full.md` — полная верификация

---

**Вывод одной строкой:** WS-опкод 2 — это явный server-to-client command channel с именованными командами под бренды `SYNC_CONTACTS` и `SEND_LOG`. Это адресный механизм точечной выгрузки контактов и логов конкретного пользователя по запросу сервера, замаскированный под «debug»-нотификацию и сохранённый в production без каких-либо ограничений или индикации.
