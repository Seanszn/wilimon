#include "fwwasm.h"
#include <stdint.h>

static constexpr int L = 7;
static constexpr int R = 1;

int main() {
  setSensorSettings(1, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  RadioSetRx(R);

  const unsigned char HELLO = 0xA1, WIN = 0xF0;

  int paired = 0, hits = 0;
  unsigned int lastTx = 0, lastStep = 0;
  int16_t baseX = 0;
  int cal = 0;

  int cur = L / 2, tgt = wilirand() % L;
  int prevCur = cur, prevTgt = tgt;

  while (1) {
    waitms(10);
    unsigned int now = millis();

    // --- radio (single-byte) ---
    if (RadioGetRxCount(R) > 0) {
      unsigned char b = 0;
      if (RadioRead(R, &b, 1) == 1) {
        if (b == WIN) { // lose
          for (int i = 0; i < L; i++)
            setBoardLED(i, 255, 0, 0, 0, ledsimplevalue);
          return 0;
        }
        if (!paired && b == HELLO)
          paired = 1;
      }
    }

    // --- pairing ---
    if (!paired) {
      if (now - lastTx >= 200) {
        RadioSetTx(R);
        RadioWrite(R, (unsigned char *)&HELLO, 1);
        RadioSetRx(R);
        lastTx = now;
      }
      continue;
    }

    // --- sensor event ---
    if (!hasEvent())
      continue;
    uint8_t d[FW_GET_EVENT_DATA_MAX] = {0};
    if (getEventData(d) != FWGUI_EVENT_GUI_SENSOR_DATA)
      continue;

    int16_t x = (int16_t)(d[0] | (d[1] << 8));
    if (!cal) {
      baseX = x;
      cal = 1;
      continue;
    }

    int dx = (int)x - (int)baseX;
    int dir = (dx > 160) - (dx < -160);

    if (dir && (now - lastStep) >= 140) {
      cur = (cur + dir + L) % L;
      lastStep = now;

      if (cur == tgt) {
        hits++;
        if (hits >= 25) {
          for (int k = 0; k < 25; k++) {
            RadioSetTx(R);
            RadioWrite(R, (unsigned char *)&WIN, 1);
            RadioSetRx(R);
            waitms(30);
          }
          for (int i = 0; i < L; i++)
            setBoardLED(i, 0, 255, 0, 0, ledsimplevalue);
          return 0;
        }
        tgt = wilirand() % L;
      }
    }

    // --- minimal redraw (only touch changed LEDs) ---
    if (prevTgt != tgt)
      setBoardLED(prevTgt, 0, 0, 0, 0, ledsimplevalue);
    if (prevCur != cur)
      setBoardLED(prevCur, 0, 0, 0, 0, ledsimplevalue);

    setBoardLED(tgt, 255, 0, 255, 0, ledsimplevalue); // pink
    setBoardLED(cur, 0, 0, 255, 0, ledsimplevalue);   // blue

    prevTgt = tgt;
    prevCur = cur;
  }
}
