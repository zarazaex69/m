---
tags: [critical, tls, quic, mitm, certificate-validation, security, production]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/qse.java
  - work/jadx_base/sources/defpackage/z68.java
related:
  - "[[220-quic-tls-disabled]]"
  - "[[52-network-session-tls-flags]]"
  - "[[125-p2p-relay-webtransport]]"
  - "[[531-wiretap-chain-collect-debug-dump]]"
---

# 537. QUIC TLS bypass: null TrustManager в production = silent MITM

## Суть

Класс `qse.java` реализует `X509TrustManager` с **пустыми** методами `checkClientTrusted()` и `checkServerTrusted()`. Этот TrustManager используется в `z68.java` для **всех QUIC-соединений** приложения. Приложение само выводит предупреждение в stdout, но использует небезопасную конфигурацию.

## Код

```java
// qse.java
public class qse implements X509TrustManager {
    public void checkClientTrusted(X509Certificate[] chain, String authType) { }
    public void checkServerTrusted(X509Certificate[] chain, String authType) { }
    public X509Certificate[] getAcceptedIssuers() { return null; }
}

// z68.java:783
qse qseVar = new qse();
System.out.println("SECURITY WARNING: INSECURE configuration! " +
    "Server certificate validation is disabled; " +
    "QUIC connections may be subject to man-in-the-middle attacks!");
tpiVar.s = qseVar;  // Sets trust-all on QUIC TLS engine
```

## Что это означает

QUIC (UDP) используется для **WebTransport** — транспорт медиа-данных звонков и потенциально сообщений. С null TrustManager:

1. **Любой** сетевой посредник (ISP, СОРМ, корпоративный прокси, домашний роутер) может представиться сервером MAX
2. Клиент **примет** любой сертификат без проверки
3. Посредник получает доступ к **расшифрованному** трафику
4. Пользователь **не получает** никакого предупреждения (stdout невидим)

## Цепочка с другими находками

```
QUIC TLS bypass (этот topic)
    + Отсутствие E2E шифрования
    + collect-debug-dump (topic 531)
    + AsrOnlineManager (серверная транскрипция)
    = Любой MITM может:
      1. Читать все сообщения
      2. Инициировать запись звонков
      3. Получать транскрипцию речи
      4. Инжектировать команды от имени сервера
```

## Почему это не «забытый debug»

- Код в `calls-sdk_release` модуле
- Предупреждение выводится в `System.out.println` (не в UI)
- Нет проверки `BuildConfig.DEBUG`
- Нет PmsKey для включения/выключения — **всегда активно**
- Библиотека `tech.kwik` (QUIC) имеет опцию `noServerCertificateCheck` — она **включена**

## Масштаб

WebTransport через QUIC используется для:
- P2P relay звонков (когда P2P невозможен)
- Потенциально — передача медиа в групповых звонках
- Потенциально — fallback для WS-соединения

PmsKey `calls-sdk-wt-enabled` контролирует, используется ли WebTransport. Если включён — весь медиа-трафик звонков идёт через MITM-уязвимый канал.

## Статус в 26.16.0

Без изменений.
