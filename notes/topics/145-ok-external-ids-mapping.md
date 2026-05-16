---
tags: [calls, api, identity, external-ids, ok-ids]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/GetOkIdsByExternalIds.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/GetExternalIdsByOkIds.java
related:
  - "[[96-vchat-api-methods]]"
  - "[[29-external-callback-idp-flow]]"
---

# vchat.getOkIdsByExternalIds и vchat.getExternalIdsByOkIds — маппинг ID

Два API метода для маппинга между OK ID (внутренние ID пользователей VK/OK) и внешними ID.

## vchat.getOkIdsByExternalIds

Параметр: `externalIds` — список внешних ID.
Возвращает: маппинг external_id → ok_id.

## vchat.getExternalIdsByOkIds

Параметр: `uids` — список OK ID.
Возвращает: маппинг ok_id → external_id.

## Что важно

1. **Маппинг ID** — это механизм для связывания пользователей MAX с внешними системами. Внешние ID могут быть ID в других сервисах VK/OK.

2. **Двусторонний маппинг** — можно получить OK ID по внешнему ID и наоборот. Это означает, что сервер может связать пользователя MAX с его аккаунтами в других сервисах VK/OK.

3. В контексте звонков — это используется для идентификации участников звонка, которые могут быть из разных сервисов (MAX, OK, VK).

## Сводка

`vchat.getOkIdsByExternalIds` и `vchat.getExternalIdsByOkIds` — двусторонний маппинг между OK ID и внешними ID. Позволяет связывать пользователей MAX с другими сервисами VK/OK.
