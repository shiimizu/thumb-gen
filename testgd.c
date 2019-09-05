#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
  FILE *fp;
  gdImagePtr in, out, image_p;
  int w, h;

  /* Help */
  if (argc<=4) {
    printf("%s  input.jpg  output.jpg  width  height\n", argv[0]);
    return 1;
  }

  /* Size */
  w = atoi(argv[3]);
  h = atoi(argv[4]);
  if (w<=0 || h<=0) {
    fprintf(stderr, "Bad size %dx%d\n", h, w);
    return 2;
  }

  /* Input */
  fp = fopen(argv[1], "rb");
  if (!fp) {
    fprintf(stderr, "Can't read image %s\n", argv[1]);
    return 3;
  }
  in = gdImageCreateFromJpeg(fp);
  fclose(fp);
  if (!in) {
    fprintf(stderr, "Can't create image from %s\n", argv[1]);
    return 4;
  }

  /* Resize */
  /*gdImageSetInterpolationMethod(in, GD_BILINEAR_FIXED);
  out = gdImageScale(in, w, h);
  if (!out) {
    fprintf(stderr, "gdImageScale fails\n");
    return 5;
  }*/

  
  image_p = gdImageCreateTrueColor(w,h);
  //gdImageCopyResized(image_p, in, 0, 0, 0, 0, w, h, 1095, 730);
  gdImageCopyResampled(image_p, in, 0, 0, 0, 0, w, h, 1095, 730);
  printf("in truecolor?: %d\n", gdImageTrueColor(in));
  printf("image_p truecolor?: %d\n", gdImageTrueColor(image_p));
  printf("in colors: %d\n", gdImageColorsTotal(in));
  printf("image_p colors: %d\n", gdImageColorsTotal(image_p));

  //imagecopyresampled($image_p, $image, 0, 0, 0, 0, $new_width, $new_height, $width, $height);

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