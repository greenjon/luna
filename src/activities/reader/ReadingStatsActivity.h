#pragma once

#include <string>
#include <vector>

#include "activities/UiListActivity.h"

// Reading statistics (Luna): this book's time and pages, today / 7 days /
// 30 days / all time, the reading pace, and the recently read books. Opened
// from the reader (with the open session's not-yet-saved time added) or
// from Settings > Reader (no book).
class ReadingStatsActivity final : public UiListActivity {
 public:
  ReadingStatsActivity(GfxRenderer& renderer, MappedInputManager& input, std::string bookPath = {},
                       uint32_t extraSeconds = 0, uint32_t extraPages = 0)
      : UiListActivity("ReadingStats", renderer, input),
        bookPath(std::move(bookPath)),
        extraSeconds(extraSeconds),
        extraPages(extraPages) {}

  void onEnter() override;

 private:
  const std::string bookPath;
  const uint32_t extraSeconds;
  const uint32_t extraPages;
  std::vector<std::string> labels;
  std::vector<std::string> values;
  std::vector<freeink::ui::ListItem> rows;

  void addRow(std::string label, std::string value);
  int listCount() const override { return static_cast<int>(rows.size()); }
  const char* headerTitle() const override;
  void buildScreen(UiScreen& screen) override;
  void activateIndex(int) override {}
};
