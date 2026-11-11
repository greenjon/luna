#pragma once

#include <GfxRenderer.h>
#include <HalFrontlight.h>

#include <algorithm>
#include <cstdlib>

#include "CrossPointSettings.h"
#include "MappedInputManager.h"
#include "util/SlideSwitch.h"

// Slide a finger up or down the left edge of the page to change the
// frontlight brightness live (Settings > Controls > Taps and gestures). The
// light needs no screen refresh, so the finger gets instant feedback. A touch
// on the edge that never slides stays an ordinary tap.
class EdgeBrightnessGesture {
 public:
  // Call once per reader loop pass while no menu is open. Returns true when
  // this frame belongs to a slide; the caller then skips its other touch
  // handling, so the slide's release is never also a tap or a swipe.
  bool update(const MappedInputManager& input, const GfxRenderer& renderer) {
    if (!enabled(input)) {
      state_ = State::Idle;
      return false;
    }
    const int width = renderer.getScreenWidth();
    const int height = renderer.getScreenHeight();
    int x = 0;
    int y = 0;
    if (state_ == State::Idle) {
      // The top and bottom edges keep their own swipes (control center, menu).
      if (input.wasScreenTouchPressed(x, y) && x < edgeWidth(width) && y > height / 10 && y < height - height / 10) {
        state_ = State::Armed;
        startY_ = y;
        startBrightness_ = Frontlight.brightness();
      }
      return false;
    }
    if (input.isScreenTouchHeld(x, y)) {
      const int dy = startY_ - y;  // up = brighter
      if (state_ == State::Armed && std::abs(dy) >= kSlideStartPx) state_ = State::Sliding;
      if (state_ != State::Sliding) return false;
      // A slide over most of the screen height covers the whole range.
      const int span = std::max(1, height * 6 / 10);
      const int next = std::clamp(startBrightness_ + dy * 100 / span, kMinBrightness, 100);
      if (next != Frontlight.brightness()) Frontlight.setBrightness(static_cast<uint8_t>(next));
      if (!Frontlight.isOn()) Frontlight.setOn(true);
      return true;
    }
    // Released.
    const bool slid = state_ == State::Sliding;
    state_ = State::Idle;
    if (slid) {
      SETTINGS.frontlightBrightness = Frontlight.brightness();
      SETTINGS.frontlightOn = 1;
      SETTINGS.saveToFile();
    }
    return slid;
  }

 private:
  enum class State : uint8_t { Idle, Armed, Sliding };
  static constexpr int kSlideStartPx = 24;
  static constexpr int kMinBrightness = 1;

  static int edgeWidth(const int width) { return std::max(40, width / 10); }
  static bool enabled(const MappedInputManager& input) {
    return SETTINGS.edgeBrightnessSlide && input.hasTouch() && SETTINGS.touchReaderControls && Frontlight.present() &&
           !slideSwitchOwnsLight();
  }

  State state_ = State::Idle;
  int startY_ = 0;
  int startBrightness_ = 0;
};
