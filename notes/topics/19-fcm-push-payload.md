---
tags: [push, fcm, server-control, surveillance, command-channel]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/sdk/vendor/push/FcmMessagingService.java
  - work/jadx_base/sources/defpackage/cei.java
  - work/jadx_base/sources/defpackage/eei.java
  - work/jadx_base/sources/defpackage/dmf.java
  - work/jadx_base/sources/defpackage/nn6.java
  - work/jadx_base/sources/defpackage/fei.java
related:
  - "[[03-pms-server-flags]]"
  - "[[18-manifest-deep-dive]]"
---

# FCM push payload — типы, ключи, что сервер может пушить

`one.me.sdk.vendor.push.FcmMessagingService extends FirebaseMessagingService` — единственный обработчик cloud-push-ей. Логика парсинга в `defpackage/cei.java` (coroutine, на которую делегирует FCM-сервис).

## Маршрутизация

В `FcmMessagingService.d(emfVar)` (`onMessageReceived`):

1. Из `Bundle` payload фильтруются `google.*`, `gcm.*`, `from`, `message_type`, `collapse_key` — остаются «бизнес»-ключи.
2. Извлекается `google.delivered_priority` / `google.priority` → priority.
3. Заворачивается в `dmf(map, priority)` («RemoteMessage»).
4. В `eeiVarF.i` (cloud-pushes coroutine scope) запускается `cei`-обработчик.

## Гейтинг recipient

В `cei.n(...)`:

```java
String str2 = (String) map.get("c");          // "c" — recipient userId
Long  recipient = parseLong(str2);
if (PmsKey[160].booleanValue() && recipient != null) {
    if (recipient != currentUser.id) {
        log("onMessageReceived: unknown consignee (recipient)!");
        return;                                // отбрасывается
    }
}
```

То есть push, в котором `c` ≠ ID текущего пользователя — отбрасывается. Хорошо. Это защищает от случайной дельта-доставки.

## Типы push (по полю `type`)

После recipient-гейта `cei` смотрит на `map.get("type")` и `map.get("mc")` / `"suid"` / `"msgid"` для рутинга:

### 1. Обычное сообщение

Условие: `(map["mc"] || map["suid"]) && map["msgid"]` и `type != "MessageRemoved"/...`.

Парсинг через `eeiVar2.c().c(currentUser.id, map)` → `on6` объект. Затем уведомление через `fei.a().d(on6, analyticsMeta, ...)`.

В payload, помимо `mc`, `suid`, `msgid`, типично присутствует и тело уведомления (text/title) — но содержимое сообщения может приходить и через WS (`api.oneme.ru`); push в этом случае — только триггер.

### 2. `MessageRemoved` / `ChatMessageRemoved` / `ChatMessageRemoved-channel`

`type` ∈ {…}. Делегирует в `eei.a(eeiVar, map, ...)`:

```java
nn6 parsed = eeiVar.c().d(currentUser.id, map);   // парсит {chatId, msgId}
fei.a().e(parsed, ...);   // удаляет сообщение из локальной БД и нотификации
```

То есть **сервер по push может удалить сообщение из локальной базы клиента**. Это нужно для disappearing messages и серверных модераций (например, удалить пост в канале), но это и канал «удалить сообщение задним числом без видимого следа».

### 3. `InboundCall`

`type == "InboundCall"`. Извлекаются ключи:

```
trid       (transaction id)         long
eKey       (encryption key)         string
suid       (sender userId)          long
chatId     (целевой чат)            long
vcId       (voice call id)          string
vcp        (VC params)              string
iv         (incoming video)         bool
userName   (отображаемое имя)       string
country    (страна вызова)          string  ← origin
phn        (phone)                  long?
rt         (?)                      long?
isContact  (флаг контактности)      bool
```

Это полный набор для отображения «входящий звонок от X» без интерактивного шага. `trid`/`eKey`/`vcId` — токены для подключения к WebRTC-сессии.

Если `forceUpdateLogic.isNeedForceUpdate()` (см. [[12-force-update-killswitch]]) — звонок не показывается, отчёт `SKIP_PUSH_FORCE_UPDATE`.

### 4. `TamtamSpam` — push с deeplink

`type == "TamtamSpam"` (имя оригинальное, из TamTam-наследия). Извлекает:

```
uri       — URI для нотификации
msg       — текст
title     — заголовок
imageUrl  — картинка нотификации
```

`fei.b(uri, msg, title, imageUrl)` — строит большую (`u1c` BigText / BigPicture) нотификацию в канале `ru.oneme.app.misc`, по тапу — открывает `uri`. Это **server-pushed deeplink** в произвольное место приложения (включая мини-апку или внешний сайт через `LinkInterceptorActivity`). См. [[13-deeplinks-idp]].

Скептический разбор: это маркетинговый канал, но ничто в коде не ограничивает `uri` определённой схемой. По факту сервер MAX может в любой момент отправить пользователю пуш-нотификацию, ведущую на любой URI, и пользователь увидит её в системной шторке от имени MAX.

### 5. `LocationRequest`

`type == "LocationRequest"`. `cei` зовёт:

```java
eeiVar5.d().a().f(false, !((oe4) r0.a.getValue()).c());
ct4.E(fei.class.getName(), "onLocationRequestPush");
```

`eqe.f(boolean callPush, boolean forceConnection)` (см. `defpackage/eqe.java:320`):

```java
public final void f(boolean z, boolean z2) {
    log("onPush: callPush=" + z + ", forceConnection=" + z2);
    f3e.a.G = System.currentTimeMillis();   // lastPushTime
    if (z2) {
        ((f3e) this.c.getValue()).a.A(true);                 // forceWsReconnect
        iodVar.k = iodVar.b();
        ((ucc) iodVar.d.getValue()).B(iodVar.k);
        ((zgi) this.e.getValue()).a();                       // background sync trigger
    }
}
```

То есть `LocationRequest` push **не отправляет координаты сразу**. Он только:

- ставит `lastPushTime`;
- если WS не подключён — поднимает соединение с `api.oneme.ru` (`forceConnection`);
- триггерит background sync.

Дальше уже сервер MAX по WS-каналу запрашивает у клиента то, что хочет (включая, видимо, `send-location` если PmsKey `send-location-enabled` true). То есть **в самом push-е координаты не возвращаются**, но push используется как «разбуди клиент, чтобы он подключился к нам» — это паттерн silent push.

PmsKey `send-location-enabled` (см. [[03-pms-server-flags]]) — отдельный feature-flag, после WS-handshake сервер может попросить координаты через WS-команду.

Скептический статус: `LocationRequest` push — это **wake-trigger**, не direct location upload. Но в комбинации с PmsKey `send-location-enabled` сервер имеет полный путь «послать silent-push → дождаться WS → запросить координаты». Это не «зеро-клик трекинг», но достаточный.

### 6. `ConversationReadOnOtherDevice`

`type == "ConversationReadOnOtherDevice"`. Нет дополнительного действия в `cei` — просто пропускается (видимо, обрабатывается отдельной веткой выше или используется как маркер сессии). Это «прочитано на другом устройстве» — sync read-receipt.

## Ключи payload (сводная таблица)

| Ключ | Что | Используется в push-типах |
|---|---|---|
| `c` | recipient userId (gating) | все |
| `type` | тип события | все, кроме обычного сообщения |
| `mc` | message-channel id | обычное сообщение |
| `suid` | sender userId | обычное / InboundCall |
| `msgid` | message id | обычное / *Removed |
| `chatId` | id чата | InboundCall, *Removed |
| `trid` | transaction id | InboundCall |
| `eKey` | session key | InboundCall |
| `vcId` | voice/video call id | InboundCall |
| `vcp` | VC params | InboundCall |
| `iv` | bool incoming-video | InboundCall |
| `userName` | display name | InboundCall |
| `country` | страна звонка | InboundCall |
| `phn` | phone | InboundCall |
| `rt` | ? | InboundCall |
| `isContact` | bool, в контактах ли звонящий | InboundCall |
| `uri` | deeplink | TamtamSpam |
| `msg` | текст | TamtamSpam |
| `title` | заголовок | TamtamSpam |
| `imageUrl` | URL картинки | TamtamSpam |

## Что важно

- Список push-типов: `обычное`, `MessageRemoved`, `ChatMessageRemoved`, `ChatMessageRemoved-channel`, `InboundCall`, `TamtamSpam` (deeplink), `LocationRequest`, `ConversationReadOnOtherDevice`. Это **командный канал**, а не только нотификации.
- **`MessageRemoved` push** позволяет серверу удалять сообщения у клиента (стандартный механизм для каналов и disappearing-msg, но он же — канал «без следа уберите сообщение Х»).
- **`TamtamSpam` push с произвольным `uri`** — открывает deeplink с любым URI. Не ограничено в коде.
- **`LocationRequest` push** — есть отдельная ветка обработки, имени соответствующая. Прямого подтверждения отправки координат в ответ — пока нет, требует разбора `eqe.f(...)`.
- Token обновление (`onNewToken`) уходит в `((yag) eeiVarF.b()).D(token)` → регистрация на сервере MAX.
- Канал FCM — единственный для cloud-push (Firebase). Для Huawei/HMS/MyPush в коде я не вижу отдельных handler-ов в этой версии (нужно проверить отдельно).

## Скептический итог

С точки зрения «что может сделать сервер по push без явного действия пользователя»:

1. Показать нотификацию с произвольным deeplink (`TamtamSpam`).
2. Удалить сообщение из локальной БД (`MessageRemoved*`).
3. Триггернуть UI входящего звонка (`InboundCall`).
4. Предположительно — запустить процесс обновления геопозиции (`LocationRequest`, требует доп. проверки).

Это типовой набор для современных мессенджеров, но в случае MAX выделяется наличие явной push-команды с именем `LocationRequest` и отсутствие whitelist-а схем для `TamtamSpam.uri`.
