CC = gcc
MPICC = mpicc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS_OPENMP = -fopenmp
LDFLAGS_MPI = -lmpi -lm

SRCDIR = src
INCDIR = include
BINDIR = bin
INPUTS = inputs
OUTPUTS = outputs

SOURCES_OPENMP = $(wildcard $(SRCDIR)/convolution_openmp.c $(SRCDIR)/helpers.c)
SOURCES_PTHREADS = $(wildcard $(SRCDIR)/*_pthreads.c $(SRCDIR)/helpers.c)
SOURCES_MPI = $(wildcard $(SRCDIR)/*_mpi.c $(SRCDIR)/helpers.c)
SOURCES_MPI_OPENMP = $(wildcard $(SRCDIR)/*_mpi_openmp.c $(SRCDIR)/helpers.c)

EXECUTABLE_OPENMP = $(BINDIR)/convolution_openmp
EXECUTABLE_PTHREADS = $(BINDIR)/convolution_pthreads
EXECUTABLE_MPI = $(BINDIR)/convolution_mpi
EXECUTABLE_MPI_OPENMP = $(BINDIR)/convolution_mpi_openmp

all: openmp pthreads mpi

openmp: $(SOURCES_OPENMP)
	$(CC) $(CFLAGS) $(LDFLAGS_OPENMP) $(SOURCES_OPENMP) -o $(EXECUTABLE_OPENMP) -I$(INCDIR)

pthreads: $(SOURCES_PTHREADS)
	$(CC) $(CFLAGS) -pthread $(SOURCES_PTHREADS) -o $(EXECUTABLE_PTHREADS) -I$(INCDIR) -lm

mpi: $(SOURCES_MPI)
	$(MPICC) $(CFLAGS) $(LDFLAGS_MPI) $(SOURCES_MPI) -o $(EXECUTABLE_MPI) -I$(INCDIR)

mpi_openmp: $(SOURCES_MPI_OPENMP)
	$(MPICC) $(CFLAGS) $(LDFLAGS_OPENMP) $(LDFLAGS_MPI) $(SOURCES_MPI_OPENMP) -o $(EXECUTABLE_MPI_OPENMP) -I$(INCDIR)

clean:
	rm -f $(EXECUTABLE_OPENMP) $(EXECUTABLE_PTHREADS) $(EXECUTABLE_MPI)

run_openmp: openmp
	./$(EXECUTABLE_OPENMP) $(INPUTS)/leaf.ppm $(OUTPUTS)/out_openmp.ppm 4

run_pthreads: pthreads
	./$(EXECUTABLE_PTHREADS) $(INPUTS)/leaf.ppm $(OUTPUTS)/out_pthreads.ppm 4

run_mpi: mpi
	mpiexec -n 4 ./$(EXECUTABLE_MPI) $(INPUTS)/leaf.ppm $(OUTPUTS)/out_mpi.ppm

run_mpi_openmp: mpi_openmp
	mpiexec -n 4 ./$(EXECUTABLE_MPI_OPENMP) $(INPUTS)/leaf.ppm $(OUTPUTS)/out_mpi_openmp.ppm 2
