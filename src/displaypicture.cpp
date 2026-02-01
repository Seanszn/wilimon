#include "fwwasm.h"

int main() {
  // Do NOT touch LEDs.
  // Just show something so you know script 2 is running.
  addPanel(0, 1, 0, 0, 0, 0, 0, 0, 1);
  addControlPictureFromFile(0, 0, 0, 0, "pip_boy.fwi", 1);
  showPanel(0);

  while (true)
    waitms(100);
  return 0;
}
