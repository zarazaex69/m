# Protobuf Schema Reconstruction — Max Messenger (TamTam)

**Source:** `ru/p027ok/tamtam/nano/Protos.java` (12352 lines, 69 message types)
**Library:** Google Protobuf Nano (lightweight Android variant)

## Top-Level Messages

| Message | Line | Purpose |
|---------|------|---------|
| Attaches | 21 | Container for message attachments |
| CallHistoryState | 5758 | Call history tracking |
| Chat | 6022 | Chat/conversation metadata |
| Contact | 10132 | User profile/contact info |
| LogEvent | 11098 | Analytics/telemetry |
| LongList | 11217 | Generic int64 list |
| MessageElement | 11342 | Rich text formatting |
| MessageElements | 11564 | Array of MessageElement |
| MessagePreview | 11676 | Message preview snippet |
| MessageReactionWithCount | 11754 | Reaction counter |
| MessageReactions | 11834 | Reactions container |
| PendingUploadData | 11970 | Upload queue state |
| ReactionData | 12047 | Reaction payload |
| RestrictionsInfo | 12128 | User/chat restrictions |
| SelfProfile | 12199 | Current user profile |

## Security-Relevant Messages

### Attach.Location (Privacy-Critical)
```proto
message Location {
  double latitude = 1;
  double longitude = 2;
  float zoom = 3;
  int64 ttl = 4;
  int64 livePeriod = 5;
  repeated LocationInfo track = 6;  // Full movement history
  string deviceId = 7;             // DEVICE FINGERPRINT
  LocationInfo lastLocation = 8;
  double altitude = 9;
  float accuracy = 10;
  float bearing = 11;
  float speed = 12;
  bool corrupted = 13;
  int64 startTime = 14;
  int64 endTime = 15;
}

message LocationInfo {
  double latitude = 1;
  double longitude = 2;
  int64 time = 3;
  double altitude = 4;
  float accuracy = 5;
  float bearing = 6;
  float speed = 7;
}
```

**SECURITY NOTE:** Location shares deviceId + full GPS track with timestamps. Live location exposes continuous movement data.

### Attach.Call
```proto
message Call {
  enum CallType { UNKNOWN_CALL_TYPE = 0; VIDEO = 1; AUDIO = 2; }
  enum HangupType { UNKNOWN_HANGUP_TYPE = 0; HANGUP = 1; CANCELED = 2; REJECTED = 3; MISSED = 4; }
  string conversationId = 1;
  CallType callType = 2;
  HangupType hangupType = 3;
  int32 duration = 4;
  repeated int64 contactIds = 5;
  int64 durationLong = 6;
  string joinLink = 7;
}
```

### Attach.File
```proto
message File {
  int64 fileId = 1;
  int64 size = 2;
  string name = 3;
  Attach preview = 4;
  string token = 5;  // Server-side access token
}
```

### Chat (Conversation Metadata)
```proto
message Chat {
  enum Type { DIALOG = 0; CHAT = 1; CHANNEL = 2; GROUP_CHAT = 3; COMMENTS = 4; }
  enum Status { ACTIVE = 0; LEFT = 1; LEAVING = 2; REMOVED = 3; REMOVING = 4; CLOSED = 5; HIDDEN = 6; BLOCKED = 7; }
  enum AccessType { PUBLIC = 0; PRIVATE = 1; }

  int64 cid = ?;
  int64 serverId = ?;
  int32 type = ?;
  int32 status = ?;
  int32 accessType = ?;
  string title = ?;
  string description = ?;
  string link = ?;
  int64 owner = ?;
  repeated int64 admins = ?;
  map<int64, AdminParticipant> adminParticipants = ?;
  map<int64, int64> participants = ?;
  int32 participantsCount = ?;
  int32 blockedParticipantsCount = ?;
  int32 restrictions = ?;
  int32 messagesTtlSec = ?;
  int64 pinnedMessageId = ?;
  ChannelInfo channelInfo = ?;
  GroupChatInfo groupChatInfo = ?;
  ChatSettings chatSettings = ?;
  ChatOptions chatOptions = ?;
  VideoConversation videoConversation = ?;
  map<int64, int64> liveLocationMessageIds = ?;
}

message AdminParticipant {
  int64 id = 1;
  int32 permissions = 2;
  int64 inviterId = 3;
  string alias = 4;
}

message ChannelInfo {
  int32 membersCount = 1;
  string description = 2;
  repeated int64 admins = 3;
  bool signAdmin = 4;
}

message GroupChatInfo {
  enum MessagingPermissions { DISABLED = 0; MEMBERS = 1; ALL = 2; }
  int64 groupId = ?;
  string name = ?;
  bool isMember = ?;
  bool isModerator = ?;
  int32 messagingPermissions = ?;
  GroupOptions groupOptions = ?;
}
```

### Contact (User Profile — Data Sent to Server)
```proto
message Contact {
  enum AccountStatus { ACTIVE = 0; BLOCKED = 1; DELETED = 2; }
  enum Gender { UNKNOWN = 0; MALE = 1; FEMALE = 2; }
  enum Type { USER_LIST = 0; BOT = 1; EXTERNAL = 1; SERVICE_ACCOUNT = 2; HAS_WEBAPP = 3; }

  int64 serverId = ?;
  int64 serverPhone = ?;       // PHONE NUMBER
  string birthday = ?;         // DOB
  string country = ?;
  string description = ?;
  int32 gender = ?;
  string link = ?;
  string baseUrl = ?;          // Avatar URL
  string deviceName = ?;       // DEVICE INFO
  string deviceAvatarUrl = ?;
  int64 registrationTime = ?;
  int64 lastSyncTime = ?;
  int32 status = ?;
  int32 accountStatus = ?;
  repeated ContactName names = ?;
  repeated int64 organizationIds = ?;
  repeated int32 options = ?;
  repeated int32 profileOptions = ?;

  message ContactName {
    enum Type { UNKNOWN = 0; CUSTOM = 1; DEVICE = 2; ONEME = 3; }
    string name = 1;
    int32 type = 2;
    string lastName = 3;
  }
}
```

**SECURITY NOTE:** Contact exposes phone number, birthday, device name, country, organization IDs — significant PII.

### SelfProfile
```proto
message SelfProfile {
  int64 serverId = 1;
  map<int32, RestrictionsInfo> restrictions = 2;
  repeated int32 profileOptions = 3;
}

message RestrictionsInfo {
  int64 expiration = 1;
}
```

### LogEvent (Telemetry)
```proto
message LogEvent {
  int64 time = 1;
  string type = 2;
  string event = 3;
  bytes params = 4;
  int64 userId = 5;
  int64 sessionId = 6;
}
```

**SECURITY NOTE:** Sends userId + sessionId with every analytics event. Params is opaque bytes (could contain anything).

## Attach Message Type (Media Attachments)

```proto
message Attaches {
  repeated Attach attach = ?;
  Attach.InlineKeyboard keyboard = ?;
  Attach.ReplyKeyboard replyKeyboard = ?;
  Attach.SendAction sendAction = ?;
}

message Attach {
  enum Type {
    UNKNOWN = 0; CONTROL = 1; PHOTO = 2; VIDEO = 3; AUDIO = 4;
    STICKER = 5; SHARE = 6; APP = 7; CALL = 8; MUSIC = 9;
    FILE = 10; CONTACT = 11; PRESENT = 12; INLINE_KEYBOARD = 13;
    LOCATION = 14; DAILY_MEDIA = 15; WIDGET = 16; POLL = 17;
  }
  enum Status { NOT_LOADED = 0; CANCELLED = 1; LOADED = 2; ERROR = 3; LOADING = 4; }

  int32 type = ?;
  int32 status = ?;
  string localId = ?;
  string localPath = ?;
  bool sensitive = ?;
  bool sensitiveContentUnlocked = ?;
  bool isDeleted = ?;
  bool isProcessingOnServer = ?;
  int64 totalBytes = ?;
  int64 bytesDownloaded = ?;

  // Union-style payload (one of):
  App app = ?;
  Audio audio = ?;
  Call call = ?;
  Contact contact = ?;
  Control control = ?;
  File file = ?;
  InlineKeyboard inlineKeyboard = ?;
  Location location = ?;
  Photo photo = ?;
  Poll poll = ?;
  Present present = ?;
  Share share = ?;
  Sticker sticker = ?;
  Video video = ?;
  Widget widget = ?;
}
```

### Attach.Photo
```proto
message Photo {
  string photoUrl = 1;
  int32 width = 2;
  int32 height = 3;
  bool gif = 4;
  bytes previewData = 5;
  string photoToken = 6;
  int64 photoId = 7;
  string mp4Url = 8;
  string baseUrl = 10;
  string previewUrl = 12;
  bytes thumbhashData = 13;
}
```

### Attach.Audio
```proto
message Audio {
  enum TranscriptionStatus { UNKNOWN = 0; PROCESSING = 1; SUCCESS = 2; FAILED = 3; NOT_SUPPORTED = 4; MEDIA_NOT_READY = 5; }
  int64 audioId = 1;
  string url = 2;
  int64 duration = 3;
  bytes wave = 4;
  string token = 5;
  int64 startTime = 6;
  int64 lastStartTimeUpdateTimestamp = 7;
  string transcription = 8;
  TranscriptionStatus transcriptionStatus = 9;
}
```

### Attach.Video
```proto
message Video {
  int64 videoId = ?;
  string token = ?;
  string thumbnail = ?;
  string embedUrl = ?;
  string externalUrl = ?;
  string externalSiteName = ?;
  int32 width = ?;
  int32 height = ?;
  int32 duration = ?;
  int32 videoType = ?;
  bool live = ?;
  bytes previewData = ?;
  bytes thumbhashData = ?;
  bytes wave = ?;
  string transcription = ?;
  int32 transcriptionStatus = ?;
  int64 startTime = ?;
  ConvertOptions convertOptions = ?;
  VideoCollage videoCollage = ?;
}
```

### Attach.Contact
```proto
message Contact {
  string vcfBody = 1;      // Full vCard data
  int64 contactId = 2;
  string name = 3;
  string phone = 4;        // Phone number in attachment
  string photoUrl = 5;
  string localPhotoUrl = 6;
  string firstName = 7;
  string lastName = 8;
}
```

### Attach.Control (System Messages)
```proto
message Control {
  enum Event {
    UNKNOWN = 0; NEW = 1; ADD = 2; REMOVE = 3; LEAVE = 4;
    TITLE = 5; ICON = 6; HELLO = 7; SYSTEM = 8;
    JOIN_BY_LINK = 9; PIN = 10; BOT_STARTED = 11;
  }
  enum ChatType { UNKNOWN_TYPE = 0; CHAT_TYPE = 1; CHANNEL_TYPE = 2; GROUP_CHAT_TYPE = 3; DIALOG = 4; }

  Event event = 1;
  int64 userId = 2;
  repeated int64 userIds = 3;
  string title = 4;
  string iconToken = 5;
  string url = 6;
  Rect crop = 7;
  string message = 8;
  string shortMessage = 9;
  bool showHistory = 10;
  ChatType chatType = 11;
  string fullUrl = 12;
  int64 pinnedMessageId = 13;
  int64 pinnedMessageServerId = 14;
  string startPayload = 16;
}
```

### Attach.Share
```proto
message Share {
  int64 shareId = ?;
  string token = ?;
  string url = ?;
  string title = ?;
  string description = ?;
  string host = ?;
  Photo image = ?;
  Attach media = ?;
  bool deleted = ?;
  bool contentLevel = ?;
}
```

## Admin/Moderation Types

1. **Chat.AdminParticipant** — Admin with permissions bitmask and alias
2. **Chat.ChannelInfo** — Channel admin list, signAdmin flag
3. **Chat.GroupChatInfo** — `isModerator` flag, `messagingPermissions` enum
4. **Chat.restrictions** — Integer bitmask for chat-level restrictions
5. **Chat.blockedParticipantsCount** — Count of blocked users
6. **SelfProfile.restrictions** — Map of restriction type → expiration
7. **Contact.accountStatus** — ACTIVE/BLOCKED/DELETED states
8. **Control.REMOVE** event — User removal from chat

**No dedicated "Moderation" or "Ban" message type found.** Moderation is handled via:
- Permission bitmasks in AdminParticipant
- The `restrictions` field (integer flags)
- Control events (ADD/REMOVE/LEAVE)

## Authentication

**No Login/Auth/Session/Register protobuf messages found in Protos.java.** Authentication likely uses a different transport (HTTP REST/JSON) rather than protobuf serialization. The `LogEvent.sessionId` field confirms sessions exist but are managed outside this schema.

## Security Summary

| Risk | Detail |
|------|--------|
| Location tracking | Full GPS track with deviceId, speed, bearing, timestamps |
| PII exposure | Phone number, birthday, country, device name in Contact |
| Session leakage | userId + sessionId sent in every LogEvent |
| File tokens | Server tokens for file access in File/Photo/Audio messages |
| No auth in proto | Auth handled separately — proto assumes authenticated context |
| Admin permissions | Simple integer bitmask — potential for privilege escalation if not server-validated |
