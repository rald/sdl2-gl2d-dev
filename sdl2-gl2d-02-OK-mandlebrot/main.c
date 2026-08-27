#include "gl2d.h"
#include <math.h>

#define WIDTH 320
#define HEIGHT 240
#define MAX_ITER 100

// Helper to map iteration count to a smooth RGB color
uint32_t mandel_color(int i, double smooth) {
    if (i >= MAX_ITER) return 0xFF000000; // Black for inside the set

    // Cyclic color palette shift based on iterations
    double t = (double)i / MAX_ITER + smooth * 0.05;
    uint8_t r = (uint8_t)(9 * (1 - t) * t * t * t * 255);
    uint8_t g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
    uint8_t b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);

    return gl2d_rgb(r, g, b);
}

int main(int argc, char* argv[]) {
    // Initialize a 320x240 virtual screen scaled up 3x
    GL2D_Context* app = gl2d_init("gl2d - Mandelbrot Zoom", WIDTH, HEIGHT, 3);
    if (!app) return -1;

    int running = 1;
    SDL_Event event;

    // Initial fractal view coordinates (focusing on an interesting valley)
    double target_x = -0.743643887037158704752191506114774;
    double target_y = 0.131825904205311970493132056385139;
    double zoom = 1.0;
    
    int frame = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Auto-zoom effect: smoothly multiply zoom factor over time
        zoom = 1.0 + frame * 0.015;
        double scale = 3.0 / zoom;

        // Render Mandelbrot Set
        for (int py = 0; py < HEIGHT; py++) {
            for (int px = 0; px < WIDTH; px++) {
                // Map pixel coordinates to complex plane centered at (target_x, target_y)
                double x0 = target_x + (px - WIDTH / 2.0) * (scale / WIDTH);
                double y0 = target_y + (py - HEIGHT / 2.0) * (scale / HEIGHT);

                double x = 0.0;
                double y = 0.0;
                int iteration = 0;

                // Mandelbrot iteration formula: z = z^2 + c
                while (x * x + y * y <= 4.0 && iteration < MAX_ITER) {
                    double xtemp = x * x - y * y + x0;
                    y = 2.0 * x * y + y0;
                    x = xtemp;
                    iteration++;
                }

                // Smooth coloring calculation to avoid color banding rings
                double smooth_val = 0;
                if (iteration < MAX_ITER) {
                    double log_zn = log(x * x + y * y) / 2.0;
                    double nu = log(log_zn / log(2.0)) / log(2.0);
                    smooth_val = iteration + 1.0 - nu;
                } else {
                    smooth_val = iteration;
                }

                uint32_t color = mandel_color(iteration, smooth_val + frame * 0.1);
                gl2d_pset(app, px, py, color);
            }
        }

        frame++;

        // Push buffer to window
        gl2d_flip(app);
    }

    gl2d_quit(app);
    return 0;
}