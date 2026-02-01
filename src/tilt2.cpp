#include "fwwasm.h"
#include <stdint.h>

static constexpr int LEDS = 7;

static inline void drawCount(int hits) {
  for (int i = 0; i < LEDS; ++i) {
    if ((hits >> i) & 1)
      setBoardLED(i, 40, 40, 40, 0, LEDManagerLEDMode::ledsimplevalue);
  }
}

static inline void winFlashAndExit() {
  for (int k = 0; k < 10; ++k) { // ~10 flashes
    int on = (k & 1);
    for (int i = 0; i < LEDS; ++i)
      setBoardLED(i, on ? 0 : 0, on ? 255 : 0, on ? 0 : 0, 0,
                  LEDManagerLEDMode::ledsimplevalue); // green on/off
    waitms(150);
  }
}

int main() {
  setSensorSettings(1, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  int16_t baseX = 0;
  int calibrated = 0;

  int cursor = LEDS / 2;
  int target = wilirand() % LEDS;

  int hits = 0;

  const int DEADZONE = 160;
  const unsigned int STEP_MS = 140;
  unsigned int last_step = 0;

  while (true) {
    waitms(10);
    if (!hasEvent())
      continue;

    uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};
    if (getEventData(data) != FWGUI_EVENT_GUI_SENSOR_DATA)
      continue;

    int16_t x = (int16_t)(data[0] | (data[1] << 8));

    if (!calibrated) {
      baseX = x;
      calibrated = 1;
      continue;
    }

    int dx = (int)x - (int)baseX;

    int dir = 0;
    if (dx > DEADZONE)
      dir = 1;
    else if (dx < -DEADZONE)
      dir = -1;

    unsigned int now = millis();
    if (dir != 0 && (now - last_step) >= STEP_MS) {
      cursor = (cursor + dir + LEDS) % LEDS;
      last_step = now;

      if (cursor == target) {
        ++hits;
        if (hits >= 25) {
          // clear, show win, then exit
          for (int i = 0; i < LEDS; ++i)
            setBoardLED(i, 0, 0, 0, 0, LEDManagerLEDMode::ledsimplevalue);
          winFlashAndExit();
          return 0;
        }
        target = wilirand() % LEDS;
      }
    }

    // clear
    for (int i = 0; i < LEDS; ++i)
      setBoardLED(i, 0, 0, 0, 0, LEDManagerLEDMode::ledsimplevalue);

    // count overlay
    drawCount(hits);

    // game LEDs on top
    setBoardLED(target, 255, 0, 255, 0,
                LEDManagerLEDMode::ledsimplevalue); // pink target
    setBoardLED(cursor, 0, 0, 255, 0,
                LEDManagerLEDMode::ledsimplevalue); // blue you
  }
}
