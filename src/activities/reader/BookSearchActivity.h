#pragma once

#include <memory>
#include <string>
#include <vector>

#include "activities/UiListActivity.h"
#include "util/BookSearch.h"

class Epub;

// Search in book (Luna): asks for the text, then searches the book chapter
// by chapter (one per loop pass, so the progress line stays live and Back
// stops it), listing each match with its chapter and surrounding text.
// Choosing a match returns a ProgressChangeResult with its visible text
// offset; the reader jumps there like a bookmark.
class BookSearchActivity final : public UiListActivity {
 public:
  // lastQuery: the reader's remembered query, prefilled and updated here (the
  // reader outlives this screen).
  BookSearchActivity(GfxRenderer& renderer, MappedInputManager& input, std::shared_ptr<Epub> epub,
                     std::string* lastQuery);

  void onEnter() override;
  void loop() override;

 private:
  std::shared_ptr<Epub> epub;
  std::string* lastQuery;
  std::string query_;
  bool closeRequested = false;
  BookSearch search;
  bool searching = false;
  bool askedForQuery = false;
  int nextSpine = 0;
  std::string status;
  std::vector<std::string> labels;
  std::vector<std::string> subtitles;
  std::vector<freeink::ui::ListItem> rows;

  void askForQuery();
  void start(const std::string& text);
  void rebuildRows();
  int listCount() const override { return static_cast<int>(rows.size()); }
  const char* headerTitle() const override;
  void buildScreen(UiScreen& screen) override;
  void activateIndex(int index) override;
  void onBackButton() override;
};
