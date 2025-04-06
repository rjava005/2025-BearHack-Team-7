#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//d1>>D5 | d2>>B2

void displaySev(int x, int dig) {
    switch (dig){
        case 1:     PORTD = PORTD | 0x10; PORTB = PORTB & 0xFD;
        case 2:     PORTB = PORTB | 0x02; PORTD = PORTD & 0xEF;
    }
    switch (x){
        case 0 :    PORTD = 0x7F | (PORTD & 0x80); PORTB = (PORTB | 0x01) & 0xFD; break;
        case 1 :    PORTD = 0x10 | (PORTD & 0x80); PORTB = (PORTB | 0x01) & 0xFD; break;
        case 2 :    PORTD = 0x74 | (PORTD & 0x80); PORTB = (PORTB | 0x02) & 0xFE; break;
        case 3 :    PORTD = 0x54 | (PORTD & 0x80); PORTB = PORTB | 0x03; break;
        case 4 :    PORTD = 0x18 | (PORTD & 0x80); PORTB = PORTB | 0x03; break;
        case 5 :    PORTD = 0x4F | (PORTD & 0x80); PORTB = PORTB | 0x03; break;
        case 6 :    PORTD = 0x6F | (PORTD & 0x80); PORTB = PORTB | 0x03; break;
        case 7 :    PORTD = 0x14 | (PORTD & 0x80); PORTB = (PORTB | 0x01) & 0xFD; break;
        case 8 :    PORTD = 0x7F | (PORTD & 0x80); PORTB = PORTB | 0x03; break;
        case 9 :    PORTD = 0x5F | (PORTD & 0x80); PORTB = PORTB | 0x03; break;
        case 10:    PORTD = 0x3F | (PORTD & 0x80); PORTB = PORTB | 0x03; break;
    }
    return;
}