OBJECTS	= matrix.o
SOURCE	= matrix.c
HEADER	= #include <stdio.h> #include <stdlib.h> #include <time.h> #include <pthread.h>
OUT		= matrix
CC		= gcc
EX		= ./
FLAGS	= -lpthread

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(OUT) $(FLAGS)
	$(EX)$(OUT)

bash: matrix.c
	$(CC) matrix.c

clean:
	rm -f $(OBJECTS) $(OUT)