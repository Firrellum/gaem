#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "entities.h"

void render_player_cube(Player* player, SDL_Renderer* renderer);
void update_player(Player* player, float delta_time);

#endif