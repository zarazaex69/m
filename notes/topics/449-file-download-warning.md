---
tags: [file-download, warning, messages, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/filedownloadwarning/FileDownloadWarningBottomSheet.java
related:
  - "[[448-warning-link-bottomsheet]]"
  - "[[369-download-file-from-webapp]]"
---

# FileDownloadWarningBottomSheet — предупреждение при скачивании файла

`FileDownloadWarningBottomSheet` — предупреждение при скачивании потенциально опасного файла.

## Параметры

| Параметр | Что |
|---|---|
| `chat_id` | ID чата |
| `message_id` | ID сообщения |
| `attach_id` | ID вложения |
| `file_id` | ID файла |
| `file_name` | **имя файла** |
| `file_url` | **URL файла** |
| `file_size` | размер файла |

## Что важно

1. **`file_url`** — URL файла передаётся в предупреждение. Это означает, что сервер знает, какой файл пользователь пытается скачать.

2. **`file_name`** — имя файла показывается пользователю.

3. Аналитика: `a(vjhVar, 1/2/3)` — показ/принятие/отмена.

## Сводка

`FileDownloadWarningBottomSheet(chat_id, message_id, attach_id, file_id, file_name, file_url, file_size)`. Предупреждение при скачивании потенциально опасного файла.
