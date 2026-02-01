#include "stepcount.cpp"
#include "battle.cpp"
#include "fwwasm.h"
#include <stdint.h>

#define LV_1 100
#define LV_2 200
#define LV_3 300

#define STEP_DIV 7

#define FROG 0
#define ORCA 1
#define SEAGULL 2

int character;
int level;
int steps;
int litLeds;
bool up = 0;

void pickCharacter(){

    // TODO: show start panel before this 
    uint8_t event_data[FW_GET_EVENT_DATA_MAX] = {0};

    int last_event;
    if (hasEvent()) {
        last_event = getEventData(event_data);
    }

    switch (last_event) {
        
        case FWGUI_EVENT_GRAY_BUTTON:
            character = SEAGULL;
            break;
        
        case FWGUI_EVENT_GREEN_BUTTON:
            character = FROG;
            break;
        // the orca is red because it wants to obliterate you
        case FWGUI_EVENT_RED_BUTTON:
            character = ORCA;
            break;
    }
}

void eventLoop(){
    uint8_t event_data[FW_GET_EVENT_DATA_MAX] = {0};
    
    int last_event;
    if(hasEvent()){
        last_event = getEventData(event_data);
    }

    if(last_event == FWGUI_EVENT_GUI_SENSOR_DATA){
        processAccelData(event_data, &up, &steps);
    }

    int rxCount = RadioGetRxCount(1);
    if(rxCount > 0){
        beginBattle(&level, &character, &steps);
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
}

int main(){

    pickCharacter();
    
    while(1){
        eventLoop();
    }

    return 0;
}