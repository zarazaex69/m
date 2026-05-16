---
tags: [calls, server-control, pms, network, redirect]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/rtd.java
  - work/apktool_base/smali/rtd.smali
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[03-pms-server-flags]]"
  - "[[43-libjingle-webrtc-custom-build]]"
---

# calls-endpoint — server-pushed URL звонкового сервера

`PmsKey.f47callsendpoint` — server-pushed строка с URL(ами) звонкового сервера. Accessor: `rtd.m = new kbg(this, PmsKey.f47callsendpoint, 2)`. В `rtd.callServers` — строка с адресами серверов для звонков.

## Что это значит

Сервер через PmsKey `calls-endpoint` задаёт, **к каким серверам подключаться для звонков**. Это означает:

1. **Сервер может перенаправить звонки на произвольный endpoint** без обновления клиента. Например, на серверы в другой юрисдикции, или на серверы с другими характеристиками (запись, мониторинг).

2. В сочетании с `calls-android-signaling-ip` (#119) (см. [[45-calls-sdk-pmskey-cluster]]) — два независимых механизма перенаправления звонкового трафика: один для сигналинга (IP), другой для медиа-сервера (endpoint URL).

3. Это стандартная практика для масштабируемых звонковых систем (балансировка нагрузки, geo-routing). Но в контексте государственного мессенджера — это также механизм для направления звонков через серверы с определёнными характеристиками.

## Связанные PmsKey

| Ключ | Что |
|---|---|
| `calls-endpoint` | URL звонкового сервера |
| `calls-android-signaling-ip` (#119) | IP сигналинг-сервера |
| `calls-android-signaling-to` (#124) | timeout сигналинга |
| `outgoing-call-uri` | URI для исходящих звонков |
| `gc-link-pre-settings` | pre-settings для групповых звонков |

## Сводка

`calls-endpoint` — server-pushed URL звонкового сервера. Сервер может в любой момент перенаправить все звонки на другой endpoint без обновления клиента. В сочетании с `calls-android-signaling-ip` — полный server-control над маршрутизацией звонкового трафика.
