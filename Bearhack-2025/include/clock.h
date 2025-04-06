#include "timerISR.h"

int timelength = 0;
bool timeWarn = false;

void setClock(int curr){
    switch (curr) {
        case  1: timelength = 15; break;
        case  2: timelength = 30; break; 
        case  3: timelength = 45; break;
        case  4: timelength = 60; break;
        default: timelength = 15; break;
    }
    return; 
}

void checkClock(int currtime){
    if ((timelength-currtime)<=5){
        timeWarn = true;
    }
    return;
}