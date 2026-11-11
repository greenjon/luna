#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Reading statistics (Luna): time read and pages turned, per book and per
// day, kept in /.crosspoint/luna_stats.json. The reader adds each session's
// active reading time (ReaderSession: dwell per page, long pauses capped)
// when it closes or the device sleeps.
namespace reading_stats {

struct BookTotal {
  std::string path;
  std::string title;
  uint32_t seconds = 0;
  uint32_t pages = 0;
  int64_t lastRead = 0;  // epoch seconds, 0 = unknown
};

struct Summary {
  BookTotal book;  // the book asked about (zeros if none)
  uint32_t todaySeconds = 0;
  uint32_t weekSeconds = 0;   // today and the 6 days before
  uint32_t monthSeconds = 0;  // today and the 29 days before
  uint32_t totalSeconds = 0;
  uint32_t totalPages = 0;
  bool datesKnown = false;        // the clock is set, so the day totals mean something
  std::vector<BookTotal> recent;  // most recently read first
};

// Add one session. startEpoch 0 (clock unknown) still counts toward the book
// and all-time totals, not toward a day.
void record(const std::string& bookPath, const std::string& title, uint32_t seconds, uint32_t pages,
            int64_t startEpoch);

// Load the totals. bookPath may be empty. extraSeconds/extraPages: the open
// session not yet recorded, added to the book, today and the totals.
Summary load(const std::string& bookPath, uint32_t extraSeconds = 0, uint32_t extraPages = 0);

}  // namespace reading_stats
