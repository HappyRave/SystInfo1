TEST=test
CFLAGS=-Wall -Werror -ggdb
LDFLAGS=-lcunit
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

$(TEST): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

#test: $(TEST)
#	@LD_LIBRARY_PATH=$HOME/local/lib ./$(TEST) 

.PHONY: exec

