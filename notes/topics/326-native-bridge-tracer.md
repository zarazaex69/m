---
tags: [telemetry, apptracer, native-bridge, jni, libtracernative]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tracer/nativebridge/NativeBridge.java
  - work/jadx_base/sources/ru/ok/tracer/nativebridge/NativeBridgeInstaller.java
related:
  - "[[228-libtracernative-exports]]"
  - "[[325-apptracer-sdk-components]]"
---

# NativeBridge — JNI мост к libtracernative

`NativeBridgeInstaller` — устанавливает JNI мост между Java и `libtracernative.so`.

## NativeBridge интерфейс

| Метод | Что |
|---|---|
| `log(msg)` | **логирование** |
| `setKey(key, value)` | **установить ключ** |

## Инициализация

```java
System.loadLibrary("tracernative");
nativeInstallBridge(qb5.Z);  // qb5.Z — реализация NativeBridge
```

## Что важно

1. **`nativeInstallBridge`** — устанавливает Java-объект как callback для нативного кода. Нативный код может вызывать `log` и `setKey` на Java-объекте.

2. **`qb5.Z`** — реализация NativeBridge. Это тот же `qb5`, который обрабатывает `WEB_APP_INIT_DATA` (см. [[287-web-app-init-data]]).

3. Это двусторонний мост: Java → native (через `tracer_*` функции) и native → Java (через `NativeBridge.log`/`setKey`).

## Сводка

`NativeBridgeInstaller.nativeInstallBridge(qb5.Z)` — устанавливает JNI мост. `NativeBridge`: log(msg)/setKey(key, value).
