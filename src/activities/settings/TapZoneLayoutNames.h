#pragma once

#include <I18n.h>

#include <array>

#include "CrossPointSettings.h"

// Tap zones layout names, assigned by enum value (persisted as numbers).
inline constexpr auto TAP_ZONE_LAYOUT_NAMES = [] {
  std::array<StrId, CrossPointSettings::TAP_ZONE_LAYOUT_COUNT> values{};
  values[CrossPointSettings::TAP_ZONES_CLASSIC] = StrId::STR_TAP_ZONES_CLASSIC;
  values[CrossPointSettings::TAP_ZONES_ROWS] = StrId::STR_TAP_ZONES_ROWS;
  values[CrossPointSettings::TAP_ZONES_COLUMNS] = StrId::STR_TAP_ZONES_COLUMNS;
  values[CrossPointSettings::TAP_ZONES_GRID_A] = StrId::STR_TAP_ZONES_GRID_A;
  values[CrossPointSettings::TAP_ZONES_GRID_B] = StrId::STR_TAP_ZONES_GRID_B;
  values[CrossPointSettings::TAP_ZONES_HALVES_TB] = StrId::STR_TAP_ZONES_HALVES_TB;
  values[CrossPointSettings::TAP_ZONES_HALVES_LR] = StrId::STR_TAP_ZONES_HALVES_LR;
  values[CrossPointSettings::TAP_ZONES_BACK_1] = StrId::STR_TAP_ZONES_BACK_1;
  values[CrossPointSettings::TAP_ZONES_BACK_2] = StrId::STR_TAP_ZONES_BACK_2;
  values[CrossPointSettings::TAP_ZONES_BACK_3] = StrId::STR_TAP_ZONES_BACK_3;
  return values;
}();
