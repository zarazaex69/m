# Полный реверс-инжиниринг мессенджера MAX (ru.oneme.app)

**Версия**: 26.15.3 (versionCode 6695)  
**Дата анализа**: 2026-05-15  
**Автор**: zarazaex  
**Инструменты**: jadx 1.5.5, apktool 2.7.0, radare2 6.1.4, strings

---

## TL;DR

Max — это ребренд мессенджера ТамТам с новым UI. Внутри:
- Полный пакет `ru.ok.tamtam.*` без изменений
- API ходит на `api.ok.ru` (Одноклассники)
- VPN детектится и репортится на сервер
- Фоновая проверка доступности push-серверов (Google/Huawei) и `api.oneme.ru`
- Нет E2E шифрования, нет root detection
- Блокировка альт-клиентов через серверную проверку подписи APK
- Трекеры: VK Analytics, AppTracer, Firebase, Google Ads

---

## 1. TamTam Heritage — Max это ТамТам

### Доказательства:
```
ru.p027ok.tamtam.android.prefs.PmsKey  — все настройки
ru.p027ok.tamtam.android.db.*          — база данных
ru.p027ok.tamtam.api.Api               — основной API
ru.p027ok.tamtam.chats.*               — чаты
ru.p027ok.tamtam.contacts.*            — контакты
ru.p027ok.tamtam.messages.*            — сообщения
ru.p027ok.tamtam.stats.*               — статистика
ru.p027ok.tamtam.nano.Protos           — protobuf схема
ru.p027ok.android.webrtc.*             — WebRTC звонки
```

API endpoint: `https://api.ok.ru` (Одноклассники!)

Внутренний флаг: `isOkPushDisabled` — "OK" = Одноклассники.

---

## 2. VPN Detection & Reporting

### Как детектит:
```java
// p000/jf4.java — NetworkMonitor
NetworkCapabilities caps = cm.getNetworkCapabilities(activeNetwork);
return caps.hasTransport(4); // TRANSPORT_VPN
```

### Что делает:
1. Отправляет `BAD_CONNECTION_ALERT` с типом `"VPN"` на сервер
2. Показывает UI: "Отключите VPN" / "Лучше без VPN"
3. Логирует: `"(VPN detected)"` в сетевой мониторинг

### Серверные флаги:
- `show-vpn-chat-bottomsheet` — предупреждение в чате
- `show-vpn-call-bottomsheet` — предупреждение при звонке  
- `show-vpn-snackbar` — снэкбар "Лучше без VPN"

**Важно**: даже если UI отключен — факт VPN всё равно отправляется на сервер.

---

## 3. Host Reachability — проверка блокировок

### Механизм:
- Фоновая задача `HostReachabilityChecker` (exact alarm)
- Проверяет доступность push-серверов:
  - Google: `mtalk.google.com`
  - Huawei: `pushtrs.push.hicloud.com`, `pushtrs1.push.hicloud.com`, `token-drcn.push.dbankcloud.com`
- Проверяет `api.oneme.ru` через DNS/ICMP + socket fallback

### Логика:
```
IF push недоступен AND api.oneme.ru доступен:
    → Запуск BackgroundListenService (foreground)
    → Режим "carpet_mode_on"
    → Отправка аналитики "BACKGROUND_MODE"
```

### Зачем:
Определяет что пользователь в зоне "белых списков" (push заблокирован, но Max работает) и переключается на persistent connection.

---

## 4. GOST Check — проверка окружения

### Флаги:
- `gost-check-env` → `gostEnvironmentCheckFlags` (int, битовая маска)
- `gostLicenseCheckEnabled` (boolean в SharedPrefs)

### Текущее состояние:
```
Build: "UseNarnia: false, Gost: false"
```
Фича **отключена** в текущей сборке, но инфраструктура готова.

### Что проверяет (когда включено):
- Наличие ГОСТ-сертификатов в системном trust store
- Использует `SSLCertificateSocketFactory` + кастомный `X509TrustManager`
- Логирует Comodo сертификаты при ошибках

---

## 5. WebRTC — архитектура звонков

### Стек:
- Native: `libjingle_peerconnection_so.so` (13MB, стандартный Google WebRTC)
- Java: `ru.p027ok.android.webrtc.*`
- Signaling: WebSocket + WebTransport (QUIC) с fallback

### STUN/TURN:
- **Нет hardcoded серверов** — всё динамически через `vchat.startConversation`
- Ответ содержит: `stun_server`, `turn_server` (urls/username/credential)
- Поддержка Signaling by IP (DNS bypass через `wsip`/`wtip` массивы)

### Кодеки:
- Видео: VP8, VP9, H.264, H.265 (приоритет), AV1
- Аудио: Opus (adaptive complexity, AI BWE)
- Simulcast: VP8 software encoder

### API звонков (12 методов):
```
vchat.startConversation, vchat.join, vchat.leave,
vchat.getParticipants, vchat.kick, vchat.mute, ...
```

### Особенности:
- P2P relay с адаптивным качеством (RTT threshold)
- Сервер может запретить P2P (`isP2PForbidden`)
- WebTransport через `tech.kwik.flupke` (QUIC library)
- 30+ feature flags для тонкой настройки

---

## 6. Сетевой протокол

### Transport:
- WebSocket over TLS (OkHttp)
- `permessage-deflate` compression
- Persistent background socket

### Message format:
- **Protobuf Nano** (`ru.p027ok.tamtam.nano.Protos`)
- Типы вложений: Photo, Video, Audio, Sticker, File, Contact, Location, Poll, Widget, etc.

### Session:
- Opcode-based messaging (как WebSocket frames)
- `subscription-timeout-seconds` для event subscription
- Custom DNS resolver (`one.me.net.dns.api.Dns`)

---

## 7. Anti-Tampering

### Что есть:
- Серверная проверка подписи APK при подключении
- `cis-enabled` (Client Integrity Service) — серверный флаг
- Проверка подписи Google Play Store

### Что отсутствует:
- ❌ Root detection
- ❌ Frida/Xposed detection  
- ❌ Play Integrity / SafetyNet
- ❌ Emulator detection
- ❌ Native anti-tampering
- ❌ Code obfuscation (только ProGuard rename)

### Как блокирует альт-клиенты:
Сервер проверяет подпись APK → если не совпадает с оригинальной VK подписью → бан.

---

## 8. Трекеры и аналитика

| Трекер | Endpoint | Что собирает |
|--------|----------|-------------|
| VK Analytics (MyTracker) | tracker-api.vk-analytics.ru | Поведение, сессии |
| AppTracer | sdk-api.apptracer.ru | Краши, перформанс |
| Firebase | googleapis.com | Push, аналитика |
| Google Ads | googlesyndication.com | Рекламный ID |

### Метрики:
- Сетевая статистика (`net-stat-config`)
- Opcodes статистика (`opcode-stat-config`)
- Батарея (`battery-slice-interval`)
- Память (`memory-slice-interval`)
- Контакты не-из-книги (`non-contact-*`)

---

## 9. Native Libraries

| Библиотека | Размер | Назначение |
|-----------|--------|-----------|
| libjingle_peerconnection_so.so | 13 MB | WebRTC |
| libEnhancementLibShared.so | 5.5 MB | TensorFlow Lite (AI) |
| libffmpg.so | 1.9 MB | FFmpeg |
| libtracernative.so | 781 KB | AppTracer native |
| libqrcode.so | 135 KB | QR scanner |

AI модуль использует NNAPI для шумоподавления и улучшения видео.

---

## 10. Permissions (56 штук)

### Критичные:
- `CAMERA` + `RECORD_AUDIO` — камера и микрофон
- `READ_CONTACTS` + `WRITE_CONTACTS` — полный доступ к контактам
- `ACCESS_FINE_LOCATION` — точная геолокация
- `NFC` — NFC (для цифрового ID?)
- `USE_BIOMETRIC` — биометрия
- `SYSTEM_ALERT_WINDOW` — overlay поверх всего
- `CHANGE_NETWORK_STATE` — изменение сетевых настроек
- `AD_ID` — рекламный идентификатор Google

---

## Выводы

1. **Max = TamTam + UI**. Ядро не переписано, API тот же (api.ok.ru).
2. **VPN детектится и репортится** даже без показа UI.
3. **Нет E2E шифрования** — все сообщения доступны серверу.
4. **Host reachability** определяет режим "белых списков" и адаптируется.
5. **GOST check** готов но отключен — инфраструктура для проверки сертификатов Минцифры.
6. **Anti-tampering минимален** — только серверная проверка подписи, нет root/frida detection.
7. **250+ серверных флагов** позволяют менять поведение без обновления APK.
8. **WebRTC стандартный** — можно использовать для OlcRTC (тот же протокол).
