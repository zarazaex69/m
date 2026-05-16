---
tags: [calls, server-control, pms, webrtc, peer-id]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# Calls early-set-offer, gen-peerid, update-endpoint-params PmsKey

| Ключ | # | Default | Описание |
|---|---|---|---|
| `calls-android-early-set-offer` | 136 | false | «Разрешить раннюю обработку оффера входящего звонка» |
| `calls-android-gen-peerid` | 137 | false | «Генерировать peer-id на клиенте» |
| `calls-android-update-endpoint-params` | 132 | false | «Параметры webSocket-а заменять, а не дублировать» |
| `calls-android-signaling-ip` | 119 | false | «Подключаться к сигналингу по IP» |

## Что важно

1. **`calls-android-gen-peerid`** — «Генерировать peer-id на клиенте». При включении клиент генерирует peer ID самостоятельно. Это влияет на идентификацию участников в WebRTC-сессии.

2. **`calls-android-early-set-offer`** — «Разрешить раннюю обработку оффера входящего звонка». Оптимизация задержки: клиент начинает обрабатывать SDP offer до полного установления соединения.

3. **`calls-android-update-endpoint-params`** — «Параметры webSocket-а заменять, а не дублировать». Это влияет на то, как обновляются параметры WS-соединения для звонков.

## Сводка

`calls-android-gen-peerid` — клиентская генерация peer ID. `calls-android-early-set-offer` — ранняя обработка SDP offer. `calls-android-update-endpoint-params` — замена vs дублирование WS-параметров.
