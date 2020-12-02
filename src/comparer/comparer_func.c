#include "comparer_lib.h"

long long int actual_file_size(FILE *file) {
    long long int size_file = 0;
    fseek(file, 0, SEEK_END);
    size_file = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size_file;
}

IMAGE_BITMAP_INFO_HEADER *decode_image_bitmap_info_header(FILE *file, enum ERROR_BLOCK *type_of_error) {
    long long int size_file = actual_file_size(file);
    IMAGE_BITMAP_INFO_HEADER *image_bitmap_info_header = malloc(sizeof(IMAGE_BITMAP_INFO_HEADER));
    if (!image_bitmap_info_header) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        *type_of_error = MEMORY_ERROR;
        return NULL;
    }
    char buffer[SIZE_OF_HEADER];
    fread(buffer, SIZE_OF_HEADER, 1, file);
    *image_bitmap_info_header = (IMAGE_BITMAP_INFO_HEADER) {0};
    image_bitmap_info_header->format = *(short *) (buffer + 0);
    if (image_bitmap_info_header->format != 0x4D42) {
        *type_of_error = FORMAT_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->size_file = *(int *) (buffer + 2);
    if (size_file != image_bitmap_info_header->size_file) {
        *type_of_error = SIZE_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->reserved1 = *(short *) (buffer + 6);
    image_bitmap_info_header->reserved2 = *(short *) (buffer + 8);
    if (image_bitmap_info_header->reserved1 != 0 || image_bitmap_info_header->reserved2 != 0) {
        *type_of_error = RESERVED_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->pixel_array_off_set = *(int *) (buffer + 10);
    image_bitmap_info_header->version_header = *(int *) (buffer + 14);
    if (image_bitmap_info_header->version_header != 40) {
        *type_of_error = VERSION_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->width = *(int *) (buffer + 18);
    if (image_bitmap_info_header->width <= 0) {
        *type_of_error = WIDTH_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->height = *(int *) (buffer + 22);
    if (image_bitmap_info_header->height == 0) {
        *type_of_error = HEIGHT_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->planes = *(short *) (buffer + 26);
    if (image_bitmap_info_header->planes != 1) {
        *type_of_error = PLANES_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->bits_per_pixel = *(short *) (buffer + 28);
    if ((image_bitmap_info_header->bits_per_pixel != 8) && (image_bitmap_info_header->bits_per_pixel != 24)) {
        *type_of_error = BITS_PER_PIXEL_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->compression = *(int *) (buffer + 30);
    if (image_bitmap_info_header->compression != 0) {
        *type_of_error = COMPRESSION_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }
    image_bitmap_info_header->size_pixels_file = *(int *) (buffer + 34);
    if (image_bitmap_info_header->size_pixels_file != 0) {
        int size_only_pixels = image_bitmap_info_header->size_file - image_bitmap_info_header->pixel_array_off_set;
        if (image_bitmap_info_header->size_pixels_file != size_only_pixels) {
            *type_of_error = PIXELS_SIZE_ERROR;
            free(image_bitmap_info_header);
            return NULL;
        }
    }
    image_bitmap_info_header->hor_res_image = *(int *) (buffer + 38);
    image_bitmap_info_header->ver_res_image = *(int *) (buffer + 42);
    image_bitmap_info_header->number_of_colors = *(int *) (buffer + 46);
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
    image_bitmap_info_header->ver_res_image = *(int *) (buffer + 50);
    if (image_bitmap_info_header->number_of_important_colors > 256 && image_bitmap_info_header->bits_per_pixel == 8) {
        *type_of_error = TABLE_COLOR_ERROR4;
        free(image_bitmap_info_header);
        return NULL;
    }
    return image_bitmap_info_header;
}

int **decode_image_pixels(FILE *file, int width, int height, int bits_per_pixel, enum ERROR_BLOCK *type_of_error) {
    int bytes_per_pixel = bits_per_pixel / 8;
    height = abs(height);
    int **array_image_pixels = malloc(sizeof(int *) * height);
    if (!array_image_pixels) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        *type_of_error = MEMORY_ERROR;
        return 0;
    }
    for (long long int i = 0; i < height; i++) {
        array_image_pixels[i] = malloc(sizeof(int) * width);
        if (!array_image_pixels[i]) {
            fprintf(stdout, "Cannot allocate memory for results string in %lld experiment\n", i);
            *type_of_error = MEMORY_ERROR;
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

int *decode_image_color_table(FILE *file, int number_of_colors, enum ERROR_BLOCK *type_of_error) {
    int size_table_colors = number_of_colors * BYTES_COLOR_TABLE;
    int *array_table_colors = malloc(sizeof(int) * size_table_colors);
    if (!array_table_colors) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        *type_of_error = MEMORY_ERROR;
        return 0;
    }
    for (long long int i = 0; i < number_of_colors; i++) {
        array_table_colors[i] = 0;
        fread(&array_table_colors[i], BYTES_COLOR_TABLE, 1, file);
    }
    return array_table_colors;
}

IMAGE *decode_image(FILE *file, enum ERROR_BLOCK *type_of_error) {
    IMAGE *image = calloc(1, sizeof(IMAGE));
    if (!image) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        *type_of_error = MEMORY_ERROR;
        return NULL;
    }
    image->meta_data_header = decode_image_bitmap_info_header(file, type_of_error);
    if (!image->meta_data_header) {
        free(image);
        return NULL;
    }
    if (image->meta_data_header->number_of_colors != 0 && image->meta_data_header->bits_per_pixel == 8) {
        image->color_table = decode_image_color_table(file, image->meta_data_header->number_of_colors, type_of_error);
        if (!image->color_table) {
            free(image);
            return NULL;
        }
    }
    image->array_of_pixels = decode_image_pixels(file, image->meta_data_header->width,
                                                 image->meta_data_header->height,
                                                 image->meta_data_header->bits_per_pixel, type_of_error);

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
    int image_height1 = image->meta_data_header->height, image_height2 = image2->meta_data_header->height;
    int image_width = image->meta_data_header->width;
    int height_abs = abs(image_height1);
    for (long long int i = 0; i < height_abs; i++) {
        for (long long int j = 0; j < image_width; j++) {
            long long int i_real1 = image_height1 > 0 ? i : -image_height1 - i - 1;
            long long int i_real2 = image_height2 > 0 ? i : -image_height2 - i - 1;
            if (image->array_of_pixels[i_real1][j] !=
                image2->array_of_pixels[i_real2][j]) {
                fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                count++;
                if (count > 100) {
                    break;
                }
            }
        }
        if (count > 100) {
            break;
        }
    }
}

int comparer_table(IMAGE *image, IMAGE *image2, enum ERROR_BLOCK *type_of_error) {
    for (long long int i = 0; i < image->meta_data_header->number_of_colors; i++) {
        if (image->color_table[i] != image2->color_table[i]) {
            *type_of_error = TABLE_COLOR_ERROR5;
            return -1;
        }
    }
    return 0;
}

int start_comparer(char *name_file, char *name_file2, enum ERROR_BLOCK *type_of_error) {
    FILE *file;
    FILE *file2;
    if ((file = fopen(name_file, "rb")) == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists. ", name_file);
        *type_of_error = OPEN_FILE_ERROR;
        return -1;
    }
    IMAGE *image = decode_image(file, type_of_error);
    if (!image) {
        fprintf(stdout, "Error for a file named %s. ", name_file);
        fclose(file);
        return -1;
    }
    if ((file2 = fopen(name_file2, "rb")) == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists. ", name_file2);
        *type_of_error = OPEN_FILE_ERROR;
        return -1;
    }
    IMAGE *image2 = decode_image(file2, type_of_error);
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
        if (!comparer_table(image, image2, type_of_error)) {
            comparer_pixels(image, image2);
        } else {
            free_image(image);
            fclose(file);
            free_image(image2);
            fclose(file2);
            return -1;
        }
    }
    free_image(image);
    fclose(file);
    free_image(image2);
    fclose(file2);
    return 0;
}