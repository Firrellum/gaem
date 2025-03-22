#include "player.h"

void render_player_cube(Player* player, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect player_rect = {(int)player->x, (int)player->y, player->size, player->size};
    SDL_RenderFillRect(renderer, &player_rect);
}