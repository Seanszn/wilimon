#include "fwwasm.h"

#define WHITE 0x999999
#define YELLOW 0x999900
#define GREEN 0x009900
#define BLUE 0x000099
#define RED 0x990000

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

#define FONT_SIZE 16

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
    int recValue = NO_EVENT;

    unsigned char buffer[4];
    buffer[0] = (val >> 0) & 0xFF;
    buffer[1] = (val >> 8) & 0xFF;
    buffer[2] = (val >> 16) & 0xFF;
    buffer[3] = (val >> 24) & 0xFF;
    RadioWrite(1, buffer, 4);

    //wait for buffer
    while (RadioGetRxCount(1) < 4) {
        waitms(100);
    }

    unsigned char recBuffer[4];
    RadioRead(1, recBuffer, 4);
    recValue = (recBuffer[0]) | (recBuffer[1] << 8) | (recBuffer[2] << 16) | (recBuffer[3] << 24);

    if(val > recValue){
        return true;
    }else{
        return false;
    }
}

void displayVictory(int* character){
    unsigned int startTime = millis();
}

void displayLoss(int* character){

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

    unsigned int startTime = millis();
    unsigned int flashTime;

    while(millis() - startTime < BATTLE_TIME){
        flashTime = millis();
        unsigned int color = (unsigned) wilirand() + startTime % 4;
        
        switch(color){
            //WHITE
            case 0:
                setAllLEDS(WHITE);
                break;
            //YELLOW
            case 1:
                setAllLEDS(YELLOW);
                break;
            //GREEN
            case 2:
                setAllLEDS(GREEN);
                break;
            //BLUE
            case 3:
                setAllLEDS(BLUE);
                break;

            default:
                break;
        }

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

    addPanel(1, 1, 0, 0, 0, 0, 0, 0, 0);
    addControlText(1, 0, 20, 80, 0, FONT_SIZE, GET_R(WHITE), GET_G(WHITE), GET_B(WHITE), "another wilimon is in the area");
    addControlText(1, 1, 110, 130, 0, FONT_SIZE, GET_R(WHITE), GET_G(WHITE), GET_B(WHITE), "challenge?");
    showPanel(1);

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
            showPanel(0);
            return;
        }

        waitms(30);
    }

    showPanel(0);
}