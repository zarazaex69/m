---
tags: [webapp, file-download, worker, foreground, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/upload/workers/DownloadFileFromWebAppWorker.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[279-tasks-file-download-prepare]]"
---

# DownloadFileFromWebAppWorker — загрузка файлов из мини-приложений

`DownloadFileFromWebAppWorker` — ForegroundWorker для загрузки файлов из мини-приложений.

## Параметры

| Параметр | Что |
|---|---|
| `fileName` | **имя файла** |
| `fileUrl` | **URL файла** |

## Что важно

1. **ForegroundWorker** — загрузка файлов из мини-приложений выполняется как foreground service. Это означает, что пользователь видит уведомление.

2. **`fileName`/`fileUrl`** — мини-приложение передаёт имя и URL файла для загрузки.

3. Сервер знает, какие файлы загружает пользователь из мини-приложений.

## Сводка

`DownloadFileFromWebAppWorker`: fileName/fileUrl. ForegroundWorker для загрузки файлов из мини-приложений.
