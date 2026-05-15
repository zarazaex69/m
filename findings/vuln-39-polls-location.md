# VULN-39: Polls & Location Sharing Privacy/Security Issues

## Executive Summary

Max messenger's polls feature exposes voter identities by design (non-anonymous polls with a "show voters" list). The location sharing system accepts server-initiated push requests that trigger location acquisition with minimal client-side consent gating. Live location has no visible client-enforced timeout — relying on server-side TTL only.

---

## POLLS ANALYSIS

### Source Files
- `one/p011me/polls/screens/create/PollCreateScreen.java` — Poll creation UI
- `one/p011me/polls/screens/result/PollResultScreen.java` — Poll results display
- `one/p011me/polls/screens/result/voterslist/PollAnswerVotersListScreen.java` — Voter list per answer
- `ru/p027ok/tamtam/nano/Protos.java` — Poll proto definition (lines 2038–2530)
- `p000/ytd.java` — Poll settings model
- `p000/txd.java` — PollModel (title, answers, canRevote)

### Proto Structure (Protos.java)

```java
public static final class Poll extends rva {
    public long pollId;
    public String title;
    public Answer[] answers;
    public int settings;       // Bitmask for poll configuration
    public State state;
    public int version;
}

public static final class AnswerStats extends rva {
    public long userId;        // ← VOTER USER ID EXPOSED
    public long timestamp;     // ← VOTE TIMESTAMP EXPOSED
}

public static final class Result extends rva {
    public int answerId;
    public int voteCount;
    public AnswerStats[] votes;  // ← Array of userId+timestamp per answer
    public int rate;
    public int options;
}

public static final class State extends rva {
    public int total;
    public Result[] result;
    public long[] voterPreviewIds;  // ← USER IDS OF VOTERS PREVIEWED
}
```

### Finding 1: Poll Votes Are De-Anonymizable (HIGH)

**Evidence:**
- `AnswerStats` contains `userId` and `timestamp` for each vote
- `State.voterPreviewIds` exposes an array of user IDs who voted
- `PollAnswerVotersListScreen` is a dedicated screen showing who voted for each answer
- Resource ID: `oneme_poll_create__show_all_voters_button_title` — explicit "show all voters" button
- Navigation route: `:polls/result/voters?chat_id=&message_id=&poll_id=&answer_id=`

**Impact:** Any chat member can see exactly who voted for which option. The `settings` field (int bitmask) controls poll behavior, but there is no evidence of an "anonymous" mode that hides voter identity from the server response. The proto always includes `userId` in `AnswerStats`.

### Finding 2: Poll Results Client-Side Manipulation (MEDIUM)

**Evidence:**
- Poll settings are stored as an integer bitmask (`poll.settings`) 
- The `settings` value is sent from client during creation (`poll.settings = ytdVar.m26159d()`)
- `PollModel` (txd.java) has `canRevote` boolean — revote capability is a client-visible setting
- No evidence of server-side validation of the settings bitmask in client code
- The `version` field suggests optimistic concurrency but vote counts come from server

**Risk:** A modified client could potentially:
- Create polls with manipulated settings bitmask
- The actual vote counting appears server-side (voteCount in Result), limiting direct manipulation
- However, the `settings` field interpretation is client-controlled

### Finding 3: Polls Enable User Enumeration (HIGH)

**Evidence:**
- `voterPreviewIds` in `State` returns user IDs of voters
- `PollAnswerVotersListScreen` uses `EndlessRecyclerView2` with pagination (threshold=20) — can enumerate all voters
- Route `PollAnswerVotersLoaderFactory` (registered as factory ID 637) loads voter lists from server
- Voting in a poll reveals your user ID to all chat members who view results

**Attack scenario:** Create a poll in a group chat → all members who vote are enumerated with their user IDs, even if the group hides its member list. This is a membership oracle.

### PmsKey Configuration
- `render-polls` — controls poll rendering
- `polls-in-p2p-chats` — polls allowed in 1:1 chats
- `polls-in-p2g-chats` — polls allowed in group chats  
- `polls-in-channels` — polls allowed in channels
- `poll-ttl` — server-controlled poll time-to-live

---

## LOCATION SHARING ANALYSIS

### Source Files
- `ru/p027ok/tamtam/nano/Tasks.java:4850` — `LocationRequest` proto
- `ru/p027ok/tamtam/nano/Tasks.java:4938` — `LocationStop` proto
- `p000/cei.java:251` — Push handler for LocationRequest
- `p000/hqg.java` — LocationRequest proto parser
- `p000/iqg.java` — LocationRequest task implementation
- `p000/rqg.java` — Send location message task
- `p000/eqe.java:354` — `m6664f()` — push connection handler
- `p000/m1c.java:255` — Live location notification channel
- `ru/p027ok/tamtam/android/prefs/PmsKey.java:311` — `send-location-enabled`

### LocationRequest Proto (Tasks.java)

```java
public static final class LocationRequest extends rva {
    public long requestId;
    public long messageId;
    public boolean liveLocation;  // Flag for live vs one-time
}

public static final class LocationStop extends rva {
    public long requestId;
    public long chatId;
    public long messageId;
}
```

### Finding 4: Server Can Request Location via Push Without Explicit Consent (CRITICAL)

**Evidence from cei.java:251-259:**
```java
} else if (zm0.m27177c(map.get("type"), "LocationRequest")) {
    // No user consent dialog shown
    // No permission check beyond app-level
    eeiVar5.m6393d().m7418a().m6664f(false, !((oe4) r0.f16562a.getValue()).m15842c());
    ct4.m4610E(fei.class.getName(), "onLocationRequestPush");
}
```

**What happens on LocationRequest push:**
1. Push arrives with `type=LocationRequest`
2. `m6664f(false, forceConnection)` is called directly
3. This forces a network connection to the server
4. The `iqg` task then acquires location and sends it back

**In iqg.java (the task executor):**
- `mo1763w()` processes the location request
- Calls location provider APIs
- Sets a 60-second timeout for non-live requests
- For `liveLocation=true`, NO timeout is set (`if (this.f28455d) return;`)

**No user-facing consent dialog is shown when the push arrives.** The only gate is whether the app has Android location permission already granted.

### Finding 5: Live Location Has No Client-Side Timeout Enforcement (HIGH)

**Evidence from iqg.java:**
```java
if (this.f28455d) {  // if liveLocation == true
    return;          // Skip timeout setup entirely
}
// Only non-live gets a 60-second timeout
```

**Evidence from rqg.java:**
```java
long millis = TimeUnit.SECONDS.toMillis(0L) + jM25806j;  // Duration = 0 seconds added
```

- `LocationStop` task exists but must be explicitly triggered
- No automatic expiry timer on the client for live location
- `liveLocationMessageIds` map in Chat proto tracks active live locations per chat
- The `poll-ttl` PmsKey pattern suggests server-side TTL, but client has no enforcement
- Notification channel `ru.oneme.app.liveLocation` exists but is just for display

### Finding 6: Location History Stored on Server (MEDIUM)

**Evidence:**
- `liveLocationMessageIds` (Map<Long, Long>) in Chat proto — maps user IDs to message IDs containing location
- Location is sent as a message attachment (via `sendLocationJob` in uf3.java)
- Messages persist on server (standard message lifecycle)
- No evidence of automatic location message deletion after live sharing ends
- `LocationStop` only stops updates — does not delete historical location messages

### Finding 7: `send-location-enabled` PmsKey (INFO)

**Location:** `PmsKey.java:311`
```java
public static final PmsKey f88842sendlocationenabled = new PmsKey("send-location-enabled", 95);
```

- Server-controlled feature flag (ordinal 95)
- Can enable/disable location sending globally
- This is a kill switch, not a privacy control — when enabled, all location features are active
- No per-user or per-chat granularity visible

---

## Risk Summary

| # | Finding | Severity | Category |
|---|---------|----------|----------|
| 1 | Poll votes expose userId+timestamp to all members | HIGH | Privacy |
| 2 | Poll settings bitmask client-controlled | MEDIUM | Integrity |
| 3 | Polls enable user enumeration / membership oracle | HIGH | Privacy |
| 4 | Server push triggers location acquisition without consent UI | CRITICAL | Privacy/Surveillance |
| 5 | Live location has no client-side timeout | HIGH | Privacy |
| 6 | Location history persists as messages on server | MEDIUM | Data Retention |
| 7 | send-location-enabled is server kill switch only | INFO | Configuration |

## Recommendations

1. **Polls:** Implement truly anonymous polls where server never returns `userId` in `AnswerStats` — use a separate anonymized counting mechanism
2. **Polls:** Don't expose `voterPreviewIds` for anonymous polls; restrict voter list API to poll creator only
3. **Location:** Show explicit user consent dialog when `LocationRequest` push is received before acquiring location
4. **Location:** Implement client-side maximum duration for live location (e.g., 8 hours) with automatic `LocationStop`
5. **Location:** Auto-delete location messages after live sharing session ends
6. **Location:** Allow users to disable server-initiated location requests entirely in privacy settings
