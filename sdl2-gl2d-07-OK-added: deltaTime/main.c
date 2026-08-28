#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "gl2d.h"

#define WIDTH 128
#define HEIGHT 128
#define SCALE 4

// Sweetie-16 Color Palette (GrafxKid)
const uint32_t SWEETIE_16_PALETTE[16] = {
    0xFF1A1C2C, // [0x00] Dark Chocolate Licorice
    0xFF5D275D, // [0x01] Blackberry Truffle
    0xFFB13E53, // [0x02] Cherry Drop
    0xFFEF7D57, // [0x03] Spiced Peach
    0xFFFFCD75, // [0x04] Butterscotch Crunch
    0xFFA7F070, // [0x05] Key Lime Jelly
    0xFF38B764, // [0x06] Spearmint Leaf
    0xFF257179, // [0x07] Blue Raspberry Frost
    0xFF29366F, // [0x08] Blueberry Hard Candy
    0xFF3B5DC9, // [0x09] Bubblegum Glaze
    0xFF41A6F6, // [0x0A] Cotton Candy Sky
    0xFF73EFF7, // [0x0B] Iced Lemonade Fizz
    0xFFF4F4F4, // [0x0C] Powdered Sugar
    0xFF94B0C2, // [0x0D] Frosting Mist
    0xFF566C86, // [0x0E] Cool Mint Swirl
    0xFF333C57  // [0x0F] Cocoa Twilight
};

void update(float dt) {
    int x = rand() % WIDTH;
    int y = rand() % HEIGHT;
    int c = rand() % 16;
    
    // dt can be used here for time-scaled logic if needed
    (void)dt; 
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    srand(time(NULL));

    GL2D_Context* ctx = gl2d_init("GL2D Game", WIDTH, HEIGHT, SCALE);
    if (!ctx) return -1;

    bool running = true;
    SDL_Event event;

    int frame = 0;
    Uint64 last_time = SDL_GetPerformanceCounter();
    double freq = (double)SDL_GetPerformanceFrequency();

    while (running) {
        Uint64 current_time = SDL_GetPerformanceCounter();
        float dt = (float)((double)(current_time - last_time) / freq);
        last_time = current_time;

        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                    }
                    break;
                default: break;
            }
        }

        update(dt);

        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;
        int c = rand() % 16;
    
        gl2d_pset(ctx, x, y, SWEETIE_16_PALETTE[c]);

        frame++;
        gl2d_flip(ctx);
    }

    gl2d_quit(ctx);

    return 0;
}
