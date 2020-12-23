#include "cpulib.h"
#include <unistd.h>
#include <SDL2/SDL.h>

//declare gfx stuff
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Event event;

static void _sig_handler(int signo){
  if (signo == SIGTERM || signo == SIGINT || signo == SIGSEGV || signo == SIGABRT){
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
  }
  exit(0);
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
    if (signal(SIGABRT, _sig_handler) == SIG_ERR) {
        fprintf(stderr,"Can't catch SIGABRT...exiting.\n");
    }
  
    //declares processor state onto stack
    cpu chip8;
  
    //initializes processor state
    cpu_init(&chip8);
    
    //loads rom directly into memory using read
    if(argc > 1) {
        puts(argv[1]);
        if (cpu_ldrm(&chip8,argv[1]) == -1) {

            puts("couldn't find rom");
            return -1;

        }

    } else {

        puts("please specify a ROM to load");
        return -1;

    }
    
    //sdl setup
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);
    SDL_RenderSetLogicalSize(renderer, 640, 320);
    SDL_RenderClear(renderer);

    //go
    for(;;) {

        //fetches and executes opcode
        cycle_next(&chip8);  

        //ticks down timers
        timers_next(&chip8);

        //draw if need be 
        if (chip8.dflag) {
            
            //"pixel"
            SDL_Rect rct;

            rct.w = 10;
            rct.h = 10;

            //black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            //render blocks
            for (unsigned int i = 0; i < (64*32); i++) {

                    unsigned int x = i / 32;
                    unsigned int y = i % 32;

                    rct.x = x*10;
                    rct.y = y*10;

                    if (chip8.gfx[(64*y)+x]) {

                        //green
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                        SDL_RenderDrawRect(renderer,&rct);
                        SDL_RenderFillRect(renderer,&rct);
                        
                    }

            }

            //show
            SDL_RenderPresent(renderer);
            chip8.dflag = 0;

        }

        //sleep
        usleep(1000);

        if (SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_QUIT:

                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;

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

    //quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}
