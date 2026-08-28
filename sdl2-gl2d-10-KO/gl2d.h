#ifndef GL2D_H
#define GL2D_H

#include <SDL2/SDL.h>
#include <stdint.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t* pixels;
    int width;
    int height;
    int pitch; // Bytes per row
} GL2D_Context;

// Global or module-local context pointer if working standalone
static GL2D_Context* _gl2d_current_ctx = NULL;

inline static GL2D_Context* gl2d_init(const char* title, int w, int h, int scale) {
    GL2D_Context* ctx = (GL2D_Context*)SDL_malloc(sizeof(GL2D_Context));
    if (!ctx) return NULL;

    ctx->width = w;
    ctx->height = h;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_free(ctx);
        return NULL;
    }

    ctx->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                     w * scale, h * scale, SDL_WINDOW_SHOWN);
    if (!ctx->window) {
        SDL_Quit();
        SDL_free(ctx);
        return NULL;
    }

    ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ctx->renderer) {
        SDL_DestroyWindow(ctx->window);
        SDL_Quit();
        SDL_free(ctx);
        return NULL;
    }

    // Create a streaming texture matching the low-res virtual screen buffer
    ctx->texture = SDL_CreateTexture(ctx->renderer, SDL_PIXELFORMAT_ARGB8888, 
                                     SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!ctx->texture) {
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
        SDL_Quit();
        SDL_free(ctx);
        return NULL;
    }

    ctx->pixels = (uint32_t*)SDL_malloc(w * h * sizeof(uint32_t));
    ctx->pitch = w * sizeof(uint32_t);
    
    _gl2d_current_ctx = ctx;
    return ctx;
}

inline static void gl2d_quit(GL2D_Context* ctx) {
    if (!ctx) return;
    if (ctx->pixels) SDL_free(ctx->pixels);
    if (ctx->texture) SDL_DestroyTexture(ctx->texture);
    if (ctx->renderer) SDL_DestroyRenderer(ctx->renderer);
    if (ctx->window) SDL_DestroyWindow(ctx->window);
    SDL_Quit();
    SDL_free(ctx);
}

// Map R, G, B, A components to native 32-bit pixel format (ARGB8888)
inline static uint32_t gl2d_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

inline static uint32_t gl2d_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

// Fast Set Pixel (PSET) with optional boundary checks
inline static void gl2d_pset(GL2D_Context* ctx, int x, int y, uint32_t color) {
    if (x >= 0 && x < ctx->width && y >= 0 && y < ctx->height) {
        ctx->pixels[y * ctx->width + x] = color;
    }
}

// Fast Get Pixel (PGET)
inline static uint32_t gl2d_pget(GL2D_Context* ctx, int x, int y) {
    if (x >= 0 && x < ctx->width && y >= 0 && y < ctx->height) {
        return ctx->pixels[y * ctx->width + x];
    }
    return 0;
}

// Clear screen buffer with color
inline static void gl2d_cls(GL2D_Context* ctx, uint32_t color) {
    int total = ctx->width * ctx->height;
    for (int i = 0; i < total; i++) {
        ctx->pixels[i] = color;
    }
}

// Push CPU pixel buffer to GPU texture and present to screen
inline static void gl2d_flip(GL2D_Context* ctx) {
    SDL_UpdateTexture(ctx->texture, NULL, ctx->pixels, ctx->pitch);
    SDL_RenderClear(ctx->renderer);
    SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, NULL);
    SDL_RenderPresent(ctx->renderer);
}


// Check if point is inside a rectangle
bool gl2d_inrect(int x, int y, int rx, int ry, int rw, int rh) {
    return (x >= rx && x < rx + rw && y >= ry && y < ry + rh);
}

// Check if point is inside a circle
bool gl2d_incirc(int x, int y, int cx, int cy, int r) {
    int dx = x - cx;
    int dy = y - cy;
    return (dx * dx + dy * dy <= r * r);
}

// Draw Line (Bresenham's Line Algorithm)
void gl2d_dline(GL2D_Context *ctx, int x0, int y0, int x1, int y1, uint32_t c) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        gl2d_pset(ctx, x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Draw Rectangle (Outline)
void gl2d_drect(GL2D_Context *ctx, int x, int y, int w, int h, uint32_t c) {
    int x1 = x + w - 1;
    int y1 = y + h - 1;
    dline(ctx, x, y, x1, y, c);
    dline(ctx, x1, y, x1, y1, c);
    dline(ctx, x1, y1, x, y1, c);
    dline(ctx, x, y1, x, y, c);
}

// Fill Rectangle
void gl2d_frect(GL2D_Context *ctx, int x, int y, int w, int h, uint32_t c) {
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            gl2d_pset(ctx, x + dx, y + dy, c);
        }
    }
}

// Draw Circle (Outline - Midpoint Circle Algorithm)
void gl2d_dcirc(GL2D_Context *ctx, int xc, int yc, int r, uint32_t c) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (y >= x) {
        gl2d_pset(ctx, xc + x, yc + y, c);
        gl2d_pset(ctx, xc - x, yc + y, c);
        gl2d_pset(ctx, xc + x, yc - y, c);
        gl2d_pset(ctx, xc - x, yc - y, c);
        gl2d_pset(ctx, xc + y, yc + x, c);
        gl2d_pset(ctx, xc - y, yc + x, c);
        gl2d_pset(ctx, xc + y, yc - x, c);
        gl2d_pset(ctx, xc - y, yc - x, c);

        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

// Fill Circle (Compatible with dcirc using symmetric horizontal spans)
void gl2d_fcirc(GL2D_Context *ctx, int xc, int yc, int r, uint32_t c) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (y >= x) {
        // Draw horizontal lines between symmetric points to fill the circle efficiently
        for (int px = xc - x; px <= xc + x; px++) {
            gl2d_pset(ctx, px, yc + y, c);
            gl2d_pset(ctx, px, yc - y, c);
        }
        for (int px = xc - y; px <= xc + y; px++) {
            gl2d_pset(ctx, px, yc + x, c);
            gl2d_pset(ctx, px, yc - x, c);
        }

        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

#endif // GL2D_H
