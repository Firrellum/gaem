#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "entities.h"

void render_player_cube(Player* player, SDL_Renderer* renderer);
void update_player(Player* player, float delta_time);
void spawn_particle(ParticleSystem* ps, Player* player);
void update_particles(ParticleSystem* ps, float delta_time);
void render_particles(ParticleSystem* ps, SDL_Renderer* renderer);

#endif