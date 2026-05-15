# VULN-55: Message Search and Caching Information Leakage

## Summary

Max messenger's search functionality sends search queries to the server in plaintext, maintains local search history/cache extractable on rooted devices, and provides a global cross-chat search that implies server-side message indexing. No "secret chat" mode was found, meaning all messages are searchable server-side.

## 1. Search Infrastructure Overview

Key search-related packages found:
- `one.p011me.chatscreen.search` — In-chat message search (SearchMessageBottomWidget)
- `one.p011me.chats.search` — Global chats list search (ChatsListSearchScreen)
- `one.p011me.search.usecase` — Search use-case logic (InvalidSearchResultMarkerException)
- `one.p011me.stickerssearch` — Sticker search
- `one.p011me.contactlist` — Contact search with `searchQuery` field
- `ru.p027ok.messages.search` — Keyboard handler for search
- `ru.p027ok.tamtam.search` — Core search engine (DuplicateDetectException)

Core obfuscated classes:
- `wcg` — Server message search handler (tag: "wcg")
- `hdg` — Public/global search handler (tag: "hdg")
- `ecg` — Search orchestrator combining server + public search
- `tcg` — Server message search coroutine (sends query to API)
- `scg` — Actual API request builder for search
- `dcg` — Search result comparator
- `i83` — In-chat search state holder (query string, results list, pagination)
- `gl3` — `ChatsListSearchState` data class (holds searchQuery, searchResult, hasMoreMessages)

## 2. Search Queries Sent to Server — CONFIRMED

**Evidence from `scg.java` (API request builder):**
```java
kfiVar.m12326e(fx2Var, this);  // executes API call
fx2Var.m18553j("query", this.f63898Y);  // sends search query as "query" param
fx2Var.m18548e(this.f63899Z, "count");  // sends result count
fx2Var.m18553j("marker", str);  // pagination marker
```

**Evidence from `tcg.java` (search messages handler):**
```java
ct4.m4610E("wcg", "search messages done " + arrayList.size() + " for " + str3 + " / " + str2);
```

**Evidence from `edg.java` (public search):**
```java
"[search][chats] search public done: " + woeVar.f77703c.size() + " results for " + str2 + ", " + i2 + ", " + l2
```

**Privacy Impact:** The server receives the exact search query text, result count, and pagination markers. The server knows exactly what users are searching for in their messages.

## 3. Search Results Cached Locally — CONFIRMED

**Evidence:**
- `ChatsListSearchState` (`gl3.java`) stores: `searchQuery`, `searchResult` (list), `hasMoreMessages`, `scrollToTop`
- `i83` class maintains in-memory search state: query string (`f26675c`), results ArrayList (`f26678f`), pagination state
- `ClearRecentSearchBottomSheet` — UI to clear recent searches, confirming persistent storage
- String resources: `question_clear_search_history`, `question_clear_search_history_title`, `recent_search_header`
- `clearRecentSearch` operation logged in `lhd.java` (success/failure)
- `RecentContactModel` — stored recent contact search results

**Extractable data on rooted device:**
- Recent search queries (persisted locally)
- Recent search contacts/results
- Chat history with `chat-history-persist` PmsKey enabling local message persistence
- Search state in memory dumps

## 4. `pub-search-limit` PmsKey Analysis

**Location:** `PmsKey.java:889` — enum ordinal 291
**Usage:** `rtd.java:230` — `new jbg(this, PmsKey.f88822pubsearchlimit, i11)`

**Purpose:** Limits the number of results returned from **public/global search** (the "hdg" handler). This is a server-controlled parameter that restricts how many public chat/channel/contact results are returned when searching across all public entities.

The `jbg` class is a PMS (Parameter Management System) value holder, meaning this limit is remotely configurable by the server. It controls rate/volume of public search queries — likely to prevent scraping of public channels/contacts.

## 5. Secret Chats — NOT FOUND

**No evidence of "secret chat" or end-to-end encrypted chat mode was found in the codebase.** There are no references to:
- Secret chat creation
- E2E encryption for messages
- Self-destructing messages
- Client-side-only message storage

**Privacy Impact:** Since there is no secret chat mode, ALL messages are searchable server-side. The server maintains a full-text search index of all user messages. Every search query reveals user interests and message content to the server operator.

## 6. Global Search Across All Chats — CONFIRMED

**Evidence:**
- `ChatsListSearchScreen` — dedicated screen for searching across all chats
- Layout IDs: `chats_list_search_recycler_view`, `chats_list_search_toolbar`
- View types: `chats_search_global_chat_view_type`, `chats_search_global_contact_view_type`
- Layout IDs: `global_search_channel`, `global_search_contact`
- String: `chats_global_search`, `contacts_global_search_profile`
- `combineSearchAndBanners` method in ChatsListSearchScreen — combines search results with banners
- `onGlobalContactClick` handler for global contact search results

**Architecture:**
The `ecg` class orchestrates search by combining:
1. **Server message search** (`wcg` → `tcg` → `scg`) — searches messages on server
2. **Public search** (`hdg` → `edg`) — searches public channels/contacts

**Privacy Impact:** The server builds and maintains a full-text search index of ALL messages across ALL chats. When a user searches, the query is sent to the server which performs the search server-side. This means:
- Server has indexed all message content
- Server logs reveal what users search for
- No client-side-only search option exists

## 7. `webview-cache-enabled` PmsKey Analysis

**Location:** `PmsKey.java:537` — enum ordinal 171
**Usage:**
- `qp6.java:1009` — `m8380j(PmsKey.f88890webviewcacheenabled, false)` — returns boolean, default `false`
- `e2h.java:104` — Debug menu entry: `"Включить кэширование WebView"` (Russian: "Enable WebView caching")

**Purpose:** Controls whether WebView content is cached locally. When enabled, WebView data (HTML, CSS, JS, images, DOM storage) from mini-apps/bots/web content is persisted to disk.

**WebView features found with DOM storage enabled:**
- `FaqWebViewWidget` — FAQ pages with `setDomStorageEnabled(true)`
- `VideoWebViewScreen` — Video player WebView with `setDomStorageEnabled(true)`
- `ahk.java` — Another WebView with `setDomStorageEnabled(true)`

**Cached data includes:**
- Web page content (HTML/CSS/JS)
- DOM storage (localStorage, sessionStorage)
- Cookies from WebView sessions
- Images and media loaded in WebViews
- Bot/mini-app interaction data

**Privacy Impact:** When enabled, all WebView interaction data is cached on disk at standard Android WebView cache paths (`/data/data/ru.oneme.app/cache/WebView/`, `/data/data/ru.oneme.app/app_webview/`). On a rooted device, this reveals:
- Which bots/mini-apps the user interacted with
- Content viewed in FAQ/help pages
- Video content metadata
- Any sensitive data passed through WebView interfaces

## Risk Assessment

| Finding | Severity | Impact |
|---------|----------|--------|
| Search queries sent to server in plaintext | HIGH | Server operator sees all search terms |
| No secret/E2E chat mode | HIGH | All messages server-searchable |
| Global cross-chat server-side search | HIGH | Full message index on server |
| Recent search history cached locally | MEDIUM | Extractable on rooted device |
| WebView cache on disk (when enabled) | MEDIUM | Bot/web interaction data exposed |
| `pub-search-limit` remotely controlled | LOW | Server controls search behavior |

## Recommendations

1. Implement client-side local search as default (search cached messages without server)
2. Add end-to-end encrypted "secret chat" mode excluded from server search index
3. Clear search history on app close or provide auto-clear timer
4. Encrypt local search cache with device credentials
5. Disable WebView caching by default (currently disabled, but remotely toggleable)
6. Add transparency about server-side message indexing in privacy policy
