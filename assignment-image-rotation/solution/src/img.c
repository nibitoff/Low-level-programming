#include "../include/img.h"

bool create_image(struct image* image,uint64_t width,uint64_t height){
    image -> height = height;
    image -> width = width;
    image -> data = malloc(sizeof(struct pixel) * width * height);
    return image->data == NULL;
}

void image_clear(struct image* image){
    free(image->data);
}
