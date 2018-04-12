/* COMP2215 Task 5---MODEL ANSWER */


#include "os.h"


int blink(int);
int update_dial(int);
int collect_delta(int);
int check_switches(int);
void tail(uint8_t lines);   /* Show last lines of a file */

/* Lines have to be shorter than this. Note: buffer is on stack. */
#define LINE_BUFF_LEN 55


FIL File;  						/* FAT File */

int position = 0;



void main(void) {
    os_init();

    os_add_task( blink,            30, 1);
    os_add_task( collect_delta,   500, 1);
    os_add_task( check_switches,  100, 1);

    sei();
    for(;;){}

}


int collect_delta(int state) {
	position += os_enc_delta();
	return state;
}


int check_switches(int state) {

	if (get_switch_press(_BV(SWN))) {
			display_string("North\n");
	}

	if (get_switch_press(_BV(SWE))) {
			display_string("East\n");
	}

	if (get_switch_press(_BV(SWS))) {
			/* display_string("South\n"); */
		tail(25);
	}

	if (get_switch_press(_BV(SWW))) {
			display_string("West\n");
	}

	if (get_switch_long(_BV(SWC))) {
		f_mount(&FatFs, "", 0);
		if (f_open(&File, "myfile.txt", FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
			f_lseek(&File, f_size(&File));
			f_printf(&File, "Encoder position is: %d \r\n", position);
			f_close(&File);
			display_string("Wrote position\n");
		} else {
			display_string("Can't write file! \n");
		}

	}

	if (get_switch_short(_BV(SWC))) {
			display_string("[S] Centre\n");
	}

	if (get_switch_rpt(_BV(SWN))) {
			display_string("[R] North\n");
	}

	if (get_switch_rpt(_BV(SWE))) {
			display_string("[R] East\n");
	}

	if (get_switch_rpt(_BV(SWS))) {
			display_string("[R] South\n");
	}

	if (get_switch_rpt(_BV(SWW))) {
			display_string("[R] West\n");
	}

	if (get_switch_rpt(SWN)) {
			display_string("[R] North\n");
	}


	if (get_switch_long(_BV(OS_CD))) {
		display_string("Detected SD card.\n");
	}

	return state;
}




int blink(int state) {
	static int light = 0;
	uint8_t level;

	if (light < -120) {
		state = 1;
	} else if (light > 254) {
		state = -20;
	}


	/* Compensate somewhat for nonlinear LED
       output and eye sensitivity:
    */
	if (state > 0) {
		if (light > 40) {
			state = 2;
		}
		if (light > 100) {
			state = 5;
		}
	} else {
		if (light < 180) {
			state = -10;
		}
		if (light < 30) {
			state = -5;
		}
	}
	light += state;

	if (light < 0) {
		level = 0;
	} else if (light > 255) {
		level = 255;
	} else {
		level = light;
	}

	os_led_brightness(level);
	return state;
}



/* A more general solution would check for the length of the file
   and if the file is long would work backwards from the end
   of the file in bufferable chunks for long files until it
   has found the required number of newline characters.
   For short files it would do the same as is done here, i.e.,
   read into a ring buffer until the end of the file has been
   encountered:
*/
void tail(uint8_t n) {
	char line[n][LINE_BUFF_LEN];
	int8_t i,j;


	f_mount(&FatFs, "", 0);
	if (f_open(&File, "myfile.txt", FA_READ) == FR_OK) {
	    for (i=0; i < n; i++) {
	        line[n][0] = '\0';
        }

		i = 0;
		while (f_gets(line[i], LINE_BUFF_LEN, &File)) {
	        i++;
	        i %= n;
		}
		clear_screen();
		for (j = 0; j < n; j++) {
			display_string(line[i++]);
	        i %= n;
		}
		f_close(&File);
	} else {
		display_string("Can't read file! \n");
	}

}
