#include "helpers.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>

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

typedef struct {
  ppm_pixel *data;
  int width;
  int start_row;
  int end_row;
} ThreadData;

void *apply_filter_thread(void *arg) {
  ThreadData *threadData = (ThreadData *)arg;

  for (int y = threadData->start_row; y < threadData->end_row; y++) {
    for (int x = 0; x < threadData->width; x++) {
      double red = 0.0, green = 0.0, blue = 0.0;

      for (int filterY = 0; filterY < filterHeigth; filterY++) {
        for (int filterX = 0; filterX < filterWidth; filterX++) {
          int imageX = x - filterWidth / 2 + filterX;
          int imageY = y - filterHeigth / 2 + filterY;

          // Ensure that imageX and imageY are within bounds
          imageX = fmin(fmax(imageX, 0), threadData->width - 1);
          imageY = fmin(fmax(imageY, 0), threadData->end_row - 1);

          red += threadData->data[imageY * threadData->width + imageX].red *
                 filter[filterY][filterX];
          green += threadData->data[imageY * threadData->width + imageX].green *
                   filter[filterY][filterX];
          blue += threadData->data[imageY * threadData->width + imageX].blue *
                  filter[filterY][filterX];
        }
      }

      threadData->data[y * threadData->width + x].red =
          min(max((int)(factor * red + bias), 0), 255);
      threadData->data[y * threadData->width + x].green =
          min(max((int)(factor * green + bias), 0), 255);
      threadData->data[y * threadData->width + x].blue =
          min(max((int)(factor * blue + bias), 0), 255);
    }
  }
  pthread_exit(NULL);
}
int main(int argc, char **argv) {
  if (argc < 4) {
    fprintf(
        stderr,
        "Usage: ./convolution_pthreads <in_file> <out_file> <no_threads>\n");
    return 1;
  }

  ppm_image *image = read_ppm(argv[1]);
  int NUM_THREADS = atoi(argv[3]);

  pthread_t threads[NUM_THREADS];
  ThreadData threadData[NUM_THREADS];

  int rows_per_thread = image->heigth / NUM_THREADS;
  int start_row = 0;

  for (int i = 0; i < NUM_THREADS; i++) {
    threadData[i].data = image->data;
    threadData[i].width = image->width;
    threadData[i].start_row = start_row;
    threadData[i].end_row =
        (i == NUM_THREADS - 1) ? image->heigth : start_row + rows_per_thread;

    pthread_create(&threads[i], NULL, apply_filter_thread,
                   (void *)&threadData[i]);

    start_row += rows_per_thread;
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  write_ppm(image, argv[2]);

  return 0;
}
