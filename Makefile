CC = clang
CFLAGS = -Wall 
LDFLAGS = -l SDL2-2.0.0
DEPS = cpulib.h

OBJ = cpulib.o ceemu_main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

ceemu: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.o ceemu
