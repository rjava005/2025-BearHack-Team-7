#include "timerISR.h"
#include "helper.h"
#include "periph.h"
#include "serialATmega.h"
#include "clock.h"

#define NUM_TASKS 3


typedef struct _task{
  signed char state; //Task's current state
  unsigned long period; //Task period
  unsigned long elapsedTime; //Time elapsed since last task tick
  int (*TickFct)(int); //Task tick function
} task;

//TASK PERIODS
const unsigned long SONAR_TASK_PERIOD = 1000;

const unsigned long GCD_PERIOD = 1;


task tasks[NUM_TASKS];

int threshold = 10;
unsigned char threshold_far = (threshold*6)/5;
unsigned char threshold_close = (threshold*4)/5;
enum SonarStates {S_Init, Sample} state1;
int SonarTick(int);
int distance_cm = 0;
int distance_in = 0;

enum RightButtonStates {RB_Init, WAIT_PRESS_2} state3;
int RightButtonTick(int);
bool calibrating = false;

enum MinuteState {MinOFF, MinCNT} state2a;
int currTime = 0;
enum setClockState {clock1,clock2,clock3,clock4};

enum LEDControlStates {DS_Init, D1, D2} state4; 
int LEDControlTick(int);

int SonarTick(int state1) {
    switch (state1) {
      case S_Init:
        state1 = Sample;
        distance_cm = 0;
        distance_in = 0;
        break;
      case Sample:
        state1 = Sample;
        break;
      default:
        state1 = S_Init;
        break;
    } //Transitions
  
    switch (state1) {
      case S_Init:
        break;
      case Sample:
        distance_cm = (int) sonar_read();
        if (distance_cm < 0) {
          distance_cm = 0;
        }
        
      return state1;
    }
}


int MinuteTick(int state2a){
  switch (state2a){
    case MinOFF: return 0;
    case MinCNT: currTime++; return 0;
  }
}

int setClockTick(int state2b){
  int potRead = ADC_read(1);
  if (potRead <= 255){state2b = clock1;}
  else if (potRead>255&&potRead<=511) {state2b = clock2;}
  else if (potRead>511&&potRead<=767) {state2b = clock3;} 
  else {state2b = clock4;}
  
  switch (state2b) {
    case clock1: setClock(1);
    case clock2: setClock(2);
    case clock3: setClock(3);
    case clock4: setClock(4);
  }
}

int RightButtonTick(int state) {
  switch (state) { //Transitions
    case RB_Init: 
      state = WAIT_PRESS_2;
      break;
    case WAIT_PRESS_2:
      state = WAIT_PRESS_2;
      break;
    default:
      state = RB_Init;
      break;
  }

  switch (state) { //Actions
    case RB_Init:
      break;
    case WAIT_PRESS_2:
      if (PINC & 0x01) {
        threshold = distance_cm;
        threshold_close = (threshold*4) / 5;
        threshold_far = (threshold*6) / 5;
        calibrating = true;
      }
      break;
    default:
      break;
  }
  return state;
}

int checkTimeWarn(int state2c){
  checkClock(currTime);
  switch(timeWarn){
    case false: break;
    case true:  PORTD = PORTD & (0x01 << 1); /*set light to red*/ break;
  }
  return 0;
}



