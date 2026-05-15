---
tags: [killswitch, server-control, force-update, sideload, oddity]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mec.java
  - work/jadx_base/sources/defpackage/pb8.java
  - work/apktool_base/AndroidManifest.xml
related:
  - "[[03-pms-server-flags]]"
  - "[[04-telemetry-endpoints]]"
---

# Force update: серверный killswitch и обновление APK мимо Google Play

## Триггер с сервера

`defpackage/mec.java`:

```java
public final boolean a() {
    if (((int) ((rtd) this.c).m(hbg.a, 0)) == 1) {                    // серверный флаг
        ri9 ri9Var = (ri9) this.b;
        String str = (String) ri9Var.G0.x(ri9Var, ri9.f1[20]);        // серверная строка-версия
        this.d.getClass();
        if (zm0.c(str, "26.15.3")) {                                  // сравнивается с текущей версией клиента
            return true;
        }
    }
    return false;
}
```

То есть проверка проста:

1. Сервер может выставить **`hbg.a == 1`** в настройках и **прислать список «устаревших» версий** (точная сравниваемая строка приходит с сервера, в декомпиляции явно сравнивается с `"26.15.3"` — это просто текущая версия APK, которую я анализирую).
2. Если совпало — клиент входит в force-update.

## Что происходит дальше

`defpackage/lec.java`:

```java
RootController rootControllerL = nuf.l(mainActivity);
if (!(rootControllerL.C() instanceof ForceUpdateScreen)) {
    d48 d48Var = rootControllerL.k1().e().isEmpty() ? null : new d48(0);
    rootControllerL.k1().T(z5l.I(new ForceUpdateScreen(), d48Var, d48Var));
}
```

Заменяет содержимое всего root-controller-а на `ForceUpdateScreen`. То есть пользователь, открывая MAX, увидит **только** этот экран:

- `«Ваша версия MAX устарела»`
- `«Писать и звонить в этой версии не получится. Обновите приложение, чтобы продолжить использовать MAX»`
- кнопка «Обновить» → ведёт на `https://download.max.ru/#android?version=26.15.3` (тут зашита текущая версия, в реальности подставится номер, актуальный на момент)

## Killswitch на функциональность

Это не просто плашка — это полноценный killswitch. В `defpackage/pb8.java` (`NotifListenerImpl.onNotifCallStart`):

```java
if (((mec) yzbVar12.c.getValue()).a()) {
    ct4.W0("NotifListenerImpl", "Early return in onNotifCallStart cuz of forceUpdateLogic.isNeedForceUpdate()", new Object[0]);
    return;
}
```

То есть **если сервер сказал «обнови» — входящие звонки не приходят** (notification не показывается). По тексту строки `force_update_subtitle` («Писать и звонить в этой версии не получится») — то же самое для отправки сообщений.

Сравнить с фразой в `force_update_subtitle`: **«Писать и звонить в этой версии не получится»** — то есть в обновляемой/устаревшей версии действительно ничего не работает.

## URL обновления — вне Google Play

Кнопка «Обновить» в ForceUpdateScreen ведёт на `https://download.max.ru/#android?version=26.15.3` (`defpackage/yt.java`):

```java
case 0:
    ((dfc) v49Var.getValue()).getClass();
    return "https://download.max.ru/#android?version=26.15.3";
```

То есть пользователь скачивает APK **напрямую с серверов MAX**, минуя Google Play. С учётом, что:

- в манифесте есть **неподставленный placeholder `${REQUEST_INSTALL_PACKAGES}`** — это значит, что в Gradle-конфиге есть переменная, которая разрешала бы клиенту устанавливать другие APK (то есть и обновления самого себя из приложения);
- `download.max.ru` — это собственный CDN MAX, не Play Store.

Получается архитектура «обновлений мимо Play», устойчивая к ситуации:

- Google Play блокирует приложение в стране (или блокирует обновления).
- Google Play вообще недоступен (например, в санкциях, после удаления магазинов).
- Власть требует, чтобы пользователи получали обязательные обновления немедленно.

Сервер MAX в любой момент может выставить флаг и заставить всю аудиторию скачать APK с собственного CDN.

## Кратко

> «У MAX встроен серверный killswitch обновления версии: один серверный флаг — и в любой клиентской версии перестают работать сообщения и звонки, на единственный экран попадает плашка "Ваша версия устарела" с кнопкой, ведущей на `https://download.max.ru/#android?version=...` — то есть на собственный CDN MAX, а не в Google Play. Эта инфраструктура полностью независима от Play Store: при необходимости массовая доставка обновлений (или прекращение работы старых версий) осуществляется самим MAX. Дополнительно в манифесте сохранилась неподставленная Gradle-переменная `${REQUEST_INSTALL_PACKAGES}`, что говорит о наличии в Gradle-конфигурации заготовки под полноценный side-loading APK прямо из приложения.»
