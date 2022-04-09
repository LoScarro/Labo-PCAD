SOURCE	= matrix.c
HEADER	= #include <stdio.h> #include <stdlib.h> #include <time.h> #include <pthread.h>
OUT		= matrix
CC		= gcc
EX		= ./
FLAGS	= -lpthread

all: $(OBJECTS)
	$(CC) $(SOURCE) -o $(OUT) $(FLAGS)
	$(EX)$(OUT)

clean:
	rm -f $(OUT)