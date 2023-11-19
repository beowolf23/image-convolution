#include "helpers.h"
#include <omp.h>
#include <stdio.h>

#define filterWidth 3
#define filterHeigth 3
#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })
#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })

#define filterWidth 9
#define filterHeigth 9

double filter[filterHeigth][filterWidth] = {
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
};

double factor = 1.0 / 9.0;
double bias = 0.0;

ppm_image *apply_filter(ppm_image *image) {

  ppm_image *result = (ppm_image *)malloc(sizeof(ppm_image));
  result->data =
      (ppm_pixel *)malloc(image->width * image->heigth * sizeof(ppm_pixel));
  result->width = image->width;
  result->heigth = image->heigth;

#pragma omp parallel for
  for (int y = 0; y < image->heigth; y++)
    for (int x = 0; x < image->width; x++) {
      double red = 0.0, green = 0.0, blue = 0.0;
      for (int filterY = 0; filterY < filterHeigth; filterY++)
        for (int filterX = 0; filterX < filterWidth; filterX++) {

          int imageX =
              (x - filterWidth / 2 + filterX + image->width) % image->width;

          int imageY =
              (y - filterHeigth / 2 + filterY + image->heigth) % image->heigth;

          red += image->data[imageY * image->width + imageX].red *
                 filter[filterX][filterY];
          green += image->data[imageY * image->width + imageX].green *
                   filter[filterX][filterY];
          blue += image->data[imageY * image->width + imageX].blue *
                  filter[filterX][filterY];
        }

      result->data[y * result->width + x].red =
          min(max((int)(factor * red + bias), 0), 255);
      result->data[y * result->width + x].green =
          min(max((int)(factor * green + bias), 0), 255);
      result->data[y * result->width + x].blue =
          min(max((int)(factor * blue + bias), 0), 255);
    }
  return result;
}

int main(int argc, char **argv) {

  if (argc < 4) {
    fprintf(stderr, "Usage: ./convolution <in_file> <out_file> <no_threads>\n");
    return 1;
  }

  omp_set_num_threads(atoi(argv[3]));

  ppm_image *image = read_ppm(argv[1]);
  ppm_image *result = apply_filter(image);
  write_ppm(result, argv[2]);

  return 0;
}
