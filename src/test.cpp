#include "fwwasm.h"
#include <stdint.h>

#define PANEL_MAIN 1

// Control IDs
#define CTRL_TITLE 1
#define CTRL_HELP 2

#define CTRL_SEL_1 10
#define CTRL_OPT_1 11

#define CTRL_SEL_2 20
#define CTRL_OPT_2 21

#define CTRL_SEL_3 30
#define CTRL_OPT_3 31

#define CTRL_STATUS 90

static void draw_menu(int sel) {
  setControlValueText(PANEL_MAIN, CTRL_SEL_1, (sel == 0) ? ">" : " ");
  setControlValueText(PANEL_MAIN, CTRL_SEL_2, (sel == 1) ? ">" : " ");
  setControlValueText(PANEL_MAIN, CTRL_SEL_3, (sel == 2) ? ">" : " ");
}

static void ui_init(void) {
  addPanel(PANEL_MAIN, 1, 0, 0, 0, 0, 0, 0, 0);

  addControlText(PANEL_MAIN, CTRL_TITLE, 10, 10, 2, 0, 255, 255, 255,
                 "Pick an option");
  addControlText(PANEL_MAIN, CTRL_HELP, 10, 30, 1, 0, 180, 180, 180,
                 "YELLOW=next  GREEN=confirm");

  // Option 1 row
  addControlText(PANEL_MAIN, CTRL_SEL_1, 10, 55, 2, 0, 255, 255, 255, ">");
  addControlText(PANEL_MAIN, CTRL_OPT_1, 25, 55, 2, 0, 255, 255, 255,
                 "Option 1");

  // Option 2 row
  addControlText(PANEL_MAIN, CTRL_SEL_2, 10, 75, 2, 0, 255, 255, 255, " ");
  addControlText(PANEL_MAIN, CTRL_OPT_2, 25, 75, 2, 0, 255, 255, 255,
                 "Option 2");

  // Option 3 row
  addControlText(PANEL_MAIN, CTRL_SEL_3, 10, 95, 2, 0, 255, 255, 255, " ");
  addControlText(PANEL_MAIN, CTRL_OPT_3, 25, 95, 2, 0, 255, 255, 255,
                 "Option 3");

  addControlText(PANEL_MAIN, CTRL_STATUS, 10, 125, 1, 0, 120, 200, 120,
                 "Not confirmed");

  showPanel(PANEL_MAIN);
}

static void flush_events(uint8_t *data) {
  while (hasEvent())
    (void)getEventData(data);
}

int main(void) {
  ui_init();

  uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};

  // Don’t treat the launch click as input
  waitms(150);
  flush_events(data);

  int sel = 0;
  draw_menu(sel);

  while (1) {
    waitms(50);
    if (!hasEvent())
      continue;

    int e = getEventData(data);

    if (e == FWGUI_EVENT_YELLOW_BUTTON) {
      sel = (sel + 1) % 3;
      draw_menu(sel);
      setControlValueText(PANEL_MAIN, CTRL_STATUS, "Not confirmed");
    } else if (e == FWGUI_EVENT_GREEN_BUTTON) {
      if (sel == 0)
        setControlValueText(PANEL_MAIN, CTRL_STATUS, "Selected: Option 1");
      if (sel == 1)
        setControlValueText(PANEL_MAIN, CTRL_STATUS, "Selected: Option 2");
      if (sel == 2)
        setControlValueText(PANEL_MAIN, CTRL_STATUS, "Selected: Option 3");

      waitms(600);
      exitToMainAppMenu();
      return 0;
    }
  }
}
