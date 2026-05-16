---
tags: [calls, rating, server-control, pms, analytics]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/calls/ui/bottomsheet/ratecall/CallRateBottomSheet.java
  - work/jadx_base/sources/defpackage/xh1.java
  - work/jadx_base/sources/defpackage/nuf.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[71-call-onelog-events]]"
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[69-remote-settings-calls-sdk]]"
---

# Call Rate — оценка качества звонков

После завершения звонка MAX показывает `CallRateBottomSheet` — диалог оценки качества.

## Данные

`CallRateBottomSheet` принимает:
- `callId` — ID звонка
- `isGroupCall` — групповой ли
- `isVideoCall` — видеозвонок ли
- `sdkReasons` — список причин плохого качества (от SDK)

Deeplink: `:call-rate?call_id=<X>&is_group=<bool>&is_video=<bool>`

## PmsKey

`call-rate` (#?) — `callRateParams` в `rtd.java` — JSON-конфиг параметров оценки звонка. Сервер задаёт, при каких условиях показывать диалог оценки.

`android.rating.limits` в `RemoteSettings` (см. [[69-remote-settings-calls-sdk]]) — пороги качества для показа диалога.

## Что важно

1. **`sdkReasons`** — список причин плохого качества от WebRTC SDK. Это технические причины (потеря пакетов, задержка, и т.п.), которые SDK автоматически определяет. Сервер получает не только субъективную оценку пользователя, но и технические причины.

2. **`call-rate` deeplink** — диалог оценки может быть открыт через deeplink с `call_id`. Это означает, что внешнее приложение теоретически может открыть диалог оценки для конкретного звонка.

3. **Сервер контролирует условия показа** через `callRateParams` и `android.rating.limits`. Сервер может настроить, чтобы диалог показывался только при определённом качестве звонка.

## Сводка

`CallRateBottomSheet` — диалог оценки звонка с `callId`, `isGroupCall`, `isVideoCall`, `sdkReasons`. Сервер получает оценку + технические причины плохого качества. Условия показа server-controlled через `call-rate` PmsKey и `android.rating.limits`.
