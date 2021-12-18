#include <stdio.h> // fopen, fread, ...
#include <stdlib.h> // malloc, free, ...
#include <string.h> // for memcmp
#include <stdint.h> // for uint8_t

typedef uint8_t ubyte;

struct Pixel_arr {
    ubyte* data;
    size_t width;
    size_t height;
    ubyte format;
};

typedef struct Pixel_arr PixelArray;

int main()
{
    FILE* fp = fopen("Font.tga", "rb");
    FILE* outfile = NULL;
    if (fp == NULL) {
        fprintf(stderr, "Cannot open File, please check your file name and directory.\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    // errno;

    fseek(fp, 0, SEEK_SET);
    ubyte* data = (ubyte*)malloc(fsize);

    const ubyte tga_sig[7] = {0, 0, 2, 0, 0, 0, 0};
    const size_t tga_header_size = 18;
    PixelArray pix;
    pix.data = NULL;

    int res = fread(data, 1, fsize, fp);
    if (res < fsize) {
        fprintf(stderr, "Invalid TGA file, sizing is incorrect.\n");
        goto done;
    }

    res = memcmp(tga_sig, data, sizeof(tga_sig));
    if (res != 0) {
        fprintf(stderr, "Invalid TGA file, please check file.\n");
        goto done;
    }

    pix.width = data[12] + (data[13] << 8);
    pix.height = data[14] + (data[15] << 8);
    pix.format = data[16];
    int data_type_field = data[3];

    if (pix.format != 24) {
        fprintf(stderr, "Unsupported TGA file format, this program only support 24-bit image files.\n");
        goto done;
    }

    size_t image_size = pix.width * pix.height * pix.format / 8;
    if ((tga_header_size + image_size) > fsize) {
        fprintf(stderr, "Invalid TGA file size.\n");
        goto done;
    }

    pix.data = (ubyte*)malloc(image_size);
    memcpy(pix.data, data + 18, image_size);

    ubyte buffer[18] = {};
    memcpy(buffer, tga_sig, sizeof(tga_sig));
    buffer[12] = pix.width;
    buffer[13] = pix.width >> 8;
    buffer[14] = pix.height;
    buffer[15] = pix.height >> 8;
    buffer[16] = pix.format;

    outfile = fopen("./test.tga", "wb");
    fwrite(buffer, 1, sizeof(buffer), outfile);
    fwrite(pix.data, 1, image_size, outfile);

    done:
    fclose(outfile);

    free(pix.data);
    free(data);
    fclose(fp);
    return 0;
}