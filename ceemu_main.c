#include "cpulib.h"
#include <SDL2/SDL.h>

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

    SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    //loads rom directly into memory using read
    //cpu_ldrm(&chip8,"chip8-test-rom/test_opcode.ch8");
    cpu_ldrm(&chip8,"a.ch8");
    //go
    for(;;) {
        //fetches and executes opcode
        cycle_next(&chip8);
        puts("cycle next");        
        //ticks down timers
        timers_next(&chip8);

        //draw if need be
        if (chip8.dflag) {
            puts("dflag on");
            //draw
            SDL_Rect rct;
            rct.w = 10;
            rct.h = 10;
            for (int i = 0; i < 64; i++) {
                for (int j = 0; j < 32; j++) {
                    rct.x = i*10;
                    rct.y = j*10;
                    if (chip8.gfx[(64*j)+i]) {
                        //printf("x: %d, y: %d",i,j);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }
                    else {
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    }
                    SDL_RenderDrawRect(renderer,&rct);
                    SDL_RenderFillRect(renderer,&rct);
                }
            }
            //end draw
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);

            //I think we should do this
            chip8.dflag = 0;
        }
        if (SDL_PollEvent(&event)) {
            //do i actually need/want this?
            //memset(chip8.key,0,16);
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
            puts("end of line");
        }
            
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}