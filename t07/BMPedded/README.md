BMP image library <https://github.com/Spoffy/BMPedded>

# BMPedded
A simple BMP library written in C for use (primarily) in embedded devices. 
It currently supports images in the BMP16 format. 

## How to use
sample_main.c shows an example usage. 
  
It has been copied and pasted below and explained.
  
Include the appropriate headers. bmp.h concerns parsing BMP files. bmp_lcd.h and bmp_lcd.c are convinience libraries for using them with the LCD library for LaFortuna (University specific).
```
#include "os.h"
#include "bmp.h"
#include "bmp_lcd.h"
```
  
The amount of RAM to use to speed up reads from block devices.
The higher the value, the more lines of the image that are cached in RAM before being requested.
```
#define IMAGE_CACHE_SIZE 2048
```
  
read_image_bytes is a function of type bmp_need_more_bytes.
It takes a request, and fills the request's buffer with the correct number of bytes, starting from the given offset into the BMP file.
```
FIL File;  						/* FAT File */

void read_image_bytes(bmp_data_request * request)
{
    UINT temp = 0;
    f_lseek(&File, request->dataOffset);
    f_read(&File, request->buffer, request->bufferSize, &temp);
}

void main(void) {
    os_init();
```
  
Creates an image loader which stores the state of the image being loaded.
Ignoring the file-system specific code, init_bmp takes the loader, the data function and the cache size.
It instantiates the loader, loads in the image headers (containing size, etc), and allocates the cache.
It then uses LCD specific code and a bounding box to draw the image on the screen.
  
After init_bmp, it's safe to call bmp_next_row up up to a maximum of the image's height times.
```
    bmp_image_loader_state imageState;
    rectangle rect = {0, 0, 0, 0};

		f_mount(&FatFs, "", 0);
		f_open(&File,"tuxsimp", FA_READ);	
    status_t stat = init_bmp(&imageState, read_image_bytes, IMAGE_CACHE_SIZE);
    if(stat == STATUS_OK) {
      rect.right = imageState.dibHeader.imageWidth;
      rect.bottom = imageState.dibHeader.imageHeight;
      display_segment_bmp(0, 0, &rect, &imageState);
    }
		f_close(&File);

}
```

