---
tags: [upstream, github, bot-sdk, ecosystem]
status: confirmed
sources:
  - "https://github.com/max-messenger"
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[10-webapp-jsbridge]]"
---

# Upstream — что публично в `github.com/max-messenger`

Краткий внешний обзор того, что MAX выкладывает публично. Никакого исходника самого мобильного/десктопного клиента в публичном виде нет.

## Публичные репозитории организации

На 16 мая 2026 в организации `max-messenger` лежит 5 репозиториев, все — bot SDK, не клиент:

| Репозиторий | Язык | Лицензия | Статус |
|---|---|---|---|
| `max-bot-api-client-ts` | TypeScript | MIT | Updated Feb 10, 2026 |
| `max-bot-api-client-go` | Go | Apache-2.0 | Updated May 7, 2026 |
| `max-ui` | TypeScript | — | Updated Apr 17, 2026 |
| `max-botapi-python` | Python | MIT | Forked from `love-apples/maxapi`, updated Jul 30, 2025 |
| `max-bot-example-todolist` | Go | Apache-2.0 | Updated May 13, 2026 |

Что важно:

1. **Исходников самого клиента (Android/iOS/Web/Desktop) в публичном доступе нет.** Только SDK для разработчиков ботов и UI-компоненты для миниапок.
2. **`max-botapi-python` — форк сторонней библиотеки** (`love-apples/maxapi`), а не оригинальная разработка. То есть VK официально форкнул чужой пользовательский Python-клиент Bot API и поддерживает его дальше под брендом max-messenger.
3. Java-SDK (`max-bot-sdk-java`, упоминавшийся в источниках 2025 года в качестве свидетельства интенсивной разработки) **публично не висит** — либо переименован, либо переведён в private, либо удалён.
4. `max-ui` — публичный набор UI-компонентов для миниапок (frontend WebApp). Не несёт серверной части.

## Что значит для реверса

- **Нет public source-of-truth для протокола.** WS-опкоды, описанные в [[20-ws-protocol-opcodes]], восстановлены из jadx, не из публичной спецификации.
- Bot API (HTTP-API на `botapi.max.ru` или подобном) — публичный, документирован в этих SDK. Но это **отдельный канал** от WS-протокола клиента; через него ходят только боты, не пользовательский трафик.
- Сценарий «использовать Bot API для проверки гипотез о сервере MAX» — рабочий, но он не покрывает поведение клиент-сервер-сессии.
- Для понимания того, как сервер пушит UserSettings, ML-модели, PmsKey — публичных открытых клиентов **нет**.

## Linux desktop клиент

В Wikipedia упоминается: «Настольные приложения для Windows, macOS и Linux (форматов Deb, AppImage и RPM)». Дистрибутивы лежат на `https://max.ru/`, не на `download.max.ru` (который — Android-CDN, см. [[12-force-update-killswitch]]).

В рамках текущего реверс-конспекта Linux-клиент не разбирался (требует отдельного скачивания и анализа). Гипотеза по аналогии с Android: тот же стек `ru.ok.tamtam.*` обёрнут под Electron или native Qt, использует тот же WS-протокол `api.oneme.ru` с теми же 159 опкодами. Подтверждение требует отдельного сеанса.

## Сводка

`github.com/max-messenger` — исключительно bot-SDK. Реверс самого клиента остаётся «black-box» reverse engineering из распакованного APK, без перекрёстной верификации с публичным исходником.
