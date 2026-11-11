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

// The scrolling list of buttons for one toolbar slot. Choosing one saves it
// and returns.
class ReaderToolPickerActivity final : public UiListActivity {
 public:
  ReaderToolPickerActivity(GfxRenderer& renderer, MappedInputManager& input, int slot)
      : UiListActivity("ReaderToolPicker", renderer, input), slot(slot) {}

  void onEnter() override;

 private:
  const int slot;
  freeink::ui::ListItem rows[static_cast<int>(ReaderTool::Count)]{};

  int listCount() const override { return static_cast<int>(ReaderTool::Count); }
  const char* headerTitle() const override;
  void buildScreen(UiScreen& screen) override;
  void activateIndex(int index) override;
};
