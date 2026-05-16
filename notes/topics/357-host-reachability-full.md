---
tags: [network, host-reachability, gosuslugi, google, huawei, calls, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/j58.java
  - work/jadx_base/sources/defpackage/f58.java
related:
  - "[[24-host-reachability-checker]]"
  - "[[356-api-oneme-ru-servers]]"
---

# HostReachabilityChecker — полный список проверяемых хостов

`j58.java` — список хостов для проверки доступности. Хосты закодированы как int-массивы.

## Хосты

| Переменная | Хост | Что |
|---|---|---|
| `a` | `gstatic.com` | **Google Static** |
| `b` | (из PmsKey) | дополнительный хост |
| `c` | `mtalk.googl.com` | **Google Talk (FCM)** |
| `d` | (из PmsKey) | дополнительный хост |
| `e` | `calls.okcdn.ru` | **OK CDN для звонков** |
| `f` | (из PmsKey) | дополнительный хост |
| `g` | `gosuslugi.ru` | **Госуслуги** |
| `h` | (из PmsKey) | дополнительный хост |
| `i` | `pushtrs.push.hicloud.com` | **Huawei Push** |

## Полный список (f58.java)

```java
List.of("api.oneme.ru", j58.b, j58.f, j58.h, j58.d)
```

## Что важно

1. **`gosuslugi.ru`** — проверяется доступность Госуслуг. Это связано с DigitalID интеграцией.

2. **`calls.okcdn.ru`** — CDN OK.ru для звонков. Медиа-трафик звонков идёт через OK CDN.

3. **`pushtrs.push.hicloud.com`** — Huawei Push. MAX поддерживает Huawei Push Services.

4. **`mtalk.googl.com`** — Google FCM. Проверка доступности FCM.

5. Хосты закодированы как int-массивы — обфускация.

## Сводка

HostReachability хосты: `gstatic.com`/`mtalk.googl.com`/`calls.okcdn.ru`/`gosuslugi.ru`/`pushtrs.push.hicloud.com` + динамические из PmsKey.
