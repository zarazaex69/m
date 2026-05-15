# План анализа Max Messenger

## Фаза 1: Сетевая активность и слежка

### 1.1 VPN Detection & Reporting
- [ ] Полный реверс `p000/jf4.java` — как детектит VPN
- [ ] Что происходит после детекта: `p000/b92.java` отправляет "BAD_CONNECTION_ALERT" с пометкой "VPN"
- [ ] Куда уходит инфа: серверный эндпоинт для аналитики
- [ ] Флаги `show-vpn-*` — когда показывает предупреждение, когда молча репортит

### 1.2 Host Reachability (проверки Telegram/WhatsApp/etc)
- [ ] Реверс `host-reachability` конфига
- [ ] Найти код который пингует внешние сервисы
- [ ] Какие домены проверяются (telegram.org, whatsapp.com, aws?)
- [ ] Куда отправляются результаты

### 1.3 GOST Check Environment
- [ ] Реверс `gost-check-env` — что проверяет
- [ ] Связь с сертификатами Минцифры
- [ ] Проверка наличия ГОСТ-сертификатов в системе?

### 1.4 Трекеры и аналитика
- [ ] MyTracker integration (`mytracker-enabled`, `mytracker-log-level`)
- [ ] `analytics-enabled` — что собирает
- [ ] `com.google.android.gms.permission.AD_ID` — рекламный ID
- [ ] `net-stat-config` — статистика сети
- [ ] `opcode-stat-config` — статистика операций
- [ ] `perf-events`, `perf-registrar-config` — перформанс метрики

## Фаза 2: Криптография и безопасность

### 2.1 Шифрование сообщений
- [ ] Есть ли E2E? (спойлер: нет, но проверить)
- [ ] Какой протокол транспорта (TLS? custom?)
- [ ] Certificate pinning — есть ли, как обойти
- [ ] `net-ssl-session-validate` — что валидирует

### 2.2 Аутентификация
- [ ] OpenID Connect с Госуслугами
- [ ] Цифровой ID (`digitalid-botid`)
- [ ] Биометрия (`USE_BIOMETRIC`)
- [ ] 2FA (`creation-2fa-config`)

### 2.3 Anti-tampering
- [ ] VK Security Gate — как работает проверка целостности
- [ ] Блокировка альтернативных клиентов — как детектит
- [ ] Root/Magisk detection?

## Фаза 3: WebRTC и звонки

### 3.1 Архитектура звонков
- [ ] `ru.p027ok.android.webrtc` — полный стек
- [ ] Signaling protocol
- [ ] STUN/TURN серверы — какие используются
- [ ] P2P vs relay (`calls-use-p2p-relay`)
- [ ] ICE candidates handling

### 3.2 Кодеки и качество
- [ ] H.265 support (`calls-android-h265-s`, `calls-sdk-h265-prioritized`)
- [ ] Opus audio (`calls-sdk-ai-opus-bwe`, `calls-sdk-linear-opus-bwe`)
- [ ] Simulcast (`calls-android-simulcast-sw-vp8`)

## Фаза 4: Серверная инфраструктура

### 4.1 Эндпоинты
- [ ] Извлечь все URL/домены из кода
- [ ] API endpoints
- [ ] CDN для медиа
- [ ] Push notification servers

### 4.2 Протокол
- [ ] Формат сообщений (protobuf? json? custom?)
- [ ] WebSocket / long-polling / custom TCP?
- [ ] `subscription-timeout-seconds` — подписка на события

## Фаза 5: Интересные фичи

### 5.1 Мини-приложения (WebApps)
- [ ] `webapp-ds-keys-count`, `webapp-ss-keys-count`
- [ ] `webapp-push-open`, `webapp-exc`
- [ ] Sandbox модель

### 5.2 Боты
- [ ] Bot API
- [ ] `stickers-botid`, `family-protection-botid`, `channel-statistics-botid`
- [ ] `money-transfer-botid` — платежи

### 5.3 Каналы и медиа
- [ ] Live streams (`live-streams`, `live-streams-url-prefix`)
- [ ] Video player (`one-video-player`, `one-video-failover`)

## Инструменты и подходы

### Статический анализ
- jadx — Java/Kotlin декомпиляция (done)
- apktool — smali + ресурсы (done)
- r2/Ghidra — native libs (arm64 split APK)
- grep/ripgrep — поиск строк, URL, ключей

### Динамический анализ (если будет эмулятор/устройство)
- Frida — хуки на ключевые методы
- objection — SSL pinning bypass, method tracing
- mitmproxy/Burp — перехват трафика
- Wireshark — анализ WebRTC

### Приоритеты для статьи на Хабр
1. VPN detection + reporting (горячая тема)
2. Host reachability checks (Telegram/WhatsApp пинги)
3. GOST check (сертификаты Минцифры)
4. Наследие ТамТам (доказательства ребренда)
5. Трекеры и сбор данных
6. WebRTC архитектура (для OlcRTC контекста)
