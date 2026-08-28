#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define GL2D_IMPLEMENTATION
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

// Mouse state structure
typedef struct {
    int x, y;
    bool left;
    bool right;
} Mouse;

static Mouse mouse = {0};

// Bouncing ball state variables
typedef struct {
    float x, y;
    float vx, vy;
    int radius;
} Ball;

static Ball ball;

void setup(void) {
    ball.x = WIDTH / 2.0f;
    ball.y = HEIGHT / 2.0f;
    ball.vx = 45.0f; // pixels per second
    ball.vy = 30.0f; // pixels per second
    ball.radius = 4;
}

void update(float dt, GL2D_Context* ctx) {
    // Clear screen each frame
    gl2d_cls(ctx, SWEETIE_16_PALETTE[0]);

    // Update ball position using delta time
    ball.x += ball.vx * dt;
    ball.y += ball.vy * dt;

    // Bounce off walls
    if (ball.x - ball.radius < 0) {
        ball.x = ball.radius;
        ball.vx = -ball.vx;
    } else if (ball.x + ball.radius >= WIDTH) {
        ball.x = WIDTH - 1 - ball.radius;
        ball.vx = -ball.vx;
    }

    if (ball.y - ball.radius < 0) {
        ball.y = ball.radius;
        ball.vy = -ball.vy;
    } else if (ball.y + ball.radius >= HEIGHT) {
        ball.y = HEIGHT - 1 - ball.radius;
        ball.vy = -ball.vy;
    }

    gl2d_fcirc(ctx, (int)ball.x, (int)ball.y, ball.radius, SWEETIE_16_PALETTE[0x01]);
    gl2d_dcirc(ctx, (int)ball.x, (int)ball.y, ball.radius, SWEETIE_16_PALETTE[0x0C]);
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    srand(time(NULL));

    GL2D_Context* ctx = gl2d_init("GL2D Game", WIDTH, HEIGHT, SCALE);
    if (!ctx) return -1;

    setup();

    bool running = true;
    SDL_Event event;

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
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION: {
                    int win_x, win_y;
                    Uint32 buttons = SDL_GetMouseState(&win_x, &win_y);
                    
                    mouse.x = win_x / SCALE;
                    mouse.y = win_y / SCALE;
                    mouse.left = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
                    mouse.right = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;                    break;
                }
                default: break;
            }
        }
        
        update(dt, ctx);
        gl2d_flip(ctx);
    }

    gl2d_quit(ctx);

    return 0;
}
