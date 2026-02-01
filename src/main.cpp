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

#define YELLOW 0x999900
#define GREEN 0x009900
#define BLUE 0x000099

#define GET_R(x) ((x >> 16) & 0xFF)
#define GET_G(x) ((x >> 8) & 0xFF)
#define GET_B(x) (x & 0xFF)

#define BATTLE_TIME 30000
#define SWITCH_TIME 6000

#define LV_1MULT 1.0f
#define LV_2MULT 1.1f
#define LV_3MULT 1.2f
#define LV_4MULT 1.4f

#define NO_EVENT -1

// ===== Panel indices =====
#define PANEL_PICK_CHAR 0
#define PANEL_GAME 1
#define PANEL_BATTLE_OFFER 2
#define PANEL_BATTLE 3
#define PANEL_VICTORY 4
#define PANEL_LOSS 5

int character;
int level;
int steps;
int litLeds;
bool up = 0;

void ui_init() {
    // Panel 0: Pick character screen
    addPanel(PANEL_PICK_CHAR, 1, 0, 0, 0, 0, 0, 0, 0);
    addControlText(PANEL_PICK_CHAR, 0, 60, 10, 0, 16, 255, 255, 255, "pick a character");
    addControlText(PANEL_PICK_CHAR, 1, 10, 200, 0, 16, 255, 255, 255, "frog");
    addControlText(PANEL_PICK_CHAR, 2, 95, 200, 0, 16, 255, 255, 255, "whale");
    addControlText(PANEL_PICK_CHAR, 3, 190, 200, 0, 16, 255, 255, 255, "bird");

    addPanel(PANEL_GAME, 1, 0, 0, 0, 0, 0, 0, 0);
    addControlText(PANEL_GAME, 0, 10, 10, 0, 16, 255, 255, 255, "level: ");
    addControlNumber(PANEL_GAME, 1, 1, 70, 10, 30, 16, 0, 255, 255, 255, 0, 0, 0, 0);
    addControlText(PANEL_GAME, 2, 130, 110, 0, 16, 255, 255, 255, "");

    addPanel(PANEL_BATTLE_OFFER, 1, 0, 0, 0, 0, 0, 0, 0);
    addControlText(PANEL_BATTLE_OFFER, 0, 20, 80, 0, 16, 255, 255, 255, "another wilimon is in the area");
    addControlText(PANEL_BATTLE_OFFER, 1, 110, 130, 0, 16, 255, 255, 255, "challenge?");

    addPanel(PANEL_BATTLE, 1, 0, 0, 0, 0, 0, 0, 0);
    addControlText(PANEL_BATTLE, 0, 80, 20, 0, 16, 255, 255, 255, "BATTLE!");
    addControlText(PANEL_BATTLE, 1, 60, 80, 0, 16, 255, 255, 255, "clicks:");
    addControlNumber(PANEL_BATTLE, 2, 1, 150, 80, 30, 16, 0, 255, 255, 255, 0, 0, 0, 0);

    addPanel(PANEL_VICTORY, 1, 0, 0, 0, 0, 0, 0, 0);
    addControlText(PANEL_VICTORY, 0, 70, 60, 0, 16, 255, 255, 255, "YOU WIN!");
    addControlText(PANEL_VICTORY, 1, 70, 110, 0, 16, 255, 255, 255, "+50 steps");

    addPanel(PANEL_LOSS, 1, 0, 0, 0, 0, 0, 0, 0);
    addControlText(PANEL_LOSS, 0, 60, 60, 0, 16, 255, 255, 255, "YOU LOSE!");
}

void makePickCharacterDisplay(){
    showPanel(PANEL_PICK_CHAR);
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
            case FWGUI_EVENT_YELLOW_BUTTON:
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

            case FWGUI_EVENT_GRAY_BUTTON:
                // do nothing for this, just loop back and wait for a valid event
                continue;

            default:
                // handle whatever else could possibly go wrong and wait for a valid event
                continue;
        }
        break;
    }
}

void setAllLEDS(int color) {
    for (int i = 0; i < 7; i++) {
        setBoardLED(i, 
            GET_R(color), 
            GET_G(color), 
            GET_B(color), 
            SWITCH_TIME, 
            ledsimplevalue);
    }
}

bool evaluateBattle(float value){
    //while(1){
    int val = (int)value;
    int recValue;

    unsigned char buffer[4];
    buffer[0] = (val >> 0) & 0xFF;
    buffer[1] = (val >> 8) & 0xFF;
    buffer[2] = (val >> 16) & 0xFF;
    buffer[3] = (val >> 24) & 0xFF;
    RadioWrite(1, buffer, 4);

    if(RadioGetRxCount(1) >= 4){
        unsigned char recBuffer[4];
        RadioRead(1, recBuffer, 4);
        recValue = (recBuffer[0]) | (recBuffer[1] << 8) | (recBuffer[2] << 16) | (recBuffer[3] << 24);
    }

    if(val > recValue){
        return true;
    }else{
        return false;
    }
}

void displayVictory(int* character){
    showPanel(PANEL_VICTORY);
    waitms(3000);
}

void displayLoss(int* character){
    showPanel(PANEL_LOSS);
    waitms(3000);
}

void beginBattle(int* level, int* character, int* steps){
    float mult;
    int clicks = 0;

    switch(*level){
        case 1:
            mult = LV_1MULT;
            break;
        case 2:
            mult = LV_2MULT;
            break;
        case 3:
            mult = LV_3MULT;
            break;
        case 4:
            mult = LV_4MULT;
            break;
        default:
            break;
    }

    showPanel(PANEL_BATTLE);
    setControlValue(PANEL_BATTLE, 2, 0);

    unsigned int startTime = millis();
    unsigned int flashTime;

    while(millis() - startTime < BATTLE_TIME){
        flashTime = millis();
        unsigned int color = (unsigned) wilirand() + startTime % 3;

        switch(color){
            //YELLOW
            case 0:
                setAllLEDS(YELLOW);
                break;
            //GREEN
            case 1:
                setAllLEDS(GREEN);
                break;
            //BLUE
            case 2:
                setAllLEDS(BLUE);
                break;

            default:
                break;
        }

        setControlValue(PANEL_BATTLE, 2, clicks);

        while(millis() - flashTime < SWITCH_TIME){

        }
    }

    float points = (float) clicks * mult;
    bool won = evaluateBattle(points);

    if(won){
        *steps += 50;
        displayVictory(character);
    }else{
        displayLoss(character);
    }
}

void offerBattle(int* level, int* character, int* steps){
    unsigned char rxBuf[32];
    RadioRead(1, rxBuf, 32);

    unsigned char challengeByte = 0x01;
    RadioWrite(1, &challengeByte, 1);

    showPanel(PANEL_BATTLE_OFFER);

    bool accepted = false;
    bool otherAccepted = false;
    unsigned int startTime = millis();

    while(millis() - startTime < 15000){
        int event_data[FW_GET_EVENT_DATA_MAX] = {0};

        int last_event = NO_EVENT;
        if(hasEvent()){
            last_event = getEventData((unsigned char*)event_data);
        }

        if(last_event == FWGUI_EVENT_GRAY_BUTTON && !accepted){
            accepted = true;
            unsigned char acceptByte = 0x02;
            RadioWrite(1, &acceptByte, 1);
        }

        if(RadioGetRxCount(1) > 0){
            unsigned char rxData[32];
            int bytesRead = RadioRead(1, rxData, 32);
            if(bytesRead > 0 && rxData[0] == 0x02){
                otherAccepted = true;
            }
        }

        if(accepted && otherAccepted){
            beginBattle(level, character, steps);
            showPanel(PANEL_GAME);
            return;
        }

        waitms(30);
    }

    showPanel(PANEL_GAME);
}

void processAccelData(uint8_t* event_data, bool* up, int* steps){
    int16_t iY;
    bool curr_up;

    iY = static_cast<int16_t>(event_data[2] | event_data[3] << 8);
    (iY > 0) ? curr_up = 1 : curr_up = 0;
    
    if(*up){
        if(curr_up != *up){ (*steps)++; }
    }

    *up = curr_up;
}

void updateDisplay(){
    setControlValue(PANEL_GAME, 1, level);
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
    setControlValue(PANEL_GAME, 1, level);
    addControlText(PANEL_GAME, 2, 130, 110, 0, 16, 255, 255, 255, getCharacterName());
    showPanel(PANEL_GAME);
}

int main() {
    ui_init();

    // prompt user to pick character
    pickCharacter();
    
    setupDisplay();

    eventLoop();

    return 0;
}