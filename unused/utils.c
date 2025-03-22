#include "utils.h"

Rect make_rect(float x, float y, float width, float height) {
    Rect rect = {
        .x = x,
        .y = y,
        .width = width,
        .height = height
    };
    return rect;
}

bool check_collision(Rect a, Rect b) {
    return (a.x < b.x + b.width &&
            a.x + a.width > b.x &&
            a.y < b.y + b.height &&
            a.y + a.height > b.y);
}

Rect get_bounds(float x, float y, float size) {
    return make_rect(x, y, size, size);
} 