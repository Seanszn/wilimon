#include "fwwasm.h"

const int battleTime = 30000;
const int switchTime = 6000;

const float lvOne = 1;
const float lvTwo = 1.1;
const float lvThree = 1.2;
const float lvFour = 1.4;

// const int WHITE = 0;
// const int YELLOW = 1;
// const int GREEN = 2;
// const int BLUE = 3;
// const int RED = 5;

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
    int flashTime = startTime;

    while(millis() - startTime < battleTime){
        int color = wilirand() + startTime % 5;
        
        switch(color){
            //WHITE
            case 0:
                setAllLEDS(255, 255, 255);
                break;
            //YELLOW
            case 1:
                setAllLEDS(255, 255, 0);
                break;
            //GREEN
            case 2:
                setAllLEDS(0, 255, 0);
                break;
            //BLUE
            case 3:
                setAllLEDS(0, 0, 255);
                break;
            //RED
            case 4:
                setAllLEDS(255, 0, 0);
                break;
            default:
                break;
        }

        while(millis() - flashTime){
            
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

bool evaluateBattle(float value){

}

void displayVictory(int* character){

}

void displayLoss(int* character){

}

void setAllLEDS(int red, int green, int blue) {
    for (int i = 0; i < 7; i++) {
        setBoardLED(i, red, green, blue, switchTime, ledsimplevalue);
    }
}