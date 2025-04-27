#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "constants.h"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    float x, y;
    int size;
    bool active;
    enum { HEALTH_PICKUP } type; 
} PowerUp;

typedef struct {
    Uint8 r, g, b;
} Color;

typedef enum {
    STATE_START_SCREEN,
    STATE_PLAYING,
    STATE_PAUSED,
} GameMode;

// player data
typedef struct {
    int hp;
    int last_hp;
    float x, y;
    int size;
    float speed;
    float dx, dy;
    bool alive;
    Uint32 last_score_update_time;
    Uint32 last_hp_update_time;
    Uint32 last_collide_update_time;
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
    TTF_Font* title_font; 
    bool running;
    double delta_time;
    double last_frame_time;
    int score;
    int last_score;
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
    Mix_Chunk* hit_sound;
    Mix_Music* main_sound_loop;
    bool restart_requested;
    bool isCollided;
    float title_y;
    float animation_time;
    PowerUp power_ups[3]; 
    int power_up_count;
} GameState;

#endif 
