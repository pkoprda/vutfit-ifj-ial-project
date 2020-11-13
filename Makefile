CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

SOURCE = src/*.c
BIN = bin/main
ZIP = xkoprd00.zip

all: $(BIN)

$(BIN): $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -DDEBUG -g -o $(BIN)

test:
	$(CC) $(SOURCE) $(CFLAGS) -o $(BIN)
	@./script.sh

zip:
	zip -j $(ZIP) src/* Makefile

clean:
	rm bin/main