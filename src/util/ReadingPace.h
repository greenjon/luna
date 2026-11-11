#pragma once

#include <I18n.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>

#include "CrossPointState.h"

// Reading pace for "time left" estimates: a running average of how long a
// page stays on screen before the reader turns forward. Kept in state.json
// (saved with the rest of the app state when the reader closes), shared by
// every book: it measures the reader and the layout, not the book.
namespace reading_pace {

// Turns faster than this are skimming; slower ones are a break, not reading.
constexpr uint32_t kMinSampleMs = 3000;
constexpr uint32_t kMaxSampleMs = 10 * 60 * 1000;
// Estimates show once the average has seen this many pages.
constexpr uint16_t kMinSamples = 5;

// A page was shown for shownMs, then the reader turned forward.
inline void notePageRead(const uint32_t shownMs) {
  if (shownMs < kMinSampleMs || shownMs > kMaxSampleMs) return;
  auto& s = APP_STATE;
  float seconds = static_cast<float>(shownMs) / 1000.0f;
  if (s.paceSamples == 0 || s.secondsPerPage <= 0.0f) {
    s.secondsPerPage = seconds;
  } else {
    // One long pause should not swing the estimate: cap a sample at 3x the average.
    seconds = std::min(seconds, s.secondsPerPage * 3.0f);
    // Moving average; settles over the last few dozen pages.
    const float weight = s.paceSamples < 10 ? 1.0f / static_cast<float>(s.paceSamples + 1) : 0.1f;
    s.secondsPerPage += (seconds - s.secondsPerPage) * weight;
  }
  if (s.paceSamples < UINT16_MAX) ++s.paceSamples;
}

inline bool known() { return APP_STATE.paceSamples >= kMinSamples && APP_STATE.secondsPerPage > 0.0f; }

inline uint32_t secondsForPages(const float pages) {
  return pages <= 0.0f ? 0 : static_cast<uint32_t>(pages * APP_STATE.secondsPerPage + 0.5f);
}

// "<1 min", "12 min", "3 h 20 min".
inline void formatDuration(char* out, const size_t size, const uint32_t seconds) {
  const uint32_t minutes = (seconds + 30) / 60;
  if (minutes < 1) {
    snprintf(out, size, "%s", tr(STR_TIME_LESS_THAN_MINUTE));
  } else if (minutes < 60) {
    snprintf(out, size, tr(STR_TIME_MINUTES_FORMAT), static_cast<unsigned>(minutes));
  } else {
    snprintf(out, size, tr(STR_TIME_HOURS_MINUTES_FORMAT), static_cast<unsigned>(minutes / 60),
             static_cast<unsigned>(minutes % 60));
  }
}

}  // namespace reading_pace
