# WS-опкод DEBUG (код 2) — полная верификация

> Волна 2 · Верификация topic из волны 1
> Версия: 26.15.3 (base), сравнение с 26.16.0

---

## TL;DR

Сервер может в любой момент отправить клиенту WS-сообщение с опкодом **2 (DEBUG)** и полем `cmd`. Клиент безусловно выполняет одну из двух команд:

| cmd строка | Внутренний маркер | Действие |
|---|---|---|
| `SYNC_CONTACTS` | `ay6.X` | Принудительная синхронизация телефонной книги устройства на сервер |
| `SEND_LOG` | `ay6.o` | Выброс `IllegalStateException("onNotifDebug")` → crash-handler собирает и отправляет логи |
| *(любое другое / null)* | `ay6.d` (default) | Ничего не делает (silent drop) |

**Нет UI-уведомления пользователю. Нет подтверждения. Нет opt-out.**

---

## 1. Структура сообщения (v75.java / smali_classes3/v75.smali)

```java
// defpackage/v75.java — парсер WS-нотификации опкода 2
public final class v75 extends jei {
    public ay6 c;       // cmd — распарсенная команда
    public List d;      // args — список строковых аргументов

    @Override
    public final void c(kxa kxaVar, String str) {
        if (str.equals("cmd")) {
            String strU = j8h.U(kxaVar);  // читает строку из потока
            ay6 ay6Var = ay6.d;            // default
            if (strU != null) {
                if (strU.equals("SYNC_CONTACTS")) {
                    ay6Var = ay6.X;
                } else if (strU.equals("SEND_LOG")) {
                    ay6Var = ay6.o;
                }
            }
            this.c = ay6Var;
        } else if (str.equals("args")) {
            int iJ = j8h.J(kxaVar);  // размер массива
            this.d = new ArrayList(iJ);
            for (int i = 0; i < iJ; i++) {
                this.d.add(j8h.U(kxaVar));  // строковые аргументы
            }
        } else {
            kxaVar.C();  // skip unknown field
        }
    }
}
```

**Поля v75:**
- `c` (ay6) — команда
- `d` (List<String>) — аргументы

Других полей (chatId, userId, URL, файл) **нет**. Структура минимальна: только `cmd` + `args`.

---

## 2. Класс ay6 — НЕ enum, а многоцелевой utility-класс

`ay6.java` (classes2.dex) — это **не** чистый enum команд DEBUG. Это обфусцированный класс, переиспользуемый в нескольких контекстах (анимация кадров, wearable extensions, модульная арифметика). Для DEBUG-опкода используются только **статические поля-синглтоны**:

```java
// Статические экземпляры (из static initializer):
static {
    c = new ay6(0, 1);   // используется в gc8/w4e/cb8/eb8 (другой контекст — fqf)
    d = new ay6(0, 2);   // DEFAULT для DEBUG cmd (неизвестная команда)
    o = new ay6(1, 2);   // SEND_LOG
    X = new ay6(2, 2);   // SYNC_CONTACTS
}
```

Конструктор `ay6(int b, int a)` — `a` это тип/дискриминатор, `b` — значение. Для DEBUG-контекста `a=2` у всех трёх (d/o/X).

---

## 3. Обработка в n77.java (case 21) — полная логика

```java
// pb8.java:352 — диспетчер WS-нотификаций:
if (s == 2) {  // opcode 2 = DEBUG
    yzb yzbVar2 = ((owb) this.c).b.w;
    yzbVar2.d(new n77(yzbVar2, 21, (v75) jeiVar));
}

// n77.java case 21 — обработчик:
case 21:
    yzb yzbVar2 = (yzb) this.b;
    v75 v75Var = (v75) this.c;
    rzb rzbVar = (rzb) yzbVar2.b().a.getValue();
    
    ct4.E(rzb.e, "onNotifDebug, response = " + v75Var);
    // ↑ логирует в внутренний лог (не UI)
    
    ay6 ay6Var = v75Var.c;
    
    if (zm0.c(ay6Var, ay6.o)) {
        // === SEND_LOG ===
        ((ckc) rzbVar.a).a(new IllegalStateException("onNotifDebug"));
        // Вызывает OneMeExceptionHandler.a(Throwable) →
        //   ct4.M("OneMeExceptionHandler", "Handle exception in " + Thread.currentThread(), th)
        // Это триггерит crash-reporter (Apptracer) для сбора и отправки логов
        
    } else if (zm0.c(ay6Var, ay6.X)) {
        // === SYNC_CONTACTS ===
        // Шаг 1: Синхронизация БД контактов
        cr5 cr5Var = rzbVar.b;  // lazy-провайдер Database
        xh8.K(((h55) cr5Var.get()).d().b().a, false, true, new g4c(22));
        // xh8.K(kufVar, z=false, z2=true, ck7Var) — запускает корутину синхронизации
        
        // Шаг 2: Принудительный checkUpdates телефонной книги
        cr5 cr5Var2 = rzbVar.c;  // lazy-провайдер Phonebook (a40)
        ((a40) cr5Var2.get()).b();
        // a40.b() → логирует "call checkUpdates from:" + stacktrace
        //         → this.h.h(p7j.a) — триггерит пайплайн синхронизации контактов
    }
```

---

## 4. Класс rzb — контейнер зависимостей обработчика

```java
public final class rzb {
    public static final h29[] d = {
        new gne(rzb.class, "db", "getDb()Lru/ok/tamtam/Database;", 0),
        io6.d(zjf.a, rzb.class, "phonebook", "getPhonebook()Lru/ok/tamtam/services/Phonebook;", 0)
    };
    public static final String e = rzb.class.getName();  // тег для логирования
    
    public final ec6 a;   // → ckc (OneMeExceptionHandler) — для SEND_LOG
    public final cr5 b;   // → h55 (Database) — для SYNC_CONTACTS шаг 1
    public final cr5 c;   // → a40 (Phonebook) — для SYNC_CONTACTS шаг 2
}
```

---

## 5. Что делает каждая команда подробно

### SEND_LOG
1. Создаёт `IllegalStateException("onNotifDebug")`
2. Передаёт в `ckc.a(Throwable)` — реализация `ec6` (exception handler)
3. `ckc.a()` вызывает `ct4.M("OneMeExceptionHandler", msg, throwable)`
4. Это штатный crash-pipeline → Apptracer собирает:
   - Stack trace
   - Логи приложения
   - Состояние потоков
   - Heap info (если включён sample-uploader)
5. Отправляет на `sdk-api.apptracer.ru`

**Итог:** Сервер может в любой момент вытянуть полные логи клиента, включая потенциально heap-dump.

### SYNC_CONTACTS
1. **Шаг 1:** `xh8.K(kufVar, false, true, callback)` — запускает корутину, которая:
   - Читает локальную БД контактов (`ru.ok.tamtam.Database`)
   - Синхронизирует с сервером (upload)
2. **Шаг 2:** `a40.b()` — вызывает `checkUpdates`:
   - Триггерит `k6h.h()` — сигнал для пайплайна обновления
   - Пайплайн читает `ContactsContract.Contacts.CONTENT_URI` (системная телефонная книга)
   - Отправляет дельту на сервер

**Итог:** Сервер может принудительно запросить полную телефонную книгу устройства в любой момент.

---

## 6. Регистрация опкода в протоколе

```java
// u0d.java — enum всех WS-опкодов:
public static final u0d Y = new u0d("DEBUG", 1, 2, null);
//                                    name   ordinal  opcode  parser
// opcode = 2, parser = null (парсится отдельно в owb.java)
```

В `owb.java:374` (диспетчер входящих WS-сообщений):
```java
} else if (s2 == 2) {
    jeiVarN = new v75(kxaVarA);  // парсит DEBUG-нотификацию
}
```

В `pb8.java:352` (обработчик распарсенных нотификаций):
```java
if (s == 2) {
    yzb yzbVar2 = ((owb) this.c).b.w;
    yzbVar2.d(new n77(yzbVar2, 21, (v75) jeiVar));
}
```

---

## 7. Сравнение с 26.16.0

В версии 26.16.0 класс **переименован**: `v75` → `fm4`, `ay6` → `m96`.

Файл: `smali_classes3/fm4.smali`

```
.field public c:Lm96;    # cmd (было ay6)
.field public d:Ljava/util/List;  # args

# Те же две строки:
const-string v0, "SYNC_CONTACTS"
const-string v0, "SEND_LOG"
```

**Изменения в 26.16.0:**
- Переименование классов (обфускация): `v75`→`fm4`, `ay6`→`m96`, `kxa`→`tw9`, `j8h`→`tf3`
- Логика **идентична** — те же два cmd, тот же default
- Новых cmd **не добавлено**
- Поля `cmd` + `args` — без изменений

---

## 8. Все места использования ay6 в контексте DEBUG

| Файл | Строка | Контекст |
|---|---|---|
| `v75.java:24` | `ay6 ay6Var = ay6.d` | default при парсинге cmd |
| `v75.java:27` | `ay6Var = ay6.X` | SYNC_CONTACTS |
| `v75.java:29` | `ay6Var = ay6.o` | SEND_LOG |
| `n77.java:242` | `zm0.c(ay6Var, ay6.o)` | проверка SEND_LOG |
| `n77.java:244` | `zm0.c(ay6Var, ay6.X)` | проверка SYNC_CONTACTS |

Другие использования `ay6.c` (в `gc8.java`, `w4e.java`, `cb8.java`, `eb8.java`) — это **другой контекст** (fqf/gqf — UI-фреймворк), не связанный с DEBUG-опкодом.

---

## 9. Поле args — использование

В текущем коде `v75.d` (args) **парсится, но не используется** в обработчике case 21. Обработчик проверяет только `v75.c` (cmd). Однако:
- args передаётся как `List<String>` произвольной длины
- Структура позволяет серверу передать любые данные
- В будущих версиях args может быть задействован без изменения протокола

---

## 10. Цепочка вызовов (call graph)

```
WS message (opcode=2)
  → owb.java: new v75(kxaVar)     [парсинг: cmd + args]
  → pb8.java: if (s == 2)          [диспетчеризация]
  → n77(yzb, 21, v75)              [создание задачи]
  → n77.invoke() case 21           [выполнение]
    → rzb.a (ckc)                  [SEND_LOG → crash + log upload]
    → rzb.b (h55/Database)         [SYNC_CONTACTS → DB sync]
    → rzb.c (a40/Phonebook)        [SYNC_CONTACTS → phonebook upload]
```

---

## 11. Выводы для статьи

1. **Серверная команда без согласия пользователя.** Опкод DEBUG позволяет серверу:
   - Принудительно выгрузить телефонную книгу устройства
   - Принудительно собрать и отправить логи/crash-dump приложения

2. **Нет защиты от злоупотребления.** Нет rate-limiting, нет UI-индикации, нет настройки отключения.

3. **Название "DEBUG" — маскировка.** Функционал production-ready: синхронизация контактов и сбор логов — это не отладочные операции.

4. **args не используется, но парсится.** Сервер может расширить протокол новыми командами без обновления клиента (через default-ветку или будущее использование args).

5. **Без изменений в 26.16.0.** Несмотря на удаление KWS, DEBUG-опкод сохранён полностью.

---

## Источники

- `work/jadx_base/sources/defpackage/v75.java` — парсер
- `work/jadx_base/sources/defpackage/ay6.java` — маркеры cmd
- `work/jadx_base/sources/defpackage/n77.java:235-252` — обработчик
- `work/jadx_base/sources/defpackage/pb8.java:348-353` — диспетчер
- `work/jadx_base/sources/defpackage/owb.java:374` — регистрация парсера
- `work/jadx_base/sources/defpackage/u0d.java:32` — enum опкодов (DEBUG=2)
- `work/jadx_base/sources/defpackage/rzb.java` — контейнер зависимостей
- `work/jadx_base/sources/defpackage/ckc.java` — exception handler (SEND_LOG)
- `work/jadx_base/sources/defpackage/a40.java:329-340` — phonebook sync (SYNC_CONTACTS)
- `work/apktool_base/smali_classes3/v75.smali` — smali-подтверждение
- `work/apktool_base/smali_classes2/ay6.smali` — smali static init
- `work_26.16.0/apktool_base/smali_classes3/fm4.smali` — 26.16.0 эквивалент
