#include <math.h>
#include <stdio.h>
#include <time.h>
#include "player.h"
#include "constants.h"
#include "utils.h"
#include "mechanics.h"

// player 
// render player cube
void render_player_cube(Player* player, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect player_rect = {(int)player->x, (int)player->y, player->size, player->size};
    SDL_RenderFillRect(renderer, &player_rect);
}

// updates player position based on current movement direction
void update_player(Player* player, float delta_time) {
    
    player->x += player->dx * player->speed * delta_time;
    player->y += player->dy * player->speed * delta_time;

    // Keep player within screen boundaries
    if (player->x < BORDER_SIZE) player->x = BORDER_SIZE;
    if (player->x > WINDOW_WIDTH - player->size - BORDER_SIZE) player->x = WINDOW_WIDTH - player->size - BORDER_SIZE;
    if (player->y < BORDER_SIZE) player->y = BORDER_SIZE;
    if (player->y > WINDOW_HEIGHT - player->size - BORDER_SIZE) player->y = WINDOW_HEIGHT - player->size - BORDER_SIZE;
    
}

// player | particles 
// alt particle colors
Color change_particle_color(Color a, Color b, float t) {
    Color result;
    result.r = a.r + (b.r - a.r) * t;
    result.g = a.g + (b.g - a.g) * t;
    result.b = a.b + (b.b - a.b) * t;
    return result;
}

void spawn_particle(GameState* game, ParticleSystem* ps, Player* player) {
    if (ps->count >= MAX_PARTICLES) return;

    // new particle with default values
    Particle particle = {0};
    
    // start at player center
    particle.x = player->x + (PLAYER_SIZE - PARTICLE_SIZE) / 2;
    particle.y = player->y + (PLAYER_SIZE - PARTICLE_SIZE) / 2;

    // rng particle spread
    float spread = (rand() % 60 - 30) * M_PI / 180.0f;  
    float base_angle = atan2(-player->dy, -player->dx); // move in oposite dir to player | trail effect
    float angle = base_angle + spread;
    
    // set particle speed to be varied slightly
    float speed = PARTICLE_SPEED * (0.5f + (rand() % 100) / 200.0f);

    // convert angle to velocity
    particle.dx = cos(angle) * speed;
    particle.dy = sin(angle) * speed;

    // particle and lifetime
    particle.lifetime = PARTICLE_LIFETIME;
    particle.alpha = 1.0f;

    // get rng color between base and tint
    float color_mix = (float)(rand() % 101) / 100.0f;
    if (!game->isCollided){
        particle.color = change_particle_color(COLOR_TINT, COLOR_WHITE, color_mix);
    }else{
        particle.color = change_particle_color(COLOR_RED, COLOR_RED2, color_mix);
    }
    

    // add new partice
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

        // set barticle color and transparency alpha
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

void spawn_collectibles(GameState* game) {
    game->collectible_count = 5; // Fixed number for now
    for (int i = 0; i < game->collectible_count; i++) {
        game->collectibles[i].x = BORDER_SIZE + (rand() % (WINDOW_WIDTH - 2 * BORDER_SIZE - PLAYER_SIZE));
        game->collectibles[i].y = BORDER_SIZE + (rand() % (WINDOW_HEIGHT - 2 * BORDER_SIZE - PLAYER_SIZE));
        game->collectibles[i].size = PLAYER_SIZE / 2; // Smaller than player
        game->collectibles[i].active = true;
    }
}

void check_and_respawn_collectibles(GameState* game) {
    int active_count = 0;
    for (int i = 0; i < game->collectible_count; i++) {
        if (game->collectibles[i].active) {
            active_count++;
        }
    }
    if (active_count == 0) {
        game->total_collectible_sets++; 
        spawn_collectibles(game);
        spawn_line_enemy(game); 
    }
}

void update_collectibles(GameState* game) {
    for (int i = 0; i < game->collectible_count; i++) {
        if (!game->collectibles[i].active) continue;

        // Simple AABB collision detection
        if (game->player.x < game->collectibles[i].x + game->collectibles[i].size &&
            game->player.x + game->player.size > game->collectibles[i].x &&
            game->player.y < game->collectibles[i].y + game->collectibles[i].size &&
            game->player.y + game->player.size > game->collectibles[i].y) {
            game->collectibles[i].active = false; 
            game->score += 10;          
            if (Mix_PlayChannel(-1, game->pick_up_sound, 0) == -1) {
                printf("Failed to play pick_up sound! Mix_Error: %s\n", Mix_GetError());
            }          
        }
    }
    check_and_respawn_collectibles(game);
}

void render_collectibles(GameState* game) {
    SDL_SetRenderDrawColor(game->renderer, 255, 215, 0, 255); // gold 
    for (int i = 0; i < game->collectible_count; i++) {
        if (game->collectibles[i].active) {
            SDL_Rect collectible_rect = {(int)game->collectibles[i].x, (int)game->collectibles[i].y, 
                                         game->collectibles[i].size, game->collectibles[i].size};
            SDL_RenderFillRect(game->renderer, &collectible_rect);
        }
    }
}

void spawn_enemy(GameState* game) {
    game->enemy.x = BORDER_SIZE + (rand() % (WINDOW_WIDTH - 2 * BORDER_SIZE - PLAYER_SIZE));
    game->enemy.y = BORDER_SIZE + (rand() % (WINDOW_HEIGHT - 2 * BORDER_SIZE - PLAYER_SIZE));
    game->enemy.size = PLAYER_SIZE;
    game->enemy.speed = 150.0f; // slower 
    game->enemy.active = true;
}

void update_enemy(GameState* game) {
    // Uint32 current_time = SDL_GetTicks();
    // game->player.last_collide_update_time = current_time;
    if (!game->enemy.active) return;

    float dx = game->player.x - game->enemy.x;
    float dy = game->player.y - game->enemy.y;
    float distance = sqrt(dx * dx + dy * dy);
    if (distance > 0) {
        dx /= distance; // Normalize
        dy /= distance;
    }

    game->enemy.x += dx * game->enemy.speed * game->delta_time;
    game->enemy.y += dy * game->enemy.speed * game->delta_time;

    if (game->enemy.x < BORDER_SIZE) game->enemy.x = BORDER_SIZE;
    if (game->enemy.x > WINDOW_WIDTH - game->enemy.size - BORDER_SIZE) game->enemy.x = WINDOW_WIDTH - game->enemy.size - BORDER_SIZE;
    if (game->enemy.y < BORDER_SIZE) game->enemy.y = BORDER_SIZE;
    if (game->enemy.y > WINDOW_HEIGHT - game->enemy.size - BORDER_SIZE) game->enemy.y = WINDOW_HEIGHT - game->enemy.size - BORDER_SIZE;

    if (game->player.x < game->enemy.x + game->enemy.size &&
        game->player.x + game->player.size > game->enemy.x &&
        game->player.y < game->enemy.y + game->enemy.size &&
        game->player.y + game->player.size > game->enemy.y) 
        {
        // conditions from these bounds detection belpw
        
            float damage_per_second = 20.0f;
            game->isCollided = true;

            if (Mix_PlayChannel(-1, game->hit_sound, 0) == -1) {
                printf("Failed to play hit sound! Mix_Error: %s\n", Mix_GetError());
            }
            
            game->player.hp -= damage_per_second * game->delta_time; 
            
            if (game->player.hp <= 0) {
                game->player.hp = 0;
                game->game_over = true; 
                if (Mix_PlayChannel(-1, game->dead_sound, 0) == -1) {
                    printf("Failed to play dead sound! Mix_Error: %s\n", Mix_GetError());
                } 
            }


    }else{game->isCollided = false;}


}

void render_enemy(GameState* game) {
    if (game->enemy.active) {
        SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255); // Red color
        SDL_Rect enemy_rect = {(int)game->enemy.x, (int)game->enemy.y, game->enemy.size, game->enemy.size};
        SDL_RenderFillRect(game->renderer, &enemy_rect);
    }
}
