---
tags: [compression, lz4, zstd, native, decompression]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/util/compressor/LZ4.java
  - work/jadx_base/sources/one/me/sdk/zsrd/ZstdUtil.java
related:
  - "[[09-native-libs]]"
  - "[[366-zstd-util]]"
---

# Компрессия: LZ4 + Zstd

MAX использует два алгоритма сжатия: LZ4 и Zstd.

## LZ4

`ru.ok.android.util.compressor.LZ4` — нативная декомпрессия LZ4.

| Метод | Что |
|---|---|
| `a(byte[], byte[])` | декомпрессия (вызывает nativeDecompress) |
| `nativeDecompress(ByteBuffer, ...)` | нативная декомпрессия |

Только декомпрессия. Данные сжимаются на сервере.

## Zstd

`one.me.sdk.zsrd.ZstdUtil` — нативная декомпрессия Zstd.

| Метод | Что |
|---|---|
| `a(byte[])` | декомпрессия (вызывает nativeDecompress) |
| `nativeDecompress(byte[])` | нативная декомпрессия |

Только декомпрессия. Данные сжимаются на сервере.

## Что важно

1. **Два алгоритма** — LZ4 и Zstd. Вероятно, используются для разных типов данных.

2. **Только декомпрессия** — клиент только распаковывает данные с сервера.

3. LZ4 использует `ByteBuffer.allocateDirect` — прямая память для производительности.

## Сводка

LZ4 (`nativeDecompress(ByteBuffer)`) + Zstd (`nativeDecompress(byte[])`). Оба — только декомпрессия. Данные сжимаются на сервере.
