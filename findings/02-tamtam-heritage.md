# Finding: TamTam Heritage & Server Infrastructure

## TamTam Code Reuse
Max напрямую содержит код ТамТам. Не "наработки" — буквально тот же пакет.

### Пакеты ТамТам в Max APK:
```
ru.p027ok.tamtam.android.prefs.PmsKey     — все серверные настройки
ru.p027ok.tamtam.android.db.DataManager    — база данных
ru.p027ok.tamtam.android.notifications.*   — уведомления
ru.p027ok.tamtam.android.media.*           — медиа
ru.p027ok.tamtam.api.Api                   — основной API
ru.p027ok.tamtam.chats.*                   — чаты
ru.p027ok.tamtam.contacts.*                — контакты
ru.p027ok.tamtam.messages.*                — сообщения
ru.p027ok.tamtam.stats.LogController       — логирование
ru.p027ok.tamtam.stickers.*                — стикеры
ru.p027ok.tamtam.stickersets.*             — наборы стикеров
ru.p027ok.tamtam.polls.*                   — опросы
ru.p027ok.android.webrtc.*                 — WebRTC (звонки)
ru.p027ok.android.externcalls.*            — внешние звонки
```

### Доказательства:
- Пакет `ru.ok.tamtam` (p027ok = обфусцированный ok)
- Класс `PmsKey` находится в `ru.ok.tamtam.android.prefs`
- API endpoint: `https://api.ok.ru` / `https://api.odnoklassniki.ru`
- Внутренние строки: `isOkPushDisabled` (OK = Одноклассники)

## Серверная инфраструктура

### Домены (извлечены из APK):
| Домен | Назначение |
|-------|-----------|
| `max.ru` | Основной домен |
| `download.max.ru` | CDN загрузок |
| `api.ok.ru` | API (наследие OK) |
| `api.odnoklassniki.ru` | API (наследие OK) |
| `tracker-api.vk-analytics.ru` | VK Analytics трекер |
| `sdk-api.apptracer.ru` | AppTracer SDK |
| `vkvideo.ru/live` | Live стримы |
| `geocode-maps.yandex.ru` | Геокодирование |
| `static-maps.yandex.ru` | Статические карты |
| `tiles.api-maps.yandex.ru` | Тайлы карт |
| `firebaseinstallations.googleapis.com` | Firebase |
| `pagead2.googlesyndication.com` | Google Ads |

### Ключевые внутренние сервисы:
- `one.me.net.dns.api.Dns` — собственный DNS resolver
- `one.me.sdk.api.links.ApiLinks` — API ссылки
- `one.me.sdk.api.messages.MessagesApi` — API сообщений
- `ru.ok.messages.ProxyChangeListener` — слушатель смены прокси

## Вывод
Max = TamTam + новый UI + государственные сервисы. Ядро не переписано.
