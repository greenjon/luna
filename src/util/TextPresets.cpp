#include "TextPresets.h"

#include <ArduinoJson.h>
#include <I18n.h>
#include <PersistableStore.h>

#include <cstdio>
#include <cstring>

#include "CrossPointSettings.h"

namespace text_presets {
namespace {

constexpr const char* kPath = "/.crosspoint/luna_presets.json";

// The text settings a preset carries, by JSON key.
struct Field {
  const char* key;
  uint8_t CrossPointSettings::* field;
};
constexpr Field kFields[] = {
    {"fontFamily", &CrossPointSettings::fontFamily},
    {"fontPointSize", &CrossPointSettings::fontPointSize},
    {"lineSpacing", &CrossPointSettings::lineSpacing},
    {"wordSpacing", &CrossPointSettings::wordSpacing},
    {"characterSpacing", &CrossPointSettings::characterSpacing},
    {"extraParagraphSpacing", &CrossPointSettings::extraParagraphSpacing},
    {"paragraphIndentSpaces", &CrossPointSettings::paragraphIndentSpaces},
    {"paragraphAlignment", &CrossPointSettings::paragraphAlignment},
    {"screenMargin", &CrossPointSettings::screenMargin},
    {"hyphenationEnabled", &CrossPointSettings::hyphenationEnabled},
    {"embeddedStyle", &CrossPointSettings::embeddedStyle},
    {"focusReadingEnabled", &CrossPointSettings::focusReadingEnabled},
    {"textAntiAliasing", &CrossPointSettings::textAntiAliasing},
    {"textWeight", &CrossPointSettings::textWeight},
};

bool validSlot(const int slot) { return slot >= 0 && slot < kSlots; }

}  // namespace

std::string summary(const int slot) {
  if (!validSlot(slot)) return {};
  JsonDocument doc;
  if (!PersistableStoreBase::readDocFromFile(kPath, doc)) return {};
  JsonObjectConst preset = doc["slots"][slot];
  if (preset.isNull()) return {};
  const char* sdFont = preset["sdFont"] | "";
  char text[64];
  snprintf(text, sizeof(text), "%s, %u pt", sdFont[0] != '\0' ? sdFont : tr(STR_NOTO_SANS),
           static_cast<unsigned>(preset["fontPointSize"] | 0u));
  return text;
}

bool save(const int slot) {
  if (!validSlot(slot)) return false;
  JsonDocument doc;
  PersistableStoreBase::readDocFromFile(kPath, doc);
  JsonArray slots = doc["slots"].is<JsonArray>() ? doc["slots"].as<JsonArray>() : doc["slots"].to<JsonArray>();
  while (static_cast<int>(slots.size()) <= slot) slots.add(nullptr);
  JsonObject preset = slots[slot].to<JsonObject>();
  for (const auto& f : kFields) preset[f.key] = SETTINGS.*(f.field);
  preset["sdFont"] = SETTINGS.sdFontFamilyName;
  return PersistableStoreBase::writeDocToFile(kPath, doc);
}

bool apply(const int slot) {
  if (!validSlot(slot)) return false;
  JsonDocument doc;
  if (!PersistableStoreBase::readDocFromFile(kPath, doc)) return false;
  JsonObjectConst preset = doc["slots"][slot];
  if (preset.isNull()) return false;
  for (const auto& f : kFields) {
    if (preset[f.key].is<uint8_t>()) SETTINGS.*(f.field) = preset[f.key].as<uint8_t>();
  }
  const char* sdFont = preset["sdFont"] | "";
  strncpy(SETTINGS.sdFontFamilyName, sdFont, sizeof(SETTINGS.sdFontFamilyName) - 1);
  SETTINGS.sdFontFamilyName[sizeof(SETTINGS.sdFontFamilyName) - 1] = '\0';
  return true;
}

}  // namespace text_presets
