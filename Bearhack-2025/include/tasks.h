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

enum SonarStates {S_Init, Sample} state1;
int SonarTick(int);
int distance_cm = 0;
int distance_in = 0;

enum MinuteState {MinOFF, MinCNT} state2a;
int currTime = 0;
enum setClockState {clock1,clock2,clock3,clock4};

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