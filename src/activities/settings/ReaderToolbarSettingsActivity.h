#pragma once

#include "activities/UiListActivity.h"
#include "activities/reader/ReaderToolIds.h"

// Settings > Toolbar buttons: which button sits in each of the reader
// toolbar's five slots, left to right. Empty slots are skipped, so the
// toolbar shows only the buttons that are set.
class ReaderToolbarSettingsActivity final : public UiListActivity {
 public:
  ReaderToolbarSettingsActivity(GfxRenderer& renderer, MappedInputManager& input)
      : UiListActivity("ReaderToolbarSettings", renderer, input) {}

 private:
  freeink::ui::ListItem rows[reader_tools::kMaxSlots]{};

  int listCount() const override { return reader_tools::kMaxSlots; }
  const char* headerTitle() const override { return tr(STR_TOOLBAR_BUTTONS); }
  void buildScreen(UiScreen& screen) override;
  void activateIndex(int index) override;
};

// The scrolling list of reader actions for one setting (a toolbar slot or a
// swipe gesture). Choosing one saves it and returns.
class ReaderToolPickerActivity final : public UiListActivity {
 public:
  // allowMenu: offer "Reader menu" (gestures; a toolbar button can't open
  // the toolbar it sits on).
  ReaderToolPickerActivity(GfxRenderer& renderer, MappedInputManager& input, uint8_t CrossPointSettings::* field,
                           StrId title, bool allowMenu);

  void onEnter() override;

 private:
  uint8_t CrossPointSettings::* const field;
  const StrId title;
  uint8_t values[static_cast<int>(ReaderTool::Count)]{};
  int valueCount = 0;
  freeink::ui::ListItem rows[static_cast<int>(ReaderTool::Count)]{};

  int listCount() const override { return valueCount; }
  const char* headerTitle() const override;
  void buildScreen(UiScreen& screen) override;
  void activateIndex(int index) override;
};
