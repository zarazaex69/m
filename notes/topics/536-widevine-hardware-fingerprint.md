---
tags: [surveillance, fingerprint, hardware-id, widevine, drm, obfuscated, persistent]
status: verified
severity: high
sources:
  - work/jadx_base/sources/defpackage/kc5.java
related:
  - "[[329-mytracker-advertising-ids]]"
  - "[[416-device-info-vm5-cgj]]"
  - "[[415-dps-client-zu5]]"
---

# 536. Widevine DRM как несбрасываемый hardware fingerprint

## Суть

Класс `kc5.java` генерирует persistent device ID через **MediaDrm Widevine** (`edef8ba9-79d6-4ace-a3c8-27dcd51d21ed`), хеширует SHA-256 и обрезает до 16 hex-символов. Этот идентификатор **привязан к железу** и не сбрасывается при:
- Сбросе к заводским настройкам
- Переустановке приложения
- Смене Google-аккаунта
- Отключении рекламного ID

Все строки обфусцированы через `z8f.a()` — намеренное сокрытие от статического анализа.

## Механизм

```java
// Расшифрованные строки из z8f.a():
UUID WIDEVINE_UUID = "edef8ba9-79d6-4ace-a3c8-27dcd51d21ed";
String PROPERTY = "deviceUniqueId";
String ALGO = "SHA-256";
String EMULATOR_ANDROID_ID = "9774d56d682e549c";

MediaDrm drm = new MediaDrm(WIDEVINE_UUID);
byte[] deviceId = drm.getPropertyByteArray(PROPERTY);
byte[] hash = MessageDigest.getInstance(ALGO).digest(deviceId);
String fingerprint = bytesToHex(hash).substring(0, 16);
```

Fallback (если Widevine недоступен):
```java
String androidId = Settings.Secure.getString(resolver, "android_id");
if (androidId == null || androidId.equals(EMULATOR_ANDROID_ID)) {
    // генерирует UUID
}
// SHA-256(androidId) → 16 hex chars
```

## Почему это важно

1. **Несбрасываемый** — Widevine device ID зашит в TEE (Trusted Execution Environment), не меняется никогда
2. **Обфусцирован** — все строки XOR-шифрованы, чтобы Google Play Protect и антивирусы не детектировали fingerprinting
3. **Используется в DPS** — этот ID отправляется на `trace-flow.ru` в поле `deviceId` каждого snapshot
4. **Кросс-приложение** — тот же Widevine ID доступен любому приложению VK/OK экосистемы, позволяя связать пользователя между MAX, VK, OK.ru даже без общего аккаунта
5. **Проверка эмулятора** — `9774d56d682e549c` — известный android_id эмулятора, используется для детекции

## Связь с другими ID

| ID | Сбрасывается | Где используется |
|---|---|---|
| Widevine deviceUniqueId | ❌ Никогда | DPS (trace-flow.ru) |
| android_id | При factory reset | DPS fallback, MyTracker |
| Google GAID | Пользователем | MyTracker |
| Firebase FID | При переустановке | Push |

## Статус в 26.16.0

Без изменений.
