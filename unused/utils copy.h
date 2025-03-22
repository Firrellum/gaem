#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <SDL2/SDL.h>

// color structure for rgb values
typedef struct {
    Uint8 r, g, b;
} Color;

// predefined colors
#define COLOR_WHITE (Color){255, 255, 255}
#define COLOR_CYAN (Color){0, 255, 255}
#define COLOR_RED (Color){255, 0, 0}

// rectangle structure for collision detection
typedef struct {
    float x, y;
    float width, height;
} Rect;

// creates a rectangle from position and size
Rect make_rect(float x, float y, float width, float height);

// checks if two rectangles overlap
bool check_collision(Rect a, Rect b);

// gets the rectangle for any game entity
Rect get_bounds(float x, float y, float size);

#endif // UTILS_H 