---
tags: [pmskey, server-control, rtd, calls, transcription, surveillance, debug]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qp6.java
related:
  - "[[03-pms-server-flags]]"
  - "[[408-pmskey-debug-logging-transcription]]"
  - "[[384-externcalls-sdk-config]]"
---

# qp6 — RTD (Remote Toggles Database) — полный список PmsKey

`qp6` — главный объект RTD. Содержит все PmsKey-флаги.

## Ключевые PmsKey в qp6

| PmsKey | Метод | Что |
|---|---|---|
| `callssdkwtenabled` | q | WebTransport для звонков |
| `callssdkwebrtclogs` | r | **логи WebRTC** |
| `callssdkdisablepipeline` | G | отключить pipeline |
| `callssdklogaudio` | H | **логирование аудио** |
| `callsandroidupdateendpointparams` | I | обновить endpoint params |
| `callssdkdntdisableaudio` | K | отключить аудио |
| `callpermissionsinterval` | V | интервал проверки разрешений |
| `energysavingrequestinterval` | W | интервал энергосбережения |
| `deletemsgfyslargechatdisabled` | d0 | отключить удаление в больших чатах |
| `mediaplaylistenabled` | l0 | медиа-плейлист |
| `informerenabled` | p0 | информер включён |
| `videomsgdownloadurlshackenabled` | q0 | хак URL видеосообщений |
| `enableunknowncontactbottomsheet` | r0 | bottom sheet неизвестного контакта |
| `opcodestatconfig` | v0 | конфиг статистики опкодов |
| `hideincomingcallnotif` | A0 | скрыть уведомление входящего звонка |
| `joinrequests` | F0 | запросы на вступление |
| `setaudiodevice` | I0 | установить аудио-устройство |
| `webappexc` | L0 | исключения WebApp |
| `reactionssettingsenabled` | N0 | настройки реакций |
| `noncontactcomplaintsenabled` | P0 | жалобы на не-контакты |
| `ovmediasendenabled` | Q0 | отправка OV медиа |
| `typingsendenabled` | R0 | отправка typing |
| `cisenabled` | T0 | CIS включён |
| `netsessionrbcenabled` | Y0 | RBC сессии |
| `logviolations` | Z0 | **логирование нарушений** |
| `videofastseekenabled` | b1 | быстрая перемотка видео |
| `scheduledmessagesenabled` | c1 | отложенные сообщения |
| `scheduledpostsenabled` | d1 | отложенные посты |
| `scheduledfavesenabled` | e1 | отложенные избранные |
| `enablefiltersforfolders` | p1 | фильтры для папок |
| `debugbrokencontact` | u1 | **отладка сломанных контактов** |
| `enableaudiomessagestranscription` | D1 | **транскрипция аудио** |
| `enablevideomessagestranscription` | E1 | **транскрипция видео** |
| `notiftypingpresence` | Q1 | typing presence в уведомлениях |
| `watchdogconfig` | U1 | конфиг watchdog |
| `chathistorylogincount` | Z1 | счётчик входов для истории чата |
| `stickerseteditenabled` | b2 | редактирование стикер-сетов |
| `batterysliceinterval` | c2 | **интервал среза батареи** |
| `logmessagesmeta` | d2 | **логирование метаданных сообщений** |
| `logchatmeta` | e2 | **логирование метаданных чатов** |
| `settingsbusiness` | k2 | настройки бизнеса |
| `keepbackgroundsocket` | o2 | держать фоновый сокет |
| `spinlockenabled` | p2 | spin lock |
| `commentsenabled` | w2 | комментарии |
| `pingbackgroundinterval` | E2 | **интервал ping в фоне** |
| `callsfakebossincomingcallenabled` | I2 | **фейковый входящий звонок** |
| `cancelstalenotifications` | J2 | отмена устаревших уведомлений |

## Что важно

1. **`enableaudiomessagestranscription`/`enablevideomessagestranscription`** — транскрипция управляется через RTD.

2. **`callssdklogaudio`/`callssdkwebrtclogs`** — логирование аудио и WebRTC управляется сервером.

3. **`logmessagesmeta`/`logchatmeta`/`logviolations`** — логирование метаданных управляется сервером.

4. **`callsfakebossincomingcallenabled`** — фейковый входящий звонок в production.

5. **`batterysliceinterval`** — интервал сбора данных батареи.

## Сводка

`qp6` (RTD): 50+ PmsKey. Критические: `enableaudiomessagestranscription`/`callssdklogaudio`/`logmessagesmeta`/`logchatmeta`/`callsfakebossincomingcallenabled`/`batterysliceinterval`.
