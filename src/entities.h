#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "constants.h"

// enemy movement direction
typedef enum {
    HORIZONTAL,
    VERTICAL
} Direction;

// player data
typedef struct {
    float x, y;           
    int size;
    float speed;
    float dx, dy;         
    bool alive;           
} Player;

// game state
typedef struct GameState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    double delta_time;
    double last_frame_time;
    int score;
    bool game_over;
    Player player;
} GameState;


#endif 