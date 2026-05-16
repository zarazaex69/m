---
tags: [host-probe, censorship-detection, surveillance, server-control, anti-vpn, network]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/uq5.java
  - work/jadx_base/sources/defpackage/i58.java
  - work/jadx_base/sources/defpackage/f58.java
  - work/jadx_base/sources/defpackage/o2.java
  - work/jadx_base/sources/defpackage/mel.java
  - work/jadx_base/sources/defpackage/rp8.java
  - work/jadx_base/sources/one/me/android/initialization/AccountInitializer.java
  - findings/raw/pms_keys.txt
related:
  - "[[02-vpn-warning]]"
  - "[[03-pms-server-flags]]"
  - "[[04-telemetry-endpoints]]"
  - "[[18-manifest-deep-dive]]"
---

# Host reachability probe — что и зачем меряет MAX

В марте 2026 в прессе («айтишники заметили…») было замечено, что MAX обращается к сторонним сервисам определения IP, проверяет наличие VPN, и пытается соединиться с доменами Telegram, WhatsApp, AWS, Google. Здесь — где это в коде и что именно отправляется на сервер.

## 1. Подсистема

Класс `defpackage/uq5.java` — `HostReachabilityChecker`. Запускается как фоновая задача `one.me.background.wake.HostReachabilityChecker` (background-job ID 144 в `defpackage/mel.java:301`) и `one.me.android.tasks.HostReachabilityTask` (Job ID 982 в `mel.java:1015`).

В `AccountInitializer.java:479` зарегистрирована задача `HostReachabilityTask` после авторизации.

PmsKey `host-reachability` (#227, в `qp6.h1`) — серверно-управляемая конфигурация задачи: список хостов, интервал, таймауты.

То есть **что проверять — задаёт сервер**, не клиент. Хосты не зашиты в код.

## 2. Что проверяется

`uq5.a(timeout, host)`:

1. Резолвит `host` через DNS (`InetAddress.getAllByName`, кэшируется в локальной hashmap).
2. По каждому IP вызывает `inetAddress.isReachable(timeout)` (Java стандартный — это TCP echo-port или ICMP echo, зависит от прав).
3. Отдельный `i58.a(host, ...)` — открывает прямой `new Socket()` к хосту с timeout (TCP-handshake вместо ICMP).
4. Возвращает map `host -> bool` («дошли ли до хоста»).

То есть на каждый хост из конфига выполняется TCP-проба + DNS-резолв.

## 3. Что отправляется на сервер

В `defpackage/f58.java` (coroutine-обработчик задачи) собирается JSON-payload и кидается в OneLog как событие `HOST_REACHABILITY / GET_HOST_REACHABILITY`. В payload:

- `hosts` — словарь `имя_хоста → bool`. Это собственно результат пинга.
- `operator` — строка `<networkOperator>:<networkOperatorName>` из `TelephonyManager` (MCCMNC код + название оператора).
- `connection_type` — текущая сеть: 0=none, 1=other, остальные значения из `bf4.b().a` (внутренний enum).
- `ip` — внешний IP (если успешно получен; см. ниже).
- `vpn` — `1` если `bf4.e()` (активный VPN).

То есть на сервер MAX уходит **полный отчёт о соединимости пользователя с внешним миром**: какие хосты доступны, через какого оператора, какой внешний IP, есть ли VPN.

## 4. Откуда берётся внешний IP

В `i58.B0 = new jai(new o2(21, this))` лениво инициализируется fetcher внешнего IP. По имени и lazy-init pattern — это HTTP-запрос на сторонний сервис (типа `api.ipify.org`, `checkip.amazonaws.com` или подобный). Прямого URL по строкам в jadx я не нашёл; вероятно, URL приходит из той же серверной конфигурации `host-reachability` (поле в JSON).

Без подключённого аккаунта и пойманного network-flow точное имя сервиса определения IP не назвать. Пресс-релиз MAX (май 2026, см. вступление в README) объяснил это «обеспечением корректной работы звонков» — что согласуется с использованием внешнего IP для STUN/ICE-candidate в WebRTC.

Скептический разбор: внешний IP действительно нужен в WebRTC для symmetric NAT detection, но в HostReachabilityChecker он попадает в общий аналитический payload, а не в SDP звонка. То есть это **отдельный канал отправки внешнего IP в аналитику MAX**.

## 5. Сводка факта vs журналистских утверждений

«MAX пытается соединяться с Telegram/WhatsApp/AWS/Google» — это **подтверждается архитектурой**: `HostReachabilityChecker` действительно делает TCP-handshake к произвольному списку хостов из серверной конфигурации. Конкретные хосты — не зашиты в клиенте, а приходят с сервера через PmsKey `host-reachability`. Поэтому в одном релизе клиента список может быть «google.com / telegram.org / whatsapp.com / aws.amazon.com», в другом — «yandex.ru / mail.ru». Содержимое определяется сервером.

«Передаётся информация об IP-адресах» — **подтверждается**: payload OneLog-события содержит `ip` поле.

«Передаётся информация об операторе и наличии VPN» — **подтверждается**: payload содержит `operator` (MCCMNC + название) и `vpn` флаг.

Объяснение MAX-пресс-службы про «корректность звонков» применимо к WebRTC-стороне (получение IP для STUN), но **не объясняет, почему IP, оператор, VPN и список достижимости хостов улетают в аналитический канал OneLog как одно событие**. Это отдельный канал телеметрии, не связанный с инициацией звонка.

## 6. Связанное: «Telegram first» в Share-chooser

Дополнительный отдельный сигнал — функция `defpackage/rp8.java` `getChooserIntentWithTgOnFirstPlaceOrDefault`:

- Перечисляет все приложения, могущие принять share-intent.
- Если среди них есть `org.telegram.messenger`, `org.telegram.messenger.beta`, `org.telegram.messenger.web` — **поднимает Telegram на первое место** в системном chooser-е.

Это side-effect: клиент **знает**, установлен ли у пользователя Telegram. Информация технически локальная (нужно `<queries>`-секция в манифесте либо API queryIntentActivities), но по такому wrapper-у видно, что код отдельно отслеживает наличие Telegram — что согласуется с темой "MAX проверяет блокировки и блокираторы".

Скептически: одиночное place-on-top в share-chooser — это не «слежка за Telegram». Но в комбинации с серверно-управляемым host-reachability checker'ом это выглядит как часть единого пайплайна «измерить экосистему пользователя».

## 7. Что важно

- **Список проверяемых хостов — серверный.** Сервер может в любой момент попросить клиент прозондировать произвольный список хостов и вернуть результат, плюс операторскую инфу, IP и VPN-статус.
- **Отчёт идёт через OneLog** (`ru.ok.android.onelog.UploadService`, см. [[04-telemetry-endpoints]]) — то есть тем же общим аналитическим каналом, что и метрики UI/перформанса.
- **VPN-флаг публикуется в каждом отчёте**, в дополнение к показу плашки «отключите VPN» (см. [[02-vpn-warning]]). То есть факт включённого VPN у пользователя записывается в телеметрию каждый раз при срабатывании HostReachabilityChecker.
- **Внешний IP попадает в аналитический payload**, не только в WebRTC SDP. Это отдельный канал.
- **Клиент знает, установлен ли Telegram** (для UI «Telegram-first share»), но это локальная проверка через PackageManager.

Скептический итог: подсистема `HostReachabilityChecker` — это **server-controlled censorship/blockage detection probe**, отдающий сырые результаты обратно на сервер MAX в составе обычной аналитики. По имени и поведению это не экспериментальный код; это штатный фоновый job клиента.
