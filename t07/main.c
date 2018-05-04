#include "main.h"

#define DIMENSION 3

volatile int board[DIMENSION][DIMENSION] = {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8}
};

/* location of gap */
volatile int y = 2;
volatile int x = 2;

volatile int in_game = 1;

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

int random_board(int times) {
	int i;
	srand(TCNT2);
	for (i = 0; i < times; i++) {
		int n = rand() % 4;
		switch(n) {
			case 0:
				move_up();
				break;
			case 1:
				move_down();
				break;
			case 2:
				move_left();
				break;
			case 3:
				move_right();
				break;
			default:
				break;
		}
	}
	return 0;
}

int reset() {
	move_down();
	random_board(2);
	return 0;
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

void display_title() {
	display_string("\n");
	display_string("       ____ \n");
	display_string("      / ___\\\n");
	display_string("      \\ \\ //\n");
	display_string("      / /_\\\n");
	display_string("      \\____/\n");
	display_string("\n");
	display.foreground = BLUE;
	display_string("                           _      \n");
	display.foreground = GREEN;
	display_string("                          | |     \n");
	display.foreground = CYAN;
	display_string("       _ __  _   _ _______| | ___ \n");
	display.foreground = RED;
	display_string("      | '_ \\| | | |_  /_  / |/ _ \\\n");
	display.foreground = MAGENTA;
	display_string("      | |_) | |_| |/ / / /| |  __/\n");
	display.foreground = YELLOW;
	display_string("      | .__/ \\__,_/___/___|_|\\___|\n");
	display.foreground = MAGENTA;
	display_string("      | |                         \n");
	display.foreground = BLUE;
	display_string("      |_|                         \n");
	display.foreground = WHITE;
	display_string("\n");
}

int main() {
	/* Clear DIV8 to get 8MHz clock */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	init_led();
    init_lcd();
	init_switches();
	set_orientation(North);

	/* enable button press inturrupt */
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	TCCR1B |= _BV(CS10);
	TIMSK1 |= _BV(OCIE1A);

	/* timer for random seed */
	TCCR2B |= (1 << CS10);

	/* start program */
	display_title();
	display_string("          Press Center to Start");
	led_on();

	do{
		reset_switches();
		while(!center_pressed()){}

		reset();

		in_game = 1;
		clear_screen();

		redraw();

		OCR1A = 65535;
		led_off();
		sei();
		while(in_game);
		cli();
		led_on();
		display_string_xy("Done", 50, 50); 
		display_string_xy("Press Center to Restart",0,10);
	} while (1);

	return -1;
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
		in_game = 0;
	}
}