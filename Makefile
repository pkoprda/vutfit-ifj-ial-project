CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

SOURCE = src/*.c
BIN = bin/ifj20
ZIP = xkoprd00.zip

all: $(BIN)

$(BIN): $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -DDEBUG -g -o $(BIN)

code:
	$(CC) $(SOURCE) $(CFLAGS) -o $(BIN)

test:
	$(CC) $(SOURCE) $(CFLAGS) -o $(BIN)
	@./script.sh

zip:
	zip -j $(ZIP) src/* Makefile

clean:
	rm bin/ifj20