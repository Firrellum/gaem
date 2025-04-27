#include <math.h>
#include <stdio.h>
#include <time.h>
#include "player.h"
#include "constants.h"
#include "mechanics.h"

void render_player_cube(Player* player, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // set color to white
    SDL_Rect player_rect = {(int)player->x, (int)player->y, player->size, player->size}; // define the player rect
    SDL_RenderFillRect(renderer, &player_rect); // render the filled rectangle representing the player
}

void update_player(Player* player, float delta_time) {
    
    player->x += player->dx * player->speed * delta_time; // update x position
    player->y += player->dy * player->speed * delta_time; // update y position

    // keep player within screen boundaries
    // prevents the player from moving outside the defined window area
    if (player->x < BORDER_SIZE) player->x = BORDER_SIZE;
    if (player->x > WINDOW_WIDTH - player->size - BORDER_SIZE) player->x = WINDOW_WIDTH - player->size - BORDER_SIZE;
    if (player->y < BORDER_SIZE) player->y = BORDER_SIZE;
    if (player->y > WINDOW_HEIGHT - player->size - BORDER_SIZE) player->y = WINDOW_HEIGHT - player->size - BORDER_SIZE;
    
    printf("X : %f  Y : %f\n", player->x, player->y); // debug log to output player's position
}

Color change_particle_color(Color a, Color b, float t) {
    Color result;
    
    // interpolate each color component (r, g, b) based on t
    result.r = a.r + (b.r - a.r) * t; // red component interpolation
    result.g = a.g + (b.g - a.g) * t; // green component interpolation
    result.b = a.b + (b.b - a.b) * t; // blue component interpolation
    
    return result; // return the resulting color
}

void spawn_particle(GameState* game, ParticleSystem* ps, Player* player) {
    // ensure we don't exceed the max number of particles
    if (ps->count >= MAX_PARTICLES) return;

    // create a new particle and initialize default values
    Particle particle = {0};
    
    // position the particle at the player's center, offset by particle size
    particle.x = player->x + (PLAYER_SIZE - PARTICLE_SIZE) / 2;
    particle.y = player->y + (PLAYER_SIZE - PARTICLE_SIZE) / 2;

    // generate a random spread angle for the particle's movement
    float spread = (rand() % 60 - 30) * M_PI / 180.0f;  // random spread between -30 and 30 degrees
    float base_angle = atan2(-player->dy, -player->dx); // calculate angle opposite of player's movement direction (for trailing effect)
    float angle = base_angle + spread;  // adjust angle by random spread

    // randomize particle speed slightly for variation
    float speed = PARTICLE_SPEED * (0.5f + (rand() % 100) / 200.0f);

    // convert the angle to velocity components (dx, dy)
    particle.dx = cos(angle) * speed;
    particle.dy = sin(angle) * speed;

    // set particle lifetime and transparency
    particle.lifetime = PARTICLE_LIFETIME;
    particle.alpha = 1.0f;

    // randomly blend between two colors depending on collision state
    float color_mix = (float)(rand() % 101) / 100.0f;  // random factor for color interpolation
    if (!game->isCollided) {
        // if no collision, mix between base color and white
        particle.color = change_particle_color(COLOR_TINT, COLOR_WHITE, color_mix);
    } else {
        // if collision occurred, mix between two red shades
        particle.color = change_particle_color(COLOR_RED, COLOR_RED2, color_mix);
    }

    // add the new particle to the particle system and increment the count
    ps->particles[ps->count] = particle;
    ps->count++;
}

void update_particles(ParticleSystem* ps, float delta_time) {
    // keeps particles moving 
    ps->spawn_timer += delta_time;

    // update each particle's position and lifetime
    for (int i = 0; i < ps->count; i++) {
        Particle* p = &ps->particles[i];
        p->x += p->dx * delta_time;
        p->y += p->dy * delta_time;
        p->lifetime -= delta_time;
        p->alpha = p->lifetime / PARTICLE_LIFETIME;  

        // remove dead particles by swapping with the last active particle
        if (p->lifetime <= 0) {
            ps->particles[i] = ps->particles[ps->count - 1];
            ps->count--;
            i--;
        }
    }
}

void render_particles(ParticleSystem* ps, SDL_Renderer* renderer) {
    // loop through all particles
    for (int i = 0; i < ps->count; i++) {
        Particle particle = ps->particles[i];

        // set particle color and transparency alpha
        SDL_SetRenderDrawColor(renderer, 
            particle.color.r, 
            particle.color.g, 
            particle.color.b, 
            (Uint8)(particle.alpha * 255));

        // create the particle rect
        SDL_Rect particle_rect = {
            (int)particle.x, 
            (int)particle.y, 
            PARTICLE_SIZE, 
            PARTICLE_SIZE
        };
        
        // draw the particle rect
        SDL_RenderFillRect(renderer, &particle_rect);
    }
}

int get_rand_collectible_pos(char axis) {
    if (axis == 'y') {
        // generates a random y position within screen bounds, avoiding borders and player size
        return BORDER_SIZE + (rand() % (WINDOW_HEIGHT - 2 * BORDER_SIZE - PLAYER_SIZE));
    } else {
        // generates a random x position within screen bounds, avoiding borders and player size
        return BORDER_SIZE + (rand() % (WINDOW_WIDTH - 2 * BORDER_SIZE - PLAYER_SIZE));
    }
}

void spawn_collectibles(GameState* game) {
    const int UI_X_MIN = 150;   // minimum x-coordinate for collectibles (to avoid overlap with UI)
    const int UI_X_MAX = 1130;  // maximum x-coordinate for collectibles (to avoid overlap with UI)
    const int UI_Y_OFFSET = 72; // y-coordinate offset to avoid overlap with UI elements
    game->collectible_count = 5; // fixed number of collectibles for now
    
    for (int i = 0; i < game->collectible_count; i++) {
        // get random positions for collectibles
        int x_collectible_pos = get_rand_collectible_pos('x');
        int y_collectible_pos = get_rand_collectible_pos('y');
        
        // ensure collectibles don't overlap with certain UI areas
        while (x_collectible_pos < UI_X_MIN && y_collectible_pos < UI_Y_OFFSET) {
            x_collectible_pos = get_rand_collectible_pos('x');
            y_collectible_pos = get_rand_collectible_pos('y');
        }

        while (x_collectible_pos > UI_X_MAX && y_collectible_pos < UI_Y_OFFSET) {
            x_collectible_pos = get_rand_collectible_pos('x');
            y_collectible_pos = get_rand_collectible_pos('y');
        }

        // set the position and properties for each collectible
        game->collectibles[i].x = x_collectible_pos;
        game->collectibles[i].y = y_collectible_pos;
        game->collectibles[i].size = PLAYER_SIZE / 2; // make collectible smaller than player
        game->collectibles[i].active = true;          // mark collectible as active
    }
}

void spawn_health_pickup(GameState* game) {
    if (game->power_up_count >= 3) return; // max 3 power-ups at a time

    PowerUp* pickup = &game->power_ups[game->power_up_count];
    pickup->active = true;
    pickup->type = HEALTH_PICKUP;
    pickup->size = PLAYER_SIZE / 2; // same size as collectibles

    const int UI_X_MIN = 150;
    const int UI_X_MAX = 1130;
    const int UI_Y_OFFSET = 72;

    // generate random position
    int x_pos = get_rand_collectible_pos('x');
    int y_pos = get_rand_collectible_pos('y');

    // avoid UI overlap
    while ((x_pos < UI_X_MIN && y_pos < UI_Y_OFFSET) || (x_pos > UI_X_MAX && y_pos < UI_Y_OFFSET)) {
        x_pos = get_rand_collectible_pos('x');
        y_pos = get_rand_collectible_pos('y');
    }

    pickup->x = x_pos;
    pickup->y = y_pos;
    game->power_up_count++;
}

void check_and_respawn_collectibles(GameState* game) {
    int active_count = 0;
    
    // count the number of active collectibles
    for (int i = 0; i < game->collectible_count; i++) {
        if (game->collectibles[i].active) {
            active_count++;
        }
    }

     // if no active collectibles, respawn a new set and a new enemy
     if (active_count == 0) {
        game->total_collectible_sets++;  // increase the number of collectible sets
        spawn_collectibles(game);         // spawn new collectibles
        spawn_line_enemy(game);          // spawn a new line enemy

        // 10% chance to spawn a health pickup
        if ((rand() % 100) < 10) { 
            spawn_health_pickup(game);
        }
    }
}

void update_collectibles(GameState* game) {
    for (int i = 0; i < game->collectible_count; i++) {
        if (!game->collectibles[i].active) continue;  // skip inactive collectibles

        // simple AABB collision detection between player and collectible
        if (game->player.x < game->collectibles[i].x + game->collectibles[i].size &&
            game->player.x + game->player.size > game->collectibles[i].x &&
            game->player.y < game->collectibles[i].y + game->collectibles[i].size &&
            game->player.y + game->player.size > game->collectibles[i].y) {
            
            // deactivate collectible and increase score
            game->collectibles[i].active = false; 
            game->score += 10;

            // play pick-up sound on collision
            if (Mix_PlayChannel(-1, game->pick_up_sound, 0) == -1) {
                printf("Failed to play pick_up sound! Mix_Error: %s\n", Mix_GetError());
            }          
        }
    }

    // update health pickups
    for (int i = 0; i < game->power_up_count; i++) {
        if (!game->power_ups[i].active) continue;

        PowerUp* pickup = &game->power_ups[i];
        if (game->player.x < pickup->x + pickup->size &&
            game->player.x + game->player.size > pickup->x &&
            game->player.y < pickup->y + pickup->size &&
            game->player.y + game->player.size > pickup->y) {
            
            // handle health pickup
            if (pickup->type == HEALTH_PICKUP) {
                game->player.hp += 25; // restore 25 HP
                if (game->player.hp > 100) game->player.hp = 100; // cap at 100
                pickup->active = false; // deactivate pickup

                // play pick-up sound
                if (Mix_PlayChannel(-1, game->pick_up_sound, 0) == -1) {
                    printf("Failed to play pick_up sound! Mix_Error: %s\n", Mix_GetError());
                }

                // shift remaining power-ups
                for (int j = i; j < game->power_up_count - 1; j++) {
                    game->power_ups[j] = game->power_ups[j + 1];
                }
                game->power_up_count--;
                i--;
            }
        }
    }
    
    // check if all collectibles are collected and respawn them
    check_and_respawn_collectibles(game);
}

void render_collectibles(GameState* game) {
    SDL_SetRenderDrawColor(game->renderer, 255, 215, 0, 255); // gold color for collectibles
    for (int i = 0; i < game->collectible_count; i++) {
        if (game->collectibles[i].active) {
            // create a rect for the collectible based on its position and size
            SDL_Rect collectible_rect = {(int)game->collectibles[i].x, (int)game->collectibles[i].y, 
                                         game->collectibles[i].size, game->collectibles[i].size};
            // render the collectible rectangle
            SDL_RenderFillRect(game->renderer, &collectible_rect);
        }
    }
    // render health pickups
    SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255); // green color for health pickups
    for (int i = 0; i < game->power_up_count; i++) {
        if (game->power_ups[i].active) {
            SDL_Rect pickup_rect = {(int)game->power_ups[i].x, (int)game->power_ups[i].y, 
                                    game->power_ups[i].size, game->power_ups[i].size};
            SDL_RenderFillRect(game->renderer, &pickup_rect);
        }
    }
}

void spawn_enemy(GameState* game) {
    game->enemy.x = BORDER_SIZE + (rand() % (WINDOW_WIDTH - 2 * BORDER_SIZE - PLAYER_SIZE)); // random x position
    game->enemy.y = BORDER_SIZE + (rand() % (WINDOW_HEIGHT - 2 * BORDER_SIZE - PLAYER_SIZE)); // random y position
    game->enemy.size = PLAYER_SIZE; // same size as player
    game->enemy.speed = 150.0f; // slower speed
    game->enemy.active = true; // set enemy as active
}

void update_enemy(GameState* game) {
    if (!game->enemy.active) return; // skip if enemy is not active

    float difficulty_factor = 1.0f + (game->score / 50.0f) * 0.1f;
    game->enemy.speed = 150.0f * difficulty_factor;

    float dx = game->player.x - game->enemy.x; // calculate x distance to player
    float dy = game->player.y - game->enemy.y; // calculate y distance to player
    float distance = sqrt(dx * dx + dy * dy); // calculate total distance to player

    if (distance > 0) {
        dx /= distance; // normalize x distance
        dy /= distance; // normalize y distance
    }

    // update enemy position based on speed and delta time
    game->enemy.x += dx * game->enemy.speed * game->delta_time;
    game->enemy.y += dy * game->enemy.speed * game->delta_time;

    // keep enemy within screen boundaries
    if (game->enemy.x < BORDER_SIZE) game->enemy.x = BORDER_SIZE;
    if (game->enemy.x > WINDOW_WIDTH - game->enemy.size - BORDER_SIZE) game->enemy.x = WINDOW_WIDTH - game->enemy.size - BORDER_SIZE;
    if (game->enemy.y < BORDER_SIZE) game->enemy.y = BORDER_SIZE;
    if (game->enemy.y > WINDOW_HEIGHT - game->enemy.size - BORDER_SIZE) game->enemy.y = WINDOW_HEIGHT - game->enemy.size - BORDER_SIZE;

    // check if the enemy collides with the player using AABB collision detection
    if (game->player.x < game->enemy.x + game->enemy.size &&
        game->player.x + game->player.size > game->enemy.x &&
        game->player.y < game->enemy.y + game->enemy.size &&
        game->player.y + game->player.size > game->enemy.y) {
        
        // handle collision effects like player damage
        float damage_per_second = 20.0f;
        game->isCollided = true;

        // play hit sound if collision occurs
        if (Mix_PlayChannel(-1, game->hit_sound, 0) == -1) {
            printf("Failed to play hit sound! Mix_Error: %s\n", Mix_GetError());
        }

        // decrease player's hp based on collision time
        game->player.hp -= damage_per_second * game->delta_time;

        // check if player's hp reaches 0, end game if so
        if (game->player.hp <= 0) {
            game->player.hp = 0;
            game->game_over = true;

            // play death sound
            if (Mix_PlayChannel(-1, game->dead_sound, 0) == -1) {
                printf("Failed to play dead sound! Mix_Error: %s\n", Mix_GetError());
            }
        }
    } else {
        game->isCollided = false; // no collision
    }
}

void render_enemy(GameState* game) {
    if (game->enemy.active) { // check if enemy is active
        SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255); // set color to red
        SDL_Rect enemy_rect = {(int)game->enemy.x, (int)game->enemy.y, game->enemy.size, game->enemy.size}; // create rectangle for enemy
        SDL_RenderFillRect(game->renderer, &enemy_rect); // render the enemy rectangle
    }
}

