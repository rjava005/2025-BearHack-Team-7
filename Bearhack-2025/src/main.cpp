#include "tasks.h"

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

enum SonarStates {S_Init, Sample};
int SonarTick(int);
int distance_cm = 0;
int distance_in = 0;

void TimerISR() {
  //TODO: sample inputs here
  for (unsigned int i = 0; i < NUM_TASKS; i++) { // Iterate through each task in the task array
    if ( tasks[i].elapsedTime == tasks[i].period ) { // Check if the task is ready to tick
      tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
      tasks[i].elapsedTime = 0; // Reset the elapsed time for the next tick
    }
    tasks[i].elapsedTime += GCD_PERIOD; // Increment the elapsed time by GCD_PERIOD
  }
}

int main(void) {
  DDRD = 0xFF; PORTD = 0x00;
  DDRB = 0xFE; PORTB = 0xED;
  DDRC = 0xFD; PORTC = 0x02;
  ADC_init(); // initializes ADC
  sonar_init(); // initializes sonar

  TimerSet(GCD_PERIOD);
  TimerOn();
  serial_init(9600);
  while (1) {}  
  return 0;
}


int SonarTick(int state) {
  switch (state) {
    case S_Init:
      state = Sample;
      distance_cm = 0;
      distance_in = 0;
      break;
    case Sample:
      state = Sample;
      break;
    default:
      state = S_Init;
      break;
  } //Transitions

  switch (state) {
    case S_Init:
      break;
    case Sample:
      distance_cm = (int) sonar_read();
      if (distance_cm < 0) {
        distance_cm = 0;
      }
      
    return state;
  }
}