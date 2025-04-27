#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "entities.h"

// renders the player’s cube on the screen
void render_player_cube(Player* player, SDL_Renderer* renderer);

// updates the player’s position and state based on time passed
void update_player(Player* player, float delta_time);

// spawns a new particle at the player's location for a particle system
void spawn_particle(GameState* game, ParticleSystem* ps, Player* player);

// updates all active particles based on time passed
void update_particles(ParticleSystem* ps, float delta_time);

// renders the active particles on the screen
void render_particles(ParticleSystem* ps, SDL_Renderer* renderer);

// spawns collectibles on the game board
void spawn_collectibles(GameState* game);  

// updates the state of all collectibles in the game
void update_collectibles(GameState* game); 

// renders the collectibles on the screen
void render_collectibles(GameState* game); 

// spawns a new enemy in the game
void spawn_enemy(GameState* game);        

// updates the state and behavior of the enemy
void update_enemy(GameState* game);        

// renders the enemy on the screen
void render_enemy(GameState* game);

// checks if collectibles should respawn and does so if necessary
void check_and_respawn_collectibles(GameState* game);  

// spawns a health pickup at a random position
void spawn_health_pickup(GameState* game);

#endif