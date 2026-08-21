CC=gcc
CFLAGS=-Wall -pedantic -Wconversion
EXE=main
SRC=main.c

all: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXE)

clean:
	rm -rf $(EXE)
