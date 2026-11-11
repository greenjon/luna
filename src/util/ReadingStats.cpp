#include "ReadingStats.h"

#include <ArduinoJson.h>
#include <PersistableStore.h>
#include <time.h>

#include <algorithm>
#include <cstdio>

namespace reading_stats {
namespace {

constexpr const char* kPath = "/.crosspoint/luna_stats.json";
constexpr size_t kMaxBooks = 40;  // most recent; older books drop off the list
constexpr size_t kMaxDays = 60;
// Dates before this mean the clock was never set.
constexpr int64_t kClockFloor = 1735689600;  // 2025-01-01

bool dayKey(const int64_t epoch, char* out, const size_t size) {
  if (epoch < kClockFloor) return false;
  const time_t t = static_cast<time_t>(epoch);
  struct tm local{};
  localtime_r(&t, &local);
  snprintf(out, size, "%04d-%02d-%02d", local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
  return true;
}

int64_t nowEpoch() { return static_cast<int64_t>(time(nullptr)); }

}  // namespace

void record(const std::string& bookPath, const std::string& title, const uint32_t seconds, const uint32_t pages,
            const int64_t startEpoch) {
  if (seconds == 0 && pages == 0) return;
  JsonDocument doc;
  PersistableStoreBase::readDocFromFile(kPath, doc);  // missing file = empty totals

  doc["total"] = (doc["total"] | 0u) + seconds;
  doc["pages"] = (doc["pages"] | 0u) + pages;

  char day[12];
  if (dayKey(startEpoch, day, sizeof(day))) {
    JsonArray days = doc["days"].is<JsonArray>() ? doc["days"].as<JsonArray>() : doc["days"].to<JsonArray>();
    bool found = false;
    for (JsonArray entry : days) {
      if (entry[0] == day) {
        entry[1] = (entry[1] | 0u) + seconds;
        found = true;
        break;
      }
    }
    if (!found) {
      JsonArray entry = days.add<JsonArray>();
      entry.add(day);
      entry.add(seconds);
    }
    while (days.size() > kMaxDays) days.remove(0);  // oldest first
  }

  JsonArray books = doc["books"].is<JsonArray>() ? doc["books"].as<JsonArray>() : doc["books"].to<JsonArray>();
  JsonObject book;
  for (size_t i = 0; i < books.size(); ++i) {
    if (books[i]["p"] == bookPath) {
      // Move it to the end (most recent).
      JsonDocument copy;
      copy.set(books[i]);
      books.remove(i);
      book = books.add<JsonObject>();
      book.set(copy.as<JsonObjectConst>());
      break;
    }
  }
  if (book.isNull()) {
    book = books.add<JsonObject>();
    book["p"] = bookPath;
  }
  book["t"] = title;
  book["s"] = (book["s"] | 0u) + seconds;
  book["n"] = (book["n"] | 0u) + pages;
  const int64_t last = startEpoch >= kClockFloor ? startEpoch : nowEpoch();
  if (last >= kClockFloor) book["l"] = last;
  while (books.size() > kMaxBooks) books.remove(0);

  PersistableStoreBase::writeDocToFile(kPath, doc);
}

Summary load(const std::string& bookPath, const uint32_t extraSeconds, const uint32_t extraPages) {
  Summary s;
  JsonDocument doc;
  PersistableStoreBase::readDocFromFile(kPath, doc);
  s.totalSeconds = (doc["total"] | 0u) + extraSeconds;
  s.totalPages = (doc["pages"] | 0u) + extraPages;

  const int64_t now = nowEpoch();
  char today[12];
  s.datesKnown = dayKey(now, today, sizeof(today));
  if (s.datesKnown) {
    // Day keys for the last 30 days, newest first.
    char keys[30][12];
    for (int d = 0; d < 30; ++d) dayKey(now - static_cast<int64_t>(d) * 86400, keys[d], sizeof(keys[d]));
    for (JsonArrayConst entry : doc["days"].as<JsonArrayConst>()) {
      const char* key = entry[0] | "";
      const uint32_t secs = entry[1] | 0u;
      for (int d = 0; d < 30; ++d) {
        if (strcmp(key, keys[d]) != 0) continue;
        if (d == 0) s.todaySeconds += secs;
        if (d < 7) s.weekSeconds += secs;
        s.monthSeconds += secs;
        break;
      }
    }
    s.todaySeconds += extraSeconds;
    s.weekSeconds += extraSeconds;
    s.monthSeconds += extraSeconds;
  }

  JsonArrayConst books = doc["books"].as<JsonArrayConst>();
  for (int i = static_cast<int>(books.size()) - 1; i >= 0; --i) {
    BookTotal b;
    b.path = books[i]["p"] | "";
    b.title = books[i]["t"] | "";
    b.seconds = books[i]["s"] | 0u;
    b.pages = books[i]["n"] | 0u;
    b.lastRead = books[i]["l"] | static_cast<int64_t>(0);
    if (!bookPath.empty() && b.path == bookPath) s.book = b;
    s.recent.push_back(std::move(b));
  }
  if (!bookPath.empty()) {
    if (s.book.path.empty()) s.book.path = bookPath;
    s.book.seconds += extraSeconds;
    s.book.pages += extraPages;
  }
  return s;
}

}  // namespace reading_stats
