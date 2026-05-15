# VULN-66: WorkManager Background Execution Vulnerabilities

## Summary

Max messenger uses Android WorkManager extensively for background tasks. Analysis reveals a bot-triggered arbitrary file download vulnerability, a worker queue exhaustion DoS vector, and insufficient URL validation in download workers.

## Registered Workers

| Worker | Base Class | Purpose |
|--------|-----------|---------|
| `DownloadFileFromWebAppWorker` | ForegroundWorker | Downloads files from URLs provided by WebApp bots |
| `DownloadAttachesWorker` | ForegroundWorker | Downloads message attachments (images, videos, files) |
| `DownloadFileAttachWorker` | ForegroundWorker | Downloads single file attachments |
| `UploadFileAttachWorker` | ForegroundWorker | Uploads file attachments to server |
| `BacklogWorker` | SdkCoroutineWorker | Processes queued workers when limit is reached |
| `TaskMonitor$TaskMonitorWorker` | SdkCoroutineWorker | Monitors and executes SDK tasks |
| `PipWorker` | SdkCoroutineWorker | Picture-in-picture video handling |
| `DbCleanUpScheduler$DbCleanUpWorker` | SdkCoroutineWorker | Database cleanup (stats older than 48h) |
| `MessageCommentsCleanupWorker` | SdkCoroutineWorker | Cleans up message comments |
| `DailyAnalyticsWorker` | Worker | Daily permission/analytics stats |
| `SampleUploadWorker` | Worker | Tracer sample upload |
| `ShrinkDumpWorker` | Worker | Heap dump shrinking |
| `DiskUsageWorker` | Worker | Disk usage monitoring |

## Vulnerability 1: Bot-Triggered Arbitrary File Download (HIGH)

**Location:** `ru.p027ok.tamtam.upload.workers.DownloadFileFromWebAppWorker`  
**Trigger:** `p000.qik` (enqueue logic from WebApp bot interaction)

### Attack Flow

1. A WebApp bot sends a `WebAppDownloadFileRequest` containing:
   - `requestId` (long)
   - `url` (arbitrary string → `fileUrl`)
   - `fileName` (arbitrary string)

2. The request is processed in `qik.mo2n()` which creates an `rgi` object:
   ```java
   rgi rgiVar = new rgi(j2, j, str2, str);  // requestId, botId, fileUrl, fileName
   ```

3. The worker is enqueued with `KEEP` policy (`zd6.f86359b`) and expedited execution.

4. In `DownloadFileFromWebAppWorker`, the input data is extracted directly from WorkerParameters:
   ```java
   y45Var.m25677d("requestId", 0L);
   y45Var.m25677d("botId", 0L);
   y45Var.m25678e("fileName");  // attacker-controlled
   y45Var.m25678e("fileUrl");   // attacker-controlled
   ```

5. The `fileUrl` is used with `URI.create()` to extract the host, but **no URL allowlist validation** is performed before download:
   ```java
   URI.create(m20602s().f60159c).getHost()  // only used for notification text
   ```

### Impact

- **Arbitrary URL download**: A malicious bot can trigger downloads from any URL (internal network, file://, etc.)
- **SSRF via download**: The device acts as a proxy to fetch internal resources
- **Storage exhaustion**: Repeated large file downloads fill device storage
- **No file size limit visible** in the worker code
- **Expedited execution**: Uses `setExpedited()` ensuring immediate execution

### Missing Controls

- No URL scheme validation (http/https only)
- No domain allowlist
- No file size limit enforcement in worker
- No rate limiting per bot
- fileName is attacker-controlled (potential path traversal if not sanitized downstream)

## Vulnerability 2: Worker Queue Exhaustion DoS (MEDIUM)

**Location:** `p000.wuk` (WorkManager wrapper)

### Mechanism

The `wm-workers-limit` PmsKey controls maximum concurrent workers:
```java
// wuk.m24706g() - calculates effective limit
int limit = PmsKey.wmworkerslimit;  // default: 16
int offset = PmsKey.wmworkersoffset; // default: 2
int effective = limit - offset;      // default: 14
```

When the limit is reached (`f78373l >= m24706g()`):
1. New work requests are **NOT rejected** — they go to a **backlog queue** (database table `WorkerQueueItem`)
2. A `BacklogWorker` is scheduled with a delay (`wm-backlog-worker-backoff-delay-sec`, default: 10s)
3. The BacklogWorker polls every `wm-backlog-worker-check-delay-sec` (default: 5s) to check if slots freed up

### Attack Vector

A malicious bot can flood `DownloadFileFromWebAppWorker` requests:
- Each request is enqueued with `KEEP` policy (won't replace existing)
- Unique work names: `"workers:DownloadFileFromWebAppWorker/" + rgiVar` (unique per request)
- This fills the worker queue AND the backlog database
- Legitimate workers (uploads, message sync) get delayed or backlogged
- The BacklogWorker itself consumes a worker slot

### Impact

- Denial of service for legitimate background operations
- Battery drain from continuous BacklogWorker polling
- Database bloat from accumulated backlog entries

## Vulnerability 3: Retry Amplification (MEDIUM)

**Location:** `DownloadFileFromWebAppWorker` enqueue in `qik.java`

### Evidence

```java
fzc fzcVar = (fzc) ((fzc) ((fzc) new fzc(DownloadFileFromWebAppWorker.class)
    .setExpedited(e4d.f14826a))
    .setBackoffCriteria(fo0.f19154b, 10000L, TimeUnit.MILLISECONDS))
    .addTag("workers:DownloadFileFromWebAppWorker");
```

- **Backoff policy**: `fo0.f19154b` (LINEAR backoff, 10 second initial delay)
- **Result.retry (`yd9`)** is returned in the worker's `mo20595n()` method when download state is `vt5` with `f74895a = true`

### Attack Scenario

If a bot provides a URL that causes transient failures (e.g., HTTP 503), the worker will:
1. Fail → retry after 10s
2. Fail → retry after 20s
3. Continue with linear backoff

Each retry consumes a worker slot, network bandwidth, and battery. Combined with queue flooding, this amplifies the DoS.

## Vulnerability 4: Sensitive Data Access in Workers (LOW-MEDIUM)

### Workers with elevated access:

1. **DownloadAttachesWorker**: Accesses chat database, message content, file storage paths, and can write to gallery (`copyOriginalImageToGallery`, `copyVideoToGallery`)
2. **UploadFileAttachWorker**: Reads local files and uploads to server with auth tokens
3. **TaskMonitor$TaskMonitorWorker**: Has access to `taskRepository` and `workerService` — can execute arbitrary SDK tasks
4. **SampleUploadWorker**: Uploads performance traces (may contain sensitive timing data)

### SdkCoroutineWorker base class:
- All custom workers get a `local_account_id` from input data
- They access the full SDK dependency graph via `m20623j()` (returns `zag` accessor)
- The accessor provides access to 476+ service components

## PmsKeys Analysis

| Key | Default | Purpose |
|-----|---------|---------|
| `wm-workers-limit` | 16 | Max concurrent WorkManager workers |
| `wm-check-workers-count-interval-sec` | 10 | How often to recount active workers |
| `wm-backlog-worker-check-delay-sec` | 5 | BacklogWorker poll interval |
| `wm-backlog-worker-backoff-delay-sec` | 10 | Delay before BacklogWorker starts processing |
| `wm-workers-offset` | 2 | Reserved slots (effective limit = limit - offset) |

**Server-controlled**: These values come from PMS (remote config). A compromised PMS could set `wm-workers-limit` to 1, causing all work to be backlogged, or set check intervals to very high values preventing recovery.

## Recommendations

1. **URL validation for DownloadFileFromWebAppWorker**: Enforce https-only, domain allowlist, reject private/internal IPs
2. **File size limits**: Enforce maximum download size before starting transfer
3. **Per-bot rate limiting**: Limit download requests per bot per time window
4. **Queue depth limits**: Cap backlog size per work type to prevent unbounded growth
5. **fileName sanitization**: Ensure no path traversal characters in bot-provided filenames
6. **Exponential backoff**: Switch from LINEAR to EXPONENTIAL backoff with max retry count
7. **PmsKey validation**: Enforce minimum/maximum bounds on server-provided config values client-side
