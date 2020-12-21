#include "cpulib.h"
#include <SDL2/SDL.h>
int main(int argc, char **argv) {
    //declares processor state onto stack
    cpu chip8;
    //initializes processor state
    cpu_init(&chip8);
    //starts gfx window
    SDL_Event *event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);
    //loads rom directly into memory using read
    cpu_ldrm(&chip8,"placeholder.rom");
    //go
    for(;;) {
        //fetches and executes opcode
        cycle_next(&chip8);
        //ticks down timers
        timers_next(&chip8);
        //draw if need be
        if (chip8.dflag) {
            //draw
            SDL_Rect rct;
            rct.w = 10;
            rct.h = 10;
            for (int i = 0; i < 64; i++) {
                for (int j = 0; j < 32; j++) {
                    if (chip8.gfx[i*j]) {
                        rct.x = i;
                        rct.y = j;
                        SDL_RenderDrawRect(renderer,&rct);
                        SDL_RenderFillRect(renderer,&rct);
                    }
                }
            }
            //end draw
            SDL_RenderPresent(renderer);


        }
        if (SDL_PollEvent(event)) {
            //do i actually need/want this?
            memset(chip8.key,0,16);
            switch (event->type) {
                case SDL_QUIT:
                    break;
                //check keypress
                case SDL_KEYDOWN:
                    //set key
                    char kp = event->key.keysym.sym;
                    switch (kp) {
                        case '1':
                            chip8.key[0] = 1;
                            break;
                        case '2':
                            chip8.key[1] = 1;
                            break;
                        case '3':
                            chip8.key[2] = 1;
                            break;
                        case '4':
                            chip8.key[3] = 1;
                            break;
                        case 'q':
                            chip8.key[4] = 1;
                            break;
                        case 'w':
                            chip8.key[5] = 1;
                            break;
                        case 'e':
                            chip8.key[6] = 1;
                            break;
                        case 'r':
                            chip8.key[7] = 1;
                            break;
                        case 'a':
                            chip8.key[8] = 1;
                            break;
                        case 's':
                            chip8.key[9] = 1;
                            break;
                        case 'd':
                            chip8.key[10] = 1;
                            break;
                        case 'f':
                            chip8.key[11] = 1;
                            break;
                        case 'z':
                            chip8.key[12] = 1;
                            break;
                        case 'x':
                            chip8.key[13] = 1;
                            break;
                        case 'c':
                            chip8.key[14] = 1;
                            break;
                        case 'v':
                            chip8.key[15] = 1;
                            break;
                        default:
                            break;
                    }
                    break;
                default: 
                    break;
            }
        }
            
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}