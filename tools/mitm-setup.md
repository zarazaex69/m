# MITM Proxy Setup — Max Messenger Traffic Analysis

## For security research on YOUR OWN device only.

## Why It Works

Max messenger has **NO certificate pinning** (vuln-08):
- OkHttp CertificatePinner is initialized with EMPTY pin set
- QUIC uses trust-all TrustManager (accepts ANY certificate)
- User CA installation is sufficient for full interception

## Prerequisites

- Rooted Android device or emulator
- mitmproxy (`pip install mitmproxy`) or Burp Suite
- Frida (`pip install frida-tools`)
- adb access to device

## Step 1: Install mitmproxy CA

```bash
# Start mitmproxy to generate CA
mitmproxy --listen-port 8080

# Push CA to device
adb push ~/.mitmproxy/mitmproxy-ca-cert.cer /sdcard/

# On device: Settings → Security → Install from storage
# Or for system-level (rooted):
adb shell mount -o rw,remount /system
adb push ~/.mitmproxy/mitmproxy-ca-cert.cer /system/etc/security/cacerts/
adb shell chmod 644 /system/etc/security/cacerts/mitmproxy-ca-cert.cer
adb shell mount -o ro,remount /system
```

## Step 2: Configure Proxy

```bash
# Set WiFi proxy on device to host:8080
# Or use iptables redirect:
adb shell iptables -t nat -A OUTPUT -p tcp --dport 443 -j REDIRECT --to-port 8080
```

## Step 3: Intercept Traffic

Expected domains:
- `api.ok.ru` — Main API (binary protobuf + JSON)
- `sdk-api.apptracer.ru` — Crash/perf analytics
- `tracker-api.vk-analytics.ru` — VK behavioral analytics
- `firebaseinstallations.googleapis.com` — Push tokens

## Step 4: Force HTTP over QUIC

Since QUIC uses UDP and is harder to proxy, force the app to use HTTP/2:

```javascript
// frida-force-http.js
Java.perform(function() {
    // Disable QUIC/WebTransport
    var qp6 = Java.use("p000.qp6");
    qp6.isCallsWebTransportEnabled.implementation = function() {
        return false;
    };
});
```

## Step 5: Decode WebSocket Binary Protocol

The binary protocol uses 10-byte headers (see vuln-16):
```
[ver:1][cmd:1][seq:2][opcode:2][size_cof:4]
```

LZ4 decompression needed when `cof > 0` (upper 8 bits of size field).

```python
# decode_ws.py
import lz4.block
import struct

def decode_packet(data):
    ver, cmd, seq, opcode = struct.unpack('>BBhh', data[:6])
    size_cof = struct.unpack('>I', data[6:10])[0]
    cof = (size_cof >> 24) & 0xFF
    payload_len = size_cof & 0xFFFFFF
    payload = data[10:10+payload_len]
    if cof > 0:
        payload = lz4.block.decompress(payload, uncompressed_size=payload_len * cof)
    return {'ver': ver, 'cmd': cmd, 'seq': seq, 'opcode': opcode, 'payload': payload}
```

## Notes

- No Frida bypass needed for cert pinning (there is none)
- WebSocket connects to api.ok.ru on standard ports
- Binary protocol payload is Protobuf Nano format
