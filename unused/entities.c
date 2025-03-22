#include "entities.h"
#include "utils.h"
#include "constants.h"

bool check_player_enemy_collision(Player* player, Enemy* enemy) {
    if (!enemy->active || enemy->collided || !player->alive) {
        return false;
    }

    Rect player_rect = get_bounds(player->x, player->y, player->size);
    Rect enemy_rect = get_bounds(enemy->x, enemy->y, ENEMY_SIZE);

    return check_collision(player_rect, enemy_rect);
}

void handle_collision(GameState* game, Player* player, Enemy* enemy) {
    if (check_player_enemy_collision(player, enemy)) {
        player->alive = false;
        enemy->collided = true;
        game->game_over = true;
    }
} 