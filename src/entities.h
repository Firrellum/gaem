
#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "constants.h"
#include "utils.h"
#include <SDL2/SDL_mixer.h>

// player data
typedef struct {
    int hp;
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

// menu options
typedef struct {
    const char* text;  
    bool selected;     
} MenuOption;

// menu placeholder
typedef struct {
    MenuOption options[4];  
    int option_count;       
    int selected_index;     
} PauseMenu;

typedef struct {
    const char* text;
} UiOptions;

typedef struct{
    UiOptions options[2];
} UiInfoScreen;

typedef struct {
    float x, y;
    int size;
    bool active;
} Collectible;

typedef struct {
    float x, y;
    int size;
    float speed;
    bool active;
} Enemy;

typedef struct {
    float start_x, start_y; 
    float end_x, end_y;      
    float target_x, target_y; 
    float speed;             
    int size;                
    bool active;             
    bool shrinking;         
    int collectible_sets;    
} LineEnemy;

// game state
typedef struct GameState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font; 
    TTF_Font* ui_font; 
    bool running;
    double delta_time;
    double last_frame_time;
    int score;
    bool game_over;
    Player player;
    ParticleSystem particles;
    // Color border_color;
    GameMode mode;
    UiInfoScreen ui_info;
    PauseMenu pause_menu;
    float menu_cooldown;
    float paused_dx, paused_dy;
    Collectible collectibles[5];
    int collectible_count;
    Enemy enemy;
    LineEnemy line_enemies[10];
    int line_enemy_count;       
    int total_collectible_sets;
    Mix_Chunk* pick_up_sound;
    Mix_Chunk* dead_sound;
    Mix_Chunk* start_sound;
    bool restart_requested;
} GameState;



#endif 
