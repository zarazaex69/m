---
tags: [webapp, mini-apps, server-control, pms, privilege]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
  - work/jadx_base/sources/defpackage/qp6.java
  - work/jadx_base/sources/defpackage/e2h.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[07-nfc-hce-webapp]]"
  - "[[03-pms-server-flags]]"
---

# WebApp privilege configuration — server-controlled allowlist

В дополнение к [[10-webapp-jsbridge]] (JS-bridge `verify_mobile_id` отдаёт MSISDN мини-апке) — здесь сводный обзор серверно-управляемых параметров привилегий мини-приложений.

## PmsKey семейства `webapp-*`

| Ключ | # | Тип | Что |
|---|---|---|---|
| `webapp-exc` | 214 | long[] | список chat-id мини-апок, которые получают расширенные привилегии («exc» = exclusion / exception list); default — пустой массив |
| `webapp-pr` | 276 | (объект) | конфиг «pr» — вероятно privileged role или private-mode; `qp6.H1` через `bbi(4, ...)` accessor |
| `webapp-push-open` | 268 | bool | разрешение открывать мини-апу прямо из push-уведомления |
| `webapp-phone-hash` | 295 | bool/config | server-pushed режим, при котором мини-апке выдаётся **хеш номера телефона абонента** как стабильный fingerprint (см. [[10-webapp-jsbridge]] §10) |
| `webapp-ds-keys-count` | 159 | int | лимит ключей в DataStore (`StorageGetDataStorageSize` JS-bridge) |
| `webapp-ss-keys-count` | 160 | int | лимит ключей в SessionStorage |

## Что это значит

1. **`webapp-exc` — серверный allowlist привилегированных мини-апок.** Сервер пушит список chat-id, и каждой такой мини-апе клиент даёт расширенные права. Точная семантика из `e2h.java`: «Боты-исключения из правила проверки пользовательского касания перед выполнением методов бриджа» — то есть мини-апки из этого списка могут вызывать JS-bridge методы **без подтверждения пользовательским касанием**. Default `[0]` (пустой список). Тестовый бот: `1496626`, продовый: `4810464`.
2. **`webapp-phone-hash` — стабильный fingerprint пользователя для мини-апок.** Без него мини-апа должна каждый раз просить у пользователя номер. С ним — мини-апа узнаёт, что это «тот же пользователь», даже без знания самого номера. Server-controlled, gates за PmsKey включают эту фичу для определённых типов мини-апок.
3. **`webapp-push-open`** — мини-апа может открыться **прямо из push-уведомления**, минуя обычный flow «открыть чат → запустить мини-апу». В сочетании с push-типом `TamtamSpam` (см. [[19-fcm-push-payload]]) — это означает «server can pop a privileged miniapp UI on the user's screen».
4. **`webapp-ds-keys-count` / `webapp-ss-keys-count`** — лимиты persistent/session storage для каждой мини-апы. Server controls how much each miniapp can persist.

## Архитектурная картина

Мини-апа в MAX — не просто WebView. Через JS-bridge она имеет:

- доступ к MSISDN абонента (через `verify_mobile_id`, использующий cleartext Mobile ID операторов; см. [[01-mobile-id-cleartext]] и [[10-webapp-jsbridge]]);
- доступ к NFC HCE с произвольным APDU (см. [[07-nfc-hce-webapp]]);
- doc-storage и session-storage с server-controlled лимитами;
- возможность открыться из push-нотификации;
- стабильный fingerprint пользователя через хеш номера.

Расширенные привилегии (тонкий список из `webapp-exc`) добавляют дополнительные возможности, не доступные обычным мини-апам.

## Скептический разбор

- Концепция «privileged mini-apps» сама по себе не криминальна — у Telegram Mini Apps есть «in-app payments» только для верифицированных ботов.
- Что делает MAX особенным: **список «привилегированных» мини-апок задаётся одним PmsKey-конфигом с сервера**. Сервер может в любой момент включить произвольной chat-id-мини-апке всё. Без обновления клиента, без user-prompt'а.
- В сочетании с тем, что государственные сервисы (Госуслуги, цифровой ID) встраиваются в MAX как мини-апки (см. вступление в README про закон «О создании многофункционального сервиса обмена информацией»), это означает, что государственные мини-апки фактически работают как часть клиента, без боковых разрешений.

## Сводка

WebApp-стек в MAX — server-managed, server-privileged. Шесть PmsKey управляют, каким мини-апкам что доступно. Whitelist привилегий (`webapp-exc`) — динамический, серверный, без user-visibility. Это полностью соответствует архитектуре «национального supersapp», где mini-apps — главный механизм расширения функциональности.
