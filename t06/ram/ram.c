#include "ram.h"

/* https://jeelabs.org/2011/05/22/atmega-memory-use/ */
int get_free_ram() {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}