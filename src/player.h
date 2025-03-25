#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "entities.h"

void render_player_cube(Player* player, SDL_Renderer* renderer);
void update_player(Player* player, float delta_time);
void spawn_particle(ParticleSystem* ps, Player* player);
void update_particles(ParticleSystem* ps, float delta_time);
void render_particles(ParticleSystem* ps, SDL_Renderer* renderer);
void spawn_collectibles(GameState* game);  
void update_collectibles(GameState* game); 
void render_collectibles(GameState* game); 
void spawn_enemy(GameState* game);        
void update_enemy(GameState* game);        
void render_enemy(GameState* game);
void check_and_respawn_collectibles(GameState* game); // New|

#endif