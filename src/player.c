
#include <math.h>
#include "player.h"
#include "constants.h"
#include "utils.h"

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

void spawn_particle(ParticleSystem* ps, Player* player) {
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
    particle.color = change_particle_color(COLOR_TINT, COLOR_WHITE, color_mix);

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
