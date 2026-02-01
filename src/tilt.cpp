#include "fwwasm.h"
#include <stdint.h>

static constexpr int LEDS = 7;

int main() {
  setSensorSettings(1, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  int16_t baseX = 0;
  int calibrated = 0;

  int cursor = LEDS / 2;
  int target = wilirand() % LEDS;

  // how far you must tilt to move
  const int DEADZONE = 160;

  // how fast it scrolls when held (ms per step)
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

    // Decide direction based on deadzone
    int dir = 0;
    if (dx > DEADZONE)
      dir = 1;
    else if (dx < -DEADZONE)
      dir = -1;

    // If tilted, keep stepping at a fixed rate
    unsigned int now = millis();
    if (dir != 0 && (now - last_step) >= STEP_MS) {
      cursor = (cursor + dir + LEDS) % LEDS;
      last_step = now;

      if (cursor == target)
        target = wilirand() % LEDS;
    }

    // draw
    for (int i = 0; i < LEDS; ++i)
      setBoardLED(i, 0, 0, 0, 0, LEDManagerLEDMode::ledsimplevalue);

    setBoardLED(target, 255, 0, 255, 0,
                LEDManagerLEDMode::ledsimplevalue); // pink = target
    setBoardLED(cursor, 0, 0, 255, 0,
                LEDManagerLEDMode::ledsimplevalue); // blue = you
  }
}
