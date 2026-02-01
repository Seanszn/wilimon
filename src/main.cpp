#include "stepcount.cpp"
#include "battle.cpp"
#include "fwwasm.h"
#include <cstdlib>
#include <stdint.h>

#define LV_1 100
#define LV_2 200
#define LV_3 300

#define STEP_DIV 7

#define FROG 0
#define ORCA 1
#define SEAGULL 2

#define NO_EVENT -1

int character;
int level;
int steps;
int litLeds;
bool up = 0;

void makePickCharacterDisplay(){
    addPanel(1, 1, 0, 0, 0, 0, 0, 0, 0);

    addControlText(0, 0, 60, 10, 0, 16, 255, 255, 255, "pick a character");
    addControlText(0, 1, 10, 200, 0, 16, 255, 255, 255, "frog");
    addControlText(0, 2, 95, 200, 0, 16, 255, 255, 255, "whale");
    addControlText(0, 3, 190, 200, 0, 16, 255, 255, 255, "bird");

    showPanel(1);
}

void pickCharacter(){
    makePickCharacterDisplay();

    uint8_t event_data[FW_GET_EVENT_DATA_MAX] = {0};

    int last_event = NO_EVENT;

    while (true) {
        //wait for an event to happen
        while(!hasEvent()) {
            waitms(100);
        }

        last_event = getEventData(event_data);

        switch (last_event) {
            // choose seagull
            case FWGUI_EVENT_GRAY_BUTTON:
                character = SEAGULL;
                break;
            // choose frog
            case FWGUI_EVENT_GREEN_BUTTON:
                character = FROG;
                break;
            // the orca is red because it wants to obliterate you
            case FWGUI_EVENT_BLUE_BUTTON:
                character = ORCA;
                break;
            
            case FWGUI_EVENT_RED_BUTTON:
                exitToMainAppMenu();

            case FWGUI_EVENT_YELLOW_BUTTON:
                // do nothing for this, just loop back and wait for a valid event
                continue;

            default:
                // handle whatever else could possibly go wrong and wait for a valid event
                continue;
        }
        break;
    }
}

void updateDisplay(){
    setControlValue(1, 1, level);
}

void eventLoop() {
    uint8_t event_data[FW_GET_EVENT_DATA_MAX] = {0};
    
    int last_event = NO_EVENT;

    while (1) {
        //wait for event
        while (!hasEvent()) {
            waitms(100);
        }

        int rxCount = RadioGetRxCount(1);

        if(rxCount > 0){
            offerBattle(&level, &character, &steps);
        }

        // if (last_event == FWGUI_EVENT_RED_BUTTON) {
        //     exitToMainAppMenu();
        // }

        if(last_event == FWGUI_EVENT_GUI_SENSOR_DATA){
            processAccelData(event_data, &up, &steps);
        } else {
            continue;
        }

        switch(level){
            case 1:
                if(steps >= LV_1){
                    level++;
                    steps = 0;
                }
                break;
            case 2:
                if(steps >= LV_2){
                    level++;
                    steps = 0;
                }
                break;
            case 3:
                if(steps >= LV_3){
                    level++;
                    steps = 0;
                }
            default:
                break;
        }
        updateDisplay();
    }
    waitms(30);
}

const char* getCharacterName() {
    switch(character){
        case FROG: return "frog";
        case ORCA: return "whale";
        case SEAGULL: return "bird";
        default: return "";
    }
}

void setupDisplay(){
    addPanel(1, 1, 0, 0, 0, 0, 0, 0, 0);

    addControlText(0, 0, 10, 10, 0, 16, 255, 255, 255, "level: ");
    addControlNumber(0, 1, 1, 70, 10, 30, 16, 0, 255, 255, 255, 0, 0, 0, 0);
    setControlValue(0, 1, level);

    addControlText(0, 2, 130, 110, 0, 16, 255, 255, 255, getCharacterName());

    showPanel(1);
}

int main() {
    // prompt user to pick character
    pickCharacter();
    
    setupDisplay();

    eventLoop();

    return 0;
}