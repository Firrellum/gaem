#include "player.h"
#include "constants.h"

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
    if (player->x < 0) player->x = 0;
    if (player->x > WINDOW_WIDTH - player->size) player->x = WINDOW_WIDTH - player->size;
    if (player->y < 0) player->y = 0;
    if (player->y > WINDOW_HEIGHT - player->size) player->y = WINDOW_HEIGHT - player->size;
    
}