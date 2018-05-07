#include <stdlib.h>
#include "bmp.h"

inline size_t calc_row_size(bmp_info_header * dibHeader) {
  /* Weird formula because BMP row sizes are padded up to a multiple of 4 bytes. */
  return (((dibHeader->bitsPerPixel * dibHeader->imageWidth) + 31) / 32) * 4;
}

/* Load in the headers and data necessary to start retrieving image data. */
/* Grossly violates strict aliasing, so compile without it. */
status_t init_bmp(bmp_image_loader_state * loaderState, bmp_need_more_bytes dataRetrievalFunc, uint16_t maxCache)
{
  bmp_data_request data_request;

  loaderState->data_request_func = dataRetrievalFunc;

  /* Load in BMP file header */
  data_request.buffer = (void *)&loaderState->fileHeader;
  data_request.bufferSize = sizeof(bmp_file_header);
  data_request.dataOffset = 0;

  dataRetrievalFunc(&data_request);

  /* Load in BMP DIB header
     For now, only support BMPINFOHEADER, as CORE doesn't have 16 bit color. */
  data_request.buffer = (void *)&loaderState->dibHeader;
  data_request.bufferSize = sizeof(bmp_info_header);
  data_request.dataOffset = sizeof(bmp_file_header);
  dataRetrievalFunc(&data_request);

  /* Calculating file constants */
  loaderState->currentRow = 0;
  loaderState->rowSize = calc_row_size(&loaderState->dibHeader);
  loaderState->endOfImage = (loaderState->fileHeader.imageDataOffset + (uint32_t)loaderState->rowSize * loaderState->dibHeader.imageHeight);
  
  /* Initialising the cache. Cache speeds up rendering when loading from block devices. */
  loaderState->cachedRows = 0;
  loaderState->cacheSizeRows = maxCache/loaderState->rowSize;
  loaderState->cacheSizeBytes = loaderState->cacheSizeRows * loaderState->rowSize;
  loaderState->imageData = (void *)malloc(loaderState->cacheSizeBytes);

  if(loaderState->imageData == NULL) {
    return STATUS_OUT_OF_MEMORY;
  }

  return STATUS_OK;
}

/* Loads the row specified by currentRow of the loader state into the buffer 
   And increments currentRow.
*/
status_t bmp_next_row(bmp_image_loader_state * loaderState) 
{
  if(loaderState->cachedRows == 0)
  {
    bmp_data_request data_request;

    data_request.buffer = (void*)loaderState->imageData;
    data_request.bufferSize = loaderState->cacheSizeBytes;
    /* BMP stores image data backwards, counting back to the row we want to start the read at. */
    data_request.dataOffset = loaderState->endOfImage 
                              - (uint32_t)loaderState->rowSize * (uint32_t)(loaderState->currentRow + loaderState->cacheSizeRows);

    loaderState->data_request_func(&data_request);
    loaderState->cachedRows = loaderState->cacheSizeRows;
    /* Move the row pointer forward to the start of the last row **/
    loaderState->rowData = loaderState->imageData + (loaderState->rowSize >> 1) * (loaderState->cacheSizeRows - 1);
  }
  else
  {
    /* Moving down a row (which is backwards in memory)
       RowData is in 2 byte chunks (uint16), rowSize in bytes. Divide by 2. */
    loaderState->rowData -= loaderState->rowSize >> 1;
  }
  --loaderState->cachedRows;
  loaderState->currentRow += 1;
  return STATUS_OK;
}

