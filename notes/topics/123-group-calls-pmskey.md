---
tags: [calls, group-calls, server-control, pms]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[03-pms-server-flags]]"
---

# Group calls PmsKey — server-controlled групповые звонки

| Ключ | # | Default | Описание |
|---|---|---|---|
| `gc-from-p2p` | 123 | false | «Переход из 1-1 звонка в групповой» |
| `gc-link-pre-settings` | 124 | false | «Преднастройки группового звонка по ссылке» |
| `gc-wait-admin` | 125 | false | «Комната ожидания администратора в групповых звонках» |
| `group-call-part-limit` | 128 | — | лимит участников группового звонка |
| `call-pinch-to-zoom` | 24 | false | «Pinch-to-zoom в звонках» |

## Что важно

1. **`gc-from-p2p`** — server-gated переход из 1-1 звонка в групповой. При включении пользователь может добавить участников в текущий звонок.

2. **`gc-link-pre-settings`** — «Преднастройки группового звонка по ссылке». При включении — при переходе по ссылке на звонок применяются преднастройки (камера/микрофон).

3. **`gc-wait-admin`** — «Комната ожидания администратора». При включении — участники ждут в «зале ожидания» до прихода администратора.

4. **`group-call-part-limit`** — сервер контролирует максимальное количество участников группового звонка.

## Сводка

5 PmsKey для групповых звонков. Ключевые: `gc-from-p2p` (переход 1-1→группа), `gc-wait-admin` (зал ожидания), `group-call-part-limit` (лимит участников).
