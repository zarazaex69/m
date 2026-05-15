# DevMenu в production: переключение API-серверов и фича-флагов из самого приложения

## Что есть

В пакете `one.me.devmenu` лежит полноценное dev-меню:

```
DevMenuScreen.java
DevMenuGeneralPageScreen.java
DevMenuFeatureTogglesPageScreen.java   (22 KB)
DevMenuInfoScreen.java
logsviewer/
memorydebugger/
threadsviewer/
tools/                                  (server-host/port bottomsheets)
utils/
```

Это не урезанная заглушка для «debug build» — это полноценный набор инструментов: переключение API-сервера, переключение feature-flags (PMS), просмотр логов, дамп памяти, просмотр тредов. По декомпилированному коду модуль называется `dev-menu_release` — то есть он остаётся в **release-билде**.

## Переключение API-сервера

`defpackage/r58.java`:

```java
public r58(...) {
    this.o = context.getSharedPreferences("dev_tools", 0);
    List listL = r04.L("api.oneme.ru", "api-test.oneme.ru",
                       "api-tg.oneme.ru", "api-test2.oneme.ru");
    ...
}

public final ed9 v() {
    ...
    String string = this.o.getString("Custom", "");
    ...
    ed9VarT.add(new r48(hd2.l("Custom", strO),
                        Boolean.valueOf(zm0.c(string, u().a.L()))));
    return r04.o(ed9VarT);
}
```

Хранилище — `SharedPreferences("dev_tools")`. Список доступных хостов:

1. `api.oneme.ru` (production)
2. `api-test.oneme.ru`
3. `api-tg.oneme.ru` (отдельный «tg»-канал)
4. `api-test2.oneme.ru`
5. `Custom (...)` — произвольный, задаётся пользователем

То есть: **в любой релизной сборке MAX встроен список четырёх серверов плюс «свой адрес», и переключение между ними сохраняется в `SharedPreferences("dev_tools")`**. Это не «отключенный dev-режим», а штатное хранилище.

## Что это значит

- Любой человек, владеющий устройством, может переключить production-приложение на test-сервер. Это удобно для исследования, но и означает, что test-сервера живые и доступны извне.
- Наличие готового UI (`one/me/devmenu/tools/server/ServerHostBottomSheet`, `ServerPortBottomSheet`) делает переключение делом пары кликов в скрытом меню — не нужно патчить APK.
- Fingerprint пользователя у production и test-сервера, очевидно, разный, и это позволяет «вылавливать» тестовые сборки и тестовых пользователей по принципу «что лежит в `dev_tools`».

## Как dev-меню активируется

Точку входа я фиксированно не находил (модуль вызывается контроллером навигации `one.me.sdk.arch.Widget`, открывается через root-controller). По коду — это часть навигации, не отдельный entry point типа BuildConfig.DEBUG. Активация, скорее всего:

- через специальный жест/долгое нажатие в общих настройках (типичная схема таких меню — длинный тап на «версия приложения» в About);
- либо deeplink/intent.

Точка входа дополнительно подвязана на `defpackage/qa5.java`, где `DevMenuScreen` упоминается прямо в коде навигации (один из cases `mo3apply`).

Важен сам факт того, что **полноценное dev-меню остаётся в production AAB**. У серьёзных мессенджеров это обычно strip-ается на release.

## Связанные feature-флаги

Через DevMenu пользователь может играть с теми же PmsKey, что и сервер: `DevMenuFeatureTogglesPageScreen.java` (22 KB) — почти наверняка содержит UI для переключения всех 334 флагов. То есть исследователь может включить себе:

- `log-full`, `log-sensitive` — полные логи с чувствительными данными;
- `fake-chats`, `fake-in-app-review` — посмотреть что именно «фейкового» развёртывается;
- `calls-fakeboss-incoming-call-enabled` — fakeboss звонки;
- `gost-check-env` — посмотреть проверку ГОСТ-окружения;
- `digitalid-botid`, `family-protection-botid` — выставить произвольные ID и посмотреть что произойдёт.

## Кратко для статьи

> «MAX в production-сборке везёт с собой полноценный dev-meню (`one.me.devmenu`, модуль `dev-menu_release`): переключение API-сервера между production / test / test2 / "tg" / Custom, переключение всех 334 фича-флагов, просмотр логов, тредов, память. Хранилище — `SharedPreferences("dev_tools")`. Это означает, что любой пользователь, умеющий вызвать это меню, может в обычном клиенте переключиться на тестовый бэкенд или вручную включить, например, `log-sensitive` (логирование чувствительных данных).»
