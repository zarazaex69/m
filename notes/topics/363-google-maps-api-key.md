---
tags: [config, google-maps, api-key, manifest, surveillance]
status: confirmed
sources:
  - work/apktool_base/AndroidManifest.xml
related:
  - "[[351-firebase-config-keys]]"
  - "[[354-yandex-maps-config]]"
---

# Google Maps API Key — AndroidManifest

`AndroidManifest.xml` содержит Google Maps API ключ.

## Значение

```xml
<meta-data android:name="com.google.android.geo.API_KEY"
           android:value="AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI"/>
```

## Что важно

1. **`AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI`** — Google Maps API ключ. Используется для Google Maps SDK.

2. Это **другой ключ** от Firebase API key (`AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q`).

3. Публичный ключ — хранится в AndroidManifest, доступен любому.

## Сводка

`com.google.android.geo.API_KEY=AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI` — Google Maps API ключ в AndroidManifest.
