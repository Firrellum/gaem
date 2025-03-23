#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
    Uint8 r, g, b;
} Color;

#define COLOR_WHITE (Color){255, 255, 255} // white
#define COLOR_TINT (Color){000, 255, 255} // tint color

typedef enum {
    STATE_START_SCREEN,
    STATE_PLAYING,
} GameMode;


#endif