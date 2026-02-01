#include "stepcount.cpp"
#include "battle.cpp"
#include "fwwasm.h"
#include <stdint.h>

#define LV_1 100
#define LV_2 200
#define LV_3 300

#define STEP_DIV 7

#define DATA_FILE "data.txt"
#define BUFFER_SIZE 256

int character;
int level;
int steps;
int litLeds;
bool up = 0;

void createNewSave(){
    //ask user to pick a character
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
    bool saved = fileExists(DATA_FILE);
    if(!saved){
        createNewSave();
    }else{
        static char buff[BUFFER_SIZE];
        int handle = openFile(DATA_FILE, 0);

        int bytesRead = readFile(handle, (unsigned char*)buff, (int*)sizeof(buff) - 1);
        buff[bytesRead] = '\0';

        character = (int)buff[0] - (int)'0';
        level = (int)buff[1] - (int)'0';
        litLeds = (int)buff[2] - (int)'0';

        switch(level){
            case 1:
                steps = (int)((LV_1 / STEP_DIV) * litLeds);
                break;
            case 2:
                steps = (int)((LV_2 / STEP_DIV) * litLeds);
                break;
            case 3:
                steps = (int)((LV_3 / STEP_DIV) * litLeds);
                break;
            default:
                break;
        }
    }

    while(1){
        eventLoop();
    }

    return 0;
}