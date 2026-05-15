# Finding: Trackers & Analytics

## Встроенные трекеры

### 1. VK Analytics (MyTracker)
- Endpoint: `https://tracker-api.vk-analytics.ru/`
- PmsKey flags: `mytracker-enabled`, `mytracker-log-level`
- Контролируется сервером — можно включить/выключить удалённо

### 2. AppTracer
- Endpoint: `https://sdk-api.apptracer.ru`
- Трассировка производительности и крашей

### 3. Firebase
- `https://firebaseinstallations.googleapis.com/v1/`
- Firebase Cloud Messaging для push-уведомлений
- Firebase Analytics (через Google)

### 4. Google Ads
- `https://pagead2.googlesyndication.com/pagead/gen_204`
- Permission: `com.google.android.gms.permission.AD_ID`
- Рекламный идентификатор Google

## Собираемые метрики (из PmsKey)

### Сетевая статистика
- `net-stat-config` — конфигурация сбора сетевой статистики
- `opcode-stat-config` — статистика операций (opcodes)
- `stat-session-background-threshold` — порог фоновой сессии

### Производительность
- `perf-events` — события производительности
- `perf-registrar-config` — конфигурация регистратора
- `memory-slice-interval` — интервал замера памяти
- `battery-slice-interval` — интервал замера батареи

### Аналитика поведения
- `analytics-enabled` — общий флаг аналитики
- `wm-analytics-enabled` — аналитика WorkManager
- `push-delivery` — отслеживание доставки пушей

### Контакты
- `non-contact-sync-time` — синхронизация не-контактов
- `non-contact-max-chunk-size` — размер чанка
- `non-contact-collection-interval` — интервал сбора

## GOST Environment Check
- PmsKey: `gost-check-env`
- SharedPrefs: `gostLicenseCheckEnabled`
- Settings class: `gostEnvironmentCheckFlags` (int — битовая маска)
- Вероятно проверяет наличие ГОСТ-сертификатов Минцифры в системе
- Может использоваться для определения "доверенного" окружения

## Вывод
Приложение собирает обширную телеметрию: сетевую активность, производительность,
состояние батареи, контакты (включая не-контакты), рекламный ID.
Всё контролируется серверными флагами — можно включить дополнительный сбор
без обновления приложения.
