#include <stdio.h> // fopen, fread, ...
#include <stdlib.h> // malloc, free, ...
#include <string.h> // for memcmp
#include <stdint.h> // for uint8_t

typedef uint8_t ubyte;

typedef struct {
    char  idlength;
    char  colourmaptype;
    char  datatypecode;
    short int colourmaporigin;
    short int colourmaplength;
    char  colourmapdepth;
    short int x_origin;
    short int y_origin;
    short width;
    short height;
    char  bitsperpixel;
    char  imagedescriptor;
} HEADER;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} PIXEL;

struct Pixel_arr {
    ubyte* data;
    size_t width;
    size_t height;
    ubyte format;
};

typedef struct Pixel_arr PixelArray;

void MergeBytes(PIXEL *pixel,unsigned char *p,int bytes)
{
    if (bytes == 4) {
        pixel->r = p[2];
        pixel->g = p[1];
        pixel->b = p[0];
        pixel->a = p[3];
    } else if (bytes == 3) {
        pixel->r = p[2];
        pixel->g = p[1];
        pixel->b = p[0];
        pixel->a = 255;
    } else if (bytes == 2) {
        pixel->r = (p[1] & 0x7c) << 1;
        pixel->g = ((p[1] & 0x03) << 6) | ((p[0] & 0xe0) >> 2);
        pixel->b = (p[0] & 0x1f) << 3;
        pixel->a = (p[1] & 0x80);
    }
}

int main()
{
    HEADER header;
    PIXEL *pixels;

    FILE* fp = fopen("Font.tga", "rb");
    FILE* outfile = NULL;
    if (fp == NULL) {
        fprintf(stderr, "Cannot open File, please check your file name and directory.\n");
        return -1;
    }

    printf("File Open Successful.\nPrinting file header information.\n");
    header.idlength = fgetc(fp);
    fprintf(stdout,"ID length:         %d\n",header.idlength);

    header.colourmaptype = fgetc(fp);
    fprintf(stdout,"Colourmap type:    %d\n",header.colourmaptype);

    header.datatypecode = fgetc(fp);
    fprintf(stdout,"Image type:        %d\n",header.datatypecode);

    fread(&header.colourmaporigin,2,1,fp);
    fprintf(stdout,"Colour map offset: %d\n",header.colourmaporigin);

    fread(&header.colourmaplength,2,1,fp);
    fprintf(stdout,"Colour map length: %d\n",header.colourmaplength);

    header.colourmapdepth = fgetc(fp);
    fprintf(stdout,"Colour map depth:  %d\n",header.colourmapdepth);

    fread(&header.x_origin,2,1,fp);
    fprintf(stdout,"X origin:          %d\n",header.x_origin);

    fread(&header.y_origin,2,1,fp);
    fprintf(stdout,"Y origin:          %d\n",header.y_origin);

    fread(&header.width,2,1,fp);
    fprintf(stdout,"Width:             %d\n",header.width);

    fread(&header.height,2,1,fp);
    fprintf(stdout,"Height:            %d\n",header.height);

    header.bitsperpixel = fgetc(fp);
    fprintf(stdout,"Bits per pixel:    %d\n",header.bitsperpixel);

    header.imagedescriptor = fgetc(fp);
    fprintf(stdout,"Descriptor:        %d\n",header.imagedescriptor);

    pixels = malloc(header.width*header.height*sizeof(PIXEL));

    for (int i=0;i<header.width*header.height;i++) {
        pixels[i].r = 0;
        pixels[i].g = 0;
        pixels[i].b = 0;
        pixels[i].a = 0;
//        printf("R: %d; G: %d; B: %d\n", pixels->r, pixels->g, pixels->b);
    }

    int num_bytes;
    int n = 0;
    unsigned char p[5];

    fseek(fp, 0, SEEK_SET);
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);

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

    if (pix.format != 24) {
        fprintf(stderr, "Unsupported TGA file format, this program only support 24-bit image files.\n");
        goto done;
    }

    printf("Starting Image manipulation........\n");
    int pixel_info_ticker;
    printf("Please Choose:\n[1]: With Pixel details\n[2]: Without Pixel details\n");
    scanf("%d", &pixel_info_ticker);

    int option;
    printf("Please Input Image Manipulation option\n");
    printf("Please Choose:\n"
           "[1]: Swap Red with Green\n"
           "[2]: Swap Red with Blue\n"
           "[3]: Swap Green with Blue\n"
           "[4]: Change Red to different color.\n"
           "[5]: Swap Specific Color with another Specific Color\n");
    scanf("%d", &option);

    int target_r, target_g, target_b, self_r, self_g, self_b;

    if (option == 4){
        printf("Enter a color code in RGB order that you wish to change red to:\nEx:lime: 0 255 0\n");
        scanf("%d %d %d", &target_r, &target_g, &target_b);
    }

    if (option == 5){
        printf("Enter a color code in RGB order that you wish to change:\nEx:lime: 0 255 0\n");
        scanf("%d %d %d", &self_r, &self_g, &self_b);
        printf("Enter a color code in RGB order that you wish to change the color to:\nEx:lime: 0 255 0\n");
        scanf("%d %d %d", &target_r, &target_g, &target_b);
    }

    if (pixel_info_ticker != 1 && pixel_info_ticker != 2){
        fprintf(stderr,"Invalid operand.\n");
        goto done;
    }
    if (option != 1 && option != 2 && option != 3 && option != 4 && option != 5){
        fprintf(stderr,"Invalid operand.\n");
        goto done;
    }

    size_t image_size = pix.width * pix.height * pix.format / 8;
    if ((tga_header_size + image_size) > fsize) {
        fprintf(stderr, "Invalid TGA file size.\n");
        goto done;
    }
    fseek(fp, 0, SEEK_SET);
    num_bytes = header.bitsperpixel / 8;
    while (n < header.width * header.height) {
        if (header.datatypecode == 2) {                     /* Uncompressed */
            if (fread(p, 1, num_bytes, fp) != num_bytes) {
                fprintf(stderr, "Unexpected end of file\n");
                exit(-1);
            }
            MergeBytes(&(pixels[n]), p, num_bytes);
            n++;
        }
    }

    if ((outfile = fopen("tgatest.tga","w")) == NULL) {
        fprintf(stderr,"Failed to open output file\n");
        exit(-1);
    }

    putc(0,outfile);
    putc(0,outfile);
    putc(2,outfile);                         /* uncompressed RGB */
    putc(0,outfile); putc(0,outfile);
    putc(0,outfile); putc(0,outfile);
    putc(0,outfile);
    putc(0,outfile); putc(0,outfile);           /* X origin */
    putc(0,outfile); putc(0,outfile);           /* y origin */
    putc((header.width & 0x00FF),outfile);
    putc((header.width & 0xFF00) / 256,outfile);
    putc((header.height & 0x00FF),outfile);
    putc((header.height & 0xFF00) / 256,outfile);
    putc(32,outfile);                        /* 24 bit bitmap */
    putc(0,outfile);
    int x =0, y=0;
    for (int i=0;i<header.height*header.width;i++) {

//        putc(pixels[i].b,outfile);
//        putc(pixels[i].g,outfile);
//        putc(pixels[i].r,outfile);
//        putc(pixels[i].a,outfile);
        if (option == 1){
            putc(pixels[i].b,outfile);
            putc(pixels[i].r,outfile);
            putc(pixels[i].g,outfile);
            putc(pixels[i].a,outfile);
        }
        else if (option == 2){
            putc(pixels[i].r,outfile);
            putc(pixels[i].g,outfile);
            putc(pixels[i].b,outfile);
            putc(pixels[i].a,outfile);
        }
        else if (option == 3){
            putc(pixels[i].g,outfile);
            putc(pixels[i].b,outfile);
            putc(pixels[i].r,outfile);
            putc(pixels[i].a,outfile);
        }
        else if (option == 4){
            if (pixels[i].r == 255 && pixels[i].g == 0 && pixels[i].b == 0){
                putc(target_b,outfile);
                putc(target_g,outfile);
                putc(target_r,outfile);
                putc(pixels[i].a,outfile);
            }
            else{
                putc(pixels[i].b,outfile);
                putc(pixels[i].g,outfile);
                putc(pixels[i].r,outfile);
                putc(pixels[i].a,outfile);
            }
        }
        else if (option == 5){
            if (pixels[i].r == self_r && pixels[i].g == self_g && pixels[i].b == self_b){
                putc(target_b,outfile);
                putc(target_g,outfile);
                putc(target_r,outfile);
                putc(pixels[i].a,outfile);
            }
            else{
                putc(pixels[i].b,outfile);
                putc(pixels[i].g,outfile);
                putc(pixels[i].r,outfile);
                putc(pixels[i].a,outfile);
            }
        }

        if (pixel_info_ticker == 1){
            printf("color at (%d, %d) [R: %d; G: %d; B: %d]\n", x, y,pixels[i].r, pixels[i].g, pixels[i].b);
            if (y == header.width){
                x++;
                y=0;
            }
            else{y++;}
        }
    }
    goto finish;

//    pix.data = (ubyte*)malloc(image_size);
//    memcpy(pix.data, data + 18, image_size);
//
//    ubyte buffer[18] = {};
//    memcpy(buffer, tga_sig, sizeof(tga_sig));
//    buffer[12] = pix.width;
//    buffer[13] = pix.width >> 8;
//    buffer[14] = pix.height;
//    buffer[15] = pix.height >> 8;
//    buffer[16] = pix.format;
//
//    outfile = fopen("./test.tga", "wb");
//    fwrite(buffer, 1, sizeof(buffer), outfile);
//    fwrite(pix.data, 1, image_size, outfile);
//    goto finish;

    finish:
    printf("Process finished, Thank you for using our service.\n");

    done:
    fclose(outfile);

    free(pix.data);
    free(data);
    fclose(fp);
    return 0;
}