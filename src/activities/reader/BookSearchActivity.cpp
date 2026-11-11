#include "BookSearchActivity.h"

#include <Epub.h>
#include <I18n.h>

#include <cstdio>

#include "activities/util/KeyboardEntryActivity.h"
#include "components/UITheme.h"

namespace fui = freeink::ui;

BookSearchActivity::BookSearchActivity(GfxRenderer& renderer, MappedInputManager& input, std::shared_ptr<Epub> epub,
                                       std::string* lastQuery)
    : UiListActivity("BookSearch", renderer, input),
      epub(std::move(epub)),
      lastQuery(lastQuery),
      query_(lastQuery ? *lastQuery : std::string()) {}

const char* BookSearchActivity::headerTitle() const { return tr(STR_SEARCH_IN_BOOK); }

void BookSearchActivity::onEnter() {
  UiListActivity::onEnter();
  if (!askedForQuery) askForQuery();
}

void BookSearchActivity::askForQuery() {
  askedForQuery = true;
  startActivityForResult(
      std::make_unique<KeyboardEntryActivity>(renderer, mappedInput, tr(STR_SEARCH_IN_BOOK), query_, 64),
      [this](const ActivityResult& result) {
        if (result.isCancelled) {
          // Nothing searched yet: leave on the next loop pass.
          if (!searching && rows.empty()) closeRequested = true;
          return;
        }
        start(std::get<KeyboardResult>(result.data).text);
      });
}

void BookSearchActivity::start(const std::string& text) {
  RenderLock lock(*this);  // the render task reads the rows and the query
  query_ = text;
  if (lastQuery) *lastQuery = text;
  rows.clear();
  labels.clear();
  subtitles.clear();
  nav.reset();
  if (!epub || !search.begin(text)) {
    status = tr(STR_SEARCH_NO_RESULTS);
    searching = false;
    requestUpdate();
    return;
  }
  searching = true;
  nextSpine = 0;
  requestUpdate();
}

void BookSearchActivity::loop() {
  if (closeRequested) {
    closeRequested = false;
    onBackButton();
    return;
  }
  if (searching) {
    // Back stops the search and keeps the matches found so far.
    if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
      searching = false;
      rebuildRows();
      requestUpdate();
      return;
    }
    const int count = epub->getSpineItemsCount();
    if (nextSpine < count && !search.full()) {
      search.searchSpine(*epub, nextSpine++);
    }
    if (nextSpine >= count || search.full()) searching = false;
    rebuildRows();
    requestUpdate();
    return;
  }
  UiListActivity::loop();
}

void BookSearchActivity::rebuildRows() {
  // The render task reads rows (and the strings they point into) while building.
  RenderLock lock(*this);
  const auto& hits = search.hits();
  char line[64];
  if (searching) {
    snprintf(line, sizeof(line), tr(STR_SEARCH_PROGRESS_FORMAT), nextSpine, epub->getSpineItemsCount(),
             static_cast<unsigned>(hits.size()));
  } else if (hits.empty()) {
    snprintf(line, sizeof(line), "%s", tr(STR_SEARCH_NO_RESULTS));
  } else {
    snprintf(line, sizeof(line), tr(STR_SEARCH_RESULTS_FORMAT), static_cast<unsigned>(hits.size()),
             search.full() ? "+" : "");
  }
  status = line;
  // Only rows that are new since the last call are built; strings stay put.
  for (size_t i = labels.size(); i < hits.size(); ++i) {
    labels.push_back("..." + hits[i].snippet + "...");
    const int toc = epub->getTocIndexForSpineIndex(hits[i].spineIndex);
    subtitles.push_back(toc >= 0 ? epub->getTocItem(toc).title : std::string());
  }
  rows.assign(labels.size(), fui::ListItem{});
  for (size_t i = 0; i < rows.size(); ++i) {
    rows[i].label = labels[i].c_str();
    rows[i].subtitle = subtitles[i].empty() ? nullptr : subtitles[i].c_str();
    rows[i].actionValue = static_cast<int16_t>(i);
  }
}

void BookSearchActivity::activateIndex(const int index) {
  const auto& hits = search.hits();
  if (index < 0 || index >= static_cast<int>(hits.size())) return;
  app.clearTapFlash();
  ProgressChangeResult jump;
  jump.spineIndex = hits[index].spineIndex;
  jump.hasVisibleTextOffset = true;
  jump.visibleTextOffset = hits[index].visibleOffset;
  setResult(std::move(jump));
  finish();
}

void BookSearchActivity::onBackButton() {
  ActivityResult cancelled;
  cancelled.isCancelled = true;
  setResult(std::move(cancelled));
  finish();
}

void BookSearchActivity::buildScreen(UiScreen& screen) {
  const auto& metrics = UITheme::getInstance().getMetrics();
  const auto& theme = screen.theme();
  screen.setContentMarginFromScreen(fui::Insets{static_cast<int16_t>(metrics.topPadding + metrics.headerHeight), 0,
                                                static_cast<int16_t>(metrics.buttonHintsHeight), 0});
  screen.spacer(static_cast<int16_t>(metrics.verticalSpacing));
  // Status line: the query and the progress or result count.
  {
    fui::TextStyle style = theme.smallText;
    style.align = fui::TextAlign::Center;
    style.maxLines = 2;
    const int16_t lineH = screen.target().lineHeight(style.font);
    const fui::Rect line = screen.takeTop(static_cast<int16_t>(lineH * 2), theme.spaceSm)
                               .inset(fui::Insets{0, theme.spaceLg, 0, theme.spaceLg});
    const std::string text = "\"" + query_ + "\"  " + status;
    screen.target().text(line, text.c_str(), style);
  }
  if (rows.empty()) return;
  fui::ListProps props;
  props.items = rows.data();
  props.count = static_cast<uint16_t>(rows.size());
  props.action = ACTION_ROW;
  props.inputMask = fui::InputTouch;
  props.labelText = theme.smallText;
  props.labelText.maxLines = 3;
  syncListViewport(screen, props);
  screen.list(props);
}
