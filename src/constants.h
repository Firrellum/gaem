#ifndef CONSTANTS_H
#define CONSTANTS_H

#define GAME_VERSION "v0.1"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define BORDER_SIZE 3
#define BORDER_COLOR (SDL_Color) {000, 255, 255, 200}
#define BASE_COLOR (SDL_Color){225,225,225,255} 
#define MENU_BACKDROP (SDL_Color){20,20,20,128}
#define SELECTED_COLOR (SDL_Color){000, 255, 255, 255}

#define GRID_CELL_SIZE 24
#define GRID_COLOR (SDL_Color) {150, 150, 150, 50}

#define FPS 120
#define FRAME_TARGET_TIME (1000.0 / FPS)

#define MOVE_SPEED 300.0f
#define PLAYER_SIZE 24

#define MAX_PARTICLES 100      
#define PARTICLE_SIZE 6        
#define PARTICLE_LIFETIME 0.8f 
#define PARTICLE_SPEED 100.0f  
#define SPAWN_RATE 0.016f     

#define MENU_DELAY 0.2f

#endif