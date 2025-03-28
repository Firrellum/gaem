#ifndef MECHANICS_H
#define MECHANICS_H

#include <SDL2/SDL.h>
#include "entities.h"

void init_line_enemies(GameState* game);
void spawn_line_enemy(GameState* game);  
void update_line_enemies(GameState* game); 
void render_line_enemies(GameState* game); 

#endif