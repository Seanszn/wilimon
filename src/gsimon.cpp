#include "fwwasm.h"
#include <cstdint>

constexpr int LEDS = 7;

// --- LED helpers ---
struct RGB {
  int r, g, b;
};
static inline void leds_off() {
  for (int i = 0; i < LEDS; ++i)
    setBoardLED(i, 0, 0, 0, 0, LEDManagerLEDMode::ledsimplevalue);
}
static inline void leds_color(const RGB &c) {
  for (int i = 0; i < LEDS; ++i)
    setBoardLED(i, c.r, c.g, c.b, 0, LEDManagerLEDMode::ledsimplevalue);
}

// --- Simon mapping (4 gestures -> 4 colors) ---
enum Move : uint8_t { LEFT = 0, RIGHT = 1, UP = 2, DOWN = 3 };

static inline RGB move_color(Move m) {
  switch (m) {
  case LEFT:
    return {0, 0, 255}; // blue
  case RIGHT:
    return {255, 255, 0}; // yellow
  case UP:
    return {0, 255, 0}; // green
  default:
    return {48, 48, 48}; // gray
  }
}

// --- Sensor sample ---
struct Accel {
  int16_t x, y, z;
};

static inline int16_t rd16(const uint8_t *d, int off) {
  return (int16_t)((int16_t)d[off] | (int16_t)(d[off + 1] << 8));
}

// Wait until we receive either:
// - sensor event -> returns true and fills out
// - red button -> exits to menu (never returns)
static bool wait_sensor_or_exit(Accel &out) {
  while (true) {
    waitms(10);
    if (hasEvent() == 0)
      continue;

    uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};
    const int ev = getEventData(data);

    if (ev == (int)FWGuiEventType::FWGUI_EVENT_RED_BUTTON) {
      exitToMainAppMenu();
      return false;
    }

    if (ev == (int)FWGuiEventType::FWGUI_EVENT_GUI_SENSOR_DATA) {
      // Parsing matches FREE-WILi docs:
      // x,y,z are little-endian shorts in btData[0..5].
      // :contentReference[oaicite:1]{index=1}
      out.x = rd16(data, 0);
      out.y = rd16(data, 2);
      out.z = rd16(data, 4);
      return true;
    }
  }
}

// Convert accel deltas -> a single gesture, edge-triggered (tilt then return
// neutral)
static Move wait_gesture(const Accel &base) {
  constexpr int TH = 900;   // tilt threshold (tweak if needed)
  constexpr int NEUT = 450; // neutral threshold
  int stable = 0;
  Move last = LEFT;

  while (true) {
    Accel a;
    if (!wait_sensor_or_exit(a))
      return LEFT; // unreachable after exit, but keeps compiler happy

    const int dx = (int)a.x - (int)base.x;
    const int dy = (int)a.y - (int)base.y;

    // decide dominant axis
    Move m;
    int mag;
    if ((dx >= 0 ? dx : -dx) > (dy >= 0 ? dy : -dy)) {
      m = (dx > 0) ? RIGHT : LEFT;
      mag = (dx >= 0 ? dx : -dx);
    } else {
      m = (dy > 0) ? UP : DOWN;
      mag = (dy >= 0 ? dy : -dy);
    }

    if (mag > TH) {
      if (m == last)
        ++stable;
      else {
        last = m;
        stable = 1;
      }
      if (stable >= 3) {
        // wait until neutral again so we don't repeat the same tilt
        while (true) {
          Accel b;
          if (!wait_sensor_or_exit(b))
            return LEFT;
          const int ndx = (int)b.x - (int)base.x;
          const int ndy = (int)b.y - (int)base.y;
          const int ax = (ndx >= 0 ? ndx : -ndx);
          const int ay = (ndy >= 0 ? ndy : -ndy);
          if (ax < NEUT && ay < NEUT)
            break;
        }
        return last;
      }
    } else {
      stable = 0;
    }
  }
}

// Show the current pattern on LEDs
static void show_pattern(const uint8_t *pat, int len) {
  leds_off();
  waitms(150);

  for (int i = 0; i < len; ++i) {
    const RGB c = move_color((Move)pat[i]);
    leds_color(c);
    waitms(420);
    leds_off();
    waitms(180);
  }
}

// quick feedback flashes
static void flash_ok() {
  leds_color({0, 255, 0});
  waitms(180);
  leds_off();
}
static void flash_bad() {
  leds_color({255, 0, 0});
  waitms(220);
  leds_off();
}

int main() {
  // enable sensor streaming; keep everything else off to save memory/CPU
  // signature in your header: setSensorSettings(accel,temp,rate, ...plot
  // stuff...)
  setSensorSettings(1, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  leds_off();

  // get a baseline accel reading for "neutral"
  Accel base;
  if (!wait_sensor_or_exit(base))
    return 0;

  // pattern buffer (max difficulty)
  uint8_t pattern[16];
  int len = 1;
  pattern[0] = (uint8_t)(wilirand() & 3);

  while (true) {
    show_pattern(pattern, len);

    // user repeats
    for (int i = 0; i < len; ++i) {
      const Move g = wait_gesture(base);
      if ((uint8_t)g != pattern[i]) {
        flash_bad();
        len = 1;
        pattern[0] = (uint8_t)(wilirand() & 3);
        goto next_round;
      }
      flash_ok();
    }

    // success -> extend
    if (len < 16) {
      pattern[len] = (uint8_t)(wilirand() & 3);
      ++len;
    } else {
      // won: a little celebration
      for (int k = 0; k < 3; ++k) {
        leds_color({255, 255, 255});
        waitms(120);
        leds_off();
        waitms(120);
      }
      len = 1;
      pattern[0] = (uint8_t)(wilirand() & 3);
    }

  next_round:
    waitms(250);
  }
}
