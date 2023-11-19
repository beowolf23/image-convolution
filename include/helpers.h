// Author: APD team, except where source was noted

#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>
#include <stdlib.h>

#define RGB_COMPONENT_COLOR 255
#define FILENAME_MAX_SIZE 50

typedef struct {
  unsigned char red, green, blue;
} ppm_pixel;

typedef struct {
  int width, heigth;
  ppm_pixel *data;
} ppm_image;

ppm_image *read_ppm(const char *filename);
void write_ppm(ppm_image *img, const char *filename);

#endif
