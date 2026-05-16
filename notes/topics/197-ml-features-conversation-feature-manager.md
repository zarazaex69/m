---
tags: [calls, ml, kws, ns, features, server-control, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/MLFeaturesManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/MLFeaturesManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feature/ConversationFeatureManager.java
related:
  - "[[70-keyword-spotter-in-calls]]"
  - "[[181-noise-suppression-manager]]"
  - "[[152-asr-online-manager]]"
  - "[[155-call-roles]]"
---

# MLFeaturesManager + ConversationFeatureManager — ML и серверный контроль фич

## MLFeaturesManager

Интерфейс минимальный: `start()` / `dispose()`.

Реализация (`MLFeaturesManagerImpl`) — оркестратор ML-фич в звонке:

| Делегат | Что |
|---|---|
| `KwsFeatureDelegate` | Keyword Spotter (KWS) |
| `NSFeatureDelegate` | Noise Suppression (NS) |

Зависимости конструктора: `KeywordSpotterManager`, `NoiseSuppressionManager`, `RemoteSettings`, `ConversationStats`, `DownloadService`.

`MLFeaturesManagerImpl` управляет загрузкой ML-моделей (`DownloadService`) и их активацией через `RemoteSettings`. `MLModelCheckResult` — результат проверки модели.

## ConversationFeatureManager

Серверный контроль фич в звонке:

| Метод | Что |
|---|---|
| `isFeatureEnabled(feature)` | включена ли фича |
| `getFeatureRoles(feature)` | роли для фичи |
| `enableFeatureForAll(feature, onComplete, onError)` | **включить для всех** |
| `enableFeatureForRoles(feature, roles, onComplete, onError)` | **включить для ролей** |
| `addFeatureListener(feature, listener)` | слушатель изменений |

`FeatureListener`:
- `onFeatureEnabledChanged(feature, isEnabled)` — фича включена/выключена
- `onFeatureRolesChanged(feature, roles)` — роли для фичи изменились

Фичи (`ConversationFeature`): `ADD_PARTICIPANT`, `RECORD`, `MOVIE_SHARE`, `ASR_RECORD` (см. [[152-asr-online-manager]]).

## Что важно

1. **`enableFeatureForAll`** — администратор может включить фичу для всех участников. Это server-initiated действие через сигналинг `enable-feature-for-roles`.

2. **`MLFeaturesManagerImpl`** координирует KWS и NS через единый lifecycle (`start`/`dispose`). Оба получают доступ к микрофону через `MicrophoneManager`.

3. **`RemoteSettings`** управляет конфигурацией KWS и NS — модели и параметры задаются сервером.

## Сводка

`MLFeaturesManager` — оркестратор KWS+NS делегатов с загрузкой моделей через `DownloadService`. `ConversationFeatureManager` — серверный контроль фич: `enableFeatureForAll`/`enableFeatureForRoles`.
