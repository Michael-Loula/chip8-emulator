#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct cpu_state {
    uint16_t opcode;
    uint16_t I;
    uint16_t pc;
    uint16_t stack[16];
    uint16_t sp;
    unsigned char mem[4096];
    unsigned char V[16];
    unsigned char key[16];
    unsigned char gfx[64*32];
    unsigned char dt, st;
    unsigned char dflag;
    
}cpu;


int timers_next(cpu*);
int cpu_exec(cpu*);
int cpu_fetch(cpu*);
int cpu_init(cpu*);
int cpu_ldrm(cpu*, const char*);
int cycle_next(cpu*);
