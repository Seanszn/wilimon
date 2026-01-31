#include <stdint.h>

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