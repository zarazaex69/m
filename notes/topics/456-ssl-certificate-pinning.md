---
tags: [ssl, certificate-pinning, tls, security, okhttp, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/lcf.java
  - work/jadx_base/sources/one/me/net/ssl/api/InvalidSslIntegrityException.java
related:
  - "[[452-account-initializer]]"
  - "[[443-calls-sdk-initializer]]"
---

# lcf — SSL/TLS соединение с certificate pinning

`lcf` — OkHttp SSL/TLS соединение с поддержкой certificate pinning.

## Certificate Pinning

```java
// Проверка SHA-256 fingerprint публичного ключа
"sha256/" + sha256(x509Certificate.getPublicKey().getEncoded())
```

При несовпадении: `SSLPeerUnverifiedException("Hostname X not verified")`

## Обработка ошибок SSL

| Исключение | Что |
|---|---|
| `SSLHandshakeException + CertificateException` | ошибка рукопожатия |
| `SSLPeerUnverifiedException` | не верифицирован peer |
| `SSLException` | общая SSL ошибка |

## Что важно

1. **Certificate pinning** — MAX проверяет SHA-256 fingerprint публичного ключа сертификата.

2. **`InvalidSslIntegrityException extends SSLException`** — собственное исключение для нарушения SSL целостности.

3. **`InvalidSslSessionException`** — исключение для невалидной SSL сессии.

4. **`SslIntegrity`** задача в `AccountInitializer` — проверка SSL целостности при каждом запуске.

## Сводка

`lcf`: OkHttp SSL с certificate pinning (SHA-256). `InvalidSslIntegrityException`/`InvalidSslSessionException`. `SslIntegrity` задача при запуске.
