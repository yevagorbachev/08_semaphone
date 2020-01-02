ifeq ($(DEBUG), true)
	CC = gcc -g
else
	CC = gcc
endif

objects = control.o write.o

all: $(objects)
	$(CC) -o control control.o
	$(CC) -o write write.o

control.o: control.c control.h
	$(CC) -c control.c

write.o: write.c write.h control.h
	$(CC) -c write.c

clean:
	rm *.o