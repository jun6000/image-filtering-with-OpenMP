CC = cc
CFLAGS = -g -Wall -fopenmp
TARGET = img-proc

all : $(TARGET)

$(TARGET) : main.o img_omp_functions.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o img_omp_functions.o

main.o : main.c img_omp.h
	$(CC) $(CFLAGS) -c main.c

img_omp_functions.o : img_omp_functions.c img_omp.h
	$(CC) $(CFLAGS) -c img_omp_functions.c

clean :
	$(RM) $(TARGET) main.o img_omp_functions.o