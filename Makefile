# Define the compiler to use
CC = gcc

# Define any compile-time flags
CFLAGS = -Wall -g -pthread

# Define libraries to link against
LIBS = -lrt -pthread

# Define the target executables
all: process_a process_b

process_a: process_a.o
	$(CC) $(CFLAGS) process_a.o -o process_a $(LIBS)

process_b: process_b.o
	$(CC) $(CFLAGS) process_b.o -o process_b $(LIBS)

process_a.o: process_a.c
	$(CC) $(CFLAGS) -c process_a.c

process_b.o: process_b.c
	$(CC) $(CFLAGS) -c process_b.c

clean:
	rm -f *.o process_a process_b
