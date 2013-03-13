CC=cc
FILTER=threadpool
CFLAGS=-Wall -Werror -pthread
LDFLAGS=-Wall -Werror -pthread
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: threadpool

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

$(FILTER): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
