#include "timerISR.h"
#include "helper.h"
#include "periph.h"
#include "serialATmega.h"
#include "clock.h"
#include "seg7.h"

#define NUM_TASKS 3


typedef struct _task{
  signed char state; //Task's current state
  unsigned long period; //Task period
  unsigned long elapsedTime; //Time elapsed since last task tick
  int (*TickFct)(int); //Task tick function
} task;

//TASK PERIODS
const unsigned long SONAR_TASK_PERIOD = 1000;

const unsigned long RIGHT_BUTTON_TASK_PERIOD = 200;

const unsigned long MINUTE_PERIOD = 60000;
const unsigned long SETCLOCK_PERIOD = 100;
const unsigned long TIMEWARN_PERIOD = 100;
const unsigned long LEDCTRL_PERIOD = 200;
const unsigned long SYSTEM_PERIOD = 1;
const unsigned long SEVENSEG_PERIOD = 50;

const unsigned long GCD_PERIOD = 1;


task tasks[NUM_TASKS];


int threshold = 10;
unsigned char threshold_far = (threshold*6)/5;
unsigned char threshold_close = (threshold*4)/5;


enum SonarStates {SO_Init, Sample} state1;
int SonarTick(int);
int distance_cm = 0;
int distance_in = 0;
bool strobe = false;
enum RightButtonStates {RB_Init, WAIT_PRESS_2} state3;
int RightButtonTick(int);

enum MinuteState {MinOFF, MinCNT, MinBREAK} state2a;
int currTime = 0;
enum setClockState {clock1,clock2,clock3,clock4} state2b;
int setClockTick(int);

enum LEDControlStates {LEDoff, GreenON, RedON} state4; 
int LEDControlTick(int);

int checkTimeWarn(int);

int MinuteTick(int);

enum SystemStates {S_Init, S_OFF, S_ON, S_WAIT} state5;
int SystemTick(int);
bool clockOn = false;
bool waitTime = false;

enum DispStates {DispOFF, Disp1, Disp2} state6;
int dispSeg(int);

void tasks_init() {
  tasks[0].period = SONAR_TASK_PERIOD;
  tasks[0].state = SO_Init;
  tasks[0].elapsedTime = tasks[0].period;
  tasks[0].TickFct = &SonarTick;

  tasks[1].period = RIGHT_BUTTON_TASK_PERIOD;
  tasks[1].state = RB_Init;
  tasks[1].elapsedTime = tasks[1].period;
  tasks[1].TickFct = &RightButtonTick;

  tasks[2].period = MINUTE_PERIOD;
  tasks[2].state = MinOFF;
  tasks[2].elapsedTime = tasks[2].period;
  tasks[2].TickFct = &MinuteTick;

  tasks[3].period = SETCLOCK_PERIOD;
  tasks[3].state = clock1;
  tasks[3].elapsedTime = tasks[3].period;
  tasks[3].TickFct = &setClockTick;

  tasks[3].period = SETCLOCK_PERIOD;
  tasks[3].state = clock1;
  tasks[3].elapsedTime = tasks[3].period;
  tasks[3].TickFct = &setClockTick;

  tasks[4].period = TIMEWARN_PERIOD;
  tasks[4].state = 0;
  tasks[4].elapsedTime = tasks[4].period;
  tasks[4].TickFct = &checkTimeWarn;

  tasks[5].period = LEDCTRL_PERIOD;
  tasks[5].state = LEDoff;
  tasks[5].elapsedTime = tasks[5].period;
  tasks[5].TickFct = &LEDControlTick;

  tasks[6].period = SYSTEM_PERIOD;
  tasks[6].state = S_Init;
  tasks[6].elapsedTime = tasks[6].period;
  tasks[6].TickFct = &SystemTick;
  
  tasks[7].period = SEVENSEG_PERIOD;
  tasks[7].state = 0;
  tasks[7].elapsedTime = tasks[7].period;
  tasks[7].TickFct = &dispSegTick;
  
}

int SystemTick(int state5) {
    switch (state5) {
      case S_Init:
        state5 = S_OFF;
        break;
      case S_OFF:
        if (state2a == MinCNT) {
          state5 = S_ON;
        } else {
          state5 = S_OFF;
        }
        break;
      case S_ON:
        if (waitTime) {
          state5 = S_WAIT;
        } else {
          state5 = S_ON;
        }
        break;
      case S_WAIT:
        if (currTime == timelength + 1) {
          state5 = S_OFF;
        } 
        else if (currTime == timelength) {
          state5 = S_WAIT;
        } 
        else {
          state5 = S_ON;
        }
      default:
        state5 = S_Init;
        break;
    } //Transitions

    switch (state5) {
      case S_Init:
        break;
      case S_OFF:
        clockOn = false;
        break;
      case S_ON:
        clockOn = true;
        if (currTime == timelength) {
          waitTime = true;
        } else {
          waitTime = false;
        }
        break;
      default:
        break;
    }
  return state5;
}

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
        if (distance_cm > threshold_far) {
          strobe = true;
        } 
        else if (distance_cm < threshold_far && distance_cm > threshold_close) {
          strobe = false;
        } 
        else if (distance_cm < threshold_close) {
          strobe = false;
        }


      return state1;
    }
}


int MinuteTick(int state2a){
  switch (state2a){
    case MinOFF: currTime = 0; return 0;
    case MinCNT: currTime++; return 0;
  }
}

int setClockTick(int state2b){
  int potRead = ADC_read(0);
  if (potRead <= 255){state2b = clock1;}
  else if (potRead>255&&potRead<=511) {state2b = clock2;}
  else if (potRead>511&&potRead<=767) {state2b = clock3;} 
  else {state2b = clock4;}
  
  switch (state2b) {
    case clock1: setClock(1); state6 = DispOFF; 
    case clock2: setClock(2);
    case clock3: setClock(3);
    case clock4: setClock(4);
  }
}

int RightButtonTick(int state3) {
  switch (state3) { //Transitions
    case RB_Init: 
      state3 = WAIT_PRESS_2;
      break;
    case WAIT_PRESS_2:
      state3 = WAIT_PRESS_2;
      break;
    default:
      state3 = RB_Init;
      break;
  }

  switch (state3) { //Actions
    case RB_Init:
      break;
    case WAIT_PRESS_2:
      if (PINC & (0x01 << 4)) {
        threshold = distance_cm;
        threshold_close = (threshold*4) / 5;
        threshold_far = (threshold*6) / 5;
        
        state2a = MinCNT;
      }
      break;
    default:
      break;
  }
  return state3;
}


int checkTimeWarn(int state2c){
  checkClock(currTime); return 0;
}

int LEDControlTick(int state4){
  switch (state4){
    case LEDoff: 
      if (clockOn){
        if (!timeWarn){state4 = GreenON;}
        else {state4 = RedON;}
      } 
      break;
    case GreenON:
      if (clockOn){

        if (timeWarn){state4 = RedON;break;}
        else if (strobe){state4 = LEDoff;break;}
      }
      else {state4 = LEDoff;}
      break;
    case RedON: 
      if (clockOn){
        if (!timeWarn){state4 = GreenON;break;}
        else if (strobe){state4 = LEDoff;break;}
      }
      else {state4 = LEDoff;}
    break;
  }
  switch (state4){
    case LEDoff: 
    //SET BIT TO TURN ON GREEN LED
    PORTC = SetBit(PORTC, 1, 0);
    PORTC = SetBit(PORTC, 2, 0);
    break;
    case GreenON: 
    PORTC = SetBit(PORTC, 1, 1); //SET BIT TO TURN ON GREEN LED
    PORTC = SetBit(PORTC, 2, 0); //SET BIT TO TURN OFF RED LED
    break;
    case RedON: 
    PORTC = SetBit(PORTC, 1, 0); //SET BIT TO TURN OFF GREEN LED
    PORTC = SetBit(PORTC, 2, 1); //SET BIT TO TURN ON RED LED
    break;
  }
}

int dispSegTick(int state6){
  switch(state2a){
    case MinOFF: state6 = 0; break;
    case MinCNT: 
      if (state6 == 2){
        break;
      }
      else {state6 = 1; break;}
  }
  switch (state6){
    case DispOFF: displaySev(0,1);
    case Disp1: displaySev(currTime%10,1); state6 = 2;
    case Disp2: displaySev(currTime/10,2); state6 = 1;
    default: state6 = 1;
  }
}