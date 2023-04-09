CC=gcc
CFLAG=-O2
OBJS=main.o parser.o input.o command.o zombie.o
TARGET=osh.app

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

main.o: osh.h main.c
parse.o: osh.h parse.c
input.o: osh.h input.c
command.o: osh.h command.c
zombie.o: osh.h zombie.c

clean:
	rm -f $(OBJS) $(TARGET)