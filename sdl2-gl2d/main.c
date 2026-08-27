#include "gl2d.h"
#include <stdlib.h>

int main(int argc, char* argv[]) {
    // Initialize a low-res virtual screen (320x240) scaled up 3x by hardware window
    GL2D_Context* app = gl2d_init("gl2d Demo", 320, 240, 3);
    if (!app) return -1;

    int running = 1;
    SDL_Event event;
    int tick = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Draw routine using PSET
        for (int y = 0; y < app->height; y++) {
            for (int x = 0; x < app->width; x++) {
                uint8_t r = (uint8_t)(x + tick);
                uint8_t g = (uint8_t)(y + tick);
                uint8_t b = (uint8_t)(x + y + tick);
                
                gl2d_pset(app, x, y, gl2d_rgb(r, g, b));
            }
        }

        tick++;

        // Render buffer onto screen
        gl2d_flip(app);
    }

    gl2d_quit(app);
    return 0;
}