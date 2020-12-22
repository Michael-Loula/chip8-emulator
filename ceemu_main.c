#include "cpulib.h"
#include <SDL2/SDL.h>
#include <unistd.h>

//declare gfx stuff
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Event event;

static void _sig_handler(int signo){
  if (signo == SIGTERM || signo == SIGINT || signo == SIGSEGV){
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
  }
}

uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char **argv) {
    if (signal(SIGINT, _sig_handler) == SIG_ERR) {
        fprintf(stderr,"Can't catch SIGINT...exiting.\n");
    }

    if (signal(SIGTERM, _sig_handler) == SIG_ERR) {
        fprintf(stderr,"Can't catch SIGTERM...exiting.\n");
    }
    if (signal(SIGSEGV, _sig_handler) == SIG_ERR) {
        fprintf(stderr,"Can't catch SIGSEGV...exiting.\n");
    }

    //declares processor state onto stack
    cpu chip8;
    //initializes processor state
    cpu_init(&chip8);

    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(1024, 512, 0, &window, &renderer);
    SDL_RenderSetLogicalSize(renderer, 1024, 512);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderClear(renderer);
    //loads rom directly into memory using read
    //cpu_ldrm(&chip8,"chip8-test-rom/test_opcode.ch8");
    cpu_ldrm(&chip8,"pong.rom");
    //go
    for(;;) {
        //fetches and executes opcode
        cycle_next(&chip8);   
        //printf("30E: %x\n",chip8.mem[0x130E & 0x0FFF]);
        //ticks down timers
        timers_next(&chip8);

        //draw if need be
        if (chip8.dflag) {
            
            //draw
            SDL_Rect rct;
            rct.w = 16;
            rct.h = 16;
            for (unsigned int i = 0; i < (64*32); i++) {
                    unsigned int x = i / 32;
                    unsigned int y = i % 32;
                    rct.x = x*16;
                    rct.y = y*16;
                    if (chip8.gfx[(64*y)+x]) {
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

                    }
                    else {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }
                    SDL_RenderDrawRect(renderer,&rct);
                    SDL_RenderFillRect(renderer,&rct);
                
            }

            SDL_RenderPresent(renderer);
            chip8.dflag = 0;
        }
        usleep(1000);
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;
                    //break;
                //check keypress
                case SDL_KEYDOWN:
                    //set key
                    for (int i = 0; i < 16; ++i) {
                        if (event.key.keysym.sym == keymap[i]) {
                            chip8.key[i] = 1;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    //set key
                    for (int i = 0; i < 16; ++i) {
                        if (event.key.keysym.sym == keymap[i]) {
                            chip8.key[i] = 0;
                        }
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