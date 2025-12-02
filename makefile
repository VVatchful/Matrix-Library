# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm

# Targets
all: matrix_test neural_network csv_test

matrix_test: matrix.c
	$(CC) $(CFLAGS) -o matrix_test matrix.c

neural_network: neural_network.c matrix.c
	$(CC) $(CFLAGS) -o neural_network neural_network.c

csv_test: csv_reader/csv_reader.c
	$(CC) $(CFLAGS) -o csv_test csv_reader/csv_reader.c

clean:
	rm -f matrix_test neural_network csv_test

.PHONY: all clean