---
tags: [proxy, pmskey, login-response, server-control, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/isg.java
related:
  - "[[460-yag-shared-prefs]]"
  - "[[03-pms-server-flags]]"
  - "[[245-login-ws-response]]"
---

# isg — обработка LOGIN ответа (proxy, lang, session)

`isg` — обработчик LOGIN ответа от сервера. Обновляет прокси, язык, сессию.

## Что обновляется при LOGIN

| Поле | Что |
|---|---|
| `PmsKey.proxy` | **список прокси** |
| `PmsKey.proxydomains` | **домены прокси** |
| `yag.e0` (multilang) | мультиязычность |
| `yag.C` (lang) | язык |
| `app-update-type` | тип обновления |
| `SESSION_FORCE_UPDATE` | принудительное обновление сессии |

## Данные в LOGIN запросе

```java
map.put("pushDeviceType", wpeVar.a);
map.put("buildNumber", cgjVarJ.c);
map.put("timezone", cgjVarJ.k.getID());
jsgVar.j(ApiProtocol.PARAM_DEVICE_ID, strA);
```

## Что важно

1. **`PmsKey.proxy`** — список прокси приходит с сервера при LOGIN.

2. **`PmsKey.proxydomains`** — домены прокси приходят с сервера.

3. **`PARAM_DEVICE_ID`** — device ID отправляется в LOGIN запросе.

4. **`pushDeviceType`/`buildNumber`/`timezone`** — данные устройства в LOGIN.

5. **`SESSION_FORCE_UPDATE`** — сервер может принудительно обновить сессию.

## Сводка

`isg`: LOGIN ответ → обновляет proxy/proxydomains/lang/multilang. LOGIN запрос → deviceId/pushDeviceType/buildNumber/timezone.
