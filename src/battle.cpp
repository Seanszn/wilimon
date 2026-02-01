#include "fwwasm.h"

const int battleTime = 30000;
const int switchTime = 6000;

const float lvOne = 1;
const float lvTwo = 1.1;
const float lvThree = 1.2;
const float lvFour = 1.4;

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
    int flashTime = millis();

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