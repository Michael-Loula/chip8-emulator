#include "cpulib.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <SDL2/SDL.h>

unsigned char chip8_fontset[80] = { 

  0xF0, 0x90, 0x90, 0x90, 0xF0,
  0x20, 0x60, 0x20, 0x20, 0x70,
  0xF0, 0x10, 0xF0, 0x80, 0xF0,
  0xF0, 0x10, 0xF0, 0x10, 0xF0,
  0x90, 0x90, 0xF0, 0x10, 0x10,
  0xF0, 0x80, 0xF0, 0x10, 0xF0,
  0xF0, 0x80, 0xF0, 0x90, 0xF0,
  0xF0, 0x10, 0x20, 0x40, 0x40,
  0xF0, 0x90, 0xF0, 0x90, 0xF0,
  0xF0, 0x90, 0xF0, 0x10, 0xF0,
  0xF0, 0x90, 0xF0, 0x90, 0x90,
  0xE0, 0x90, 0xE0, 0x90, 0xE0,
  0xF0, 0x80, 0x80, 0x80, 0xF0,
  0xE0, 0x90, 0x90, 0x90, 0xE0,
  0xF0, 0x80, 0xF0, 0x80, 0xF0,
  0xF0, 0x80, 0xF0, 0x80, 0x80

};

int cpu_fetch(cpu* chip8) {

    chip8->opcode = chip8->mem[chip8->pc] << 8 | chip8->mem[chip8->pc + 1];
    return 0;

}

//advance timers
int timers_next(cpu* chip8)  {

    if (chip8->dt > 0) {

        chip8->dt--;

    }
    if (chip8->st > 0) {

        if (chip8->st == 1) {

            //beep!
            puts("beep");

        }

        chip8->st--;
    }

    return 0;

}

//initialize cpu struct onto stack
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

//load rom 
int cpu_ldrm(cpu* chip8, const char* rom_name) {

    int fd = open(rom_name,O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    read(fd,chip8->mem+512,size);

    close(fd);

    return 0;
}


//advance CPU -- fetches and executes opcode at pc
int cycle_next(cpu* chip8) {

    cpu_fetch(chip8);
    cpu_exec(chip8);

    return 0;

}

int cpu_exec(cpu* chip8) {

    /*opcode switch*/
    switch(chip8->opcode & 0xF000) {

        case 0xA000:

            chip8->I = chip8->opcode & 0x0FFF;
            chip8->pc += 2;
            break;

        case 0x0000:

            switch(chip8->opcode & 0x000F) {

                case 0x0000:
                
                    memset(chip8->gfx,0,64*32);
                    chip8->dflag = 1;
                    chip8->pc += 2;
                    break;
        
                case 0x000E:

                    chip8->sp -= 1;
                    chip8->pc = chip8->stack[chip8->sp];
                    chip8->pc += 2;
                    break;

                default:

                    break;

            }

            break;

        case 0x1000:

            chip8->pc = (0x0FFF & chip8->opcode);
            break;

        case 0x2000:

            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp += 1;
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

            chip8->V[(chip8->opcode & 0x0F00) >> 8] = (chip8->opcode & 0x00FF);
            chip8->pc += 2;
            break;

        case 0x7000:

            chip8->V[(chip8->opcode & 0x0F00) >> 8] += (chip8->opcode & 0x00FF);
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

                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] + chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->V[15] = ((0xFF - chip8->V[(chip8->opcode & 0x0F00) >> 8]) < (chip8->V[(chip8->opcode & 0x00F0) >> 4]));
                    chip8->pc += 2;
                    break;

                case 0x0005:

                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] - chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->V[15] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] < chip8->V[(chip8->opcode & 0x00F0) >> 4]);
                    chip8->pc += 2;
                    break;

                case 0x0006:

                    chip8->V[15] = chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0x1;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] >> 1;
                    chip8->pc += 2;
                    break;

                case 0x0007:

                    chip8->V[15] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] < chip8->V[(chip8->opcode & 0x00F0) >> 4]);
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4] - chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;

                case 0x000E:

                    chip8->V[15] = chip8->V[(chip8->opcode & 0x0F00) >> 8] >> 7;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] << 1);
                    chip8->pc += 2;
                    break;

            }

            break;

        case 0x9000:

            if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->V[(chip8->opcode & 0x00F0) >> 4]) { 
                chip8->pc += 2;
            }

            chip8->pc += 2;
            break;

        case 0xB000:

            chip8->pc = chip8->V[0] + (chip8->opcode & 0x0FFF);
            break;

        case 0xC000:

            chip8->V[(chip8->opcode & 0x0F00) >> 8] = ((rand() % 256) & (chip8->opcode & 0x00FF));
            chip8->pc += 2;
            break;

        case 0xD000:

            {
            
            uint16_t x = chip8->V[(chip8->opcode & 0x0F00) >> 8];
            uint16_t y = chip8->V[(chip8->opcode & 0x00F0) >> 4];

            if (x >= 64) x %= 64;
            if (y >= 32) y %= 32;

            uint16_t h = h = chip8->opcode & 0x000F;

            uint16_t pixel;

            chip8->V[15] = 0;

            for (int yl = 0; yl < h; yl++) {

                pixel = chip8->mem[chip8->I + yl];

                for(int xl = 0; xl < 8; xl++) {
    
                    if((pixel & (0x80 >> xl)) != 0) {

                        if(chip8->gfx[(x + xl + ((y + yl) * 64))] == 1) {
                            chip8->V[15] = 1;

                        }

                        chip8->gfx[x + xl + ((y + yl) * 64)] ^= 1;
                    }

                }

            }

            chip8->dflag = 1;
            chip8->pc += 2;

            break;

            }
        case 0xE000:

            switch(chip8->opcode & 0x00FF) {

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

            switch(chip8->opcode & 0x00FF) {

                case 0x0007:

                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->dt;
                    chip8->pc += 2;
                    break;

                case 0x000A:

                    {

                    int flag = 0;

                    for (int i = 0; i < 16; i++) {

                        if (chip8->key[i] != 0) {

                            chip8->V[(chip8->opcode & 0x0F00) >> 8] = i;
                            flag = 1;
                        }

                    }

                    if (!flag) {
                        //do not increment pc
                        break;
                    }

                    chip8->pc += 2;
                    break;

                    }

                case 0x0015:

                    chip8->dt = chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;

                case 0x0018:

                    chip8->st = chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;

                case 0x001E:

                    chip8->I += chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;

                case 0x0029:

                    chip8->I = 0x5 * chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;

                case 0x0033:

                    chip8->mem[chip8->I] = chip8->V[(chip8->opcode & 0x0F00) >> 8] / 100;
                    chip8->mem[chip8->I+1] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] / 10) % 10;
                    chip8->mem[chip8->I+2] = ((chip8->V[(chip8->opcode & 0x0F00) >> 8] % 100) % 10);
                    chip8->pc += 2;
                    break;

                case 0x0055:

                    {
                    void* ptr = &chip8->mem;
                    memcpy(ptr+chip8->I,&chip8->V,((chip8->opcode & 0x0F00) >> 8)+1);
                    chip8->pc += 2;
                    break;
                    }

                case 0x0065:

                    {
                    void* ptr = &chip8->mem;
                    memcpy(&chip8->V,ptr+chip8->I,((chip8->opcode & 0x0F00) >> 8)+1);
                    chip8->pc += 2;
                    break;
                    }

                default:

                    exit(0);
                    break;

            }

            break;

        default:

            printf("unknown instruction: 0x%X\n", chip8->opcode);
            
    }

    return 0;
}
