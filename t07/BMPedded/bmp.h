/**
  Library for reading BMP images and displaying them on LaFortuna
**/

#ifndef BMP_IMAGE_HEADER
#define BMP_IMAGE_HEADER

#include <stdint.h>
#include <string.h>

typedef enum {READ=0,SKIP=1} data_read_type_t;

typedef struct {
  /** Buffer to load data into. */
  void * buffer;
  size_t bufferSize;
  /** Offset of the data requested from the beginning of the image file */
  uint32_t dataOffset;
} bmp_data_request;

typedef void (*bmp_need_more_bytes)(bmp_data_request * request);

typedef enum {STATUS_OK=0, STATUS_OUT_OF_MEMORY} status_t; 

typedef struct __attribute__ ((packed)) {
  uint16_t fileIdentifier;
  uint32_t fileSize;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t imageDataOffset;
} bmp_file_header;

typedef struct __attribute__ ((packed)) {
  uint32_t headerSize;
  uint16_t imageWidth;
  uint16_t imageHeight;
  uint16_t colorPlanes;
  uint16_t bitsPerPixel;
} bmp_core_header;

typedef struct __attribute__ ((packed)) {
  uint32_t headerSize;
  int32_t imageWidth;
  int32_t imageHeight;
  uint16_t colorPlanes;
  uint16_t bitsPerPixel;
  uint32_t compressionMethod;
  uint32_t imageSize; /*Size of raw bitmap data*/
  int32_t horizontalRes;
  int32_t verticalRes;
  uint32_t colorsInPalette;
  uint32_t numImportantColors;
} bmp_info_header;

typedef struct {
  bmp_need_more_bytes data_request_func;
  /** Size of a row in bytes **/
  size_t rowSize;
  /** Current row, starting at 0 **/
  uint16_t currentRow;
  /** Number of rows currently cached **/
  uint8_t cachedRows;
  /** Cache Size in rows **/
  uint16_t cacheSizeRows;
  /** Cache Size in bytes **/
  uint32_t cacheSizeBytes;
  /** Loaded image data **/
  uint16_t * imageData;
  /** Current row data **/
  uint16_t * rowData;
  /** End of the image data in the file **/
  uint32_t endOfImage;
  bmp_file_header fileHeader;
  bmp_info_header dibHeader;
} bmp_image_loader_state;

/**
 * Initialises the image loader and loads in BMP files headers.
 * 
 * @param (Output) Image loader in which to store information.
 * @param (Input) Function to load in image data.
*/
status_t init_bmp(bmp_image_loader_state * loaderState, bmp_need_more_bytes dataRetrievalFunc, uint16_t maxCache);

/**
 * Loads in the next row of the image given by a particular loader
*/
status_t bmp_next_row(bmp_image_loader_state * loaderState);

#endif
