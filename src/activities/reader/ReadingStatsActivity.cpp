#include "ReadingStatsActivity.h"

#include <I18n.h>

#include <cstdio>

#include "components/UITheme.h"
#include "util/ReadingPace.h"
#include "util/ReadingStats.h"

namespace fui = freeink::ui;

namespace {
std::string duration(const uint32_t seconds) {
  char text[24];
  if (seconds < 60) {
    snprintf(text, sizeof(text), tr(STR_TIME_MINUTES_FORMAT), 0u);
  } else {
    reading_pace::formatDuration(text, sizeof(text), seconds);
  }
  return text;
}

std::string timeAndPages(const uint32_t seconds, const uint32_t pages) {
  char text[48];
  snprintf(text, sizeof(text), tr(STR_STATS_TIME_PAGES_FORMAT), duration(seconds).c_str(),
           static_cast<unsigned>(pages));
  return text;
}
}  // namespace

const char* ReadingStatsActivity::headerTitle() const { return tr(STR_READING_STATS); }

void ReadingStatsActivity::addRow(std::string label, std::string value) {
  labels.push_back(std::move(label));
  values.push_back(std::move(value));
}

void ReadingStatsActivity::onEnter() {
  UiListActivity::onEnter();
  const auto s = reading_stats::load(bookPath, extraSeconds, extraPages);
  labels.clear();
  values.clear();
  if (!bookPath.empty()) addRow(tr(STR_STATS_THIS_BOOK), timeAndPages(s.book.seconds, s.book.pages));
  if (s.datesKnown) {
    addRow(tr(STR_STATS_TODAY), duration(s.todaySeconds));
    addRow(tr(STR_STATS_WEEK), duration(s.weekSeconds));
    addRow(tr(STR_STATS_MONTH), duration(s.monthSeconds));
  }
  addRow(tr(STR_STATS_ALL_TIME), timeAndPages(s.totalSeconds, s.totalPages));
  if (reading_pace::known()) {
    char pace[32];
    snprintf(pace, sizeof(pace), tr(STR_STATS_PACE_FORMAT), static_cast<unsigned>(APP_STATE.secondsPerPage + 0.5f));
    addRow(tr(STR_STATS_PACE), pace);
  }
  for (const auto& book : s.recent) {
    if (book.seconds == 0) continue;
    addRow(book.title.empty() ? book.path : book.title, duration(book.seconds));
  }
  // ListItems point into the strings: build them once both vectors are final.
  rows.assign(labels.size(), fui::ListItem{});
  for (size_t i = 0; i < rows.size(); ++i) {
    rows[i].label = labels[i].c_str();
    rows[i].value = values[i].c_str();
    rows[i].actionValue = static_cast<int16_t>(i);
  }
}

void ReadingStatsActivity::buildScreen(UiScreen& screen) {
  const auto& metrics = UITheme::getInstance().getMetrics();
  screen.setContentMarginFromScreen(fui::Insets{static_cast<int16_t>(metrics.topPadding + metrics.headerHeight), 0,
                                                static_cast<int16_t>(metrics.buttonHintsHeight), 0});
  screen.spacer(static_cast<int16_t>(metrics.verticalSpacing));
  fui::ListProps props;
  props.items = rows.data();
  props.count = static_cast<uint16_t>(rows.size());
  props.action = ACTION_ROW;
  props.inputMask = fui::InputTouch;
  props.labelText = screen.theme().smallText;
  props.labelText.maxLines = 2;
  syncListViewport(screen, props);
  screen.list(props);
}
