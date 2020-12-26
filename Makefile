CC = clang
CFLAGS = -Wall 
LDFLAGS = -l SDL2-2.0.0
DEPS = cpulib.h

OBJ = cpulib.o c8_main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

chip8-emulator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.o chip8-emulator
