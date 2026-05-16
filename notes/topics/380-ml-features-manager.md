---
tags: [ml, kws, ns, tflite, model-download, calls, surveillance, server-control, partially-removed-in-26.16.0]
status: partially-removed-in-26.16.0
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/MLFeaturesManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/model/MLFeatureType.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/model/MLFeatureConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/MLFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/KwsFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/NSFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/config/kws/KwsFeatureConfigProvider.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/config/ns/NSFeatureConfigProvider.java
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[530-version-26.16.0-diff]]"
---

> ## ⚠️ В MAX 26.16.0 — частично удалено
>
> ❌ `KwsFeatureDelegate` + `Companion` — **удалены**.  
> ❌ `KwsFeatureConfigProvider` + `Companion` — **удалены**.  
> ❌ `MLFeaturesManagerImpl$delegates$1$2` (lambda для регистрации KWS-делегата) — удалена.  
> ❌ Ключ `android.mlfeatures.ws_0` (KWS-модель с сервера) — больше не загружается.  
> ✅ `NSFeatureDelegate` + `NSFeatureConfigProvider` (NoiseSuppression) — **на месте**.  
> ✅ `MLFeaturesManager` интерфейс — на месте (теперь обслуживает только NS).  
> ✅ Ключ `android.mlfeatures.ns_1` (NS-модель с сервера) — продолжает работать.
>
> Метрика: `MLFeatures` упоминаний 144 → 131 (-13, ровно KWS-делегаты).
>
> Ниже — описание для **26.15.3** с обоими делегатами. См. [[530-version-26.16.0-diff]].
>
> ---
  - "[[376-asr-online-manager]]"
---

# MLFeaturesManager — серверно-управляемые ML-модели в SDK

`MLFeaturesManager` — загрузка и управление ML-моделями для звонков. Модели приходят с сервера.

## Типы моделей (MLFeatureType)

| Тип | prefsKey | subDirName | Что |
|---|---|---|---|
| `WS` | `ws` | `ws` | **KWS** (Keyword Spotting) |
| `NS` | `ns` | `ns` | **NS** (Noise Suppression) |

## Версии моделей

| Модель | Версия |
|---|---|
| KWS | `ws_0` |
| NS | `ns_1` |

## MLFeatureConfig (с сервера)

| Поле | Что |
|---|---|
| `url` | **URL для загрузки модели** |
| `checksum` | MD5-контрольная сумма |
| `enabled` | включена ли модель |

## Remote keys

| Модель | Remote key |
|---|---|
| KWS | `android.mlfeatures.ws_0` |
| NS | `android.mlfeatures.ns_1` |

## Жизненный цикл модели

```
checkModel()
  → fetchConfig() → MLFeatureConfig{url, checksum, enabled}
  → validateCurrentModel()
      → NeedUpdate → downloadModel(url) → DownloadStage
          → unzipModel() → UnzipStage
          → saveNewModelInfo() → SaveNewModelInfoStage
      → UpToDate → ExistentModel(file)
```

Файлы: `<filesDir>/ml_features/<type>/<version>.zip` → распаковка → `.tflite` + `.cfg`

## Что важно

1. **Сервер контролирует URL и checksum** — модели загружаются с произвольного URL, заданного сервером. Это позволяет серверу подменить модель.

2. **`enabled` флаг** — сервер может отключить модель (`enabled=false`).

3. **MD5 checksum** — проверка целостности при загрузке.

4. **`.tflite` формат** — TensorFlow Lite. Модели запускаются на устройстве.

5. **`MLDownloadStat.readyToUse(version, durationMs)`** — статистика загрузки отправляется на сервер.

6. **Remote key `android.mlfeatures.*`** — конфиг приходит через RemoteSettings (PmsKey-подобный механизм).

## Сводка

`MLFeaturesManager`: KWS (`ws_0`) + NS (`ns_1`). Конфиг с сервера: `{url, checksum, enabled}`. Remote keys: `android.mlfeatures.ws_0` / `android.mlfeatures.ns_1`. Файлы: `.tflite` + `.cfg`. Сервер контролирует URL модели.
