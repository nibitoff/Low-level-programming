#include "../include/bmp_functions.h"
#include "../include/rotate.h"
#include "../include/util.h"


bool rotate_bmp(FILE *input,FILE *output){
    struct image orig;
    struct image trans;

    if (from_bmp(input, &orig)) {
        error_log("ERROR! Couldn't convert file from BMP!\n");
    }
    fprintf(stdout, "File successfully converted!\n");

    trans = rotate_image(&orig);
    if(to_bmp(output,&trans)){
        error_log("ERROR! Couldn't convert transformed picture to BMP\n");
    }

    fprintf(stdout, "Successfully transformed picture by 90 degrees!\n");
    image_clear(&orig);
    image_clear(&trans);
    return true;
}


