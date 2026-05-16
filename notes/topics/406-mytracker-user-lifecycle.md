---
tags: [mytracker, user-lifecycle, install-referrer, login-tracking, registration-tracking, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/userlifecycle/MyTrackerUserLifecycle.java
  - work/jadx_base/sources/com/my/tracker/campaign/MultipleInstallReceiver.java
related:
  - "[[385-mytracker-details]]"
  - "[[405-install-referrer]]"
---

# MyTrackerUserLifecycle + MultipleInstallReceiver

## MyTrackerUserLifecycle — события жизненного цикла пользователя

| Метод | Что |
|---|---|
| `trackLoginEvent(userId, type, params)` | **событие входа** |
| `trackRegistrationEvent(userId, type, params)` | **событие регистрации** |
| `trackInviteEvent(params)` | событие приглашения |

## MultipleInstallReceiver — перехват Install Referrer

`BroadcastReceiver` для `com.android.vending.INSTALL_REFERRER`.

Логика: при получении broadcast — вызывает `CampaignReceiver.a(context, intent)`, затем перебирает все зарегистрированные receivers для `INSTALL_REFERRER` и вызывает их.

## Что важно

1. **`trackLoginEvent(userId, type)`** — каждый вход в систему отслеживается MyTracker с userId и типом входа.

2. **`trackRegistrationEvent(userId, type)`** — каждая регистрация отслеживается.

3. **`MultipleInstallReceiver`** — перехватывает `INSTALL_REFERRER` broadcast и передаёт всем зарегистрированным receivers. Это означает, что MyTracker получает данные об установке даже если другие receivers зарегистрированы.

## Сводка

`MyTrackerUserLifecycle`: `trackLoginEvent(userId, type)` / `trackRegistrationEvent(userId, type)` / `trackInviteEvent()`. `MultipleInstallReceiver`: перехват `INSTALL_REFERRER` → `CampaignReceiver`.
