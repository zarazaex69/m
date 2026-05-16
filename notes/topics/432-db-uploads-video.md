---
tags: [database, uploads, draft, video, worker-queue, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
related:
  - "[[427-db-additional-tables]]"
  - "[[369-download-file-from-webapp]]"
---

# DB — uploads, draft_uploads, video_conversions, WorkerQueueItem

## uploads

```sql
CREATE TABLE `uploads` (
  `attach_local_id` TEXT,
  `prepared_path` TEXT,
  `file_name` TEXT,
  `upload_url` TEXT,
  `upload_progress` REAL NOT NULL,
  `total_bytes` INTEGER NOT NULL,
  `upload_status` INTEGER,
  `created_time` INTEGER NOT NULL,
  `path` TEXT NOT NULL,
  `last_modified` INTEGER NOT NULL,
  `upload_type` INTEGER NOT NULL,
  `photo_token` TEXT,
  `attach_id` INTEGER,
  `thumbhash_base64` TEXT,
  `desired_uploader` TEXT,
  PRIMARY KEY(`path`, `last_modified`, `upload_type`)
)
```

## draft_uploads

```sql
CREATE TABLE `draft_uploads` (
  `path` TEXT NOT NULL,
  `last_modified` INTEGER NOT NULL,
  `upload_type` INTEGER NOT NULL,
  `chat_id` INTEGER NOT NULL,
  `attach_id` TEXT NOT NULL,
  `video_quality` INTEGER,
  `video_start_trim_position` REAL,
  `video_end_trim_position` REAL,
  `mute` INTEGER DEFAULT false,
  PRIMARY KEY(`chat_id`, `attach_id`)
)
```

## video_conversions

```sql
CREATE TABLE `video_conversions` (
  `finished` INTEGER NOT NULL,
  `prepared_path` TEXT,
  `result_path` TEXT,
  `source_uri` TEXT NOT NULL,
  `quality` INTEGER NOT NULL,
  `start_trim_position` REAL NOT NULL,
  `end_trim_position` REAL NOT NULL,
  `mute` INTEGER NOT NULL DEFAULT false,
  PRIMARY KEY(`source_uri`, `quality`, `start_trim_position`, `end_trim_position`, `mute`)
)
```

## Что важно

1. **`uploads.upload_url`** — URL для загрузки файла хранится в БД.

2. **`uploads.thumbhash_base64`** — хэш превью изображения.

3. **`uploads.desired_uploader`** — желаемый загрузчик.

4. **`draft_uploads.video_start_trim_position`/`video_end_trim_position`** — позиции обрезки видео в черновиках.

5. **`video_conversions`** — история конвертации видео с параметрами качества и обрезки.

## Сводка

`uploads(upload_url/upload_progress/total_bytes/photo_token/thumbhash_base64)`. `draft_uploads(video_quality/start_trim/end_trim/mute)`. `video_conversions(source_uri/quality/start_trim/end_trim/mute)`.
