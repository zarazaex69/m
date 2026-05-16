---
tags: [config, manifest, apptracer, tracer-initializers, facebook, surveillance]
status: confirmed
sources:
  - work/apktool_base/AndroidManifest.xml
related:
  - "[[325-apptracer-sdk-components]]"
  - "[[363-google-maps-api-key]]"
---

# AndroidManifest meta-data — полный список

Все `<meta-data>` из AndroidManifest (без Firebase component registrars).

## Apptracer инициализаторы

| UUID | Инициализатор |
|---|---|
| `ca60b3bb-9ba5-4307-8c3f-5218111e10c5` | `NativeBridgeInitializer` |
| `b7b8ba75-c336-4dee-8e2a-64ef488e0218` | `CrashReportInitializer` |
| `06f2fae1-db2b-4f87-bc14-fc15c26da3b9` | `PerformanceMetricsInitializer` |
| `25573e8a-bb0f-473f-938a-ca5b3541bda1` | `LoggerInitializer` |
| `e416cfb4-5661-4523-b12c-0ff4ccabd436` | `TracerInitializer` |
| `0d46a772-bffc-4ede-ac88-5dcaf5f822c1` | `DiskUsageInitializer` |
| `c776f0d5-2110-4920-b53e-7b72536d8135` | `HeapDumpInitializer` |

## Другие

| Ключ | Значение |
|---|---|
| `com.google.android.geo.API_KEY` | `AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI` |
| `backend:com.google.android.datatransport.cct.CctBackendFactory` | `cct` |
| `com.facebook.soloader.enabled` | `false` |
| `com.android.vending.splits.required` | `true` |
| `com.android.stamp.type` | `STAMP_TYPE_DISTRIBUTION_APK` |
| `com.android.vending.derived.apk.id` | `4` |

## Что важно

1. **`com.facebook.soloader.enabled=false`** — Facebook SoLoader отключён. Это значит, что Facebook SDK присутствует, но SoLoader не используется.

2. **`STAMP_TYPE_DISTRIBUTION_APK`** — APK помечен как дистрибутивный (не debug).

3. **`derived.apk.id=4`** — ID сплита APK.

## Сводка

7 Apptracer инициализаторов + Google Maps API key + Facebook SoLoader disabled + STAMP_TYPE_DISTRIBUTION_APK.
