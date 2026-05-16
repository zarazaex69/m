---
tags: [pmskey, server-control, mytracker, tracer, analytics, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[235-qp6-additional-pmskeys]]"
  - "[[328-mytracker-init]]"
---

# PmsKey — полный список (334+) — ключевые новые флаги

Дополнение к [[03-pms-server-flags]] и [[235-qp6-additional-pmskeys]]. Новые PmsKey, не покрытые ранее.

## Телеметрия и аналитика

| PmsKey | Что |
|---|---|
| `mytracker-enabled` | **MyTracker включён** |
| `mytracker-log-level` | уровень логирования MyTracker |
| `tracer-non-fatal-crashed-enabled` | **нефатальные крашы Apptracer** |
| `analytics-enabled` | аналитика включена |

## Рабочие процессы (WorkManager)

| PmsKey | Что |
|---|---|
| `wm-workers-limit` | лимит воркеров |
| `wm-check-workers-count-interval-sec` | интервал проверки |
| `wm-backlog-worker-check-delay-sec` | задержка проверки backlog |
| `wm-backlog-worker-backoff-delay-sec` | задержка backoff |
| `wm-workers-offset` | смещение воркеров |
| `wm-analytics-enabled` | аналитика WorkManager |
| `wm-ex-count` | количество потоков |
| `worker-early-fg` | ранний foreground |
| `worker-download-fg-fix` | фикс foreground загрузки |

## Медиа

| PmsKey | Что |
|---|---|
| `calc-audio-wave` | вычислять аудио-волну |
| `calc-video-wave` | вычислять видео-волну |
| `audio-peaks-count` | количество пиков аудио |
| `memory-slice-interval` | интервал среза памяти |
| `battery-slice-interval` | интервал среза батареи |

## Прочее

| PmsKey | Что |
|---|---|
| `ymap` | Яндекс.Карты |
| `views-count-enabled` | счётчик просмотров |
| `webview-cache-enabled` | кэш WebView |
| `pub-search-limit` | лимит публичного поиска |
| `client-conv-id` | ID разговора клиента |
| `send-queue-size` | размер очереди отправки |
| `upload-hang-barrier` | барьер зависания загрузки |
| `progress-diff-for-notify` | разница прогресса для уведомления |
| `worker-progress-time-diff-for-notify-ms` | время разницы прогресса |
| `max-downloaded-size-for-notify-kb` | максимальный размер для уведомления |

## Сводка

Новые PmsKey: `mytracker-enabled`/`tracer-non-fatal-crashed-enabled`/`wm-*`(WorkManager)/`calc-audio-wave`/`battery-slice-interval`/`ymap`/`views-count-enabled`.
