# Image Convolution with PThreads, MPI and OpenMP

The provided solution implements image convolution using parallel computing paradigms, specifically OpenMP and MPI, in the C programming language. Image convolution is a fundamental operation in image processing, involving the application of a filter or kernel to each pixel of an image to produce a modified version of the image.

## Solution Components

### Image Convolution Functions
- The core functionality is implemented in separate C files for each parallelization strategy: `convolution_openmp.c`, `convolution_pthreads.c`, `convolution_mpi.c`, and `convolution_mpi_openmp.c`.
- These files contain functions to perform image convolution with their respective parallelization techniques.

### Helpers Functions
- The `helpers.c` file provides utility functions for reading and writing PPM (Portable Pixmap) images, as well as other helper functions used in the convolution process.

### Makefile
- The Makefile defines compilation rules for each version of the code, making it easy to compile and run different implementations.

### Input and Output Directories
- The `inputs` directory contains sample PPM images for testing the convolution algorithms.
- The `outputs` directory is used to store the resulting images after convolution.

## Parallelization Strategies

### OpenMP Version
- The `convolution_openmp.c` file utilizes OpenMP directives to parallelize the convolution process across multiple threads.
- The Makefile includes a target `run_openmp` to compile and execute the OpenMP version with a default input image.

### Pthreads Version
- The `convolution_pthreads.c` file uses the Pthreads (POSIX Threads) library to implement parallelization through threads.
- The Makefile includes a target `run_pthreads` to compile and execute the Pthreads version.

### MPI Version
- The `convolution_mpi.c` file employs the Message Passing Interface (MPI) for parallelization across multiple processes.
- The Makefile includes a target `run_mpi` to compile and execute the MPI version.

### MPI with OpenMP Version
- The `convolution_mpi_openmp.c` file combines both MPI and OpenMP for parallelization.
- The Makefile includes a target `run_mpi_openmp` to compile and execute the MPI with OpenMP version.

## Directory Structure
|-- bin
|-- include
|-- inputs
|-- outputs
|-- src
| |-- convolution_openmp.c
| |-- convolution_pthreads.c
| |-- convolution_mpi.c
| |-- convolution_mpi_openmp.c
| |-- helpers.c
|-- Makefile
|-- README.md

- **bin**: Executables will be stored here after compilation.
- **include**: Header files.
- **inputs**: Input images for testing.
- **outputs**: Output images after convolution.
- **src**: Source code files.
- **Makefile**: Compilation script.
- **README.md**: Project documentation.

## Compilation

The project uses a Makefile for easy compilation. To compile all versions (OpenMP, Pthreads, MPI, MPI with OpenMP), run the following command:

```bash
make all

To compile specific versions, you can use individual targets: openmp, pthreads, mpi, mpi_openmp.

## Running the Code
OpenMP
make run_openmp
This command will execute the OpenMP version with the default input image (inputs/leaf.ppm) and store the result in outputs/out_openmp.ppm.

Pthreads
 - make run_pthreads
This command will execute the Pthreads version with the default input image and store the result in outputs/out_pthreads.ppm.

MPI
make run_mpi
This command will execute the MPI with OpenMP version with 4 processes and 2 OpenMP threads per process. The result will be stored in outputs/out_mpi_openmp.ppm.

Make sure to replace the placeholder values (`4` processes, `2` threads) in the `mpi_openmp` target with appropriate values based on your system configuration.
