---
tags: [ab-testing, server-control, pms, calls, network]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ubi.java
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/defpackage/ak1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[41-server-side-client-diagnostic-struct]]"
---

# ab-status и cfs — A/B тестирование и клиентский бэкенд звонков

## ab-status — A/B тест статус

`ab-status` — PmsKey типа long. Значение отправляется на сервер в составе WS-сессионного конфига (`ubi.java:70` — `u6d("ab-status", String.valueOf(...))`).

По имени — это идентификатор A/B-тест группы, в которую попал пользователь. Сервер присваивает пользователю `ab-status` через PmsKey, и клиент отправляет его обратно при каждом подключении. Это стандартный механизм A/B-тестирования: сервер знает, в какой группе пользователь, и может анализировать поведение по группам.

Что важно: `ab-status` — это **идентификатор сегментации пользователя**. Сервер может использовать его для включения разных фич разным группам пользователей (не только для A/B-тестов, но и для постепенного rollout или для таргетированного включения фич конкретным пользователям).

## cfs — «Быстрый старт через клиентский бэкенд»

`cfs` — PmsKey с описанием из `kd1.java`: «Быстрый старт через клиентский бэкенд». Default `true`.

В `ak1.java:213` используется для инициализации `P2pStartConversationDelegate` в `conversationFactory` (звонковый стек). При `cfs=true` — используется клиентский бэкенд для быстрого старта P2P-соединения. При `cfs=false` — делегат не устанавливается (fallback на стандартный путь).

«Клиентский бэкенд» (client-side backend) в контексте звонков — это вероятно локальный signaling proxy или pre-connection механизм, который ускоряет установку P2P-соединения. Сервер может отключить это через `cfs=false`, заставив клиент использовать стандартный серверный signaling.

## Сводка

`ab-status` — long-идентификатор A/B-группы пользователя, отправляемый на сервер при каждом подключении. `cfs` (default=true) — «быстрый старт через клиентский бэкенд» для P2P-звонков; при `false` — fallback на серверный signaling.
