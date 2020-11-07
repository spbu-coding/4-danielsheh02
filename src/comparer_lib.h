#ifndef CONVERTER_H
#define CONVERTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTES_COLOR_TABLE 4

enum ERROR_BLOCK {
    NO_ERROR,
    FORMAT_ERROR,
    SIZE_ERROR,
    RESERVED_ERROR,
    VERSION_ERROR,
    WIDTH_ERROR,
    HEIGHT_ERROR,
    PLANES_ERROR,
    BITS_PER_PIXEL_ERROR,
    COMPRESSION_ERROR,
    PIXELS_SIZE_ERROR,
    TABLE_COLOR_ERROR1,
    TABLE_COLOR_ERROR2,
    TABLE_COLOR_ERROR3,
    TABLE_COLOR_ERROR4,
    TABLE_COLOR_ERROR5
};

typedef struct image_bitmap_info_header {
    short format;
    int size_file;
    short reserved1;//must be 0
    short reserved2;//must be 0
    int pixel_array_off_set;
    int version_header;
    int width;
    int height;
    short planes; //must be 1
    short bits_per_pixel;
    int compression;
    int size_pixels_file; //may be 0
    int hor_res_image;
    int ver_res_image;
    int number_of_colors;
    int number_of_important_colors;
} IMAGE_BITMAP_INFO_HEADER;

typedef struct image {
    IMAGE_BITMAP_INFO_HEADER *meta_data_header;
    int *color_table;
    int **array_of_pixels;
} IMAGE;

int start_comparer(char *name_file, char *name_file2, enum ERROR_BLOCK *type_of_error);

#endif