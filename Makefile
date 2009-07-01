CC = gcc
CFLAGS = -Wall --std=c99 `sdl-config --cflags`
LIBS = `sdl-config --libs`
SRC = emul8r.c rom.c video.c chip8.c
OBJ = ${SRC:.c=.o}
OUT = emul8r

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

.c.o: emul8r.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm *.o
	rm $(OUT)
