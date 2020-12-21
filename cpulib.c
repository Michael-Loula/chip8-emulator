#include "cpulib.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <SDL2/SDL.h>



int cpu_fetch(cpu* chip8) {
    chip8->opcode = chip8->mem[chip8->pc] << 8 | chip8->mem[chip8->pc + 1];
    return 0;
}

int timers_next(cpu* chip8)  {
    if (chip8->dt > 0) {
        chip8->dt--;
    }
    if (chip8->st > 0) {
        if (chip8->st == 1) {
            puts("beep");
        }
        chip8->st--;
    }
}
int cpu_init(cpu* chip8) {
    chip8->pc = 0x200;
    chip8->opcode = 0;
    chip8->I = 0;
    chip8->sp = 0;
    chip8->dflag = 0; 
    memset(chip8->mem,0,4096);
    memset(chip8->gfx,0,64*32);
    memset(chip8->stack,0,16);
    memset(chip8->V,0,16);
    memset(chip8->key,0,16);
    memcpy(chip8->mem,chip8_fontset,80);
    return 0;
}

int cpu_ldrm(cpu* chip8, const char* rom_name) {
    int fd = open(rom_name,O_RDONLY);
    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    read(fd,chip8->mem+512,size);
    close(fd);
    return 0;
}

int cycle_next(cpu* chip8) {
    cpu_fetch(chip8);
    cpu_exec(chip8);
    return 0;
}

int cpu_exec(cpu* chip8) {
    /*big ol switch statement*/
    switch(chip8->opcode) {
        case 0xA000:
            chip8->I = chip8->opcode & 0x0FFF;
            chip8->pc += 2;
            break;
        case 0x0000:
            switch(chip8->opcode & 0x000F) {
                case 0x0000:
                    memcpy(chip8->gfx,0,64*32);
                    break;
        case 0x000E:
            chip8->pc = chip8->stack[chip8->sp];
            chip8->sp -= 1;
            break;
        case 0x1000:
            chip8->pc = 0x0FFF & chip8->opcode;
            break;
        case 0x2000:
            chip8->sp += 1;
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->pc = 0x0FFF & chip8->opcode;
            break;
        case 0x3000:
            if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == (chip8->opcode & 0x0FF)) {
                chip8->pc += 2;
            }
            chip8->pc += 2;
            break;
        case 0x4000:
            if (chip8->V[(chip8->opcode & 0x0F00) >> 8] != (chip8->opcode & 0x0FF)) {
                chip8->pc += 2;
            }
            chip8->pc += 2;
            break;
        case 0x5000:
            if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->V[(chip8->opcode & 0x00F0) >> 4]) {
                chip8->pc += 2;
            }
            chip8->pc += 2;
            break;
        case 0x6000:
            chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
            chip8->pc += 2;
            break;
        case 0x7000:
            chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF;
            chip8->pc += 2;
            break;
        case 0x8000:
            switch(chip8->opcode & 0x000F) {
                case 0x0000:
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0001:
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] | chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0002:
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] & chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0003:
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] ^ chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0004:
                    chip8->V[16] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] > (0x00FF - chip8->V[(chip8->opcode & 0x00F0) >> 4]));
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] + chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0005:
                    chip8->V[16] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] < chip8->V[(chip8->opcode & 0x00F0) >> 4]);
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] - chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0006:
                    chip8->V[16] = chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0x000F;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] >> 1;
                    chip8->pc += 2;
                    break;
                case 0x0007:
                    chip8->V[16] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] < chip8->V[(chip8->opcode & 0x00F0) >> 4]);
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4] - chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;
                case 0x000E:
                    chip8->V[16] = chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0xF000;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] << 1;
                    chip8->pc += 2;
                    break;
            }
            break;
        case 0xB000:
            chip8->pc = chip8->V[0] + (chip8->opcode & 0x0FFF);
            break;
        case 0xC000:
            chip8->V[(chip8->opcode & 0x0F00) >> 8] == (rand() % 256) & (chip8->opcode & 0x00FF);
            chip8->pc += 2;
            break;
        case 0xD000:
            //need to check that this works
            for (int i = 0; i < (0x000F & chip8->opcode); i++) {
                for (int j = 0; j < 8; j++) {
                    chip8->gfx[(chip8->V[(chip8->opcode & 0x0F00) >> 8] + j)*(chip8->V[(chip8->opcode & 0x00F0) >> 4]) + i] = chip8->mem[chip8->I+i+j];
                    chip8->V[16] = (chip8->mem[chip8->I+i+j] == 1) && (chip8->gfx[(chip8->V[(chip8->opcode & 0x0F00) >> 8] + j)*(chip8->V[(chip8->opcode & 0x00F0) >> 4]) + i] == 0);
                }
            }
            chip8->dflag = 1;
            chip8->pc += 2;
            break;
        case 0xE000:
            switch(0x00FF) {
                case 0x009E:
                    if (chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] != 0) {
                        chip8->pc += 2;
                    }
                    chip8->pc += 2;
                    break;
                case 0x00A1:
                    if (chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] == 0) {
                        chip8->pc += 2;
                    }
                    chip8->pc += 2;
                    break;
            }
            break;
        case 0xF000:
            switch(0x00FF) {
                case 0x0007:
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->dt;
                    chip8->pc += 2;
                    break;
                case 0x000A:
                    unsigned char c = getchar();
                    chip8->V[(chip8->opcode & 0x0F00)] = c;
                    chip8->pc += 2;
                    break;
                case 0x0015:
                    chip8->dt = chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;
                case 0x0018:
                    chip8->st = chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    break;
                case 0x001E:
                    chip8->I += chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;
                case 0x0029:
                    chip8->I = 0x050 + 20*chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;
                case 0x0033:
                    chip8->mem[chip8->I] = chip8->V[(chip8->opcode & 0x0F00) >> 8] / 100;;
                    chip8->mem[chip8->I+1] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] / 10) % 10;
                    chip8->mem[chip8->I+2] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] % 100) % 10;
                    chip8->pc +=2 ;
                    break;
                case 0x0055:
                    for (int i = 0; i < (chip8->opcode & 0x0F00); i++) {
                        chip8->mem[chip8->I+i] = chip8->V[i];
                    }
                    chip8->pc += 2;
                    break;
                case 0x0065:
                for (int i = 0; i < (chip8->opcode & 0x0F00); i++) {
                    chip8->V[i] = chip8->mem[chip8->I+i];
                }
                chip8->pc += 2;
                break;
            }
            break;
        default:
            printf("unknown instruction: 0x%X\n", chip8->opcode);
        }
}