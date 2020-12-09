CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

SOURCE = src/*.c
BIN = bin/ifj20
ZIP = xkoprd00.zip

all: $(BIN)

$(BIN): $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -o $(BIN)

debug:
	$(CC) $(SOURCE) $(CFLAGS) -DDEBUG -g -o $(BIN)

zip:
	zip -j $(ZIP) src/* doc/* Makefile rozdeleni 

clean:
	rm bin/ifj20