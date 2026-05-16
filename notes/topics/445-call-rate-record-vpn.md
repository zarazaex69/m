---
tags: [calls, rate-call, record, vpn-panel, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/calls/ui/bottomsheet/ratecall/CallRateBottomSheet.java
  - work/jadx_base/sources/one/me/calls/ui/bottomsheet/record/StartRecordBottomSheet.java
  - work/jadx_base/sources/one/me/calls/ui/ui/call/panels/VpnPanelWidget.java
related:
  - "[[377-record-manager]]"
  - "[[393-rate-manager]]"
  - "[[02-vpn-warning]]"
---

# CallRateBottomSheet + StartRecordBottomSheet + VpnPanelWidget

## CallRateBottomSheet — оценка звонка

Параметры: `call_id`, `is_group_call`, `is_video_call`, `sdk_reasons` (список причин).

Содержит: звёздный рейтинг + вопросы (из `RateCallData`) + кнопка отправки.

## StartRecordBottomSheet — начало записи

Поле ввода имени записи (max 250 символов). Переключатель (toggle). Кнопка запуска.

## VpnPanelWidget — панель VPN в звонке

Панель предупреждения о VPN внутри звонка.

## Что важно

1. **`sdk_reasons`** — список причин плохого качества из SDK передаётся в UI для оценки.

2. **`is_group_call`/`is_video_call`** — тип звонка передаётся в оценку.

3. **`StartRecordBottomSheet`** — имя записи вводится пользователем (max 250 символов).

4. **`VpnPanelWidget`** — VPN-предупреждение показывается прямо в интерфейсе звонка.

## Сводка

`CallRateBottomSheet(call_id, is_group_call, is_video_call, sdk_reasons)`. `StartRecordBottomSheet`: имя записи (max 250 символов). `VpnPanelWidget`: VPN-предупреждение в звонке.
