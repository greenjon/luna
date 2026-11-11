#pragma once

#include <FreeInkUIIcon.h>
#include <I18n.h>

#include "EpubReaderMenuActivity.h"
#include "ReaderToolIds.h"
#include "components/icons/readerToolIcons.h"
#include "components/icons/readerToolbarIcons.h"

namespace reader_tools {

struct ToolInfo {
  StrId label;
  const freeink::Icon* icon;                  // nullptr for None
  EpubReaderMenuActivity::MenuAction action;  // direct tools only
};

inline const ToolInfo& info(const ReaderTool tool) {
  using MA = EpubReaderMenuActivity::MenuAction;
  static const ToolInfo kInfo[] = {
      {StrId::STR_NONE_OPT, nullptr, MA::SELECT_CHAPTER},
      {StrId::STR_TOOL_CONTENTS, &icon_reader_contents_24, MA::SELECT_CHAPTER},
      {StrId::STR_TOOL_TEXT, &icon_reader_text_24, MA::TEXT_SETTINGS},
      {StrId::STR_TOOL_MORE, &icon_reader_more_24, MA::SELECT_CHAPTER},
      {StrId::STR_TOGGLE_BOOKMARK, &icon_tool_bookmark_24, MA::TOGGLE_BOOKMARK},
      {StrId::STR_BOOKMARKS, &icon_tool_bookmarks_24, MA::BOOKMARKS},
      {StrId::STR_GO_TO_PERCENT, &icon_tool_percent_24, MA::GO_TO_PERCENT},
      {StrId::STR_LOOKUP, &icon_tool_dictionary_24, MA::DICTIONARY},
      {StrId::STR_SAVE_CLIPPING, &icon_tool_clip_24, MA::SAVE_CLIPPING},
      {StrId::STR_NIGHT_MODE, &icon_tool_night_24, MA::NIGHT_MODE},
      {StrId::STR_FRONTLIGHT, &icon_tool_light_24, MA::FRONTLIGHT},
      {StrId::STR_ORIENTATION, &icon_tool_rotate_24, MA::ROTATE_SCREEN},
      {StrId::STR_AUTO_TURN_PAGES_PER_MIN, &icon_tool_autoturn_24, MA::AUTO_PAGE_TURN},
      {StrId::STR_SCREENSHOT_BUTTON, &icon_tool_screenshot_24, MA::SCREENSHOT},
      {StrId::STR_SYNC_PROGRESS, &icon_tool_sync_24, MA::SYNC},
      {StrId::STR_GO_HOME_BUTTON, &icon_tool_home_24, MA::GO_HOME},
      {StrId::STR_READER_MENU, &icon_reader_more_24, MA::SELECT_CHAPTER},
      {StrId::STR_READING_STATS, &icon_tool_stats_24, MA::READING_STATS},
      {StrId::STR_SEARCH_IN_BOOK, &icon_tool_search_24, MA::SEARCH},
  };
  static_assert(sizeof(kInfo) / sizeof(kInfo[0]) == static_cast<size_t>(ReaderTool::Count), "tool table");
  const auto index = static_cast<size_t>(tool);
  return kInfo[index < static_cast<size_t>(ReaderTool::Count) ? index : 0];
}

inline bool isPanel(const ReaderTool tool) {
  return tool == ReaderTool::Contents || tool == ReaderTool::Text || tool == ReaderTool::More;
}

}  // namespace reader_tools
