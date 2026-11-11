#include "TapZonesPickerActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include <algorithm>

#include "CrossPointSettings.h"
#include "MappedInputManager.h"
#include "TapZoneLayoutNames.h"
#include "activities/reader/ReaderUtils.h"
#include "components/UITheme.h"

namespace fui = freeink::ui;

namespace {
constexpr int kLayoutCount = CrossPointSettings::TAP_ZONE_LAYOUT_COUNT;
// Thirds and halves both fall on sixths, so a 6x6 sample draws every layout.
constexpr int kCells = 6;
constexpr int16_t kTilePad = 8;  // air between the tile outline and the page drawing
// Names wrap to three lines, so none is cut short at four columns.
constexpr int kNameLines = 3;

// Position of a layout in the picker (TAP_ZONE_LAYOUT_ORDER).
int positionOf(const uint8_t layout) {
  for (int i = 0; i < kLayoutCount; ++i) {
    if (TAP_ZONE_LAYOUT_ORDER[i] == layout) return i;
  }
  return 0;
}
}  // namespace

TapZonesPickerActivity::TapZonesPickerActivity(GfxRenderer& renderer, MappedInputManager& mappedInput)
    : UiListActivity("TapZonesPicker", renderer, mappedInput) {}

void TapZonesPickerActivity::onEnter() {
  UiListActivity::onEnter();
  nav.reset(positionOf(SETTINGS.tapZoneLayout));
  // A clean first refresh: the gray drawings otherwise keep a ghost of the
  // previous screen's text after a fast refresh.
  renderer.promoteNextRefresh(HalDisplay::HALF_REFRESH);
}

int TapZonesPickerActivity::listCount() const { return kLayoutCount; }

const char* TapZonesPickerActivity::headerTitle() const { return tr(STR_TAP_ZONES); }

void TapZonesPickerActivity::activateIndex(const int index) {
  if (index < 0 || index >= kLayoutCount) return;
  SETTINGS.tapZoneLayout = TAP_ZONE_LAYOUT_ORDER[index];
  SETTINGS.saveToFile();
  app.clearTapFlash();
  finish();
}

void TapZonesPickerActivity::drawLayout(UiScreen& screen, const uint8_t layout, const fui::Rect page) const {
  // Sample at the reader's own screen size, so the drawing matches the reader.
  const int screenW = renderer.getScreenWidth();
  const int screenH = renderer.getScreenHeight();
  for (int row = 0; row < kCells; ++row) {
    for (int col = 0; col < kCells; ++col) {
      const auto zone = ReaderUtils::tapZoneAt(layout, (2 * col + 1) * screenW / (2 * kCells),
                                               (2 * row + 1) * screenH / (2 * kCells), screenW, screenH, false);
      if (zone == ReaderUtils::TapZone::Forward) continue;  // the page stays white
      const auto x0 = static_cast<int16_t>(page.x + page.width * col / kCells);
      const auto x1 = static_cast<int16_t>(page.x + page.width * (col + 1) / kCells);
      const auto y0 = static_cast<int16_t>(page.y + page.height * row / kCells);
      const auto y1 = static_cast<int16_t>(page.y + page.height * (row + 1) / kCells);
      const fui::Paint paint = zone == ReaderUtils::TapZone::Menu ? fui::Paint::solid(fui::Color::Black)
                                                                  : fui::Paint::dither(fui::Color::DarkGray);
      screen.target().fill(fui::Rect{x0, y0, static_cast<int16_t>(x1 - x0), static_cast<int16_t>(y1 - y0)}, paint);
    }
  }
  screen.target().stroke(page, fui::Paint::solid(fui::Color::Black), 2);
}

void TapZonesPickerActivity::buildScreen(UiScreen& screen) {
  const auto& metrics = UITheme::getInstance().getMetrics();
  const auto& tokens = screen.theme();
  screen.setContentMarginFromScreen(fui::Insets{static_cast<int16_t>(metrics.topPadding + metrics.headerHeight), 0,
                                                static_cast<int16_t>(metrics.buttonHintsHeight), 0});
  screen.insetContent(fui::Insets{0, tokens.spaceLg, 0, tokens.spaceLg});
  screen.spacer(static_cast<int16_t>(metrics.verticalSpacing));

  fui::TextStyle legendStyle = tokens.smallText;
  legendStyle.align = fui::TextAlign::Center;
  legendStyle.maxLines = 2;
  const int16_t smallLineH = screen.target().lineHeight(legendStyle.font);
  const fui::Rect legend = screen.takeTop(static_cast<int16_t>(smallLineH * 2), tokens.spaceMd);
  screen.target().text(legend, tr(STR_TAP_ZONES_LEGEND), legendStyle);

  // Pick the column count that gives the biggest page drawings. Each tile is
  // the drawing plus a three-line name, at the screen's own aspect ratio.
  const fui::Rect area = screen.body();
  const int16_t gap = tokens.spaceMd;
  const auto labelH = static_cast<int16_t>(smallLineH * kNameLines);
  const int screenW = std::max(1, renderer.getScreenWidth());
  const int screenH = std::max(1, renderer.getScreenHeight());
  int columns = 3;
  int pageH = 0;
  for (int cols = 2; cols <= 5; ++cols) {
    const int rows = (kLayoutCount + cols - 1) / cols;
    const int tileW = (area.width - gap * (cols - 1)) / cols;
    const int tileH = (area.height - gap * (rows - 1)) / rows;
    const int byWidth = (tileW - 2 * kTilePad) * screenH / screenW;
    const int byHeight = tileH - labelH - 3 * kTilePad;
    const int h = std::min(byWidth, byHeight);
    if (h > pageH) {
      pageH = h;
      columns = cols;
    }
  }
  if (pageH <= 0) return;
  const int pageW = pageH * screenW / screenH;
  const int rows = (kLayoutCount + columns - 1) / columns;
  const auto tileW = static_cast<int16_t>((area.width - gap * (columns - 1)) / columns);
  const auto tileH = static_cast<int16_t>(pageH + labelH + 3 * kTilePad);
  const auto gridH = static_cast<int16_t>(rows * tileH + (rows - 1) * gap);
  const auto top = static_cast<int16_t>(area.y + std::max(0, (area.height - gridH) / 2));

  fui::TextStyle nameStyle = tokens.smallText;
  nameStyle.align = fui::TextAlign::Center;
  nameStyle.maxLines = kNameLines;
  for (int i = 0; i < kLayoutCount; ++i) {
    const uint8_t layout = TAP_ZONE_LAYOUT_ORDER[i];
    const int col = i % columns;
    const int row = i / columns;
    const fui::Rect tile{static_cast<int16_t>(area.x + col * (tileW + gap)),
                         static_cast<int16_t>(top + row * (tileH + gap)), tileW, tileH};
    const fui::Rect page{static_cast<int16_t>(tile.x + (tile.width - pageW) / 2),
                         static_cast<int16_t>(tile.y + kTilePad), static_cast<int16_t>(pageW),
                         static_cast<int16_t>(pageH)};
    drawLayout(screen, layout, page);
    const fui::Rect name{static_cast<int16_t>(tile.x + 2), static_cast<int16_t>(page.bottom() + kTilePad),
                         static_cast<int16_t>(tile.width - 4), labelH};
    fui::TextStyle style = nameStyle;
    style.bold = layout == SETTINGS.tapZoneLayout;  // the saved choice
    screen.target().text(name, I18N.get(TAP_ZONE_LAYOUT_NAMES[layout]), style);
    if (i == nav.selected) {
      screen.target().stroke(tile, fui::Paint::solid(fui::Color::Black), 3, tokens.controlRadius);
    }
    screen.frame().hit(tile, ACTION_ROW, static_cast<int16_t>(i), fui::InputTouch);
  }
}
