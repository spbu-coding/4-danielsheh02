#include "comparer_lib.h"

int main(int argc, char **argv) {
    enum ERROR_BLOCK type_of_error = NO_ERROR;
    if (argc != 3) {
        fprintf(stdout, "Use %s <file1_name>.bmp <file2_name>.bmp\n", argv[0]);
        return -1;
    }
    if (start_comparer(argv[1], argv[2], &type_of_error)) {
        switch (type_of_error) {
            case FORMAT_ERROR:
                fprintf(stdout,
                        "Error format. The converter only supports BMP format.");
                return -1;
            case SIZE_ERROR:
                fprintf(stdout, "Error size file. The file size in the metadata does not match the actual.");
                return -2;
            case RESERVED_ERROR:
                fprintf(stdout, "Error reserved. Reserved bytes must contain 0.");
                return -2;
            case VERSION_ERROR:
                fprintf(stdout, "Error version. The converter only supports BMP third versions.");
                return -2;
            case WIDTH_ERROR:
                fprintf(stdout, "Error width. Width is always a positive number.");
                return -2;
            case HEIGHT_ERROR:
                fprintf(stdout, "Error height. Height cannot be 0.");
                return -2;
            case PLANES_ERROR:
                fprintf(stdout, "Error planes. The number of planes must be 1.");
                return -2;
            case BITS_PER_PIXEL_ERROR:
                fprintf(stdout,
                        "Error bits per pixel error. The converter only supports 8 and 24 bits per pixel images.");
                return -2;
            case COMPRESSION_ERROR:
                fprintf(stdout, "Error compression. Only uncompressed images are supported");
                return -2;
            case PIXELS_SIZE_ERROR:
                fprintf(stdout,
                        "Error size pixels. The size of the pixel data recorded in the metadata does not match the actual size.");
                return -2;
            case TABLE_COLOR_ERROR1:
                fprintf(stdout,
                        "Error color table. According to the metadata, the number of colors value is 0, but 8 bits per pixel images always have a color table.");
                return -2;
            case TABLE_COLOR_ERROR2:
                fprintf(stdout,
                        "Error color table. According to the metadata, the number of colors value over 256, but 8 bits per pixel images always use no more 256 colors in the table.");
                return -2;
            case TABLE_COLOR_ERROR3:
                fprintf(stdout,
                        "Error color table. According to the metadata, the number of colors value is not 0, but 24 bits per pixel images never have a color table.");
                return -2;
            case TABLE_COLOR_ERROR4:
                fprintf(stdout,
                        "Error color table. According to the metadata, the number of important colors value over 256, but 8 bits per pixel images always use no more 256 of important colors in the table.");
                return -2;
            case TABLE_COLOR_ERROR5:
                fprintf(stdout,
                        "Error color table. Color tables do not match.");
                return -2;
            case MEMORY_ERROR:
                fprintf(stdout, "Memory error.");
                return -1;
            case OPEN_FILE_ERROR:
                fprintf(stdout, "Error open file.");
                return -1;
            default:
                fprintf(stdout, "Unsupported error.");
                return -1;
        }
    }
    return 0;
}

