---
tags: [tls, security, quic, certificate-validation, mitm]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qse.java
  - work/jadx_base/sources/defpackage/z68.java
related:
  - "[[57-ws-session-config-fingerprint]]"
  - "[[125-p2p-relay-webtransport]]"
---

# Отключение проверки TLS-сертификата в QUIC

`qse` — кастомный `X509TrustManager` с пустыми реализациями `checkClientTrusted` и `checkServerTrusted`. Принимает любой сертификат.

## Контекст использования

Используется в `z68.java` — QUIC-соединение (библиотека `tech.kwik`):

```
qse qseVar = new qse();
System.out.println("SECURITY WARNING: INSECURE configuration! 
  Server certificate validation is disabled; 
  QUIC connections may be subject to man-in-the-middle attacks!");
tpiVar.s = qseVar;
```

Само приложение выводит предупреждение в stdout, но всё равно использует небезопасную конфигурацию.

## Что важно

1. **QUIC-соединения не проверяют сертификат сервера**. Это означает, что QUIC-трафик уязвим к MITM.

2. Это QUIC (UDP), а не обычный HTTPS. Используется для WebTransport (см. [[125-p2p-relay-webtransport]]).

3. Приложение само предупреждает о небезопасности (`tech.kwik.core.no-security-warnings`), но не исправляет.

## Сводка

`qse` — null TrustManager для QUIC/WebTransport. `checkServerTrusted` пустой — любой сертификат принимается. Приложение само выводит SECURITY WARNING, но использует небезопасную конфигурацию.
