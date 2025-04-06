#include "tasks.h"

//RIGHT BUTTON (CALIBRATION + START): PIN C4
//POTIENTIOMETER: PIN C0

//RED: PIN C2
//GREEN: PIN C1

//ECHO: PIN D3
//TRIGGER: PIN D4

//BUZZER: PIN C5


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


