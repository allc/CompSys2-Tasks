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

/* game stat */
volatile int moves = 0;

volatile int in_game = 0;

/* images */
FIL file;
FATFS FatFs;

bmp_image_loader_state image_state;
status_t stat;

void read_image_bytes(bmp_data_request * request) {
	UINT temp = 0;
	f_lseek(&file, request->dataOffset);
	f_read(&file, request->buffer, request->bufferSize, &temp);
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

void draw_grid() {
	int i;
	rectangle rect;
	for (i = 0; i < 2; i++) {
		/* draw vertical lines */
		rect.left = (i + 1) * 80;
		rect.right = (i + 1) * 80;
		rect.top = 0;
		rect.bottom = 240;
		fill_rectangle(rect, WHITE);
		/* draw horizontal lines */
		rect.left = 0;
		rect.right = 239;
		rect.top = (i + 1) * 80;
		rect.bottom = (i + 1) * 80;
		fill_rectangle(rect, WHITE);		
	}
	rect.left = 0;
	rect.right = 239;
	rect.top = 240;
	rect.bottom = 240;
	fill_rectangle(rect, WHITE);
}

int tileToY(int tile) {
	return tile / 3;
}

int tileToX(int tile) {
	return tile % 3;
}

void draw_tile(int i, int j) {
	rectangle rect;
	int x = tileToX(board[i][j]);
	int y = tileToY(board[i][j]);
	rect.top = y * 80;
	rect.left = x * 80;
	rect.bottom = (y + 1) * 80;
	rect.right = (x + 1) * 80;
	if (stat == STATUS_OK) {
		display_segment_bmp(j * 80, i * 80, &rect, &image_state);
	}
}

void draw_blank(int i, int j) {
	rectangle rect;
	rect.top = i * 80;
	rect.left = j * 80;
	rect.bottom = (i + 1) * 80;
	rect.right = (j + 1) * 80;
	fill_rectangle(rect, BLACK);
}

int move_up() {
	if (x < DIMENSION - 1) {
		moves++;
		board[x][y] = board[x + 1][y];
		board[x + 1][y] = 8;
		x++;
		if (in_game) {
			draw_blank(x, y);
			draw_tile(x - 1, y);
		}
	}
	return 0;
}

int move_down() {
	if (x > 0) {
		moves++;
		board[x][y] = board[x - 1][y];
		board[x - 1][y] = 8;
		x--;
		if (in_game) {
			draw_blank(x, y);
			draw_tile(x + 1, y);
		}
	}
	return 0;
}

int move_left() {
	if (y < DIMENSION - 1) {
		moves++;
		board[x][y] = board[x][y + 1];
		board[x][y + 1] = 8;
		y++;
		if (in_game) {
			draw_blank(x, y);
			draw_tile(x, y - 1);
		}
	}
	return 0;
}

int move_right() {
	if (y > 0) {
		moves++;
		board[x][y] = board[x][y - 1];
		board[x][y - 1] = 8;
		y--;
		if (in_game) {
			draw_blank(x, y);
			draw_tile(x, y + 1);
		}
	}
	return 0;
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
	moves = 0;
	return 0;
}

int reset() {
	random_board(4);
	return 0;
}

int draw_board() {
	int i;
	/* draw image */
    for (i = 0; i < 3; i++) {
        int j;
        for (j = 0; j < 3; j++) {
			/*
            char tile[1];
			sprintf(tile,"%d",board[i][j]);
    		display_string_xy(tile, j * 80, i * 80);
			_delay_ms(1);
			*/

			if (board[i][j] != 8) {
				draw_tile(i, j);
			} else {
				draw_blank(i, j);
			}
        }
    }

	return 0;
}

void display_moves() {
	char move_string[10];
	sprintf(move_string,"Moves: %d",moves);
	display_string_xy(move_string, 0, 242);
}

void moved() {
	display_moves();
	if (check()) {
		in_game = 0;
		draw_tile(2,2);
	}
}

void display_title() {
	display_string_xy("\n", 0, 0);
	display_string("                    ____ \n");
	display_string("                   / ___\\\n");
	display_string("                   \\ \\ //\n");
	display_string("                   / /_\\\\\n");
	display_string("                   \\____/"); display.foreground = GREEN; display_string("  _      \n");
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
	display.foreground = GREEN;
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
	TCCR1B |= _BV(CS11);
	TCCR1B |= _BV(CS10);
	TIMSK1 |= _BV(OCIE1A);

	/* timer for random seed */
	TCCR2B |= (1 << CS10);

	/* start program */
	display_title();
	display_string("          Press Center to Start");
	led_on();

	/* mount file system */
	f_mount(&FatFs, "", 0);

	do{
		reset_switches();
		while(!center_pressed()){}

		/* enter game */
		reset();
		while (check()) {
			reset();
		}

		in_game = 1;
		clear_screen();

		/* draw_grid(); */

		/* load image */
		f_open(&file, "puzzle.bmp", FA_READ);
		stat = init_bmp(&image_state, read_image_bytes, 4096);

		display_moves();
		draw_board();

		OCR1A = 20000;

		led_off();
		sei();
		while(in_game);
		cli();

		/* close image file */
		free(image_state.imageData);
		f_close(&file);

		led_on();

		/* finish game */
		_delay_ms(2000);
		clear_screen();
		display_title();
		display_string("                  Done\n");
		display_string("\n");
		display_string("        Press Center to Restart");
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
		moved();
	}
	if(right_pressed()){
		move_right();
		moved();
	}
	if(up_pressed()){
		move_up();
		moved();
	}
	if(down_pressed()){
		move_down();
		moved();
	}
}