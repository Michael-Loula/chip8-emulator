ceemu:
	clang -Wall -o ceemu ceemu_main.c cpulib.c -I include -L lib -l SDL2-2.0.0
clean:
	rm ceemu