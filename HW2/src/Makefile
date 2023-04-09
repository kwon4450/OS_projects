CC=gcc
CFLAGS=-O2
OBJS=sudoku.o
TARGET=sudoku.app

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

sudoku.o: sudoku.c

clean:
	rm -f *.o $(TARGET)
