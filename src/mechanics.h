#ifndef MECHANICS_H
#define MECHANICS_H

#include <SDL2/SDL.h>
#include "entities.h"

// initializes the line enemies, setting their initial states
void init_line_enemies(GameState* game);

// spawns a new line enemy at a random position on the screen
void spawn_line_enemy(GameState* game);  

// updates the movement and behavior of active line enemies
void update_line_enemies(GameState* game); 

// renders the active line enemies on the screen
void render_line_enemies(GameState* game); 


#endif