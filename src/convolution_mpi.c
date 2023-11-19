#include "../include/helpers.h"
#include <mpi/mpi.h>
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

double factor = 1.0; // used for scaling the colors
double bias = 0;     // used for adding brightness

double filter[filterWidth][filterHeigth] = {
    {0, 0.2, 0}, {0.2, 0.2, 0.2}, {0, 0.2, 0}};

ppm_image *apply_filter_local(ppm_pixel *local_data, int width, int heigth) {
  ppm_image *local_result = (ppm_image *)malloc(sizeof(ppm_image));
  local_result->data = (ppm_pixel *)malloc(width * heigth * sizeof(ppm_pixel));
  local_result->width = width;
  local_result->heigth = heigth;

  for (int y = 0; y < heigth; y++) {
    for (int x = 0; x < width; x++) {
      double red = 0.0, green = 0.0, blue = 0.0;

      for (int filterY = 0; filterY < filterHeigth; filterY++) {
        for (int filterX = 0; filterX < filterWidth; filterX++) {
          int imageX = (x - filterWidth / 2 + filterX + width) % width;
          int imageY = (y - filterHeigth / 2 + filterY + heigth) % heigth;

          red += local_data[imageY * width + imageX].red *
                 filter[filterY][filterX];
          green += local_data[imageY * width + imageX].green *
                   filter[filterY][filterX];
          blue += local_data[imageY * width + imageX].blue *
                  filter[filterY][filterX];
        }
      }

      local_result->data[y * width + x].red =
          min(max((int)(factor * red + bias), 0), 255);
      local_result->data[y * width + x].green =
          min(max((int)(factor * green + bias), 0), 255);
      local_result->data[y * width + x].blue =
          min(max((int)(factor * blue + bias), 0), 255);
    }
  }

  return local_result;
}

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc < 3) {
    if (rank == 0) {
      fprintf(stderr, "Usage: mpiexec -n <num_processes> ./convolution "
                      "<in_file> <out_file>\n");
    }
    MPI_Finalize();
    return 1;
  }

  ppm_image *image;

  if (rank == 0) {
    // Read the image on process 0
    image = read_ppm(argv[1]);
  } else {
    image = (ppm_image *)malloc(sizeof(ppm_image));
  }

  // Broadcast the image dimensions to all processes
  MPI_Bcast(&image->width, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&image->heigth, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Calculate the number of rows each process will handle
  int rows_per_process = image->heigth / size;
  int start_row = rank * rows_per_process;
  int end_row =
      (rank == size - 1) ? image->heigth : start_row + rows_per_process;

  // Allocate memory for the local image data
  ppm_pixel *local_data = (ppm_pixel *)malloc(
      image->width * (end_row - start_row) * sizeof(ppm_pixel));

  // Scatter the image data to all processes
  MPI_Scatter(image->data, image->width * rows_per_process, MPI_UNSIGNED_CHAR,
              local_data, image->width * rows_per_process, MPI_UNSIGNED_CHAR, 0,
              MPI_COMM_WORLD);

  // Process local image data
  ppm_image *local_result =
      apply_filter_local(local_data, image->width, rows_per_process);

  // Gather the results to process 0
  MPI_Gather(local_result->data, image->width * rows_per_process,
             MPI_UNSIGNED_CHAR, image->data, image->width * rows_per_process,
             MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  // Process 0 writes the final result
  if (rank == 0) {
    ppm_image *result = (ppm_image *)malloc(sizeof(ppm_image));
    result->data =
        (ppm_pixel *)malloc(image->width * image->heigth * sizeof(ppm_pixel));
    result->width = image->width;
    result->heigth = image->heigth;
    result->data = image->data;
    write_ppm(result, argv[2]);
  }

  MPI_Finalize();

  return 0;
}
