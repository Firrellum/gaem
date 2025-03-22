#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    Uint8 r, g, b;
} Color;

#define COLOR_WHITE (Color){255, 255, 255} // white
#define COLOR_TINT (Color){000, 255, 255} // tint color

#endif