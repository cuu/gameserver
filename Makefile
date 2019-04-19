CC=gcc -g
CFLAGS=-I/usr/include/SDL2/ -I. -Ilibmill
##-lSDL2, -lSDL2_gfx, -lSDL2_ttf,-lSDL2_image,-lSDL2_mixer
LDFLAGS=-lSDL2 -lSDL2_ttf libmill/.libs/libmill.a
DEPS = 
OBJ = client.o gamethread.o pico8.o lisp_parser.o utils.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o client
