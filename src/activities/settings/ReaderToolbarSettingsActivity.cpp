#include "ReaderToolbarSettingsActivity.h"

#include <Logging.h>
#include <Memory.h>

#include <memory>

#include "components/UITheme.h"

namespace fui = freeink::ui;

namespace {
constexpr StrId kSlotNames[] = {StrId::STR_TOOLBAR_BUTTON_1, StrId::STR_TOOLBAR_BUTTON_2, StrId::STR_TOOLBAR_BUTTON_3,
                                StrId::STR_TOOLBAR_BUTTON_4, StrId::STR_TOOLBAR_BUTTON_5};
constexpr int kToolCount = static_cast<int>(ReaderTool::Count);

int slotValue(const int slot) {
  const uint8_t value = SETTINGS.*reader_tools::SLOT_FIELDS[slot];
  return value < kToolCount ? value : 0;
}

// Same list chrome as the other settings sub-screens.
void listChrome(UiAppHost::UiScreen& screen) {
  const auto& metrics = UITheme::getInstance().getMetrics();
  screen.setContentMarginFromScreen(fui::Insets{static_cast<int16_t>(metrics.topPadding + metrics.headerHeight), 0,
                                                static_cast<int16_t>(metrics.buttonHintsHeight), 0});
  screen.spacer(static_cast<int16_t>(metrics.verticalSpacing));
}
}  // namespace

void ReaderToolbarSettingsActivity::activateIndex(const int index) {
  if (index < 0 || index >= listCount()) return;
  app.clearTapFlash();
  nav.selected = index;
  auto picker = makeUniqueNoThrow<ReaderToolPickerActivity>(renderer, mappedInput, index);
  if (!picker) {
    LOG_ERR("SET", "OOM: ReaderToolPickerActivity");
    return;
  }
  startActivityForResult(std::move(picker), [this](const ActivityResult&) { requestUpdate(); });
}

void ReaderToolbarSettingsActivity::buildScreen(UiScreen& screen) {
  listChrome(screen);
  for (int i = 0; i < listCount(); ++i) {
    rows[i].label = I18N.get(kSlotNames[i]);
    rows[i].actionValue = static_cast<int16_t>(i);
    rows[i].value = I18N.get(reader_tools::TOOL_NAMES[slotValue(i)]);
  }
  fui::ListProps props;
  props.items = rows;
  props.count = static_cast<uint16_t>(listCount());
  props.action = ACTION_ROW;
  props.inputMask = fui::InputTouch;
  syncListViewport(screen, props);
  screen.list(props);
}

void ReaderToolPickerActivity::onEnter() {
  UiListActivity::onEnter();
  nav.reset(slotValue(slot));
}

const char* ReaderToolPickerActivity::headerTitle() const { return I18N.get(kSlotNames[slot]); }

void ReaderToolPickerActivity::activateIndex(const int index) {
  if (index < 0 || index >= listCount()) return;
  SETTINGS.*reader_tools::SLOT_FIELDS[slot] = static_cast<uint8_t>(index);
  SETTINGS.saveToFile();
  app.clearTapFlash();
  finish();
}

void ReaderToolPickerActivity::buildScreen(UiScreen& screen) {
  listChrome(screen);
  const int current = slotValue(slot);
  for (int i = 0; i < listCount(); ++i) {
    rows[i].label = I18N.get(reader_tools::TOOL_NAMES[i]);
    rows[i].actionValue = static_cast<int16_t>(i);
    rows[i].value = i == current ? tr(STR_SELECTED) : nullptr;
  }
  fui::ListProps props;
  props.items = rows;
  props.count = static_cast<uint16_t>(listCount());
  props.action = ACTION_ROW;
  props.inputMask = fui::InputTouch;
  syncListViewport(screen, props);
  screen.list(props);
}
