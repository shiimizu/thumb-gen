#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gd.h"

int main(int argc, char* argv[])
{
    FILE *fp, *fpp;
    gdImagePtr in, out, image_p;
    int ow, oh, nw, nh, width, height, file_type, jpg_quality;
    int reply = 1;

    const int MAX_W = 250;
    const int MAX_H = 250;
    const int MAXR_W = 125;
    const int MAXR_H = 125;

    /* Help */
    if (!(argc == 3 || argc == 4)) {
        printf("Generate thumbnails from 4chan\n");
        printf("%s  <input-file>  <output-file>  [reply?1:0]\n", argv[0]);
        return 1;
    }

    /* Reply Check */
    if (argc > 3) {
        reply = atoi(argv[3]);
    }
    if (reply <= 0) {
        width = MAX_W; //output width
        height = MAX_H; //output height
        jpg_quality = 50;
    }
    else {
        width = MAXR_W; //output width (imgreply)
        height = MAXR_H; //output height (imgreply)
        jpg_quality = 40;
    }

    /* Input */
    fp = fopen(argv[1], "rb");
    fpp = fopen(argv[1], "rb"); // A clone so fgetc doesn't consume the original
    if (!fp || !fpp) {
        fprintf(stderr, "An error occurred trying to open the file %s\n", argv[1]);
        return 3;
    }

    int i1 = fgetc(fpp);
    int i2 = fgetc(fpp);
    int i3 = fgetc(fpp);
    int i4 = fgetc(fpp);
    int i5 = fgetc(fpp);
    int i6 = fgetc(fpp);

    // https://en.wikipedia.org/wiki/Magic_number_%28programming%29
    if (i1 == 0x89 && i2 == 0x50) {
        // PNG: 89 50 4E 47 0D 0A 1A 0A
        // starts with 0x89 0x50
        // printf("PNG\n");
        in = gdImageCreateFromPng(fp);
        file_type = 3;
    }
    else if (i1 == 0x47 && i2 == 0x49 && i3 == 0x46 && i4 == 0x38 && i5 == 0x39 && i6 == 0x61) {
        // GIF: 47 49 46 38 39 61
        // printf("GIF\n");
        in = gdImageCreateFromGif(fp);
        file_type = 1;
    }
    else if (i1 == 0xFF && i2 == 0xD8) {
        // JPEG: FF D8
        // printf("JPG\n");
        in = gdImageCreateFromJpeg(fp);
        file_type = 2;
    }
    else {
        fclose(fpp);
        fclose(fp);
        fprintf(stderr, "Unsupported file.\n");
        return 1;
    }

    if (!in) {
        fprintf(stderr, "An error occurred trying to access the file %s\n", argv[1]);
        return 1;
    }

    ow = gdImageSX(in);
    oh = gdImageSY(in);

    /* Resize */
    if (ow > width || oh > height || file_type == 1) {
        double key_w = (double)width / ow;
        double key_h = (double)height / oh;
        double keys;
        if (key_w < key_h) {
            keys = key_w;
        }
        else {
            keys = key_h;
        }
        nw = floor(ow * keys);
        nh = floor(oh * keys);
    }
    else {
        nw = ow;
        nh = nh;
    }

    /*gdImageSetInterpolationMethod(in, GD_BILINEAR_FIXED);
      out = gdImageScale(in, w, h);
      if (!out) {
        fprintf(stderr, "gdImageScale err\n");
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
    fclose(fpp);
    fclose(fp);
    fp = fopen(argv[2], "wb");
    if (!fp) {
        fprintf(stderr, "Error saving image %s\n", argv[2]);
        return 1;
    }
    gdImageJpeg(image_p, fp, jpg_quality);
    fclose(fp);

    /* Cleanups */
    gdImageDestroy(in);
    gdImageDestroy(image_p);

    return 0;
}