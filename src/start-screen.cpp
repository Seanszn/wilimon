#include "fwwasm.h"
#include <stdint.h>

#define P 1
#define BOOT 1
#define MSG 2

static void ui_init(void) {
  addPanel(P, 1, 0, 0, 0, 0, 0, 0, 1);
  addControlText(P, BOOT, 10, 20, 1, 0, 255, 255, 255, "BOOT");
  addControlText(P, MSG, 10, 60, 1, 0, 255, 255, 255, "Press Gray/Green/Red");
  showPanel(P);
}

static void ui_msg(const char *s) {
  addControlText(P, MSG, 10, 60, 1, 0, 255, 255, 255, s);
  showPanel(P);
}

int main(void) {
  ui_init();

  uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};

  while (1) {
    waitms(33);

    if (!hasEvent())
      continue;

    int e = getEventData(data);

    // Debug: confirm events are arriving at all
    printInt("event=%d\n", printOutColor::printColorNormal,
             printOutDataType::printUInt32, e);

    if (e == FWGUI_EVENT_GRAY_BUTTON) {
      ui_msg("Grey -> Seagull");
    }
    if (e == FWGUI_EVENT_GREEN_BUTTON) {
      ui_msg("Green -> Frog");
    }
    if (e == FWGUI_EVENT_RED_BUTTON) {
      ui_msg("Red -> Orca");
      break;
    }
  }

  waitms(500);
  exitToMainAppMenu();
  return 0;
}
