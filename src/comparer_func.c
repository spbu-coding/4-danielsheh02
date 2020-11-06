#include "comparer_lib.h"

long long int actual_file_size(FILE *file) {
    long long int size_file = 0;
    fseek(file, 0, SEEK_END);
    size_file = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size_file;
}

IMAGE_BITMAP_INFO_HEADER *decoding_image_bitmap_info_header(FILE *file, enum ERROR_BLOCK *type_of_error) {
    long long int size_file = actual_file_size(file);
    IMAGE_BITMAP_INFO_HEADER *image_bitmap_info_header = malloc(sizeof(IMAGE_BITMAP_INFO_HEADER));
    if (!image_bitmap_info_header) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return NULL;
    }
    image_bitmap_info_header->format = 0;
    image_bitmap_info_header->size_file = 0;
    image_bitmap_info_header->reserved1 = 0;
    image_bitmap_info_header->reserved2 = 0;
    image_bitmap_info_header->pixel_array_off_set = 0;
    image_bitmap_info_header->version_header = 0;
    image_bitmap_info_header->width = 0;
    image_bitmap_info_header->height = 0;
    image_bitmap_info_header->planes = 0;
    image_bitmap_info_header->bits_per_pixel = 0;
    image_bitmap_info_header->compression = 0;
    image_bitmap_info_header->size_pixels_file = 0;
    fread(&image_bitmap_info_header->format, sizeof(image_bitmap_info_header->format), 1, file);
    if (image_bitmap_info_header->format != 0x4D42) {
        *type_of_error = FORMAT_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->size_file, sizeof(image_bitmap_info_header->size_file), 1, file);
    if (size_file != image_bitmap_info_header->size_file) {
        *type_of_error = SIZE_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->reserved1, sizeof(image_bitmap_info_header->reserved1), 1, file);
    fread(&image_bitmap_info_header->reserved2, sizeof(image_bitmap_info_header->reserved2), 1, file);
    if (image_bitmap_info_header->reserved1 != 0 || image_bitmap_info_header->reserved2 != 0) {
        *type_of_error = RESERVED_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->pixel_array_off_set, sizeof(image_bitmap_info_header->pixel_array_off_set), 1,
          file);
    fread(&image_bitmap_info_header->version_header, sizeof(image_bitmap_info_header->version_header), 1, file);
    if (image_bitmap_info_header->version_header != 40) {
        *type_of_error = VERSION_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->width, sizeof(image_bitmap_info_header->width), 1, file);
    if (image_bitmap_info_header->width <= 0) {
        *type_of_error = WIDTH_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->height, sizeof(image_bitmap_info_header->height), 1, file);
    if (image_bitmap_info_header->height == 0) {
        *type_of_error = HEIGHT_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->planes, sizeof(image_bitmap_info_header->planes), 1, file);
    if (image_bitmap_info_header->planes != 1) {
        *type_of_error = PLANES_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->bits_per_pixel, sizeof(image_bitmap_info_header->bits_per_pixel), 1, file);
    if ((image_bitmap_info_header->bits_per_pixel != 8) && (image_bitmap_info_header->bits_per_pixel != 24)) {
        *type_of_error = BITS_PER_PIXEL_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->compression, sizeof(image_bitmap_info_header->compression), 1, file);
    if (image_bitmap_info_header->compression != 0) {
        *type_of_error = COMPRESSION_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->size_pixels_file, sizeof(image_bitmap_info_header->size_pixels_file), 1, file);
    if (image_bitmap_info_header->size_pixels_file != 0) {
        int size_only_pixels = image_bitmap_info_header->size_file - image_bitmap_info_header->pixel_array_off_set;
        if (image_bitmap_info_header->size_pixels_file != size_only_pixels) {
            *type_of_error = PIXELS_SIZE_ERROR;
            free(image_bitmap_info_header);
            return NULL;
        }
    }
    fread(&image_bitmap_info_header->hor_res_image, sizeof(image_bitmap_info_header->hor_res_image), 1, file);
    fread(&image_bitmap_info_header->ver_res_image, sizeof(image_bitmap_info_header->ver_res_image), 1, file);
    fread(&image_bitmap_info_header->number_of_colors, sizeof(image_bitmap_info_header->number_of_colors), 1, file);
    if (image_bitmap_info_header->number_of_colors == 0 && image_bitmap_info_header->bits_per_pixel == 8) {
        *type_of_error = TABLE_COLOR_ERROR1;
        free(image_bitmap_info_header);
        return NULL;
    }
    if (image_bitmap_info_header->number_of_colors > 256 && image_bitmap_info_header->bits_per_pixel == 8) {
        *type_of_error = TABLE_COLOR_ERROR2;
        free(image_bitmap_info_header);
        return NULL;
    }
    if (image_bitmap_info_header->number_of_colors != 0 && image_bitmap_info_header->bits_per_pixel == 24) {
        *type_of_error = TABLE_COLOR_ERROR3;
        free(image_bitmap_info_header);
        return NULL;
    }
    fread(&image_bitmap_info_header->number_of_important_colors,
          sizeof(image_bitmap_info_header->number_of_important_colors), 1, file);
    if (image_bitmap_info_header->number_of_important_colors > 256 && image_bitmap_info_header->bits_per_pixel == 8) {
        *type_of_error = TABLE_COLOR_ERROR4;
        free(image_bitmap_info_header);
        return NULL;
    }

    return image_bitmap_info_header;
}

int **decoding_image_pixels(FILE *file, int width, int height, int bits_per_pixel) {
    int bytes_per_pixel = bits_per_pixel / 8;
    height = abs(height);
    int **array_image_pixels = malloc(sizeof(int *) * height);
    if (!array_image_pixels) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return 0;
    }
    for (long long int i = 0; i < height; i++) {
        array_image_pixels[i] = malloc(sizeof(int) * width);
        if (!array_image_pixels[i]) {
            for (int j = 0; j < i; j++) {
                free(array_image_pixels[i]);
            }
            free(array_image_pixels);
            return 0;
        }
    }
    for (long long int i = 0; i < height; i++) {
        for (long long int j = 0; j < width; j++) {
            array_image_pixels[i][j] = 0;
            fread(&array_image_pixels[i][j], bytes_per_pixel, 1, file);
        }
    }
    return array_image_pixels;
}

int *decoding_image_color_table(FILE *file, int number_of_colors) {
    int size_table_colors = number_of_colors * BYTES_COLOR_TABLE;
    int *array_table_colors = malloc(sizeof(int) * size_table_colors);
    if (!array_table_colors) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return 0;
    }
    for (long long int i = 0; i < number_of_colors; i++) {
        array_table_colors[i] = 0;
        fread(&array_table_colors[i], BYTES_COLOR_TABLE, 1, file);
    }
    return array_table_colors;
}

IMAGE *decoding_image(FILE *file, enum ERROR_BLOCK *type_of_error) {
    IMAGE *image = calloc(1, sizeof(IMAGE));
    if (!image) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return NULL;
    }
    image->meta_data_header = decoding_image_bitmap_info_header(file, type_of_error);
    if (!image->meta_data_header) {
        free(image);
        return NULL;
    }
    if (image->meta_data_header->number_of_colors != 0 && image->meta_data_header->bits_per_pixel == 8) {
        image->color_table = decoding_image_color_table(file, image->meta_data_header->number_of_colors);
        if (!image->color_table) {
            free(image);
            return NULL;
        }
    }
    image->array_of_pixels = decoding_image_pixels(file, image->meta_data_header->width,
                                                   image->meta_data_header->height,
                                                   image->meta_data_header->bits_per_pixel);

    if (!image->array_of_pixels) {
        free(image);
        return NULL;
    }
    return image;
}

void free_image(IMAGE *image) {
    for (long long int i = 0; i < abs(image->meta_data_header->height); i++) {
        free(image->array_of_pixels[i]);
    }
    free(image->array_of_pixels);
    free(image->meta_data_header);
    free(image->color_table);
    free(image);
    image = NULL;
}

void comparer_pixels(IMAGE *image, IMAGE *image2) {
    int count = 0;
    if (image->meta_data_header->height > 0 && image2->meta_data_header->height > 0) {
        for (long long int i = 0; i < image->meta_data_header->height; i++) {
            for (long long int j = 0; j < image->meta_data_header->width; j++) {
                if (count < 100 && image->array_of_pixels[i][j] != image2->array_of_pixels[i][j]) {
                    if (image->meta_data_header->height > 0 && image2->meta_data_header->height > 0) {
                        fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                    }
                    count++;
                }
            }
        }
    } else if (image->meta_data_header->height < 0 && image2->meta_data_header->height < 0) {
        for (long long int i = 0; i < abs(image->meta_data_header->height); i++) {
            for (long long int j = 0; j < image->meta_data_header->width; j++) {
                if (count < 100 && image->array_of_pixels[abs(image->meta_data_header->height) - i - 1][j] !=
                                   image2->array_of_pixels[abs(image2->meta_data_header->height) - i - 1][j]) {
                    fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                    count++;
                }
            }
        }
    } else if (image->meta_data_header->height > 0 && image2->meta_data_header->height < 0) {
        for (long long int i = 0; i < image->meta_data_header->height; i++) {
            for (long long int j = 0; j < image->meta_data_header->width; j++) {
                if (count < 100 && image->array_of_pixels[i][j] !=
                                   image2->array_of_pixels[abs(image2->meta_data_header->height) - i - 1][j]) {
                    fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                    count++;
                }
            }
        }
    } else if (image->meta_data_header->height < 0 && image2->meta_data_header->height > 0) {
        for (long long int i = 0; i < image2->meta_data_header->height; i++) {
            for (long long int j = 0; j < image->meta_data_header->width; j++) {
                if (count < 100 && image->array_of_pixels[abs(image->meta_data_header->height) - i - 1][j] !=
                                   image2->array_of_pixels[i][j]) {
                    fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                    count++;
                }
            }
        }
    }
}

int start_comparer(char *name_file, char *name_file2, enum ERROR_BLOCK *type_of_error) {
    FILE *file;
    FILE *file2;
    if ((file = fopen(name_file, "rb")) == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists. ", name_file);
        return -1;
    }
    IMAGE *image = decoding_image(file, type_of_error);
    if (!image) {
        fprintf(stdout, "Error for a file named %s. ", name_file);
        fclose(file);
        return -1;
    }
    if ((file2 = fopen(name_file2, "rb")) == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists. ", name_file2);
        return -1;
    }
    IMAGE *image2 = decoding_image(file2, type_of_error);
    if (!image2) {
        fprintf(stdout, "Error for a file named %s. ", name_file2);
        fclose(file2);
        return -1;
    }
    if ((image->meta_data_header->width != image2->meta_data_header->width) ||
        (abs(image->meta_data_header->height) != abs(image2->meta_data_header->height))) {
        fprintf(stdout, "Images have different values of width or height. ");
        free_image(image);
        free_image(image2);
        fclose(file);
        fclose(file2);
        return -1;
    } else if ((image->meta_data_header->size_file != image2->meta_data_header->size_file)) {
        fprintf(stdout, "Images have different values of sizes. ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    } else if ((image->meta_data_header->size_pixels_file != image2->meta_data_header->size_pixels_file)) {
        fprintf(stdout, "Images have different values of pixels data size. ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    } else if ((image->meta_data_header->bits_per_pixel != image2->meta_data_header->bits_per_pixel)) {
        fprintf(stdout, "Images have different values of bits_per_pixel. ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    } else if ((image->meta_data_header->number_of_colors != image2->meta_data_header->number_of_colors)) {
        fprintf(stdout, "Images have different number of colors in the table. ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    } else {
        comparer_pixels(image, image2);
    }
    free_image(image);
    fclose(file);
    free_image(image2);
    fclose(file2);
    return 0;
}
