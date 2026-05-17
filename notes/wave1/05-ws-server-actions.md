# WS-опкоды: серверно-инициируемые действия на клиенте

> Анализ 161 WS-опкода TamTam/MAX-протокола с фокусом на **серверно-инициируемые действия** — команды, которые сервер отправляет клиенту и клиент выполняет без подтверждения пользователя.

## TL;DR — 5 самых опасных опкодов

| # | Опкод | Код | Действие | Уровень угрозы |
|---|-------|-----|----------|----------------|
| 1 | **DEBUG** | 2 | Сервер отправляет команды `SYNC_CONTACTS` (принудительная выгрузка контактов) и `SEND_LOG` (принудительная отправка логов на сервер). Поле `args` — массив строк, потенциально расширяемый. | 🔴 CRITICAL |
| 2 | **RECONNECT** | 3 | Сервер перенаправляет клиент на **произвольный host:port** (`redirectHost`). Клиент сохраняет новый `serverHost`/`serverPort` в настройки и переподключается. Поле `tls` может быть `false` → MitM. | 🔴 CRITICAL |
| 3 | **NOTIF_CONFIG** | 134 | Сервер пушит полную конфигурацию: 334 PmsKey, serverSettings, userSettings, experiments. Включает `proxy`/`proxydomains` — перенаправление трафика через произвольный прокси. Включает `forceUpdateVersion` → killswitch. | 🔴 CRITICAL |
| 4 | **NOTIF_CALL_START** | 137 | Сервер инициирует входящий звонок. В контексте звонка доступны sub-notifications: `switch-micro` (принудительное включение/выключение микрофона), `force-media-settings-change` (принудительное отключение камеры), `asr-started` (включение серверной транскрипции), `record-started` (начало записи). | 🔴 CRITICAL |
| 5 | **NOTIF_BANNERS** | 292 | Сервер пушит баннеры с полем `url` — при клике открывается произвольный URL/deeplink. Баннеры хранятся в БД `informer_banner` с `priority`/`repeat`/`rerun` — навязчивый показ. | 🟠 HIGH |

---

## Полная таблица серверно-инициируемых опкодов

### Категория 1: Прямые команды (сервер → действие на клиенте)

| Опкод | Код | Клиентское действие | Источник |
|-------|-----|---------------------|----------|
| **DEBUG** | 2 | Парсит поле `cmd`: `SYNC_CONTACTS` → принудительная синхронизация контактов (вызов `xh8.K` на БД + `Phonebook.sync`); `SEND_LOG` → выгрузка логов через `ckc.a(IllegalStateException)`. Поле `args` — массив строк-аргументов. | `v75.java`, `n77.java:240` |
| **RECONNECT** | 3 | Парсит `redirectHost` (формат `host:port`) и `tls` (boolean). Сохраняет в `ri9.serverHost`/`ri9.serverPort`/`ri9.useTls`. Перезапускает WS-соединение на новый адрес. | `fef.java`, `pb8.java:370-400` |
| **NOTIF_CONFIG** | 134 | Доставляет `cb4` объект: `hash`, `serverSettings` (e4), `pmsMap`, `userSettings` (sgj), `experiments`. Обновляет все 334 PmsKey. Устанавливает `proxy`/`proxydomains`. | `ozb.java`, `cb4.java` |
| **NOTIF_LOCATION_REQUEST** | 148 | Логирует `onNotifLocationRequest`. Триггерит запрос GPS-координат пользователя. | `wzb.java:18` |
| **NOTIF_CALL_START** | 137 | Инициирует входящий звонок (если не в режиме ForceUpdate). Запускает CallsEngine. | `pb8.java:497` |
| **NOTIF_BANNERS** | 292 | Пушит массив баннеров с `url`, `title`, `priority`, `repeat`, `rerun`, `showTime`. Сохраняет в `informer_banner` таблицу. | `ayb.java:591-600` |

### Категория 2: Внутри-звонковые sub-notifications (через JSON `notification` поле)

Эти команды приходят **внутри активного звонка** через тот же WS-канал (обработчик `z91.java`):

| Sub-notification | Действие | Подтверждение пользователя |
|-----------------|----------|---------------------------|
| **`switch-micro`** | Принудительное mute/unmute микрофона (`cj1.I0`/`cj1.J0`) | ❌ НЕТ |
| **`force-media-settings-change`** | Принудительное отключение камеры (`bobVar.e = false`) | ❌ НЕТ |
| **`mute-participant`** | Сервер мьютит участника | ❌ НЕТ |
| **`asr-started`** | Включение серверной транскрипции (ASR) | ❌ НЕТ |
| **`asr-stopped`** | Выключение серверной транскрипции | ❌ НЕТ |
| **`record-started`** | Начало записи звонка на сервере | ❌ НЕТ (только UI-уведомление) |
| **`record-stopped`** | Остановка записи | ❌ НЕТ |
| **`topology-changed`** | Переключение P2P↔Server topology (при server topology весь трафик через сервер) | ❌ НЕТ |
| **`transmitted-data`** | Инъекция SDP offer/answer — сервер может перенаправить медиа-поток | ❌ НЕТ |
| **`closed-conversation`** | Принудительное завершение звонка | ❌ НЕТ |
| **`hungup`** | Удаление из звонка | ❌ НЕТ |
| **`roles-changed`** | Изменение ролей участника | ❌ НЕТ |
| **`options-changed`** | Изменение опций звонка | ❌ НЕТ |
| **`custom-data`** | Произвольные данные в звонке | ❌ НЕТ |
| **`pin-participant`** | Закрепление участника | ❌ НЕТ |

### Категория 3: Управление данными (сервер → модификация клиентской БД)

| Опкод | Код | Действие |
|-------|-----|----------|
| **NOTIF_MESSAGE** | 128 | Доставка сообщений (включая invisible — topic 538) |
| **NOTIF_MSG_DELETE** | 142 | Удаление сообщений из клиентской БД |
| **NOTIF_MSG_DELETE_RANGE** | 140 | Массовое удаление диапазона сообщений |
| **NOTIF_CHAT** | 135 | Обновление метаданных чата |
| **NOTIF_CONTACT** | 131 | Обновление контакта в клиентской БД |
| **NOTIF_CONTACT_SORT** | 139 | Изменение порядка контактов |
| **NOTIF_ASSETS_UPDATE** | 150 | Обновление ассетов |
| **NOTIF_DRAFT** | 152 | Серверная синхронизация черновиков |
| **NOTIF_DRAFT_DISCARD** | 153 | Удаление черновика |
| **NOTIF_FOLDERS** | 277 | Обновление папок |
| **NOTIF_PROFILE** | 159 | Обновление профиля |
| **NOTIF_TRANSCRIPTION** | 293 | Доставка транскрипции |

### Категория 4: Триггеры через deeplinks/callbacks

| Опкод | Код | Действие |
|-------|-----|----------|
| **EXTERNAL_CALLBACK** | 105 | Навигация по deeplink `:external_callback` с произвольным `params` |
| **MSG_SEND_CALLBACK** | 118 | Бот-callback → `CallbackAnswerEvent` с `text` |
| **NOTIF_CALLBACK_ANSWER** | 143 | Ответ бота на callback — доставляет `chatId` + `text` |

### Категория 5: Управление сессией

| Опкод | Код | Действие |
|-------|-----|----------|
| **LOGOUT** | 20 | Принудительный logout |
| **SESSIONS_CLOSE** | 97 | Закрытие сессий |
| **PING** | 1 | Keep-alive (клиент обязан ответить) |
| **LOG** | 5 | Серверный запрос логов |

---

## Детальный анализ критических опкодов

### 1. DEBUG (код 2) — Серверная команда произвольного действия

**Файл:** `defpackage/v75.java`

```java
public final class v75 extends jei {
    public ay6 c;  // команда: SYNC_CONTACTS | SEND_LOG | unknown
    public List d;  // args — массив строк

    public final void c(kxa kxaVar, String str) {
        if (str.equals("cmd")) {
            String strU = j8h.U(kxaVar);
            if (strU.equals("SYNC_CONTACTS")) {
                ay6Var = ay6.X;
            } else if (strU.equals("SEND_LOG")) {
                ay6Var = ay6.o;
            }
        }
        if (str.equals("args")) {
            // массив произвольных строк-аргументов
            this.d = new ArrayList(iJ);
        }
    }
}
```

**Обработчик** (`n77.java:240`):
- `SEND_LOG` → `((ckc) rzbVar.a).a(new IllegalStateException("onNotifDebug"))` — триггерит crash-reporter, который выгружает логи на `sdk-api.apptracer.ru`
- `SYNC_CONTACTS` → `xh8.K(db, false, true, ...)` + `Phonebook.sync()` — принудительная выгрузка всей адресной книги на сервер

**Критичность:** Сервер может в любой момент:
1. Выгрузить все контакты пользователя
2. Выгрузить логи приложения (которые содержат URL, chatId, userId, timestamps)
3. Потенциально расширяемо — поле `cmd` парсится как строка, новые команды могут быть добавлены серверно

### 2. RECONNECT (код 3) — Перенаправление на произвольный сервер

**Файл:** `defpackage/fef.java`

```java
public final class fef extends jei {
    public String c;    // redirectHost — формат "host:port"
    public boolean d;   // tls — может быть false!

    public final void c(kxa kxaVar, String str) {
        if (str.equals("tls")) {
            this.d = kxaVar.L0();  // boolean
        } else if (str.equals("redirectHost")) {
            this.c = j8h.U(kxaVar);  // произвольная строка
        }
    }
}
```

**Обработчик** (`pb8.java:370-400`):
```java
if (fefVar.c.length() > 0) {
    ri9Var.serverHost = fefVar.d();  // сохраняет новый host
    ri9Var.serverPort = fefVar.e();  // сохраняет новый port
    ri9Var.useTls = fefVar.d;        // может быть FALSE
}
// перезапуск соединения на новый адрес
((pwb) ueiVar.C0.getValue()).v(false);
```

**Критичность:** Сервер может:
1. Перенаправить клиент на **любой** IP/домен
2. Отключить TLS (`tls: false`) → весь WS-трафик в открытом виде
3. Это сохраняется в SharedPreferences — переживает перезапуск приложения
4. Комбинация с DoH C2 redirect (topic 533) = полный MitM

### 3. NOTIF_CONFIG (код 134) — Полная перезапись конфигурации

**Обработчик** (`ozb.java`):
- Step 1: hash (версия конфига)
- Step 2: `serverSettings` — включает `proxy` и `proxydomains` (перенаправление HTTP-трафика через произвольный прокси)
- Step 3: `userSettings` — пользовательские настройки
- Step 4: `experiments` — A/B флаги (могут включать/выключать любую функциональность)

**Ключевые PmsKey, устанавливаемые через этот опкод:**
- `proxy` / `proxydomains` — HTTP-прокси для всего трафика
- `forceUpdateVersion` — killswitch
- `keepconnection` — фоновое соединение
- Все 334 ключа из `pms_keys.txt`

### 4. Внутри-звонковые команды — Полный контроль медиа

Сервер через JSON-notifications внутри звонка может:
1. **`switch-micro`** — включить/выключить микрофон без ведома пользователя
2. **`force-media-settings-change`** — отключить камеру
3. **`topology-changed`** → переключить на server topology → весь аудио/видео через сервер
4. **`asr-started`** — включить серверную транскрипцию (AsrOnlineManager)
5. **`record-started`** — начать запись звонка
6. **`transmitted-data`** с `p2pRelay: true` — принудительный relay через сервер

**Цепочка атаки:**
```
NOTIF_CALL_START → topology-changed(server) → asr-started → record-started
= Сервер инициирует звонок, переключает на серверную топологию,
  включает транскрипцию и запись — всё без согласия пользователя
```

### 5. NOTIF_BANNERS (код 292) — Серверно-управляемые URL-баннеры

Баннеры содержат:
- `url` — произвольный URL/deeplink
- `priority` — приоритет показа
- `repeat` / `rerun` — повторный показ
- `settings` — флаги поведения

При клике на баннер открывается URL, который может быть:
- Deeplink MAX (`max://...`) → навигация внутри приложения
- Внешний URL → открытие в WebView (с JS-bridge!)
- `startwebapp` path → запуск мини-приложения

---

## Дополнительные находки

### ForceUpdateLogic через CONFIG

Класс `ru.ok.tamtam.ForceUpdateLogic` (mel.java:170) проверяется при каждом NOTIF_CALL_START. Если `isNeedForceUpdate() == true`, звонки блокируются. Это значит:
- Сервер через NOTIF_CONFIG может установить `forceUpdateVersion` → заблокировать ВСЕ функции приложения
- Единственный выход — скачать новую версию с `download.max.ru` (мимо Play Store)

### WebApp JS-Bridge delegates (полный список)

Мини-приложения через JS-bridge имеют доступ к:
- `WebAppRequestPhone` — получение номера телефона
- `WebAppDownloadFile` — скачивание файлов
- `WebAppOpenLink` / `WebAppOpenMaxLink` — открытие URL
- `WebAppShare` / `WebAppMaxShare` — шаринг контента
- `WebAppVerifyMobileId` — верификация Mobile ID (MSISDN)
- `WebAppNfcEmulateNfcTag` — эмуляция NFC-тега
- `WebAppNfcOpenSystemSettings` — открытие системных настроек NFC
- `WebAppBiometryAuth` / `WebAppBiometryGetInfo` — доступ к биометрии
- `WebAppSetupScreenCaptureBehavior` — управление screen capture
- `WebAppChangeScreenBrightness` — изменение яркости
- `WebAppOpenCodeReader` — открытие сканера QR/штрих-кодов
- `WebAppStorageSaveKey` / `WebAppStorageGetKey` / `WebAppStorageClear` — локальное хранилище

### EXTERNAL_CALLBACK (код 105) — Deeplink-навигация

Опкод `EXTERNAL_CALLBACK` используется для навигации по deeplink `:external_callback` с произвольным параметром `params`. Это позволяет серверу:
- Открыть любой экран приложения
- Передать произвольные параметры
- Использовать MAX как Identity Provider для внешних приложений

---

## Выводы

1. **Протокол не имеет клиентской валидации команд.** Все NOTIF-опкоды выполняются безусловно.
2. **DEBUG опкод — это backdoor.** Две команды (`SYNC_CONTACTS`, `SEND_LOG`) + расширяемое поле `args`.
3. **RECONNECT — MitM by design.** Сервер может перенаправить на любой хост с отключённым TLS.
4. **Внутри-звонковые команды дают полный контроль** над микрофоном, камерой, записью и транскрипцией.
5. **Нет E2E — всё проходит через сервер.** Комбинация topology-changed + asr-started = прослушка в реальном времени.

---

## Файлы-источники

- `defpackage/u0d.java` — enum всех 161 опкодов
- `defpackage/pb8.java` — главный WS-диспетчер NOTIF-сообщений
- `defpackage/v75.java` — payload DEBUG-команды
- `defpackage/fef.java` — payload RECONNECT
- `defpackage/ozb.java` — NotifConfigLogic
- `defpackage/z91.java` — обработчик внутри-звонковых notifications (49 sub-types)
- `defpackage/n77.java` — обработчики NOTIF_ATTACH, NOTIF_CALLBACK_ANSWER, onNotifDebug
- `defpackage/wzb.java` — NOTIF_LOCATION_REQUEST handler
- `findings/raw/ws_opcodes.txt` — полный список 159 опкодов
