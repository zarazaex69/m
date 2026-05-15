# Server-Side Capabilities — Max Messenger v26.15.3

## Factual Documentation of Server Operator Capabilities

This document lists technically verified capabilities available to the server
operator (VK/Mail.ru Group) based on static code analysis. Each capability
is cited with the specific code location and finding reference.

---

## Message Access

| Capability | Evidence | Finding |
|-----------|----------|---------|
| Read all messages | No E2E encryption, plaintext on server | vuln-14 |
| Read drafts before sending | drafts-sync-enabled, real-time upload | vuln-48 |
| Full-text search index | Server-side search, all messages indexed | vuln-55 |
| Read scheduled messages | Content uploaded at schedule time | vuln-48 |

## Location & Network

| Capability | Evidence | Finding |
|-----------|----------|---------|
| Request location via push | LocationRequest push type, no UI consent | vuln-39 |
| Detect VPN usage | hasTransport(4) + BAD_CONNECTION_ALERT | vuln-15 |
| Check host reachability | carpet_mode + host-reachability PmsKey | vuln-15 |
| Force DNS through ISP | net-client-dns-enabled toggle | vuln-36 |

## Remote Configuration (335 PmsKey flags)

| Capability | PmsKey | Finding |
|-----------|--------|---------|
| Disable SSL validation | net-ssl-session-validate | vuln-28 |
| Enable sensitive logging | log-sensitive, log-full | vuln-28 |
| Enable GOST crypto | gost-check-env | vuln-15 |
| Suppress telemetry events | devnull config | vuln-30 |
| Show VPN warnings | show-vpn-chat/call/snackbar | vuln-15 |
| Increase audio recording quality | opus-recorder-bitrate/sample-rate | vuln-47 |
| Swap KWS model | android.mlfeatures.ws_0 | vuln-20 |
| Force app lockout | app-update-type=1 | vuln-50 |
| Inject fake chats | fake-chats PmsKey | vuln-73 |
| Push non-dismissable banners | informer system | vuln-30 |
| Control connection timeouts | conn-timeouts | vuln-36 |
| Invalidate local database | invalidate-db-force | vuln-28 |

## Data Collection

| Data | Method | Opt-out? | Finding |
|------|--------|----------|---------|
| All contacts (phone+name) | Plaintext sync, batches of 100 | No | vuln-22 |
| Device fingerprint | Model, SIM, carrier, screen | No | vuln-24 |
| Battery/CPU metrics | Periodic slices | No | vuln-24 |
| Sensor data | Proximity, gyroscope, accelerometer | No | vuln-24 |
| Search queries | Sent to server | No | vuln-55 |
| Photo GPS/EXIF | Not stripped before upload | No | vuln-25 |
| Online status | Always tracked | No | vuln-42 |
| Read receipts | Always sent | No | vuln-42 |
| Typing indicators | Server-controlled | No | vuln-42 |
| VK ecosystem IDs | OK/VK/ICQ cross-tracking | No | vuln-74 |

## Kill Switches

| Action | Mechanism | Finding |
|--------|-----------|---------|
| Force logout | Server push LOGOUT opcode | vuln-12 |
| Force update (block all) | app-update-type=1, blocks calls/pushes/UI | vuln-50 |
| Destroy local database | invalidate-db-force PmsKey | vuln-28 |
| Redirect traffic | Server host/endpoint change | vuln-53 |
| Disable custom DNS | net-client-dns-enabled=false | vuln-36 |

## Call Surveillance

| Capability | Evidence | Finding |
|-----------|----------|---------|
| Force media through relay | p2p_forbidden flag | vuln-67 |
| Server-side transcription | transcribes-dialog API, ASR streaming | vuln-47 |
| Swap keyword detection model | android.mlfeatures.ws_0 remote config | vuln-20 |
| Start recording via signaling | AsrCommandsExecutor.startRecord() | vuln-47 |
| Log call audio | calls-sdk-log-audio PmsKey | vuln-28 |

## Summary Statistics

- **335** server-controlled behavior flags (PmsKey)
- **0** user-facing opt-out mechanisms for data collection
- **0** end-to-end encrypted communication channels
- **14** distinct data categories collected without consent
- **5** kill switch mechanisms available to server operator
- **9** outbound domains receiving user data
