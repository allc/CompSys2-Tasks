#ifndef BMP_LCD_HEADER
#define BMP_LCD_HEADER

#include "bmp.h"
#include "lcd.h"

/** Displays a segment of the BMP file, as restricted by area.
    
    @param Top left X coordinate of where the image should be drawn
    @param Top left Y coordinate of where the image should be drawn
    @param Rectangle to restrict the image to, starting at top left. 
    @param Loader to use for image
**/

status_t display_segment_bmp(uint16_t x, uint16_t y, rectangle * area, bmp_image_loader_state * loaderState);
#endif
