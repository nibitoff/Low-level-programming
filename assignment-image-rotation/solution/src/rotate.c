#include "../include/img.h"
#include "../include/rotate.h"

struct image rotate_image(struct image *image){
    struct image image_out;
    uint64_t width = image -> width, height = image -> height;
    image_out.width = height;
    image_out.height = width;
    struct pixel *pixels = malloc(sizeof (struct pixel) * height * width);
    for (int i = 0; i < image -> height; i++){
        for (int j = 0; j< image -> width; j++){
            pixels[(height) * (j+1) - i - 1] = image -> data[(i * width) + j];
        }
    }
    image_out.data = pixels;
    return image_out;
}
