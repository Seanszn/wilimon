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

}

void displayVictory(int* character){

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
        unsigned int color = (unsigned) wilirand() + startTime % 5;
        
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
            //RED
            case 4:
                setAllLEDS(RED);
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