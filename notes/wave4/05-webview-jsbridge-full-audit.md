# 05 — Полный аудит JS-bridge методов во всех WebView

**Версия:** 26.15.3 (подтверждено без изменений в 26.16.0, только обфускация имён)  
**Источники:** `defpackage/ahk.java`, `defpackage/ijk.java`, `defpackage/upk.java`, `defpackage/i6e.java`, `defpackage/hgk.java`, все delegate-классы (`lz8` implementations)  
**Статус:** complete

---

## 1. Архитектура JS-bridge

В MAX ровно **один WebView с JS-bridges** — `WebAppRootScreen` (мини-приложения/боты). Регистрация в `ahk.java`:

```java
v9gVar.addJavascriptInterface(new upk(webAppRootScreen4.s1()), "WebViewHandler");
v9gVar.addJavascriptInterface(new hgk(webAppRootScreen4.D0), "AndroidPerf");
if (webAppRootScreen4.s1().b1) {
    v9gVar.addJavascriptInterface(new i6e(webAppRootScreen4.s1()), "PrivateWebViewHandler");
}
```

**Другие WebView (VideoWebViewScreen, FaqWebViewWidget)** — НЕ имеют `addJavascriptInterface`. У них JS включён, но bridge отсутствует.

---

## 2. Сводная таблица JS-bridges

| # | JS interface name | Java class | Методов | Доступ | Критичные методы |
|---|---|---|---|---|---|
| 1 | `WebViewHandler` | `upk` | 2 | Все мини-аппы | `postEvent`, `resolveShare` |
| 2 | `PrivateWebViewHandler` | `i6e` | 1 | Только whitelist-боты (PmsKey `webapp-pr`) | `postEvent` (private=true) |
| 3 | `AndroidPerf` | `hgk` | 1 | Все мини-аппы | `trackFcp` (безопасный) |

---

## 3. Механизм маршрутизации событий

Оба `postEvent(name, data)` вызывают `ijk.A(name, data, isPrivate)`:

1. **Private-check:** если `isPrivate=true` и `b1=false` → событие отклоняется с логом
2. **User-click check:** для событий из `L1` = {`WebAppMaxShare`, `WebAppShare`, `WebAppDownloadFile`, `WebAppOpenLink`, `WebAppOpenMaxLink`} требуется клик пользователя в последние 3000ms — **НО** боты из PmsKey `webapp-exc` (серверный список) освобождены от этой проверки
3. **Routing:** событие передаётся в `gz8` (JsBridgeFactory) → ищется `lz8` (JsDelegate) по имени → вызывается `delegate.a(name, data)`

---

## 4. Полный перечень JS-bridge событий (32 метода)

### 4.1 Системные (delegate: `knk`, enum: `enk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppReady` | нет | Сигнал готовности | нет |
| `WebAppClose` | нет | Закрытие WebView | нет |
| `WebAppSetupBackButton` | нет | Показать/скрыть кнопку «назад» | нет |
| `WebAppSetupClosingBehavior` | нет | Подтверждение при закрытии | нет |
| `WebAppBackButtonPressed` | нет | Обработка нажатия «назад» | нет |
| `WebAppSetupScreenCaptureBehavior` | нет | **Снимает FLAG_SECURE** (topic 541) | **СРЕДНЯЯ** |

### 4.2 Ссылки (delegate: `bek`, enum: `xdk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppOpenLink` | нет | Открывает ЛЮБОЙ URL через `Intent.ACTION_VIEW` | **СРЕДНЯЯ** |
| `WebAppOpenMaxLink` | нет | Передаёт URL в внутренний deeplink-роутер (`:link-intercept`) | **ВЫСОКАЯ** |

### 4.3 Шаринг (delegate: `jlk`, enum: `alk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppShare` | нет | Системный share-intent (text + link, max 200 chars) | низкая |
| `WebAppMaxShare` | нет | Внутренний share (text, link, messageId, chatId) | низкая |

### 4.4 Haptic feedback (delegate: `odk`, enum: `edk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppHapticFeedbackImpact` | нет | Вибрация | нет |
| `WebAppHapticFeedbackNotification` | нет | Вибрация | нет |
| `WebAppHapticFeedbackSelectionChange` | нет | Вибрация | нет |

### 4.5 Телефон (delegate: `sgk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppRequestPhone` | нет | Запрашивает номер телефона пользователя | **СРЕДНЯЯ** |

**Защита:** показывает `ConfirmationBottomSheet` (пользователь должен подтвердить).  
**Ответ:** `{phone, hash, authDate}` — полный номер телефона + HMAC.  
**PmsKey `webapp-phone-hash`:** контролирует, включать ли hash в ответ.

### 4.6 Хранилище (delegate: `vmk`, enum: `lmk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppSecureStorageSaveKey` | нет | Сохранить в secure storage | низкая |
| `WebAppSecureStorageGetKey` | нет | Прочитать из secure storage | низкая |
| `WebAppSecureStorageClear` | нет | Очистить secure storage | низкая |
| `WebAppDeviceStorageSaveKey` | нет | Сохранить в device storage | низкая |
| `WebAppDeviceStorageGetKey` | нет | Прочитать из device storage | низкая |
| `WebAppDeviceStorageClear` | нет | Очистить device storage | низкая |

**Защита:** origin-check через `ijk.K()` — сравнивает origin запроса с `f1` (URL мини-аппа, установленный при загрузке). Каждый бот видит только своё хранилище.  
**Лимиты:** `webapp-ss-keys-count` = 10 (secure), `webapp-ds-keys-count` = 100 (device). Серверно-настраиваемые.

### 4.7 Биометрия (delegate: `k9k`, enum: `t8k`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppBiometryGetInfo` | нет | Получить тип биометрии устройства | низкая |
| `WebAppBiometryRequestAccess` | нет | Запросить доступ к биометрии | **СРЕДНЯЯ** |
| `WebAppBiometryUpdateToken` | нет | Сохранить токен, защищённый биометрией | **СРЕДНЯЯ** |
| `WebAppBiometryRequestAuth` | нет | Аутентифицировать через биометрию | **СРЕДНЯЯ** |
| `WebAppBiometryOpenSettings` | нет | Открыть настройки биометрии | низкая |

**Защита:** стандартный BiometricPrompt (пользователь подтверждает пальцем/лицом).  
**Риск:** мини-апп может хранить произвольный токен, защищённый биометрией пользователя. Это легитимная фича (аналог Telegram WebApp).

### 4.8 NFC (delegate: `rfk`, enum: `gfk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppNfcGetInfo` | нет | Проверить доступность NFC | низкая |
| `WebAppNfcEmulateNfcTag` | нет | **Эмулировать NFC-тег** (HCE) | **ВЫСОКАЯ** |
| `WebAppNfcOpenSystemSettings` | нет | Открыть настройки NFC | низкая |

**Параметры EmulateNfcTag:** `{queryId, requestId, nfctag}` — мини-апп передаёт данные для эмуляции NFC-тега.  
**Защита:** нет whitelist по ботам (доступно всем мини-аппам). Ошибки: `already_enabled`, `not_found`, `not_enabled`, `not_supported`.

### 4.9 QR/Code Reader (delegate: `tak`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppOpenCodeReader` | нет | Открыть камеру для сканирования QR/штрихкода | **СРЕДНЯЯ** |

**Защита:** стандартный permission-запрос камеры от Android.

### 4.10 Скачивание файлов (delegate: `nbk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppDownloadFile` | нет | Скачать файл по URL | **СРЕДНЯЯ** |

**Параметры:** `{requestId, url, file_name}`.  
**Защита:** user-click requirement (3s) — но боты из `webapp-exc` освобождены.

### 4.11 Viewport (delegate: `eok`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppGetViewportSize` | нет | Получить размер viewport | нет |

### 4.12 Яркость (delegate: `eak`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppChangeScreenBrightness` | нет | Изменить яркость экрана | низкая |

### 4.13 Verify Mobile ID — PRIVATE (delegate: `rnk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `WebAppVerifyMobileId` | **ДА** | HTTP-запрос к Mobile ID оператора через cellular (минуя VPN) | **КРИТИЧЕСКАЯ** |

**Параметры:** `{requestId, url}` — мини-апп передаёт URL, приложение делает HTTP-запрос через cellular network.  
**Ответ:** `{requestId, statusCode, headers, data}` — полный HTTP-ответ, включая заголовки оператора с MSISDN.  
**Защита:**
- Только для ботов из PmsKey `webapp-pr` (серверный whitelist)
- VPN-детект: если VPN активен → ошибка `has_vpn`
- Если нет cellular → ошибка `no_cellular`

**Риск:** бот из whitelist получает MSISDN абонента без его ведома (оператор подставляет заголовок в HTTP). Пользователь НЕ видит диалога подтверждения.

### 4.14 resolveShare (прямой метод в `upk`)

| Метод | Private | Действие | Опасность |
|---|---|---|---|
| `resolveShare` | нет | Отправить share-данные (text + binary + mimeType + fileName) | низкая |

**Параметры:** `resolveShare(String json, byte[] data, String mimeType, String fileName)`.  
**Защита:** user-click requirement (через `L1` set).

---

## 5. Контроль доступа: Public vs Private bridge

### Public bridge (`WebViewHandler`) — доступен ВСЕМ мини-аппам:
- 31 метод (все кроме `WebAppVerifyMobileId`)
- Включает: NFC HCE, камеру, телефон, биометрию, deeplinks, скачивание файлов
- User-click requirement для 5 методов (Share, MaxShare, Download, OpenLink, OpenMaxLink)

### Private bridge (`PrivateWebViewHandler`) — только whitelist:
- 1 метод: `WebAppVerifyMobileId`
- Whitelist: PmsKey `webapp-pr` — серверно-управляемый список bot ID
- Дополнительно: `WebAppHttpClient` (cellular-only HTTP) регистрируется только для private-ботов

### Серверные PmsKey, управляющие bridge:

| PmsKey | Назначение |
|---|---|
| `webapp-pr` | Список bot ID с доступом к PrivateWebViewHandler |
| `webapp-exc` | Список bot ID, освобождённых от user-click requirement |
| `webapp-phone-hash` | Включить hash в ответ WebAppRequestPhone |
| `webapp-ds-keys-count` | Лимит ключей device storage (default: 100) |
| `webapp-ss-keys-count` | Лимит ключей secure storage (default: 10) |

---

## 6. Критические находки

### 6.1 WebAppOpenMaxLink — deeplink injection без whitelist (НОВОЕ)

**Класс:** `bek` → `udk` → `wik` → `uhk`  
**Поток:**
1. Мини-апп вызывает `WebViewHandler.postEvent("WebAppOpenMaxLink", '{"url":"..."}')`
2. URL парсится как `Uri`, к нему добавляется `?webappChatId=...`
3. Результат передаётся в `:link-intercept` — внутренний deeplink-роутер MAX

**Проблема:** URL НЕ валидируется. Нет whitelist доменов, нет regex, нет проверки схемы. Мини-апп может передать:
- `max://` deeplinks (навигация внутри приложения)
- `https://max.ru/:auth?externalCallback=1` (использование MAX как IdP)
- Любой внутренний route

**User-click requirement:** да (3s), НО боты из `webapp-exc` освобождены.

**Severity:** ВЫСОКАЯ. Бот из `webapp-exc` может программно навигировать пользователя по любому deeplink без его ведома.

### 6.2 WebAppOpenLink — открытие произвольных URL

**Класс:** `bek` → `vdk` → `xhk`  
**Поток:** `startActivity(new Intent(ACTION_VIEW, Uri.parse(url)))` — без валидации.

**Проблема:** мини-апп может открыть любой URL/intent-scheme. Нет whitelist.  
**Mitigation:** user-click requirement (3s), но `webapp-exc` боты освобождены.

### 6.3 WebAppVerifyMobileId — silent MSISDN exfiltration

Уже задокументировано в topic 10, но уточнение: **пользователь НЕ видит никакого диалога**. Единственная защита — серверный whitelist `webapp-pr`. Сервер решает, какие боты получают MSISDN.

### 6.4 WebAppNfcEmulateNfcTag — HCE без whitelist

**Доступен ВСЕМ мини-аппам** (не требует private bridge). Мини-апп может эмулировать NFC-тег с произвольными данными. Потенциально: клонирование пропусков, платёжных карт (если данные известны).

**Mitigation:** Android HCE API требует, чтобы приложение было в foreground. Но мини-апп внутри MAX — это foreground.

### 6.5 Server-side control over bridge access

Сервер через `webapp-exc` может в любой момент дать конкретному боту возможность:
- Открывать deeplinks без клика пользователя
- Скачивать файлы без клика
- Шарить контент без клика

Это не backdoor per se, но даёт серверу возможность «включить» автоматические действия для конкретного бота адресно.

---

## 7. Что НЕ является проблемой

- **WebAppRequestPhone** — показывает диалог подтверждения (пользователь видит запрос)
- **Biometry** — стандартный BiometricPrompt (пользователь подтверждает)
- **Storage** — изолировано по origin, лимитировано по количеству ключей
- **WebAppOpenCodeReader** — стандартный permission-запрос камеры
- **AndroidPerf.trackFcp** — только отправляет метрику FCP (First Contentful Paint), безопасно
- **resolveShare** — требует user-click, передаёт данные ОТ мини-аппа (не к нему)

---

## 8. Сравнение с Telegram WebApp API

MAX JS-bridge — это расширенный клон Telegram Bot API WebApp с добавлением:
- `WebAppVerifyMobileId` (нет в Telegram) — silent MSISDN через Mobile ID
- `WebAppNfcEmulateNfcTag` (нет в Telegram) — HCE эмуляция
- `WebAppSetupScreenCaptureBehavior` (нет в Telegram) — снятие FLAG_SECURE
- `WebAppOpenMaxLink` (аналог `openTelegramLink`) — но без whitelist домена

В Telegram `openTelegramLink` принимает только `https://t.me/...`. В MAX `WebAppOpenMaxLink` принимает **любой URL** и передаёт его в deeplink-роутер.

---

## 9. Кандидаты для отдельных topic (548+)

| # | Тема | Severity |
|---|---|---|
| 548 | `WebAppOpenMaxLink` deeplink injection без URL whitelist | HIGH |
| 549 | `webapp-exc` PmsKey: серверное отключение user-click requirement per-bot | MEDIUM |
| 550 | `WebAppNfcEmulateNfcTag` доступен всем мини-аппам без whitelist | MEDIUM |

---

## 10. Методология

1. `grep -rln 'addJavascriptInterface'` → найдено 1 файл (`ahk.java`), 3 регистрации
2. Для каждого bridge-класса (`upk`, `i6e`, `hgk`) — прочитан полный код
3. Трассировка `postEvent` → `ijk.A()` → `gz8` → `ez8` → `lz8` delegates
4. Для каждого delegate — прочитан enum с именами методов и реализация
5. Cross-reference с PmsKey через `qp6.java` и `PmsKey.java`
6. Проверка VideoWebViewScreen и FaqWebViewWidget — подтверждено отсутствие JS-bridges
