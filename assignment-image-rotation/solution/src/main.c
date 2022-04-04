#include "../include/rotation_check.h"
#include "../include/util.h"
int main( int argc, char** argv ) {
    if (argc != 3){
        error_log("Error! Check arguments! \n");
    }
    //initializing arguments
    FILE *input_file= NULL;
    FILE *output_file= NULL;

    //opening input file
    input_file= fopen(argv[1], "r");
    if (input_file == NULL){
        error_log("Error! Can't open this file :( \n");
    }

    //opening output file
    output_file= fopen(argv[2], "w");
    if (output_file == NULL){
        error_log("Error! Can't open this file :( \n");
    }

    //rotating the picture
    rotate_bmp(input_file, output_file);

    //closing pictures
    if (fclose(input_file) == EOF){
        error_log("Error! Problems with closing input file! \n");
    }

    if (fclose(output_file) == EOF){
        error_log("Error! Problems with closing output file! \n");
    }

    return 0;

}
