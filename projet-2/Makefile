FILTER=filter
CFLAGS=-Wall -Werror -pthread -g
LDFLAGS=-Wall -Werror -g
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: filter

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

$(FILTER): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
