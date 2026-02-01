#include "fwwasm.h"
#include <stdint.h>

static constexpr int L = 7;

static inline void all(int r, int g, int b) {
  for (int i = 0; i < L; i++)
    setBoardLED(i, r, g, b, 0, ledsimplevalue);
}

int main() {
  const unsigned char B = 0xA1;

  RadioSetRx(2);   // listen on radio 2
  RadioSetIdle(1); // keep radio 1 quiet until we TX

  unsigned int start = millis(), last = 0;

  while (1) {
    waitms(10);
    unsigned int now = millis();

    if (now - last >= 120) { // transmit rate
      RadioSetTx(1);
      RadioWrite(1, (unsigned char *)&B, 1);
      RadioSetIdle(1); // stop TX quickly
      last = now;
    }

    if (RadioGetRxCount(2) > 0) {
      unsigned char x = 0;
      if (RadioRead(2, &x, 1) == 1 && x == B) {
        all(0, 255, 0);
        return 0;
      }
    }

    if (now - start > 50000) {
      all(255, 0, 0);
      return 0;
    }
  }
}
