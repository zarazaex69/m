# URLs, Domains & API Endpoints — Max Messenger (ru.oneme.app)

## 1. Primary Application Domains

| Domain | Purpose |
|--------|---------|
| `max.ru` | Main app domain, deep links, user profiles |
| `download.max.ru` | App download/update distribution |
| `api.ok.ru` | Primary OK API endpoint (base URI for all API calls) |
| `api.odnoklassniki.ru` | Legacy OK API endpoint (Odnoklassniki) |

## 2. Internal URI Schemes

- `max://max.ru/...` — Deep link routing (chats, conversations, calls)
- `ok://api/api/...` — Internal API method dispatch (converts dotted method names to paths)
- `https://max.ru/:share-self-out` — Profile sharing
- `https://max.ru/joincall/<id>` — Call join links
- `max://max.ru/:start-conversation` — Start conversation intent
- `max://max.ru/:chats?id=<id>&type=local` — Open specific chat

## 3. API Methods (OK API via `ok://api/api/` scheme)

The app uses an RPC-style API where method names are converted to URI paths:

| Method Name | Purpose |
|-------------|---------|
| `vchat.startConversation` | Start a video/voice call |
| `vchat.joinConversation` | Join an existing call |
| `vchat.joinConversationByLink` | Join call via invite link |
| `vchat.hangupConversation` | End a call |
| `vchat.createJoinLink` | Create call invite link |
| `vchat.removeJoinLink` | Remove call invite link |
| `vchat.getConversationParams` | Get call parameters |
| `vchat.clientSupportedCodecs` | Report supported codecs |
| `vchat.getOkIdByExternalId` | Resolve external ID to OK ID |
| `vchat.getOkIdsByExternalIds` | Batch resolve external IDs |
| `vchat.getExternalIdsByOkIds` | Reverse ID resolution |
| `settings.get` | Fetch remote settings/config |

## 4. Third-Party Services & SDKs

### Analytics & Crash Reporting
| Domain | Service | Endpoints |
|--------|---------|-----------|
| `sdk-api.apptracer.ru` | AppTracer (VK crash/perf SDK) | `api/sample/initUpload`, `api/sample/upload`, `api/crash/upload`, `api/crash/uploadAnr`, `api/crash/uploadNative`, `api/perf/upload`, `api/crash/trackSession` |
| `tracker-api.vk-analytics.ru` | VK MyTracker | Attribution & analytics |

### Maps & Geolocation
| Domain | Service |
|--------|---------|
| `geocode-maps.yandex.ru` | Yandex Geocoding API (`/v1`) |
| `static-maps.yandex.ru` | Yandex Static Maps (`/v1`) |
| `tiles.api-maps.yandex.ru` | Yandex Map Tiles (`/v1/tiles/`) |

### Google Services
| Domain | Service |
|--------|---------|
| `firebaseinstallations.googleapis.com` | Firebase Installations (`/v1/`) |
| `accounts.google.com` | OAuth2 (revoke endpoint) |
| `www.googleapis.com` | Google Games auth |
| `pagead2.googlesyndication.com` | Google Ads |
| `play.google.com` | Play Store links |

### Video
| Domain | Purpose |
|--------|---------|
| `vkvideo.ru` | VK Video livestreams (`/live`) |

### DNS
| Domain | Purpose |
|--------|---------|
| `dns.google.com` | DNS-over-HTTPS resolution (`/resolve`) |

## 5. Network Security Configuration

### Cleartext Traffic (HTTP allowed)
The following domains permit cleartext HTTP traffic (mobile carrier auth):
- `mobileid.megafon.ru`
- `idgw.mobileid.mts.ru`
- `hhe.mts.ru`
- `he-mc.tele2.ru`
- `he-mc.t2.ru`
- `balance.beeline.ru`

These are Russian mobile carrier domains used for phone-number-based authentication (Mobile ID).

### SSL/TLS Implementation
- **Custom root CA**: `rootca_ssl_rsa2022.cer` bundled in `res/raw/`
- **Custom TrustManager**: `qse.java` implements `X509TrustManager`
- **SSL session cache**: `tamtam_sslcache` directory
- **SSL session validation**: Configurable via `net-ssl-session-validate` PMS key
- **Certificate pinning**: SHA-256 pin computation found in `lcf.java` (OkHttp-based)
- **Conscrypt**: Used as SSL provider with custom hostname verifier (`ng4.java`)
- **BouncyCastle JSSE**: Alternative TLS provider (`n31.java`, PKIX via BCJSSE)
- **WebView SSL**: Custom `onReceivedSslError` handler that may proceed on errors (`xyc.java`)
- **SSL provider class**: `one.me.net.ssl.api.SslProvider` (registered in DI)
- **Custom exceptions**: `InvalidSslSessionException`, `InvalidSslIntegrityException`

### GOST Check
- PMS key `gost-check-env` suggests Russian GOST cryptography environment checking

## 6. Remote Configuration Keys (Network-Related)

Key PMS (Platform Management Service) settings:
- `calls-endpoint` — Calls server endpoint
- `calls-android-update-endpoint-params` — Dynamic endpoint parameter updates
- `calls-android-signaling-ip` — Signaling server IP
- `calls-android-signaling-to` — Signaling timeout
- `net-client-dns-enabled` — Custom DNS client toggle
- `net-ssl-session-validate` — SSL session validation toggle
- `net-session-rbc-enabled` — Session RBC
- `net-session-suppress-bad-disconnected-state` — Connection state handling
- `proxy` — Proxy configuration
- `proxy-domains` — Proxy domain list
- `host-reachability` — Host reachability checks
- `conn-timeouts` — Connection timeouts
- `keep-connection` — Keep-alive
- `keep-background-socket` — Background socket persistence
- `ping-background-interval` — Background ping interval
- `livestreams-url-prefix` — Livestream URL prefix (configurable)

## 7. Deep Link Routes

The app handles these URL patterns:
- `https://max.ru/<username>` — User profiles
- `https://max.ru/joincall/<id>` — Join video calls
- `https://max.ru/:share-self-out` — Share own profile
- `max://max.ru/:start-conversation` — Start new conversation
- `max://max.ru/:chats?id=<id>&type=local` — Open chat
- `join`, `joincall` path segments — Call joining

## 8. All Extracted URLs (54 unique)

See `~/max/findings/all-urls.txt` for the complete list.

## 9. All Unique Domains (35)

See `~/max/findings/all-domains.txt` for the complete list.

Key categories:
- **App infrastructure**: max.ru, download.max.ru
- **Backend API**: api.ok.ru, api.odnoklassniki.ru
- **Analytics**: sdk-api.apptracer.ru, tracker-api.vk-analytics.ru
- **Maps**: geocode-maps.yandex.ru, static-maps.yandex.ru, tiles.api-maps.yandex.ru
- **Google**: firebase, googleapis, google.com, play.google.com
- **Video**: vkvideo.ru
- **DNS**: dns.google.com (DoH)

## 10. Key Findings Summary

1. **The app is built on OK (Odnoklassniki) infrastructure** — API calls go to `api.ok.ru` using an RPC-style method dispatch system
2. **VK ecosystem integration** — Uses VK analytics (MyTracker) and AppTracer for crash reporting
3. **Custom SSL stack** — Bundles its own root CA, uses Conscrypt + BouncyCastle, has certificate pinning
4. **Mobile carrier auth** — Cleartext HTTP allowed for 6 Russian carrier domains (Megafon, MTS, Tele2, Beeline)
5. **Yandex Maps** — Full integration with geocoding, static maps, and tile rendering
6. **Highly configurable** — Hundreds of remote config keys control network behavior, endpoints, and features
7. **App version**: 26.15.3 (visible in download URL)
