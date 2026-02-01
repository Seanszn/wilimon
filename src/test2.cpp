#include "fwwasm.h"
#include <stdint.h>

// ===== Events =====
#define EVT_NOISE 29
#define EVT_GREY 0
#define EVT_YELLOW 1
#define EVT_GREEN 2
#define EVT_BLUE 3

// ===== Panels (0 reserved) =====
#define PANEL_MAIN 0
#define PANEL_GREY 1
#define PANEL_YELLOW 2
#define PANEL_BLUE 3
#define PANEL_GREEN 4

static void ui_init(void) {
  addPanel(PANEL_MAIN, 1, 1, 0, 101, 0, 0, 0, 0);
  addControlText(PANEL_MAIN, 1, 10, 20, 1, 0, 255, 255, 255, "BOOT");
  addControlText(PANEL_MAIN, 2, 10, 55, 1, 0, 255, 255, 255, "G/Y/B");
  showPanel(PANEL_MAIN);

  addPanel(PANEL_GREY, 1, 1, 0, 101, 0, 0, 0, 0);
  addControlText(PANEL_GREY, 1, 10, 30, 1, 0, 255, 255, 255, "YOU PICKED");
  addControlText(PANEL_GREY, 2, 10, 60, 1, 0, 255, 255, 255, "GREY");

  addPanel(PANEL_YELLOW, 1, 1, 0, 101, 0, 0, 0, 0);
  addControlText(PANEL_YELLOW, 1, 10, 30, 1, 0, 255, 255, 255, "YOU PICKED");
  addControlText(PANEL_YELLOW, 2, 10, 60, 1, 0, 255, 255, 255, "YELLOW");

  addPanel(PANEL_BLUE, 1, 1, 0, 101, 0, 0, 0, 0);
  addControlText(PANEL_BLUE, 1, 10, 30, 1, 0, 255, 255, 255, "YOU PICKED");
  addControlText(PANEL_BLUE, 2, 10, 60, 1, 0, 255, 255, 255, "BLUE");

  addPanel(PANEL_GREEN, 1, 1, 0, 101, 0, 0, 0, 0);
  addControlText(PANEL_GREEN, 1, 10, 30, 1, 0, 255, 255, 255, "YOU PICKED");
  addControlText(PANEL_GREEN, 2, 10, 60, 1, 0, 255, 255, 255, "GREEN");
}

// Drain all queued events and return the last non-noise one
static int read_last_non_noise(uint8_t *data) {
  int last = -1;
  while (hasEvent()) {
    int e = getEventData(data);
    if (e != EVT_NOISE)
      last = e;
  }
  return last;
}

// NEW: flush/discard any queued events (like the launch button press)
static void flush_events(uint8_t *data) {
  while (hasEvent()) {
    (void)getEventData(data);
  }
}

int main(void) {
  ui_init();

  static uint8_t panel_for_event[64] = {0};
  panel_for_event[EVT_GREY] = PANEL_GREY;
  panel_for_event[EVT_YELLOW] = PANEL_YELLOW;
  panel_for_event[EVT_BLUE] = PANEL_BLUE;
  panel_for_event[EVT_GREEN] = PANEL_GREEN;

  uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};

  // ===== KEY FIX =====
  // Give the system a moment, then discard any "startup" events
  waitms(150);
  flush_events(data);
  // ===================

  while (1) {
    waitms(33);

    if (!hasEvent())
      continue;

    int e = read_last_non_noise(data);
    if (e < 0 || e >= 64)
      continue;

    printInt("event=%d\n", printOutColor::printColorNormal,
             printOutDataType::printUInt32, e);

    uint8_t p = panel_for_event[(unsigned)e];
    if (p) {
      showPanel(p);
      waitms(400);
      // showPanel(PANEL_MAIN);
    }
  }
}
