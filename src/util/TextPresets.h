#pragma once

#include <string>

// Text presets (Luna): up to three saved sets of the reader's text settings
// (font, size, spacing, alignment, margins, hyphenation, weight, ...), kept
// in /.crosspoint/luna_presets.json and applied in one step from the
// toolbar's Text panel.
namespace text_presets {

constexpr int kSlots = 3;

// "Literata, 12 pt", or empty for an unused slot.
std::string summary(int slot);
// Save the current text settings into slot.
bool save(int slot);
// Copy slot into SETTINGS (not saved to file: the caller applies and saves).
bool apply(int slot);

}  // namespace text_presets
