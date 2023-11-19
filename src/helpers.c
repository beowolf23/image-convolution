// Author: APD team, except where source was noted

#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Source: [1]
ppm_image *read_ppm(const char *filename) {
  char buff[16];
  ppm_image *img;
  FILE *fp;
  int c, rgb_comp_color;

  // open PPM file for reading
  fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  // read image format
  if (!fgets(buff, sizeof(buff), fp)) {
    perror(filename);
    exit(1);
  }

  // check the image format
  if (buff[0] != 'P' || buff[1] != '6') {
    fprintf(stderr, "Invalid image format (must be 'P6')\n");
    exit(1);
  }

  // alloc memory for image
  img = (ppm_image *)malloc(sizeof(ppm_image));
  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  // check for comments
  c = getc(fp);
  while (c == '#') {
    while (getc(fp) != '\n')
      ;

    c = getc(fp);
  }

  ungetc(c, fp);

  // read image size information
  if (fscanf(fp, "%d %d", &img->width, &img->heigth) != 2) {
    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
    exit(1);
  }

  // read RGB component
  if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
    fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
    exit(1);
  }

  // check RGB component depth
  if (rgb_comp_color != RGB_COMPONENT_COLOR) {
    fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
    exit(1);
  }

  while (fgetc(fp) != '\n')
    ;

  // memory allocation for pixel data
  img->data = (ppm_pixel *)malloc(img->width * img->heigth * sizeof(ppm_pixel));

  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  // read pixel data from file
  if ((int)fread(img->data, 3 * img->width, img->heigth, fp) != img->heigth) {
    fprintf(stderr, "Error loading image '%s'\n", filename);
    exit(1);
  }

  fclose(fp);
  return img;
}

// Source: [1]
void write_ppm(ppm_image *img, const char *filename) {
  FILE *fp;

  // open file for output
  fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  // write the header file image format
  fprintf(fp, "P6\n");

  // image size
  fprintf(fp, "%d %d\n", img->width, img->heigth);

  // RGB component depth
  fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

  // pixel data
  fwrite(img->data, 3 * img->width, img->heigth, fp);
  fclose(fp);
}
