#include "fwwasm.h"

#define WHITE 0x999999
#define YELLOW 0x999900
#define GREEN 0x009900
#define BLUE 0x000099
#define RED 0x990000

#define GET_R(x) ((x >> 16) & 0xFF)
#define GET_G(x) ((x >> 8) & 0xFF)
#define GET_B(x) (x & 0xFF)

const int battleTime = 30000;
const int switchTime = 6000;

const float lvOne = 1;
const float lvTwo = 1.1;
const float lvThree = 1.2;
const float lvFour = 1.4;

void setAllLEDS(int color) {
    for (int i = 0; i < 7; i++) {
        setBoardLED(i, 
            GET_R(color), 
            GET_G(color), 
            GET_B(color), 
            switchTime, 
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
    int mult;
    int clicks = 0;

    switch(*level){
        case 1:
            mult = lvOne;
            break;
        case 2:
            mult = lvTwo;
            break;
        case 3:
            mult = lvThree;
            break;
        case 4:
            mult = lvFour;
            break;
        default:
            break;
    }

    int startTime = millis();
    int flashTime;

    while(millis() - startTime < battleTime){
        int color = wilirand() + startTime % 5;

        flashTime = millis();
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

        while(millis() - flashTime < switchTime){

        }
    }

    float points = clicks * mult;
    bool won = evaluateBattle(points);

    if(won){
        *steps += 50;
        displayVictory(character);
    }else{
        displayLoss(character);
    }
}