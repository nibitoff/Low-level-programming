#include "img.h"
#include <stdio.h>

#pragma pack(push, 1)
struct bmp_header
{
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)


//errors
enum status_read  {
    READ_OK = 0,
    READ_NULL,
    READ_FAIL,
    READ_HEADER
};

enum status_read from_bmp(FILE *file, struct image *image);

//errors
enum  status_write  {
    PARSE_OK = 0,
    PARSE_NULL,
    PARSE_ERROR
};

enum status_write to_bmp( FILE* out, struct image const* image );

