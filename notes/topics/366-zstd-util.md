---
tags: [compression, zstd, native, decompression]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/sdk/zsrd/ZstdUtil.java
related:
  - "[[09-native-libs]]"
---

# ZstdUtil — нативная декомпрессия Zstandard

`ZstdUtil` — нативная декомпрессия Zstandard (zstd).

## Методы

| Метод | Что |
|---|---|
| `a(byte[])` | **декомпрессия** (вызывает nativeDecompress) |
| `nativeDecompress(byte[])` | нативная декомпрессия |

## Что важно

1. **Zstandard** — современный алгоритм сжатия от Facebook. Используется для сжатия данных.

2. Только **декомпрессия** — нет метода compress. Это означает, что сжатые данные приходят с сервера.

3. Возможно используется для сжатия WS-сообщений или медиа-данных.

## Сводка

`ZstdUtil.a(bytes)` → `nativeDecompress(bytes)`. Только декомпрессия. Данные сжимаются на сервере.
