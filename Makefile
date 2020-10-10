CC = gcc
CFLAGS = -Wall -Wextra

SOURCE = src/*.c
BIN = bin/main

all: $(BIN)

$(BIN): $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -DDEBUG -g -o $(BIN)

test:
	$(CC) $(SOURCE) $(CFLAGS) -o $(BIN)
	@./test.sh

clean:
	rm bin/main