---
tags: [idp, oauth, external-callback, ws, deeplink, government]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/android/externalcallback/ExternalCallbackWidget.java
  - work/jadx_base/sources/defpackage/ei6.java
  - work/jadx_base/sources/defpackage/fi6.java
  - work/jadx_base/sources/defpackage/zh6.java
  - work/jadx_base/sources/defpackage/uq4.java
  - work/jadx_base/sources/defpackage/x8b.java
  - work/jadx_base/sources/defpackage/u0d.java
related:
  - "[[13-deeplinks-idp]]"
  - "[[20-ws-protocol-opcodes]]"
  - "[[10-webapp-jsbridge]]"
  - "[[22-gost-digitalid-family]]"
---

# ExternalCallback / OK_TOKEN — MAX как Identity Provider

В дополнение к [[13-deeplinks-idp]] (deeplink `:auth?externalCallback=1`) и опкоду `EXTERNAL_CALLBACK(105)` из [[20-ws-protocol-opcodes]] — здесь конкретика об архитектуре flow.

## 1. Цепочка компонентов

| Компонент | Роль |
|---|---|
| `:auth?externalCallback=1` deeplink | вход с external-host'а |
| `:external_callback?params=<...>` deeplink | внутренний роут (`zh6.java:13`) |
| `defpackage/uq4.java:177` | передача `external_callback_param_arg` через Intent |
| `one.me.android.externalcallback.ExternalCallbackWidget` | bottom-sheet UI «подтвердить вход в внешний сервис» |
| `defpackage/fi6.java` | ViewModel `ExternalCallbackViewModel` |
| `defpackage/ei6.java` | coroutine handler, делает WS-запрос |
| `defpackage/ci6.java` | WS request формирователь |
| `defpackage/wh6.java` | WS response (содержит поле `c: Long?`, `d`) |
| WS-опкод `EXTERNAL_CALLBACK = 105` | сам запрос на сервер |

## 2. Поток

1. Внешнее приложение/сайт открывает на устройстве deeplink `https://max.ru/:auth?externalCallback=1` или `max://...:external_callback?params=...`.
2. `LinkInterceptorActivity` парсит, передаёт `external_callback_param_arg` в Intent.
3. Открывается `ExternalCallbackWidget` (bottom-sheet) — UI для подтверждения пользователем.
4. `fi6.X` (coroutine `umh`) запускает `ei6` → внутренний `ci6` посылает WS-запрос с опкодом 105.
5. Сервер MAX отвечает (`wh6`) с полем `c` (видимо, target user id или session id) и `d` (нагрузка).
6. Если `c != null` — клиент через `vq9.c.n0(c, b6k.Y, d)` оформляет ответ-навигацию (передаёт сигнатуру обратно во внешнее приложение).
7. Если запрос упал (`Throwable`) — клиент логирует «ExternalCallback request failed due to …» и кидает в UI ошибку.

## 3. Что значит для безопасности

- **Сервер MAX выступает в роли Identity Provider** (как Google Sign-In, Apple ID): внешнее приложение редиректит пользователя в MAX, MAX подтверждает личность, возвращает подпись/токен.
- Подпись/токен оформляется **серверной стороной MAX**; клиент только UI-обёртка для пользовательского согласия.
- Идентичность пользователя — `userId` MAX-аккаунта + связанные с ним атрибуты (имя, аватар, привязанный номер телефона, и потенциально DigitalID-данные через `digitalid-botid`, см. [[22-gost-digitalid-family]]).
- Параллельно есть `OK_TOKEN(158)` — отдельный опкод для выдачи токена в экосистему OK.ru / VK ID. Можно использовать MAX-сессию для входа в Mail.ru, ВКонтакте, Одноклассники, RuStore и другие сервисы холдинга.

## 4. Что особенного у `:auth?externalCallback=1`

Этот вариант (с `?externalCallback=1`) — именно тот, через который **внешние сервисы могут запросить вход через MAX как через OIDC-провайдер**. Параметры (state, nonce, redirect-uri и т.д.) — в `external_callback_param_arg`, который сервер сам валидирует.

В Wikipedia упоминается, что MAX поддерживает связь с ЕСИА (Госуслуги) через OpenID Connect. Если MAX оформлен как RP в ЕСИА, то цепочка:

`Внешний сервис → MAX (IdP) → ЕСИА (upstream IdP) → Госуслуги аккаунт`

Технически возможна, и `EXTERNAL_CALLBACK(105)` — серверный вход для такой цепочки. Подтверждать без живого аккаунта нельзя; в коде клиента видна **только локальная сторона flow** (UI согласия + WS-вызов с параметрами).

## 5. Скептический разбор

- Identity Provider — нормальная штатная фича для современного мессенджера. Это не «бэкдор».
- НО: совмещённый с PmsKey `digitalid-botid` (см. [[22-gost-digitalid-family]]) и инструкцией про Госуслуги (Wikipedia: «получать электронные государственные услуги, удостоверять личность, использовать усиленную электронную подпись и цифровой ID»), это означает, что **MAX-аккаунт постепенно становится единым Russian-state-bound user identity**.
- Опкод `OK_TOKEN(158)` обеспечивает SSO в смежные сервисы экосистемы. То есть взлом или санкции против MAX-аккаунта аффектируют доступ к Госуслугам, банкингу через MAX-боты (ВТБ, Альфа), и другим интегрированным сервисам.

## 6. Сводка

ExternalCallback flow — это **штатный OAuth-style Identity Provider stack**, оформленный отдельным WS-опкодом + deeplink-схемой + UI bottom-sheet согласия. Архитектурно — нормальная штука. Стратегически — централизация идентичности пользователя в государственном мессенджере с обязательной предустановкой создаёт single-point-of-control для всей цифровой личности в РФ.
