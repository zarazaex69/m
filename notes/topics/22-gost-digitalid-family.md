---
tags: [government, gost, digital-id, family-protection, server-control, user-settings]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
  - work/jadx_base/sources/defpackage/qp6.java
  - work/jadx_base/sources/defpackage/u1h.java
  - work/jadx_base/sources/defpackage/sgj.java
  - work/jadx_base/sources/defpackage/ma4.java
related:
  - "[[03-pms-server-flags]]"
  - "[[11-state-bots-and-content-policy]]"
  - "[[20-ws-protocol-opcodes]]"
---

# GOST / Digital ID / Family Protection — где это в коде

## 1. Bot-ID идентификаторы (серверно-задаваемые)

Три PmsKey, значения которых — chat-id специальных «системных» ботов на сервере MAX. Клиент не хардкодит ID; он спрашивает у сервера.

| PmsKey | Имя | Что значит |
|---|---|---|
| `family-protection-botid` | `PmsKey.f114familyprotectionbotid` (#187) | chat-id бота «Семейная защита». UI-секция в настройках выводится только если бот настроен сервером. |
| `digitalid-botid` | `PmsKey.f98digitalidbotid` (HttpStatus.SC_USE_PROXY = #305) | chat-id бота «Цифровой ID» — государственная цифровая идентификация. |
| `gost-check-env` | `PmsKey.f127gostcheckenv` (#127) | флаги проверки ГОСТ-окружения (характерный интерфейс для приложений, работающих с КриптоПро/ВипНет). |

В `defpackage/qp6.java` все три выставлены через делегаты `oo6/no6` (мост к PmsKey-стораджу):

- `qp6.w0` — `PmsKey.f114familyprotectionbotid`
- `qp6.q2` — `PmsKey.f98digitalidbotid`
- `qp6.B2` — `PmsKey.f127gostcheckenv`

`qp6` — это объект-аккессор для всех 334 server-controlled параметров клиента (см. [[03-pms-server-flags]]).

## 2. Family Protection — UX

В `defpackage/u1h.java:234` есть лог-сообщение функции, которая добавляет UI-секцию «Семейная защита» в список настроек: «Early return in addSectionFamilyProtection cuz of !isFamilyProtectionEnabled». То есть UI секции появляется только если `isFamilyProtectionEnabled == true`.

`isFamilyProtectionEnabled` определяется не локальным переключателем, а полем UserSettings.familyProtection, которое приходит с сервера в одном объекте с другими настройками профиля (`defpackage/sgj.java` — `toString()` содержит `familyProtection=...`; `defpackage/ma4.java` — парсинг карты с ключом `familyProtection`).

То есть последовательность:

1. Сервер пушит UserSettings с `familyProtection: true`.
2. PmsKey `family-protection-botid` получает chat-id бота (которого тоже сервер задаёт).
3. UI добавляет соответствующую секцию.
4. По нажатию пользователь попадает в чат с этим серверным ботом.

Управление режимом — на сервере. Пользователь не может локально «отключить» бота, если сервер пушит `familyProtection: true`.

## 3. Digital ID — связки

Аналогично Family Protection, есть пара (PmsKey `digitalid-botid` + UserSettings-поле `digitalIdEnabled` — нужно проверить отдельно по строкам в jadx). Открытие функции — переход в чат с ботом DigitalID, который выполняет роль Identity Provider.

В сочетании с deeplink `https://max.ru/:auth?externalCallback=1` (см. [[13-deeplinks-idp]]) и WS-опкодом `EXTERNAL_CALLBACK(105)` (см. [[20-ws-protocol-opcodes]]) — это полный путь:

1. Пользователь авторизуется в MAX.
2. Внешний сервис открывает deeplink `:auth?externalCallback=1`.
3. MAX по WS оформляет подпись через `EXTERNAL_CALLBACK(105)`.
4. Внешний сервис получает подтверждение личности от MAX.

`digitalid-botid` — это chat-id, через который сервер MAX делает дополнительные шаги (например, привязывает паспортные данные через цифровой ID, согласно вступительной справке проекта).

## 4. GOST environment check

`PmsKey gost-check-env` — `Int`-значение, флаги. По имени и контексту — bitmask проверок наличия ГОСТ-крипто-окружения. Что именно проверяется (наличие SIM-устройства с ГОСТ-сертификатом? присутствие КриптоПро-провайдера? аппаратный ключ Рутокен?) — без вычитки исполнения этого PmsKey сказать нельзя. В коде клиента я не нашёл импорта `ru.CryptoPro.*` или подобных классов; то есть **сам клиент MAX 26.15.3 не содержит ГОСТ-крипто-провайдера**, только UI-хук под него.

Сценарий, к которому это собирается: при `gost-check-env != 0` клиент выполнит набор проверок «доступен ли ГОСТ-сертификат на этом устройстве» и сообщит результат серверу. Если ответ положительный — у пользователя появится дополнительный UI (электронная подпись), который привязан к серверной фиче «Госключ» (государственная электронная подпись РФ) через DigitalID-бот.

Подтвердить полную цепочку без ловли реального network-flow с зарегистрированным аккаунтом нельзя. Текущий статус: **в коде есть hook'и под ГОСТ-окружение, реализация самой ГОСТ-крипто отсутствует в этой версии**.

## 5. Что важно

- Все три фичи — Family Protection, DigitalID, GOST environment — **серверно-управляемые точки расширения**. Клиентский код содержит только хуки.
- Список ботов (`*-botid`) — не whitelist, не зашит в код. Сервер может в рантайме поменять chat-id любого из них на произвольный.
- `gost-check-env` — единственная PmsKey, которая по имени пахнет российской криптографией. Сам ГОСТ-провайдер в APK не вкомпилен.
- `family-protection-botid` и `digitalid-botid` пришли в эту версию через PmsKey механизм, и UserSettings `familyProtection` приходит в одном объекте с `safeMode`, `contentLevelAccess`, `phoneNumberPrivacy`, `searchByPhone`, `unsafeFiles`, `audioTranscriptionEnabled`, `hiddenOnline` (см. [[11-state-bots-and-content-policy]]). То есть гос-фильтры технически перемешаны с настройками приватности профиля и приходят одним пакетом.

Скептический итог: «семейная защита» в MAX — не встроенный родительский контроль, а UI-секция, появляющаяся по серверной команде и ведущая в чат-бот, поведение которого определяется на стороне сервера. Аналогично DigitalID. GOST-крипто в самой Android-сборке отсутствует, но интерфейс для проверки окружения подготовлен.
