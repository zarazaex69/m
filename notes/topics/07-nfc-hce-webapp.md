# NFC HCE для мини-приложений: телефон отвечает APDU тем, что положит мини-апа

## Что в манифесте

```xml
<service android:exported="true"
         android:name="one.me.webapp.util.WebAppNfcService"
         android:permission="android.permission.BIND_NFC_SERVICE">
    <intent-filter>
        <action android:name="android.nfc.cardemulation.action.HOST_APDU_SERVICE"/>
    </intent-filter>
    <meta-data android:name="android.nfc.cardemulation.host_apdu_service"
               android:resource="@xml/apduservice"/>
</service>
```

`res/xml/apduservice.xml`:

```xml
<host-apdu-service xmlns:android="http://schemas.android.com/apk/res/android">
    <aid-group android:category="other">
        <aid-filter android:name="F0010203040303" />
    </aid-group>
</host-apdu-service>
```

AID `F0010203040303` — proprietary AID (диапазон `F0..` это «proprietary, non-EMV»). Он не банковский. Категория `other`, не `payment`.

То есть MAX **регистрирует себя как Host Card Emulation сервис в NFC-стеке Android**: при поднесении телефона к терминалу, который запрашивает SELECT AID `F0010203040303`, ОС маршрутизирует APDU-команды в `WebAppNfcService` MAX-а.

## Что делает сервис

`one/me/webapp/util/WebAppNfcService.java`:

```java
public final class WebAppNfcService extends HostApduService {
    public final v49 b = ((uak) new jai(new ubk(10)).getValue()).a();   // jxb store

    public final byte[] a() {
        jxb jxbVar = (jxb) this.b.getValue();
        jxbVar.e.h(lxb.a);
        return new byte[]{111, 0};       // 0x6F 0x00 — generic error
    }

    @Override public final byte[] processCommandApdu(byte[] bArr, Bundle bundle) {
        ...
        if (bArr.length < 4) return a();
        byte b = bArr[1];
        if (b != -92) {                   // 0xA4 = SELECT
            return a();
        }
        try {
            byte[] bArr2 = (byte[]) ((jxb) this.b.getValue()).d.get();
            if (bArr2 != null) {
                ((jxb) this.b.getValue()).e.h(lxb.b);
                return bArr2;             // <- ответ APDU = что положил WebApp
            }
            return a();
        } catch (Exception e) { ... return a(); }
    }
}
```

Логика проста и важна:

1. На любую команду кроме SELECT (INS=0xA4) возвращается ошибка `6F 00`.
2. На SELECT — берётся **байтовый блоб**, заранее положенный в `jxb.d.get()`, и **возвращается терминалу как APDU-ответ**.
3. Источник этого блоба — **код мини-приложения внутри MAX**, через JS-bridge, который пишет в `jxb`.

То есть мини-апа внутри MAX может в рантайме «зарядить» NFC HCE сервис своим payload и при касании к терминалу пользователь отдаст этот payload по NFC.

## Что это в реальности

Это полноценный программный канал:

- Мини-апа лояльности может эмулировать карту магазина: «приложите телефон к терминалу — спишут баллы».
- Мини-апа транспорта может эмулировать пропуск.
- Мини-апа верификации может «передать» через NFC blob, скажем, цифровой ID — терминал запросит, MAX-приложение ответит тем, что мини-апа положила в `jxb`.

Что фактически уезжает по NFC, **полностью контролируется текущей открытой мини-апой**. Это не банковский HCE (для оплаты нужен другой AID и `category=payment`), но любой другой сценарий тут возможен.

Никакого UX-предупреждения «мини-апа сейчас активирует NFC-эмуляцию» в коде сервиса не видно. Активация — через `lxb.a / lxb.b` (`jxbVar.e.h(...)`), это event-flow, на который где-то ещё подписан UI; событие, дискретное по факту чтения APDU от терминала.

## Кратко

> «MAX публикует в манифесте `WebAppNfcService` — отдельный Host APDU Service для NFC HCE с proprietary AID `F0010203040303`. На SELECT-команду от внешнего NFC-терминала сервис возвращает байтовый блоб, заранее положенный текущей мини-апой через JS-bridge. То есть произвольная мини-апа внутри MAX может эмулировать произвольную (не-EMV) NFC-карту/чип силами телефона пользователя. AID не банковский, но любые другие сценарии — пропуск, лояльность, идентификация — на этой архитектуре делаются без отдельного приложения.»
