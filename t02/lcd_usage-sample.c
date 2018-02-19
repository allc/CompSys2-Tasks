/* COMP2215: Task 02 */
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
    int i;
    for (i = 0; i < 100; i++) {
        char *str = malloc(11);
        snprintf(str, 11, "String %d\n", i);
        display_string(str);
    }
        display_string("Hello Southampton!Hello Southampton!Hello Southampton!Hello Southampton!Hello Southampton!Hello Southampton!Hello Southampton!Hello Southampton!Hello Southampton!Hello Southampton!Hello Southampton!\n");
}


void init(void) {
    /* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = _BV(CLKPS1) | _BV(CLKPS0);
        /* slow the clock down so I can see the animation */
    CLKPR = 0;

    init_lcd();
}
