# «Работа в фоне»: foreground service, wake на boot, упаковка как «обход перебоев интернета»

## UX-обёртка

В русских строках:

```
oneme_main_background_wake_suggestion        = «Оставайтесь на связи при перебоях интернета»
oneme_main_background_wake_enable_button     = «Включить»
oneme_main_background_wake_enabled           = «Вы включили работу в фоне — это поможет оставаться на связи»
oneme_notifications_settings_background_wake_section_title = «Настройки при ограничениях сети»
oneme_notifications_settings_background_wake_description    = «Оставайтесь на связи при перебоях интернета»
oneme_notifications_settings_background_wake_title          = «Работа в фоне»

oneme_background_wake_notification_title    = «MAX работает в фоне»
oneme_background_wake_notification_subtitle = «Вы будете получать уведомления»
```

То есть пользователю это представлено как «функция, которая помогает оставаться на связи при **перебоях интернета** и **ограничениях сети**». В контексте РФ 2026-го это прямой адрес блокировок и DPI: «у вас интернет ломается — нажмите Включить, и MAX будет работать дольше».

## Что включается на самом деле

В манифесте:

```xml
<receiver android:enabled="false" android:exported="false"
          android:name="one.me.background.wake.BackgroundCheckReceiver"/>
<receiver android:enabled="false" android:exported="true"
          android:name="one.me.background.wake.BackgroundWakeBootReceiver">
    <intent-filter>
        <action android:name="android.intent.action.MY_PACKAGE_REPLACED"/>
        <action android:name="android.intent.action.BOOT_COMPLETED"/>
    </intent-filter>
</receiver>
<service android:exported="false"
         android:foregroundServiceType="dataSync"
         android:name="one.me.background.wake.BackgroundListenService"/>
```

(плюс `RECEIVE_BOOT_COMPLETED` permission и `WAKE_LOCK`, `REQUEST_IGNORE_BATTERY_OPTIMIZATIONS`)

`one/me/background/wake/BackgroundListenService.java`:

```java
public final class BackgroundListenService extends Service {
    public final Notification a() {
        ...
        soc socVarC = stVar.c();
        PendingIntent pendingIntentX = ij9.x(this, 9001, socVarC.h(false));
        w1c w1cVarJ = socVarC.j("ru.oneme.app.misc", true, true);
        w1cVarJ.e = w1c.c(getString(t2f.oneme_background_wake_notification_title));
        w1cVarJ.f = w1c.c(getString(t2f.oneme_background_wake_notification_subtitle));
        w1cVarJ.f(2, true);                  // FLAG_ONGOING_EVENT
        w1cVarJ.k = -1;
        w1cVarJ.G = true;                    // foreground notification
        w1cVarJ.g = pendingIntentX;
        return w1cVarJ.a();
    }

    @Override public final void onCreate() {
        ct4.E("KeepBackground", "onCreate");
        try {
            startForeground(9001, a());
            ((u6e) b().getAccessor().c(42)).c(64L);
            ok9.h(((sn0) b().getAccessor().c(147)).a(),
                  "BACKGROUND_MODE", "system_curtain_shown", null, 12);
        } catch (Throwable th) { stopSelf(); }
    }
    ...
}
```

То есть `BackgroundListenService`:

- стартует foreground-сервис c notification «MAX работает в фоне / Вы будете получать уведомления», `notificationId=9001`;
- channel — `ru.oneme.app.misc`, ongoing, не убираемый смахиванием;
- по факту запуска отправляется аналитический эвент `BACKGROUND_MODE / system_curtain_shown`. То есть факт, что у юзера развернулся такой сервис, репортится в OneLog/трейсер.

Внутренний тег для логов: `KeepBackground`.

`BackgroundWakeBootReceiver` ловит `BOOT_COMPLETED` и `MY_PACKAGE_REPLACED`:

```java
public final void onReceive(Context context, Intent intent) {
    ...
    try {
        q7 q7Var = q7.a;
        pn0 pn0Var = (pn0) new dn0(q7.b(lg9.b)).getAccessor().c(149);
        if (pn0Var.d()) {
            ct4.E("KeepBackground", "App updated, rescheduling background wake alarm");
            pn0Var.e();                  // re-arm AlarmManager
        }
    } catch (Throwable th) { ... }
}
```

Это значит: после **перезагрузки телефона** или **установки апдейта** MAX перерасставляет alarm-ы и заново будит фоновую работу. Чтобы продолжать держать сокет/слушать пуши даже когда система чистит app-state.

## Что это значит

- «Сидеть в фоне 24/7 и обходить ограничения сети» — позиционируется как фича на фоне блокировок.
- Технически — это foreground-service `dataSync` + boot-receiver + battery-optimization-bypass + WAKE_LOCK. Стандартный набор, какой мы видим у Telegram/WhatsApp с push-fallback. Разница — **подача**: MAX называет это прямо «при перебоях интернета» / «при ограничениях сети», т.е. именно как контрмеру блокировок и DPI.
- Параллельно через PMS-флаги `keep-background-socket`, `ping-background-interval` сервер регулирует, как часто пинговать и держать ли фоновой сокет вообще.
- Факт включения «фонового режима» репортится в аналитику отдельным событием.

## Кратко для статьи

> «У MAX внутри пакет `one.me.background.wake`: foreground-сервис типа `dataSync` (`BackgroundListenService`), boot-receiver на `BOOT_COMPLETED`+`MY_PACKAGE_REPLACED` (`BackgroundWakeBootReceiver`) и контроллер `BackgroundCheckReceiver`. Включение этого режима пользователю преподносится как «работа в фоне при перебоях интернета и ограничениях сети» — буквально язык про блокировки и DPI. Internal tag — `KeepBackground`. Включение и работа сервиса регулируются серверными флагами `keep-background-socket`, `ping-background-interval`, и сам факт включения летит в аналитику.»
