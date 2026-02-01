#include "fwwasm.h"
#include <stdint.h>

// ===== Events =====
#define EVT_NOISE 29
#define EVT_GREY 0
#define EVT_YELLOW 1
#define EVT_GREEN 2
#define EVT_BLUE 3

// ===== Panels (0 is event log; don't use it) =====
#define PANEL_MAIN 1
#define PANEL_YELLOW 2
#define PANEL_BLUE 3
#define PANEL_GREEN 4

static void ui_init(void) {
  // addPanel(panel, visible, in_rotation, use_tile, tile_id, bg_r, bg_g, bg_b,
  // show_menu) IMPORTANT: in_rotation=0 so nothing auto-rotates to event log
  addPanel(PANEL_MAIN, 1, 0, 0, 101, 0, 0, 0, 0);
  addControlText(PANEL_MAIN, 1, 10, 25, 1, 0, 255, 255, 255, "Pick Y/G/B");
  showPanel(PANEL_MAIN);

  addPanel(PANEL_YELLOW, 1, 0, 0, 101, 0, 0, 0, 0);
  addControlPictureFromFile(PANEL_YELLOW, 0, 0, 0, "pip_boy.fwi", 1);
  addControlText(PANEL_YELLOW, 1, 10, 25, 1, 0, 255, 255, 255, "You picked");
  addControlText(PANEL_YELLOW, 2, 10, 65, 1, 0, 255, 255, 255, "YELLOW");

  addPanel(PANEL_GREEN, 1, 0, 0, 101, 0, 0, 0, 0);
  addControlPictureFromFile(PANEL_GREEN, 0, 0, 0, "green.fwi", 1);
  addControlText(PANEL_GREEN, 1, 10, 25, 1, 0, 255, 255, 255, "You picked");
  addControlText(PANEL_GREEN, 2, 10, 65, 1, 0, 255, 255, 255, "GREEN");

  addPanel(PANEL_BLUE, 1, 0, 0, 101, 0, 0, 0, 0);
  addControlPictureFromFile(PANEL_GREEN, 0, 0, 0, "red.fwi", 1);
  addControlText(PANEL_BLUE, 1, 10, 25, 1, 0, 255, 255, 255, "You picked");
  addControlText(PANEL_BLUE, 2, 10, 65, 1, 0, 255, 255, 255, "BLUE");
}

// Drain queue and return last non-noise event
static int read_last_non_noise(uint8_t *data) {
  int last = -1;
  while (hasEvent()) {
    int e = getEventData(data);
    if (e != EVT_NOISE)
      last = e;
  }
  return last;
}

// Flush startup events (e.g., the button press used to launch)
static void flush_events(uint8_t *data) {
  while (hasEvent())
    (void)getEventData(data);
}

int main(void) {
  ui_init();

  // Event -> panel table. Only map Y/G/B (grey intentionally unmapped).
  static uint8_t panel_for_event[64] = {0};
  panel_for_event[EVT_YELLOW] = PANEL_YELLOW;
  panel_for_event[EVT_GREEN] = PANEL_GREEN;
  panel_for_event[EVT_BLUE] = PANEL_BLUE;

  uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};

  // Prevent the launch-click from being treated as a choice
  waitms(150);
  flush_events(data);

  while (1) {
    waitms(200);

    if (!hasEvent())
      continue;

    int e = read_last_non_noise(data);
    if (e < 0)
      continue;

    // Clamp out-of-range safely to "no action"
    unsigned idx = (unsigned)e;
    if (idx >= 64)
      continue;

    uint8_t p = panel_for_event[idx];
    if (!p)
      continue; // ignores Grey and anything else

    showPanel(p);

    // Done: stop taking input (or exit)
    waitms(1200);
    exitToMainAppMenu();
    return 0;
  }
}
