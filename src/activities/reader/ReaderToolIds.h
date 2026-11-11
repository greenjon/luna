#pragma once

#include <I18n.h>

#include <array>
#include <cstdint>

#include "CrossPointSettings.h"

// The buttons the reader toolbar can show (Settings > Reader toolbar). The
// first three open the toolbar's panels; the rest run a reader menu action
// directly. Persisted as numbers in toolbarTool1..5: append only.
enum class ReaderTool : uint8_t {
  None = 0,
  Contents,
  Text,
  More,
  Bookmark,
  Bookmarks,
  GoToPercent,
  Dictionary,
  Clip,
  NightMode,
  Light,  // frontlight on/off ("Frontlight" is a macro)
  Orientation,
  AutoTurn,
  Screenshot,
  Sync,
  Home,
  Menu,  // open the reader menu (gestures only, not a toolbar button)
  Stats,
  Count
};

namespace reader_tools {

constexpr int kMaxSlots = 5;

inline constexpr std::array<uint8_t CrossPointSettings::*, kMaxSlots> SLOT_FIELDS = {
    &CrossPointSettings::toolbarTool1, &CrossPointSettings::toolbarTool2, &CrossPointSettings::toolbarTool3,
    &CrossPointSettings::toolbarTool4, &CrossPointSettings::toolbarTool5};

// Names in ReaderTool order, for the settings popup and the web page.
inline constexpr auto TOOL_NAMES = [] {
  std::array<StrId, static_cast<size_t>(ReaderTool::Count)> names{};
  names[static_cast<size_t>(ReaderTool::None)] = StrId::STR_NONE_OPT;
  names[static_cast<size_t>(ReaderTool::Contents)] = StrId::STR_TOOL_CONTENTS;
  names[static_cast<size_t>(ReaderTool::Text)] = StrId::STR_TOOL_TEXT;
  names[static_cast<size_t>(ReaderTool::More)] = StrId::STR_TOOL_MORE;
  names[static_cast<size_t>(ReaderTool::Bookmark)] = StrId::STR_TOGGLE_BOOKMARK;
  names[static_cast<size_t>(ReaderTool::Bookmarks)] = StrId::STR_BOOKMARKS;
  names[static_cast<size_t>(ReaderTool::GoToPercent)] = StrId::STR_GO_TO_PERCENT;
  names[static_cast<size_t>(ReaderTool::Dictionary)] = StrId::STR_LOOKUP;
  names[static_cast<size_t>(ReaderTool::Clip)] = StrId::STR_SAVE_CLIPPING;
  names[static_cast<size_t>(ReaderTool::NightMode)] = StrId::STR_NIGHT_MODE;
  names[static_cast<size_t>(ReaderTool::Light)] = StrId::STR_FRONTLIGHT;
  names[static_cast<size_t>(ReaderTool::Orientation)] = StrId::STR_ORIENTATION;
  names[static_cast<size_t>(ReaderTool::AutoTurn)] = StrId::STR_AUTO_TURN_PAGES_PER_MIN;
  names[static_cast<size_t>(ReaderTool::Screenshot)] = StrId::STR_SCREENSHOT_BUTTON;
  names[static_cast<size_t>(ReaderTool::Sync)] = StrId::STR_SYNC_PROGRESS;
  names[static_cast<size_t>(ReaderTool::Home)] = StrId::STR_GO_HOME_BUTTON;
  names[static_cast<size_t>(ReaderTool::Menu)] = StrId::STR_READER_MENU;
  names[static_cast<size_t>(ReaderTool::Stats)] = StrId::STR_READING_STATS;
  return names;
}();

// Reader swipe gestures (Taps and gestures > Swipe up / Swipe down): a
// ReaderTool value each, None = nothing.
inline constexpr std::array<uint8_t CrossPointSettings::*, 2> GESTURE_FIELDS = {&CrossPointSettings::swipeUpAction,
                                                                                &CrossPointSettings::swipeDownAction};

}  // namespace reader_tools
