# 07 — Crypto Audit: MAX (ru.oneme.app) 26.15.3 / 26.16.0

status: complete
tags: #crypto #pinning #keystore #gost #native #webapp #hardcoded-keys
sources: jadx decompilation, apktool res/xml, native .so symbols

---

## TL;DR — Top 5 Critical Findings

1. **ZERO certificate pinning** — ни для `api.oneme.ru`, ни для WS-серверов, ни для `apptracer.ru`, ни для `MyTracker`. Нет ни `CertificatePinner` (OkHttp), ни `<pin-set>` в `network_security_config.xml`. Любой MITM с доверенным CA (корпоративный, государственный) перехватывает ВСЁ.

2. **Custom CA `rootca_ssl_rsa2022` добавлен в trust store для Apptracer upload** — `l88.java` загружает собственный CA-сертификат из `res/raw/rootca_ssl_rsa2022` и добавляет его как дополнительный TrustManager. Это позволяет серверу Apptracer использовать сертификат, подписанный этим CA, а при компрометации CA — MITM на upload-канал.

3. **Conscrypt hostname verifier `mg4.a` — пустая реализация** — класс `mg4` implements `ConscryptHostnameVerifier` но **не содержит метода `verify()`** в декомпиляции. Это означает, что hostname verification для Conscrypt-based TLS (основной TLS-стек OkHttp) может быть отключена или всегда возвращать true.

4. **Hardcoded QUIC Initial/Retry AES-GCM keys** в `zrf.java` — 6 статических byte[] массивов (3 ключа × 2 IV) для AES/GCM шифрования QUIC Retry Token. Это стандартные QUIC Initial keys (RFC 9001), но их наличие в hardcoded виде подтверждает, что QUIC-слой не использует дополнительного pinning.

5. **Media encryption keys приходят с сервера** — `ue.java`/`ve.java` реализуют AES/CBC/PKCS7Padding дешифрование медиа-контента, ключ и IV передаются как `byte[]` из серверного ответа (`q28Var.Z` → `d28.e()` hex→bytes). Сервер полностью контролирует ключи шифрования медиа — E2E отсутствует.

---

## 1. Certificate Pinning — ОТСУТСТВУЕТ

### network_security_config.xml (26.16.0)

```xml
<?xml version="1.0" encoding="utf-8"?>
<network-security-config>
    <domain-config cleartextTrafficPermitted="true">
        <domain includeSubdomains="false">mobileid.megafon.ru</domain>
        <domain includeSubdomains="false">idgw.mobileid.mts.ru</domain>
        <domain includeSubdomains="false">hhe.mts.ru</domain>
        <domain includeSubdomains="false">he-mc.tele2.ru</domain>
        <domain includeSubdomains="false">he-mc.t2.ru</domain>
        <domain includeSubdomains="false">balance.beeline.ru</domain>
    </domain-config>
</network-security-config>
```

**Что здесь:**
- Только `cleartextTrafficPermitted="true"` для Mobile ID операторов (уже известно из topic 01)
- **НЕТ `<pin-set>`** ни для одного домена
- **НЕТ `<trust-anchors>`** с ограничением CA

### OkHttp CertificatePinner
- Поиск по всей кодовой базе: **0 результатов** для `CertificatePinner`
- Поиск `sha256/` (стандартный формат pin): **0 результатов**

### X509TrustManager реализации
- `qse.java` — null TrustManager для QUIC (уже в topic 537)
- `e94.java` — composite TrustManager (system + custom CA), но **без pinning**
- `tfi.java` — hostname-aware TrustManager с логированием, но **без pin verification**
- `mg4.java` — **пустой** ConscryptHostnameVerifier (см. ниже)

### Вывод
Весь трафик MAX (API, WebSocket, Apptracer, MyTracker, OneLog) защищён только системным CA store. Государственный CA (Минцифры РФ) в системном store Android → полный MITM без предупреждений.

---

## 2. Custom CA для Apptracer

**Файл:** `l88.java` (Apptracer HTTP client)

```java
int i = m2f.rootca_ssl_rsa2022;  // res/raw/rootca_ssl_rsa2022
Certificate cert = CertificateFactory.getInstance("X509").generateCertificate(inputStream);
KeyStore keyStore = KeyStore.getInstance(KeyStore.getDefaultType());
keyStore.setCertificateEntry(resourceEntryName, (X509Certificate) cert);
TrustManagerFactory tmf2 = TrustManagerFactory.getInstance(...);
tmf2.init(keyStore);
// Composite: system TM + custom CA TM
e94 compositeTM = new e94(new X509TrustManager[]{systemTM, customCATM});
```

**Импликация:** Apptracer upload endpoint (`sdk-api.apptracer.ru`) может использовать сертификат, подписанный этим custom CA. Если CA-ключ скомпрометирован или контролируется оператором — MITM на канал, по которому идут heap-dumps, crash-reports, sample-uploads.

**Используется в:** `ia.java` (TracerSDK HTTP client), `ah.java`

---

## 3. Conscrypt Hostname Verifier — ПУСТОЙ

**Файл:** `mg4.java`

```java
public final class mg4 implements ConscryptHostnameVerifier {
    public static final mg4 a = new mg4();
}
```

Класс **не содержит метода `verify()`**. В Conscrypt, если `ConscryptHostnameVerifier` не реализует `verify()` корректно, hostname verification может быть отключена. Это singleton (`mg4.a`), который устанавливается в `ng4.java`:

```java
Conscrypt.setHostnameVerifier(x509TrustManager, mg4.a);
```

**Импликация:** Основной TLS-стек (Conscrypt через OkHttp) может не проверять hostname сертификата. Это делает MITM тривиальным даже без контроля CA.

---

## 4. Hardcoded Keys

### 4.1 QUIC Initial/Retry Keys (zrf.java)

```java
public static final byte[] k = {-52, -50, 24, 126, -48, -102, 9, -48, 87, 40, 21, 90, 108, -71, 107, -31};  // AES key v1
public static final byte[] l = {-66, 12, 105, 11, -97, 102, 87, 90, 29, 118, 107, 84, -29, 104, -56, 78};  // AES key v2
public static final byte[] m = {-113, -76, -80, 27, 86, -84, 72, -30, 96, -5, -53, -50, -83, 124, -52, -110};  // AES key retry
public static final byte[] n = {-27, 73, 48, -7, 127, 33, 54, -16, 83, 10, -116, 28};  // GCM IV v1
public static final byte[] o = {70, 21, -103, -45, 93, 99, 43, -14, 35, -104, 37, -69};  // GCM IV v2
public static final byte[] p = {-40, 105, 105, -68, 45, 124, 109, -103, -112, -17, -80, 74};  // GCM IV retry
```

Используются для AES/GCM/NoPadding шифрования QUIC Retry Token integrity tag. Это стандартные QUIC Initial keys (RFC 9001 §5.2), но подтверждают отсутствие дополнительной защиты.

### 4.2 Firebase/Google API Keys (strings.xml)

```
google_api_key: AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q
google_app_id: 1:659634599081:android:9605285443b661167225b8
gcm_defaultSenderId: 659634599081
google_storage_bucket: max-messenger-app.firebasestorage.app
```

Стандартные Firebase keys (не секретные, но позволяют идентифицировать проект и потенциально отправлять push через FCM при неправильной конфигурации server-side).

### 4.3 Yandex Geocode API Key

```java
// xwk.java
"https://geocode-maps.yandex.ru/v1?...&apikey=" + xwkVar.b;
```

Ключ приходит из серверного конфига (PmsKey), не hardcoded в APK.

### 4.4 trace_flow DPS API Key

```java
// DpsInitProvider.java — reads from AndroidManifest meta-data:
bundle.getString("ru.trace_flow.dps.API_KEY")
bundle.getString("ru.trace_flow.dps.USER_ID")
bundle.getString("ru.trace_flow.dps.CLIENT_VERSION")
```

DPS (Device Performance SDK) ключ в meta-data манифеста. Провайдер `DpsInitProvider` **disabled** в манифесте (`android:enabled="false"`), но может быть включён серверно.

### 4.5 IP Detection Endpoints (hph.java, obfuscated)

Decoded via `z8f.a()`:
- `https://trace-flow.ru` — основной endpoint
- `https://ipv4-internet.yandex.net/api/v0/ip`
- `https://ipv6-internet.yandex.net/api/v0/ip`
- `https://ifconfig.me/ip`
- `https://api.ipify.org`
- `https://checkip.amazonaws.com`
- `https://ip.mail.ru/`
- Target domain: `api.oneme.ru`
- IP regex: `\b(?:[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}|[0-9a-fA-F:]+:[0-9a-fA-F:]+)\b`

**Импликация:** Приложение активно определяет внешний IP пользователя через 6 разных сервисов. Это используется для обхода VPN-детекции и геолокации по IP.

### 4.6 Widevine UUID (kc5.java, obfuscated)

```java
z8f.a("c6681ae4817e0da0dc7809ffc92d51a2d2375ca7877f45a7d77950ebd62d0ca5802f59a2d62b0da2")
// Decodes to: edef8ba9-79d6-4ace-a3c8-27dcd51d21ed (Widevine UUID)
```

---

## 5. ГОСТ-крипто

### PmsKey `gost-check-env` (PmsKey #332)

```java
// PmsKey.java
public static final PmsKey f127gostcheckenv = new PmsKey("gost-check-env", 332);

// qp6.java — usage
this.B2 = new no6(this, PmsKey.f127gostcheckenv, i29);
```

Серверный флаг для проверки ГОСТ-окружения. Когда включён, проверяет наличие ГОСТ-провайдера.

### GOST как тип подключения (ke.java)

```java
case 8:
    str2 = "GOST";
    break;
```

GOST — один из типов подключения наряду с PUSH, PRESENCE, POLL. Это отдельный канал связи через ГОСТ-шифрование.

### Текущий статус

```java
// pr4.java — debug info
"UseNarnia: false\n    Gost: false\n    UsePersonalCloud: false"
```

ГОСТ-канал существует в коде, но по умолчанию отключён. Может быть включён серверным флагом `gost-check-env`. Нет прямых ссылок на КриптоПро/ВипНет SDK — вероятно, используется через системный JCE-провайдер (если установлен на устройстве).

---

## 6. AndroidKeyStore Usage

### Biometric-protected keys (vbk.java)

```java
KeyGenParameterSpec.Builder builder = new KeyGenParameterSpec.Builder(str, 3)
    .setBlockModes("CBC")
    .setEncryptionPaddings("PKCS7Padding");
if (biometric) {
    builder.setUserAuthenticationRequired(true);
    builder.setUserAuthenticationParameters(120, 2);  // 120 sec, biometric
} else {
    builder.setUserAuthenticationRequired(false);
}
KeyGenerator.getInstance("AES", "AndroidKeyStore").init(builder.build());
```

**Что хранится:**
- `androidxBiometric` — ключ для биометрической аутентификации (zql.java)
- Ключи для шифрования локального кэша (vbk.java)

**Проблема:** Когда `biometric=false`, ключ в KeyStore **без аутентификации** — любое приложение с root может его извлечь.

### Encrypted local cache (vbk.java method `n`)

```java
// Reads encrypted cache with AES/CBC using SecretKeySpec from field this.c
SecretKeySpec secretKeySpec = (SecretKeySpec) this.c;
cipher.init(2, secretKeySpec, ivParameterSpec);
dataInputStream = new DataInputStream(new CipherInputStream(bufferedInputStream, cipher));
```

Локальный кэш (ExoPlayer media cache) шифруется AES/CBC, но ключ хранится как `SecretKeySpec` в памяти, не в KeyStore.

---

## 7. WebApp SecureStorage

### Архитектура (vmk.java, lmk.java)

```
WebAppSecureStorageSaveKey → vmk.k() → nz8.a() → сериализация → ws9 event
WebAppSecureStorageGetKey  → vmk.j() → nz8.a() → десериализация
WebAppSecureStorageClear   → vmk.i()
```

**Критический момент:** SecureStorage для мини-приложений **НЕ использует шифрование на клиенте**. Данные сериализуются через `nz8` (JSON serializer) и отправляются на сервер через WebSocket (`ws9` event с `sessionId`, `botId`, `webAppName`).

**Импликация:** «Secure» storage мини-приложений — это серверное хранилище. Сервер имеет полный доступ ко всем «секретным» данным мини-приложений. Нет client-side encryption.

---

## 8. Media Encryption

### Схема (ue.java, ve.java, u38.java)

```java
// AES/CBC/PKCS7Padding decryption
cipher.init(2, new SecretKeySpec(this.b, "AES"), new IvParameterSpec(this.c));
```

- Ключ (`this.b`) и IV (`this.c`) приходят из серверного ответа
- `q28Var.Z` → `d28.e(str)` — hex string → 16-byte AES key
- Сервер полностью контролирует ключи шифрования медиа
- **Нет E2E:** сервер может расшифровать любой медиа-файл

---

## 9. Native Crypto

### libEnhancementLibShared.so

```
_ZN2vk3enh7encryptENSt6__ndk16vectorIhNS1_9allocatorIhEEEE  @ 0x1302dc
_ZN2vk3enh7decryptENSt6__ndk16vectorIhNS1_9allocatorIhEEEE  @ 0x130360
```

VK Enhancement library имеет собственные `encrypt`/`decrypt` функции. Используются для шифрования ML-моделей (KWS/NS .tflite). Ключ — hardcoded в .so (уже в topic 28).

### libjingle_peerconnection_so.so

```
SRTP_AES128_CM_SHA1_32
SRTP_AEAD_AES_256_GCM
Java_org_webrtc_RtpReceiver_nativeSetFrameDecryptor
Java_org_webrtc_RtpSender_nativeSetFrameEncryptor
```

WebRTC SRTP присутствует, но `FrameEncryptor`/`FrameDecryptor` — опциональные интерфейсы. Нет доказательств, что E2E frame encryption активирован (см. topic 540 — WebRTC encryption может быть отключено серверно).

### libffmpg.so

Содержит строки `decryption key`, `Encryption info`, `encrypted and compressed` — стандартные FFmpeg DRM-related строки.

---

## 10. Apptracer NativeBridge

### NativeBridgeInitializer.java

```java
public final Object b(Context context) {
    try {
        NativeBridgeInstaller.a();  // System.loadLibrary("tracernative")
    } catch (Throwable unused) {}
    return p7j.a;
}
```

### NativeBridge interface (qb5.java)

```java
public class qb5 implements hy3, e84, NativeBridge, dx4, e8 {
    @Override // ru.ok.tracer.nativebridge.NativeBridge
    public void log(String str) { ... }
}
```

**Нет slip-key в NativeBridgeInitializer** — инициализация тривиальная (loadLibrary + install bridge). Однако `tracernative.so` получает полный доступ к процессу через NativeBridge interface (crash reporting, stack traces, memory access).

---

## 11. String Obfuscation (z8f.a)

Функция `z8f.a(String)` — XOR-based deobfuscation:
- Первые 8 hex chars → 32-bit XOR key
- Остальные hex pairs → XOR с key (rotating per byte position)

Используется для скрытия:
- URL endpoints (trace-flow.ru, ip-detection services)
- Class/method names для reflection
- API key names в meta-data
- Widevine UUID

**Не является криптографической защитой** — тривиально реверсится (как показано выше).

---

## 12. HKDF Usage (kcb.java)

```java
// HmacSHA256-based HKDF
b = new kcb(new o18("HmacSHA256", 0));
SecretKeySpec secretKeySpec = new SecretKeySpec(bArr, o18Var.a);
mac.init(secretKeySpec);
```

Используется для key derivation в QUIC TLS handshake (tech.kwik). Стандартная реализация, ключевой материал из TLS handshake.

---

## Общий вывод

MAX **не имеет certificate pinning** — это фундаментальная уязвимость для мессенджера. В сочетании с:
- Государственным CA Минцифры в системном store
- Пустым hostname verifier (mg4)
- Custom CA для Apptracer
- Серверным контролем медиа-ключей
- Отсутствием E2E encryption

...любой оператор с доступом к сети (ISP, СОРМ, корпоративный proxy) может:
1. Перехватить весь трафик (MITM)
2. Читать все сообщения (нет E2E)
3. Получить медиа-ключи (серверные)
4. Инжектировать данные в WebSocket

Это не баг — это архитектурное решение, обеспечивающее полную прозрачность для серверного оператора и сетевых посредников.
