# Deeplink-схема: `:auth` external callback и Identity Provider

## Регистрация

Манифест регистрирует `LinkInterceptorActivity` для:

```
https://max.ru/...
http://max.ru/...
max://max.ru/...
```

`autoVerify="true"` для https — Android автоматически забирает у пользователя выбор «открывать в браузере или MAX» и навсегда отдаёт MAX.

## Распознаваемые «специальные» пути в `defpackage/gb9.java`

В коде явно перечислены:

| Путь | Что значит |
|---|---|
| `https://max.ru/:auth`  | **аутентификация по ссылке** (см. ниже) |
| `https://max.ru/:current` | текущий открытый чат / scope |
| `https://max.ru/:folder` | папка чатов |
| `https://max.ru/:share-self-out` | специальная цель шеринга «наружу из себя» |
| `https://max.ru/@username` | профиль пользователя |
| `https://max.ru/join/<code>` | присоединиться к чату по короткому коду |
| `https://max.ru/joincall/<code>` | присоединиться к звонку (deeplink звонка) |
| `https://max.ru/c/<chatId>/<msgId>?...` | конкретное сообщение в чате |
| `https://max.ru/stickerset/<name>` | набор стикеров |
| `https://max.ru/<bot>?startapp=...` | запуск мини-приложения с параметром |
| `https://max.ru/<bot>?start=...` | bot-start-param |

## `:auth` + `externalCallback=1`

```java
String queryParameter = uri.getQueryParameter("externalCallback");
if (queryParameter != null && queryParameter.equals("1")) {
    return uri.toString();
}
```

И отдельный UI-класс `ExternalCallbackWidget` (`defpackage/t3.java`, `gi6.java` — кейсы UI-render-а).

То есть существует сценарий:

1. Внешнее приложение (банк, гос-сервис, сайт) генерирует `https://max.ru/:auth?...&externalCallback=1`.
2. Открывается MAX, где пользователь авторизуется/подтверждает.
3. MAX через `ExternalCallbackWidget` показывает intent / возвращает результат вызывающему приложению.

Параллельно — `one/me/settings/devices/hintdialog/QrAuthHintBottomSheet.java` (QR-логин).

То есть архитектурно MAX — **Identity Provider**: внешние приложения через deeplink с `externalCallback=1` могут просить MAX подтвердить личность пользователя, и MAX возвращает результат в callback. В сочетании с `verify_mobile_id` JS-bridge (который выдаёт MSISDN мини-апам внутри MAX) это полноценная SSO-инфраструктура: единый вход через MAX, подтверждение номера через оператора, возможный выход в `digitalid-botid`.

## Кратко

> «MAX публикует deeplink-схему `https://max.ru/:auth?...&externalCallback=1` и отдельный UI-виджет `ExternalCallbackWidget`, делая приложение полноценным Identity Provider для других приложений на телефоне. Вместе с `verify_mobile_id` JS-bridge внутри мини-апок и системным ботом DigitalID это формирует SSO-инфраструктуру: единая точка входа в государственные/коммерческие сервисы через MAX-аккаунт, привязанный к номеру телефона.»
