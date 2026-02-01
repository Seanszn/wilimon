#include "stepcount.cpp"
#include "battle.cpp"
#include "fwwasm.h"
#include <stdint.h>

const int levelOne = 100;
const int levelTwo = 200;
const int levelThree = 300;

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
            if(steps >= levelOne){
                level++;
                steps = 0;
            }
            break;
        case 2:
            if(steps >= levelTwo){
                level++;
                steps = 0;
            }
            break;
        case 3:
            if(steps >= levelThree){
                level++;
                steps = 0;
            }
        default:
            break;
    }
}

WASM_EXPORT int main(){
    bool saved = fileExists("data.txt");
    if(!saved){
        createNewSave();
    }else{
        static char buff[256];
        int handle = openFile("data.txt", 0);

        int bytesRead = readFile(handle, (unsigned char*)buff, (int*)sizeof(buff) - 1);
        buff[bytesRead] = '\0';

        character = (int)buff[0] - (int)'0';
        level = (int)buff[1] - (int)'0';
        litLeds = (int)buff[2] - (int)'0';

        switch(level){
            case 1:
                steps = (int)((100 / 7) * litLeds);
                break;
            case 2:
                steps = (int)((200 / 7) * litLeds);
                break;
            case 3:
                steps = (int)((300 / 7) * litLeds);
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