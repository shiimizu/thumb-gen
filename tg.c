#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    FILE *fp, *fpp;
    gdImagePtr in, out, image_p;
    int w, h;

    /* Help */
    if (argc == 5 || argc == 3) {
    }
    else {
        printf("%s  input.jpg  output.jpg  [new-width]  [new-height]\n", argv[0]);
        return 1;
    }

    /* Size */
    int ow, oh, nw, nh;
    if (argc == 5) {
        nw = atoi(argv[3]);
        nh = atoi(argv[4]);
        if (nw <= 0 || nh <= 0) {
            fprintf(stderr, "Bad size %dx%d\n", nw, nh);
            return 2;
        }
    }

    /* Input */
    fp = fopen(argv[1], "rb");
    fpp = fopen(argv[1], "rb");
    if (!fp) {
        fprintf(stderr, "Can't read image %s\n", argv[1]);
        return 3;
    }

    int i1 = fgetc(fpp);
    int i2 = fgetc(fpp);
    int i3 = fgetc(fpp);
    int i4 = fgetc(fpp);
    int i5 = fgetc(fpp);
    int i6 = fgetc(fpp);
    /*
    printf("%x\n",i1);
    printf("%x\n",fgetc(fp));
    printf("%x\n",0x23);*/
    // https://en.wikipedia.org/wiki/Magic_number_%28programming%29
    if (i1 == 0x89 && i2 == 0x50) {
        // printf("PNG\n" );
        // PNG: 89 50 4E 47 0D 0A 1A 0A
        // gd-jpeg: JPEG library reports unrecoverable error: Not a JPEG file:
        // starts with 0x89 0x50
        in = gdImageCreateFromPng(fp);
    }
    else if (i1 == 0x47 && i2 == 0x49 && i3 == 0x46 && i4 == 0x38 && i5 == 0x39 && i6 == 0x61) {
        // GIF: 47 49 46 38 39 61
        // printf("GIF\n" );
        in = gdImageCreateFromGif(fp);
    }
    else {
        // JPEG: FF D8
        // printf("JPG\n" );
        in = gdImageCreateFromJpeg(fp);
        // or crash lul
    }
    fclose(fpp);
    fclose(fp);
    if (!in) {
        fprintf(stderr, "Can't create image from %s\n", argv[1]);
        return 4;
    }
    ow = gdImageSX(in);
    oh = gdImageSY(in);
    if (argc == 3) {
        // Calculate aspect ratio
        if (ow == oh) {
            nw = 125;
            nh = 125;
        }
        else if (ow > oh) {
            nw = 125;
            nh = nw / ((float)ow / (float)oh);
        }
        else if (oh > ow) {
            nh = 125;
            nw = nh / ((float)oh / (float)ow);
        }
        else {
            fprintf(stderr, "Error determining image size\n");
            return 5;
        }
    }
    /* Resize */
    /*gdImageSetInterpolationMethod(in, GD_BILINEAR_FIXED);
      out = gdImageScale(in, w, h);
      if (!out) {
        fprintf(stderr, "gdImageScale fails\n");
        return 5;
      }*/

    image_p = gdImageCreateTrueColor(nw, nh);
    // gdImageCopyResized(image_p, in, 0, 0, 0, 0, nw, nh, ow, oh);
    gdImageCopyResampled(image_p, in, 0, 0, 0, 0, nw, nh, ow, oh);
    /*printf("in truecolor?: %d\n", gdImageTrueColor(in));
      printf("image_p truecolor?: %d\n", gdImageTrueColor(image_p));
      printf("in colors: %d\n", gdImageColorsTotal(in));
      printf("image_p colors: %d\n", gdImageColorsTotal(image_p));
      */
    // imagecopyresampled($image_p, $image, 0, 0, 0, 0, $new_width, $new_height, $width, $height);

    // Debug aspect ratio
    /*printf("%d x %d\n",ow,oh);
      printf("%d x %d\n",nw,nh);
      printf("%d x %d\n",gdImageSX(image_p),gdImageSY(image_p));*/

    /* Output */
    fp = fopen(argv[2], "wb");
    if (!fp) {
        fprintf(stderr, "Can't save image %s\n", argv[2]);
        return 6;
    }
    gdImageJpeg(image_p, fp, 40);
    fclose(fp);

    /* Cleanups */
    gdImageDestroy(in);
    gdImageDestroy(image_p);

    return 0;
}