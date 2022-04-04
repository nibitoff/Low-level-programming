#include "../include/bmp_functions.h"
#define COUNTER 1

static uint64_t get_padding(uint64_t width) {
    return (4 - width * sizeof(struct pixel) % 4) % 4;
}

//functions for read
static enum status_read read_header(FILE *file, struct bmp_header *header);
static enum status_read read_bmp_pixels(FILE *file, struct image *image);


static enum status_read read_bmp_pixels(FILE *file, struct image *image) {
    size_t const size = (image -> width) * sizeof(struct pixel);
    size_t const padding = get_padding(image->width);
    for (size_t i = 0; i < image->height; i++) {
        size_t first_res =fread(image->data + i * (image->width), size, COUNTER, file);
        size_t second_res =fseek(file, padding, SEEK_CUR);
        if ((first_res != COUNTER) || (second_res != 0)) {
            return READ_FAIL;
        }
    }
    return READ_OK;
}

static enum status_read read_header(FILE *file, struct bmp_header *header) {
    size_t res = fread(header, sizeof(struct bmp_header), COUNTER, file);
    if (res != COUNTER) {
        return READ_HEADER;
    }
    return READ_OK;
}


//functions for write
static struct bmp_header create_bmp_header(struct image const *image);
static enum status_write write_bmp_pixels(FILE *out, struct image const *image);

static enum status_write write_bmp_pixels(FILE *file_out, struct image const *image) {
    uint32_t const padding_val = 0;
    uint64_t const padding = get_padding(image -> width);
    for (size_t i = 0; i < image -> height; ++i) {
        size_t first_res = fwrite(image->data + i * image->width, image->width * sizeof(struct pixel), COUNTER, file_out);
        size_t second_res = fwrite(&padding_val, padding, COUNTER, file_out );
        if (first_res != COUNTER || second_res != COUNTER) {
            return PARSE_ERROR;
        }
    }
    return PARSE_OK;
}

enum status_write to_bmp(FILE *out, struct image const *image){
    struct bmp_header header = create_bmp_header(image);
    if (fwrite(&header,sizeof(struct bmp_header), COUNTER, out) != 1){
        return PARSE_ERROR;
    }
    fseek(out, header.bOffBits, SEEK_SET);
    if (write_bmp_pixels(out, image)){
        return PARSE_ERROR;
    }
    return PARSE_OK;
}

static struct bmp_header create_bmp_header(struct image const *image) {
    struct bmp_header header = {0};
    size_t pad = get_padding(image -> width);
    header.bfType = 0x4D42;
    header.bfileSize = header.bOffBits + header.biSizeImage;;
    header.bfReserved = 0  ;
    header.bOffBits = sizeof (struct bmp_header);
    header.biSize = 40;
    header.biWidth = image -> width;
    header.biHeight = image -> height;
    header.biPlanes = 1;
    header.biBitCount = 24;
    header.biCompression = 0;
    header.biSizeImage = image -> height * (image -> width * sizeof(struct pixel) + pad);;
    header.biXPelsPerMeter = 0;
    header.biYPelsPerMeter = 0;
    header.biClrUsed = 0;
    header.biClrImportant = 0;

    return header;

}

enum status_read from_bmp(FILE *file, struct image *image) {
    if (!(file || image)) {
        return READ_NULL;
    }
    struct bmp_header header = {0};
    if (read_header(file, &header)) {
        return  READ_FAIL;
    }
    fseek(file, header.bOffBits, SEEK_SET);
    if (create_image(image, header.biWidth, header.biHeight)) {
        return READ_FAIL;
    }
    if(read_bmp_pixels(file, image)){
        return READ_FAIL;
    }
    return READ_OK;
}


