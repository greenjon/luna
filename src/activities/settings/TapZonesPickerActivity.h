#pragma once

#include "activities/UiListActivity.h"

// Tap zones picker: every layout drawn as a small page (gray = back, white =
// forward, black = menu) from ReaderUtils::tapZoneAt, so the drawing and the
// reader can never disagree. Tapping a layout (or Select on it) saves it and
// returns.
class TapZonesPickerActivity final : public UiListActivity {
 public:
  TapZonesPickerActivity(GfxRenderer& renderer, MappedInputManager& mappedInput);

  void onEnter() override;

 private:
  int listCount() const override;
  void buildScreen(UiScreen& screen) override;
  void activateIndex(int index) override;
  const char* headerTitle() const override;
  void drawLayout(UiScreen& screen, uint8_t layout, freeink::ui::Rect page) const;
};
