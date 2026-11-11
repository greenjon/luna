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
  values[CrossPointSettings::TAP_ZONES_CLASSIC_RIGHT] = StrId::STR_TAP_ZONES_CLASSIC_RIGHT;
  values[CrossPointSettings::TAP_ZONES_COLUMNS_RIGHT] = StrId::STR_TAP_ZONES_COLUMNS_RIGHT;
  values[CrossPointSettings::TAP_ZONES_HALVES_RL] = StrId::STR_TAP_ZONES_HALVES_RL;
  return values;
}();

// Picker order: each "back on right" layout right after its left-hand one.
inline constexpr std::array<uint8_t, CrossPointSettings::TAP_ZONE_LAYOUT_COUNT> TAP_ZONE_LAYOUT_ORDER = {
    CrossPointSettings::TAP_ZONES_CLASSIC,       CrossPointSettings::TAP_ZONES_CLASSIC_RIGHT,
    CrossPointSettings::TAP_ZONES_ROWS,          CrossPointSettings::TAP_ZONES_COLUMNS,
    CrossPointSettings::TAP_ZONES_COLUMNS_RIGHT, CrossPointSettings::TAP_ZONES_GRID_A,
    CrossPointSettings::TAP_ZONES_GRID_B,        CrossPointSettings::TAP_ZONES_HALVES_TB,
    CrossPointSettings::TAP_ZONES_HALVES_LR,     CrossPointSettings::TAP_ZONES_HALVES_RL,
    CrossPointSettings::TAP_ZONES_BACK_1,        CrossPointSettings::TAP_ZONES_BACK_2,
    CrossPointSettings::TAP_ZONES_BACK_3};
