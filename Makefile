CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

SOURCE = src/*.c
BIN = bin/main

all: $(BIN)

$(BIN): $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -DDEBUG -g -o $(BIN)

test:
	$(CC) $(SOURCE) $(CFLAGS) -o $(BIN)
	@./script.sh 2>/dev/null

clean:
	rm bin/main