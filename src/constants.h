#ifndef CONSTANTS_H
#define CONSTANTS_H

#define GAME_VERSION "v1.0.0"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define BORDER_SIZE 3
#define BORDER_COLOR (SDL_Color) {000, 255, 255, 200}
#define BASE_COLOR (SDL_Color){225,225,225,255} 
#define MENU_BACKDROP (SDL_Color){20,20,20,128}
#define SELECTED_COLOR (SDL_Color){000, 255, 255, 255}

#define COLOR_WHITE (Color){255, 255, 255} // white
#define COLOR_TINT (Color){000, 255, 255} // tint color
#define COLOR_RED (Color){255, 000, 0} // tint color
#define COLOR_RED2 (Color){100, 000, 0} // tint color

#define GRID_CELL_SIZE 12
#define GRID_COLOR (SDL_Color) {150, 150, 150, 50}

#define DEFAULT_FPS 120 
#define FRAME_TARGET_TIME (1000.0 / DEFAULT_FPS)

#define MOVE_SPEED 300.0f
#define PLAYER_SIZE 12

#define MAX_PARTICLES 100      
#define PARTICLE_SIZE 6        
#define PARTICLE_LIFETIME 0.8f 
#define PARTICLE_SPEED 100.0f  
#define SPAWN_RATE 0.016f     

#define MENU_DELAY 0.2f

#define DEFAULT_MUSIC_VOLUME 16 
#define DEFAULT_SFX_VOLUME 64   

#endif