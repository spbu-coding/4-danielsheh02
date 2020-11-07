#include "converter_lib.h"

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

int *decoding_image_pixels(FILE *file, int width, int height, int bits_per_pixel) {
    int bytes_per_pixel = bits_per_pixel / 8;
    height = abs(height);
    int *array_image_pixels = malloc(sizeof(int) * width * height * bytes_per_pixel);
    if (!array_image_pixels) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return 0;
    }
    for (long long int i = 0; i < width * height * bytes_per_pixel; i++) {
        array_image_pixels[i] = 0;
        fread(&array_image_pixels[i], 1, 1, file);
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
    for (long long int i = 0; i < size_table_colors; i++) {
        array_table_colors[i] = 0;
        fread(&array_table_colors[i], 1, 1, file);
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
    image->array_of_rgb_pixels = decoding_image_pixels(file, image->meta_data_header->width,
                                                       image->meta_data_header->height,
                                                       image->meta_data_header->bits_per_pixel);

    if (!image->array_of_rgb_pixels) {
        free(image);
        return NULL;
    }
    return image;
}

void converting_negative(IMAGE *image_file) {
    for (long long int i = 0;
         i < image_file->meta_data_header->width * abs(image_file->meta_data_header->height) *
             (image_file->meta_data_header->bits_per_pixel / 8); i++) {
        image_file->array_of_rgb_pixels[i] = ~image_file->array_of_rgb_pixels[i];
    }
}

void converting_negative_if_table_color_exist(IMAGE *image_file) {
    for (long long int i = 0; i < image_file->meta_data_header->number_of_colors * BYTES_COLOR_TABLE; i++) {
        if ((i + 1) % 4 != 0) {
            image_file->color_table[i] = ~image_file->color_table[i];
        }
    }
}

void create_new_file(IMAGE *image_file, char *name_ot_put) {
    FILE *new_file = fopen(name_ot_put, "wb");
    fwrite(&image_file->meta_data_header->format, sizeof(image_file->meta_data_header->format), 1, new_file);
    fwrite(&image_file->meta_data_header->size_file, sizeof(image_file->meta_data_header->size_file), 1, new_file);
    fwrite(&image_file->meta_data_header->reserved1, sizeof(image_file->meta_data_header->reserved1), 1, new_file);
    fwrite(&image_file->meta_data_header->reserved2, sizeof(image_file->meta_data_header->reserved2), 1, new_file);
    fwrite(&image_file->meta_data_header->pixel_array_off_set,
           sizeof(image_file->meta_data_header->pixel_array_off_set), 1, new_file);
    fwrite(&image_file->meta_data_header->version_header, sizeof(image_file->meta_data_header->version_header), 1,
           new_file);
    fwrite(&image_file->meta_data_header->width, sizeof(image_file->meta_data_header->width), 1, new_file);
    fwrite(&image_file->meta_data_header->height, sizeof(image_file->meta_data_header->height), 1, new_file);
    fwrite(&image_file->meta_data_header->planes, sizeof(image_file->meta_data_header->planes), 1, new_file);
    fwrite(&image_file->meta_data_header->bits_per_pixel, sizeof(image_file->meta_data_header->bits_per_pixel), 1,
           new_file);
    fwrite(&image_file->meta_data_header->compression, sizeof(image_file->meta_data_header->compression), 1, new_file);
    fwrite(&image_file->meta_data_header->size_pixels_file, sizeof(image_file->meta_data_header->size_pixels_file), 1,
           new_file);
    fwrite(&image_file->meta_data_header->hor_res_image, sizeof(image_file->meta_data_header->hor_res_image), 1,
           new_file);
    fwrite(&image_file->meta_data_header->ver_res_image, sizeof(image_file->meta_data_header->ver_res_image), 1,
           new_file);
    fwrite(&image_file->meta_data_header->number_of_colors, sizeof(image_file->meta_data_header->number_of_colors), 1,
           new_file);
    fwrite(&image_file->meta_data_header->number_of_important_colors,
           sizeof(image_file->meta_data_header->number_of_important_colors), 1, new_file);
    if (image_file->meta_data_header->number_of_colors != 0 && image_file->meta_data_header->bits_per_pixel == 8) {
        for (long long int i = 0; i < image_file->meta_data_header->number_of_colors * BYTES_COLOR_TABLE; i++) {
            fwrite(&image_file->color_table[i], 1, 1, new_file);
        }
    }
    for (long long int i = 0;
         i < image_file->meta_data_header->width * abs(image_file->meta_data_header->height) *
             (image_file->meta_data_header->bits_per_pixel / 8); i++) {
        fwrite(&image_file->array_of_rgb_pixels[i], 1, 1, new_file);
    }
    fclose(new_file);
}

void free_image(IMAGE *image) {
    free(image->meta_data_header);
    free(image->color_table);
    free(image->array_of_rgb_pixels);
    free(image);
    image = NULL;
}

int mine_implementation(char *name_in_put, char *name_ot_put, enum ERROR_BLOCK *type_of_error) {
    FILE *file;
    if ((file = fopen(name_in_put, "rb")) == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists.", name_in_put);
        return -1;
    }
    IMAGE *image = decoding_image(file, type_of_error);
    if (!image) {
        fclose(file);
        return -1;
    }
    if (image->meta_data_header->number_of_colors != 0 && image->meta_data_header->bits_per_pixel == 8) {
        converting_negative_if_table_color_exist(image);
    } else {
        converting_negative(image);
    }
    create_new_file(image, name_ot_put);
    free_image(image);
    fclose(file);
    return 0;
}