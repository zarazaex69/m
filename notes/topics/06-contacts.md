# Контакты: ContentObserver на адресной книге, обратная запись через SyncAdapter, не-контакты

## Чтение

`defpackage/a40.java`:

```java
public final void c() {
    if (!((aed) ((fec) this.f.getValue()).a.getValue()).d(aed.g)) {
        ct4.E(this.d, "subscribeOnSystemChanges: no permissions, return");
    } else if (this.i == null) {
        u30 u30Var = new u30(this, new Handler(Looper.getMainLooper()));
        this.a.getContentResolver()
              .registerContentObserver(ContactsContract.Contacts.CONTENT_URI, true, u30Var);
        this.i = u30Var;
    }
}
```

ContentObserver регистрируется на корне `ContactsContract.Contacts.CONTENT_URI` с `notifyForDescendants=true`. Любое изменение в любой записи адресной книги телефона дёргает callback в реальном времени.

## Запись

`defpackage/ndi.java`:

```java
public final Uri c() {
    return ContactsContract.RawContacts.CONTENT_URI.buildUpon()
        .appendQueryParameter("caller_is_syncadapter", "true")
        .appendQueryParameter("account_name", this.a)
        .appendQueryParameter("account_type", this.b)
        .build();
}

public final void d(...) {
    arrayList.add(builderWithValue
        .withValue("account_type", str4)
        .withValue("sync1", Long.valueOf(di4Var.r())).build());        // sync1 = MAX user-id
    arrayList.add(ContentProviderOperation.newInsert(b(ContactsContract.Settings.CONTENT_URI))
        .withValue("account_name", this.a)
        .withValue("account_type", str4)
        .withValue("ungrouped_visible", 1).build());
    Uri uri = ContactsContract.Data.CONTENT_URI;
    arrayList.add(ContentProviderOperation.newInsert(b(uri))
        .withValueBackReference("raw_contact_id", 0)
        .withValue("mimetype", "vnd.android.cursor.item/name")
        .withValue("data2", str).withValue("data3", str2).build());     // имя
    arrayList.add(ContentProviderOperation.newInsert(b(uri))
        .withValueBackReference("raw_contact_id", 0)
        .withValue("mimetype", "vnd.android.cursor.item/phone_v2")
        .withValue("data1", str3).withValue("data2", 2).build());       // телефон
    arrayList.add(ContentProviderOperation.newInsert(b(uri))
        .withValueBackReference("raw_contact_id", 0)
        .withValue("mimetype", context.getString(j5f.tt_contact_mimetype))   // tt = TamTam
        .withValue("data1", Long.valueOf(di4Var.r()))
        .withValue("data2", Long.valueOf(di4Var.s()))
        .withValue("data3", di4Var.e()).build());
    contentResolver.applyBatch("com.android.contacts", arrayList);
}
```

MAX:

1. Заводит собственный аккаунт в ContactsContract (account_type — TamTam-овская строка `tt_contact_mimetype`).
2. Через SyncAdapter пишет в адресную книгу телефона найденных в MAX людей: имя, телефон, плюс собственный MIME-type «карточки MAX-пользователя» (поля `sync1=tamtam_user_id`, `data1=server_id`, `data2`, `data3=email/extra`).
3. Использует флаг `caller_is_syncadapter=true`, который обходит часть стандартных проверок Android и помечает запись как системно-синхронизированную.

С точки зрения пользователя: «когда мне звонит человек, у которого я нашёл MAX, у меня в звонилке появляется его имя» — фича. С точки зрения адресной книги: записи, которые пользователь не вносил вручную, появляются от имени MAX.

## Не-контакты («non-contacts»)

В PmsKey есть отдельная серверная подсистема:

- `non-contact-sync-time` — `nonContactSyncTimeInSec`
- `non-contact-max-chunk-size`
- `non-contact-collection-interval` — интервал «сбора» не-контактов

Записи адресной книги, которые не являются пользователями MAX, тоже синхронизируются с сервером (хеш номера, отдельный батчинг). Это известная схема всех мессенджеров с автодобавлением контактов; у MAX это управляется **серверно** — частоту, размер чанка и сам факт сбора задаёт сервер.

Дополнительно: PmsKey-флаги `non-contact-complaints-enabled`, `enable-unknown-contact-bottom-sheet`, `contact-add-bottom-sheet`, `not-contact-placeholder` — UX-обвязка про «неизвестных контактов» (повышение конверсии в добавление контакта, жалобы на контакт).

## Кратко

> «MAX подписывается ContentObserver-ом на корень `ContactsContract.Contacts.CONTENT_URI` с `notifyForDescendants=true` и реагирует на любое изменение адресной книги. Через свой SyncAdapter он же пишет в адресную книгу телефона записи о найденных пользователях MAX (с собственным MIME-type, сохраняющим связь с MAX-id). Сервер MAX управляет «сбором не-контактов» (записей, которые не являются пользователями MAX) тремя серверными параметрами — `non-contact-sync-time`, `non-contact-max-chunk-size`, `non-contact-collection-interval`.»
