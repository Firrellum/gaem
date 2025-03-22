#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "constants.h"
#include "utils.h"

// player data
typedef struct {
    float x, y;           
    int size;
    float speed;
    float dx, dy;         
    bool alive;           
} Player;

// particle data
typedef struct {
    float x, y;          
    float dx, dy;         
    float lifetime;  
    Color color;     
    float alpha;          
} Particle;

// particle system data
typedef struct {
    Particle particles[MAX_PARTICLES];  
    int count;               
    float spawn_timer;       
} ParticleSystem;

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
    ParticleSystem particles;
    Color border_color;
} GameState;


#endif 