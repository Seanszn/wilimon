#include "fwwasm.h"
#include <stdint.h>

void handleInput(int e) {
  if (e == 1) {
    addControlText(1, 1, 10, 20, 1, 0, 255, 255, 255, "Grey Pressed");
    showPanel(1);
  } else if (e == 2) {
    addControlText(1, 1, 10, 20, 1, 0, 255, 255, 255, "Yellow Pressed");
    showPanel(1);
  }
}

int main(void) {
  uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};

  // minimal UI so you know it launched
  addPanel(1, 1, 0, 0, 0, 0, 0, 0, 1);
  addControlText(1, 1, 10, 20, 1, 0, 255, 255, 255, "BOOT");
  addControlText(1, 1, 10, 20, 1, 0, 255, 255, 255,
                 "Press buttons; watch console");
  showPanel(1);

  while (1) {
    waitms(1000);

    if (!hasEvent())
      continue;

    int e = getEventData(data);

    // Print the raw event id
    printInt("event=%d\n", printOutColor::printColorNormal,
             printOutDataType::printUInt32, e);

    addControlText(1, 1, 10, 20, 1, 0, 255, 255, 255, "Grey Pressed");
    showPanel(1);

    // else if (e == 4) {
    // return 0;
    // }
    // if (e == 3) {
    //   addControlText(1, 1, 10, 20, 1, 0, 255, 255, 255, "Blue Pressed");
    //   showPanel(1);
    // }
    // if (e == 4) {
    //   exitToMainAppMenu();
    // }
  }
}
