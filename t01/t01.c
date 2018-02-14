/* Compared with model answer */
/* COMP2215 15/16: Task 01 */

/* For La Fortuna board 

   | Port | Pin | Function         |
   |------+-----+------------------|
   | B    |   7 | Green LED        |
   | E    |   4 | Rotary Encoder A |
   | E    |   5 | Rotary Encoder B |
 
*/

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define STEP_DELAY_MS 5
#define MIN_STEP    2    /* > 0 */
#define MAX_STEP  255

void init(void);

int8_t enc_delta(void);
volatile int8_t delta;


void main(void) {
	uint8_t cnt = MAX_STEP/2;
	uint8_t i;
	int16_t res;
	
	
	init();

    /* ENABLE GLOBAL INTERRUPTS HERE */ 
    sei();

	for (;;) {
		for (i=cnt; i > 0; --i) {
		   _delay_ms(STEP_DELAY_MS);
		   res = cnt + enc_delta();
		   if (res > MAX_STEP) {
			   cnt = MAX_STEP;
		   } else if (res < MIN_STEP) {
			   cnt = MIN_STEP;
		   } else {
			   cnt = res;			   
		   }
		}
		PINB |= _BV(PINB7);   /* toggle LED */
	}
}



/* Configure I/O Ports */
void init(void) {

	/* 8MHz clock, no prescaling (DS, p. 48) */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;


    /* Configure I/O Ports */

	DDRB  |=  _BV(PB7);   /* LED pin out */
	PORTB &= ~_BV(PB7);   /* LED off */
        /* _BV(PB7) -> 1 << PB7 */
	
	/* ENABLE ENCODER INPUTS AND PULL-UPS */
    DDRE &= ~_BV(PE4) & ~_BV(PE5);
    PORTE |= _BV(PE4) | _BV(PE5);

	/* Timer 0 for switch scan interrupt: */

	TCCR0A = _BV(WGM01); /* CTC */
	TCCR0B = _BV(CS01)
          | _BV(CS00);   /* F_CPU / 64 */

    /* SET OCR0A FOR A 1 MS PERIOD */        
    OCR0A = (int8_t) (F_CPU / (64.0 * 1000) - 1); /* (DS, p. 128) */
        /* OCR0A: Output Compare Register */
        /* Waveform Frequency 1000 */
    		
    /* ENABLE TIMER INTERRUPT */
    TIMSK0 |= _BV(OCIE0A);
        /* TIMSK0: Timer/Counter Interrupt Mask Register */
        /* (DS, p. 113) */

}



/* Interrupt Service Routines */
/* Interrupt vectors DS, p. 68 */
 ISR( TIMER0_COMPA_vect ) {
     static int8_t last;
     int8_t new, diff;
     uint8_t wheel;


     /*
        Scan rotary encoder
        ===================
        This is adapted from Peter Dannegger's code available at:
        http://www.mikrocontroller.net/articles/Drehgeber
     */

     wheel = PINE;
        /* Port E Input Pins Address */
     new = 0;
     if( wheel  & _BV(PE4) ) new = 3; 
        /* 0b00000011 */
     if( wheel  & _BV(PE5) )
	 new ^= 1;		        	/* convert gray to binary */
        /* new = new ^ 1; */
        /* 0b00000001 */
     diff = last - new;			/* difference last - new  */
     if( diff & 1 ){			/* bit 0 = value (1) */
	     last = new;		       	/* store new as next last  */
	     delta += (diff & 2) - 1;	/* bit 1 = direction (+/-) */
     }

}


/* read two step encoder */
int8_t enc_delta() {
    int8_t val;

    cli();
    val = delta;
    delta &= 1;
    sei();

    return val >> 1;
}

