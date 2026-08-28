#include "gl2d.h"
#include <stdint.h>

#define WIDTH 512
#define HEIGHT 512
#define SCALE 1
#define MAX_ITER 512

// 16.16 Fixed-point representation (16 bits integer, 16 bits fractional)
typedef int32_t fix;

#define TO_FIX(x) ((fix)((x) * 65536.0))
#define MUL_FIX(x, y) (fix)(((int64_t)(x) * (y)) >> 16)

uint32_t mandel_color(int i) {
    if (i >= MAX_ITER) return 0xFF000000; // Black inside

    // Retro palette mapping based on iteration count
    uint8_t r = (uint8_t)(i * 8);
    uint8_t g = (uint8_t)(i * 4);
    uint8_t b = (uint8_t)(i * 16);

    return gl2d_rgb(r, g, b);
}

int main(int argc, char* argv[]) {
    // Initialize 128x128 virtual screen scaled up 4x
    GL2D_Context* app = gl2d_init("gl2d - Fractint DOS Style (128x128x4)", WIDTH, HEIGHT, SCALE);
    if (!app) return -1;

    int running = 1;
    SDL_Event event;

    int frame = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Dynamic zoom scaling using fixed-point progression
        double zoom_factor = 1.0 + frame * 0.02;
        
        // Center coordinates of the famous seahorse valley
        fix x_center = TO_FIX(-0.743643);
        fix y_center = TO_FIX(0.131825);
        
        // Span scaling mapped inversely to zoom
        fix scale = TO_FIX(3.0 / zoom_factor);
        fix x_min = x_center - (scale >> 1);
        fix y_min = y_center - (scale * HEIGHT / WIDTH >> 1);
        
        fix dx = scale / WIDTH;
        fix dy = scale / HEIGHT;

        // Render loop using pure 32-bit integer operations
        for (int py = 0; py < HEIGHT; py++) {
            fix y0 = y_min + py * dy;
            for (int px = 0; px < WIDTH; px++) {
                fix x0 = x_min + px * dx;

                fix x = 0;
                fix y = 0;
                int iteration = 0;

                while (iteration < MAX_ITER) {
                    fix x2 = MUL_FIX(x, x);
                    fix y2 = MUL_FIX(y, y);
                    
                    if (x2 + y2 > TO_FIX(4.0)) break;

                    fix xy = MUL_FIX(x, y);
                    x = x2 - y2 + x0;
                    y = xy + xy + y0;
                    iteration++;
                }

                uint32_t color = mandel_color(iteration);
                gl2d_pset(app, px, py, color);
            }
        }

        frame++;
        gl2d_flip(app);
    }

    gl2d_quit(app);
    return 0;
}
