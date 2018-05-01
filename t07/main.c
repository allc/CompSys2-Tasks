#include "main.h"

#define DIMENSION 3
volatile int board[DIMENSION][DIMENSION] = {
    {0, 1, 2},
    {3, 4, 5},
    {6, 8, 7}
};

/* location of gap */
volatile int y = 1;
volatile int x = 2;

int move_up() {
	if (x < DIMENSION - 1) {
		board[x][y] = board[x + 1][y];
		board[x + 1][y] = 8;
		x++;
	}
	return 0;
}

int move_down() {
	if (x > 0) {
		board[x][y] = board[x - 1][y];
		board[x - 1][y] = 8;
		x--;
	}
	return 0;
}

int move_left() {
	if (y < DIMENSION - 1) {
		board[x][y] = board[x][y + 1];
		board[x][y + 1] = 8;
		y++;
	}
	return 0;
}

int move_right() {
	if (y > 0) {
		board[x][y] = board[x][y - 1];
		board[x][y - 1] = 8;
		y--;
	}
	return 0;
}

int check() {
	int i, j, k;
	k = 0;
	for (i = 0; i < DIMENSION; i++) {
		for (j = 0; j < DIMENSION; j++) {
			if (board[i][j] != k) {
				return 0;
			}
			k++;
		}
	}
	return 1;
}

int redraw() {
	int i;
    for (i = 0; i < 3; i++) {
        int j;
        for (j = 0; j < 3; j++) {
            char tile[1];
    		sprintf(tile,"%d",board[i][j]);
            display_string_xy(tile, j * 80, i * 100);
        }
    }

	return 0;
}

int main() {
	/* Clear DIV8 to get 8MHz clock */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;
    
	/* init led */
	/*
	DDRB |= _BV(PB7);
	PORTB &= ~_BV(PB7);
	*/

    init_lcd();
	init_switches();
    
	redraw();

	/* enable button press inturrupt */
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	TCCR1B |= _BV(CS10);
	TIMSK1 |= _BV(OCIE1A);
	OCR1A = 65535;

	/* enable global interrupt */
	sei();

	while(1) {
		
	}

	return 0;
}

ISR(TIMER1_COMPA_vect)
{
	if (center_pressed()){
		/* display_string("center\n"); */
	}
	if(left_pressed()){
		move_left();
	}
	if(right_pressed()){
		move_right();
	}
	if(up_pressed()){
		move_up();
	}
	if(down_pressed()){
		move_down();
	}
	redraw();
	if (check()) {
		display_string_xy("Done", 50, 50);
	}
}