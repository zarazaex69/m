---
tags: [calls, webrtc, fingerprint, custom-data, network-status]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ConversationImpl.java
related:
  - "[[136-ev1-webrtc-session-config]]"
  - "[[140-conversation-params]]"
---

# ConversationImpl callbacks — fingerprint, custom data, network status

## onCallParticipantFingerprint

`onCallParticipantFingerprint(mv1 participant, long fingerprint)` — callback при изменении DTLS fingerprint участника звонка.

При 1-1 звонке вызывает `conversationEventsListener.onOpponentFingerprintChanged(fingerprint)`. Это означает, что клиент отслеживает DTLS fingerprint собеседника. Изменение fingerprint может означать MITM-атаку.

## onCustomData

`onCustomData(hv1 participant, JSONObject data)` — callback при получении кастомных данных от участника звонка через WebRTC DataChannel.

Вызывает `conversationEventsListener.onCustomData(data)`. Это означает, что через DataChannel можно передавать произвольные JSON-данные между участниками звонка.

## onCallParticipantNetworkStatusChanged

`onCallParticipantNetworkStatusChanged(List<mv1> participants)` — callback при изменении сетевого статуса участников.

Обновляет `callParticipant` для каждого участника и вызывает `onCallParticipantsNetworkStatusChanged`.

## onRateCall

`onRateCall(JSONObject data)` — callback для оценки звонка. Сервер может инициировать показ диалога оценки.

## onMeInWaitingRoomChanged

`onMeInWaitingRoomChanged(boolean inWaitingRoom)` — callback при изменении статуса «в зале ожидания».

## Что важно

1. **`onCustomData`** — произвольные JSON-данные через DataChannel. Это потенциально мощный канал для передачи данных между участниками звонка без WS.

2. **`onCallParticipantFingerprint`** — отслеживание DTLS fingerprint. Клиент может детектировать MITM.

3. **`onRateCall`** — сервер может инициировать показ диалога оценки в любой момент.

## Сводка

`ConversationImpl` callbacks: `onCustomData` (произвольные JSON через DataChannel), `onCallParticipantFingerprint` (DTLS fingerprint tracking), `onRateCall` (server-initiated rating dialog), `onMeInWaitingRoomChanged` (waiting room status).
