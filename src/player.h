#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "entities.h"

void render_player_cube(Player* player, SDL_Renderer* renderer);
void update_player(Player* player, float delta_time);
void spawn_particle(ParticleSystem* ps, Player* player);
void update_particles(ParticleSystem* ps, float delta_time);
void render_particles(ParticleSystem* ps, SDL_Renderer* renderer);
void spawn_collectibles(GameState* game);  // New: Spawn collectibles
void update_collectibles(GameState* game); // New: Update collectibles (collision and scoring)
void render_collectibles(GameState* game); // New: Render collectibles
void spawn_enemy(GameState* game);         // New: Spawn enemy
void update_enemy(GameState* game);        // New: Update enemy (movement and collision)
void render_enemy(GameState* game);

#endif