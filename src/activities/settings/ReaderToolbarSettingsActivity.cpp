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
  auto picker = makeUniqueNoThrow<ReaderToolPickerActivity>(renderer, mappedInput, reader_tools::SLOT_FIELDS[index],
                                                            kSlotNames[index], /*allowMenu=*/false);
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

ReaderToolPickerActivity::ReaderToolPickerActivity(GfxRenderer& renderer, MappedInputManager& input,
                                                   uint8_t CrossPointSettings::* field, const StrId title,
                                                   const bool allowMenu)
    : UiListActivity("ReaderToolPicker", renderer, input), field(field), title(title) {
  for (int v = 0; v < kToolCount; ++v) {
    if (!allowMenu && v == static_cast<int>(ReaderTool::Menu)) continue;
    values[valueCount++] = static_cast<uint8_t>(v);
  }
}

void ReaderToolPickerActivity::onEnter() {
  UiListActivity::onEnter();
  const uint8_t current = SETTINGS.*field;
  int index = 0;
  for (int i = 0; i < valueCount; ++i) {
    if (values[i] == current) index = i;
  }
  nav.reset(index);
}

const char* ReaderToolPickerActivity::headerTitle() const { return I18N.get(title); }

void ReaderToolPickerActivity::activateIndex(const int index) {
  if (index < 0 || index >= listCount()) return;
  SETTINGS.*field = values[index];
  SETTINGS.saveToFile();
  app.clearTapFlash();
  finish();
}

void ReaderToolPickerActivity::buildScreen(UiScreen& screen) {
  listChrome(screen);
  const uint8_t current = SETTINGS.*field;
  for (int i = 0; i < listCount(); ++i) {
    rows[i].label = I18N.get(reader_tools::TOOL_NAMES[values[i]]);
    rows[i].actionValue = static_cast<int16_t>(i);
    rows[i].value = values[i] == current ? tr(STR_SELECTED) : nullptr;
  }
  fui::ListProps props;
  props.items = rows;
  props.count = static_cast<uint16_t>(listCount());
  props.action = ACTION_ROW;
  props.inputMask = fui::InputTouch;
  syncListViewport(screen, props);
  screen.list(props);
}
