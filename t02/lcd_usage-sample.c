/* COMP2215: Task 02---MODEL ANSWER */
/* For La Fortuna board.            */


#include <avr/io.h>
#include "lcd.h"


#define BUFFSIZE 256

void init(void);

void main(void) {
    init();
    display_string("Hello Southampton!\n");
    display_string("Hello UK!\n");
    display_string("Hello World!\n");
}


void init(void) {
    /* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    init_lcd();
}
