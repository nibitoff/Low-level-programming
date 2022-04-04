#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

struct pixel {
    uint8_t b, g, r;
};

#pragma pack(push, 1)
struct image {
    uint64_t width,height;
    struct pixel* data;
};
#pragma pack(pop)

bool create_image( struct image* image,uint64_t width,uint64_t height);
void image_clear( struct image* image );
