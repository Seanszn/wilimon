#include "fwwasm.h"
#include <array>
#include <cstdint>

constexpr int NUMBER_OF_LEDS = 7;

struct Color {
  uint8_t r, g, b;
  constexpr Color(uint8_t rr, uint8_t gg, uint8_t bb) : r(rr), g(gg), b(bb) {}
};

constexpr Color RED(255, 0, 0);
constexpr Color YELLOW(255, 255, 0);
constexpr Color GREEN(0, 255, 0);
constexpr Color BLUE(0, 0, 255);
constexpr Color GRAY(0x30, 0x30, 0x30);
constexpr Color WHITE(255, 255, 255);

struct PanelInfo {
  uint8_t index;
  FWGuiEventType event_type;
  Color color;
  const char *text;
};

constexpr std::array<PanelInfo, 5> Panels{{
    {1, FWGuiEventType::FWGUI_EVENT_GRAY_BUTTON, GRAY, "GRAY"},
    {2, FWGuiEventType::FWGUI_EVENT_YELLOW_BUTTON, YELLOW, "YELLOW"},
    {3, FWGuiEventType::FWGUI_EVENT_GREEN_BUTTON, GREEN, "GREEN"},
    {4, FWGuiEventType::FWGUI_EVENT_BLUE_BUTTON, BLUE, "BLUE"},
    {5, FWGuiEventType::FWGUI_EVENT_RED_BUTTON, RED, "RED (double tap exits)"},
}};

static void setup_panels() {
  addPanel(0, 1, 0, 0, 0, 0, 0, 0, 1);
  addControlText(0, 1, 20, 20, 1, 32, WHITE.r, WHITE.g, WHITE.b,
                 "Pick a color. Double-tap RED to exit.");
  showPanel(0);

  for (const auto &p : Panels) {
    addPanel(p.index, 1, 0, 0, 0, p.color.r, p.color.g, p.color.b, 1);
    addControlText(p.index, 1, 10, 50, 2, 0, 0, 0, 0, p.text);
  }
}

static void set_all_leds(const Color &c) {
  for (int i = 0; i < NUMBER_OF_LEDS; ++i) {
    setBoardLED(i, c.r, c.g, c.b, 0, LEDManagerLEDMode::ledsimplevalue);
  }
}

static FWGuiEventType wait_button_event() {
  while (true) {
    waitms(25);
    if (hasEvent() == 0)
      continue;
    uint8_t data[FW_GET_EVENT_DATA_MAX] = {
        0}; // keep EXACT behavior that worked
    return static_cast<FWGuiEventType>(getEventData(data));
  }
}

int main() {
  setup_panels();

  bool red_armed = false;

  while (true) {
    const FWGuiEventType e = wait_button_event();

    // Exit logic: double-tap red to exit WITHOUT forcing red LEDs
    if (e == FWGuiEventType::FWGUI_EVENT_RED_BUTTON) {
      if (red_armed) {
        exitToMainAppMenu();
        return 0;
      }

      // First red press: arm exit, DO NOT set LEDs/panel
      red_armed = true;

      // Give a short window for the second tap
      // If another RED arrives quickly -> exit; otherwise disarm
      for (int i = 0; i < 24; ++i) { // ~24 * 25ms = ~600ms
        waitms(25);
        if (hasEvent() == 0)
          continue;
        uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};
        const FWGuiEventType e2 =
            static_cast<FWGuiEventType>(getEventData(data));
        if (e2 == FWGuiEventType::FWGUI_EVENT_RED_BUTTON) {
          exitToMainAppMenu();
          return 0;
        } else {
          // any other button cancels exit + gets processed normally below
          red_armed = false;

          // process e2 like usual:
          for (const auto &p : Panels) {
            if (p.event_type == e2) {
              showPanel(p.index);
              set_all_leds(p.color);
              showPanel(0);
              break;
            }
          }
          break;
        }
      }

      red_armed = false;
      continue;
    }

    red_armed = false;

    // Normal color behavior (same as your working code)
    for (const auto &p : Panels) {
      if (p.event_type == e) {
        showPanel(p.index);
        set_all_leds(p.color);
        showPanel(0);
        break;
      }
    }
  }
}
