// [mechanics.c]
#include <math.h>
#include <stdlib.h>
#include "mechanics.h"
#include "constants.h"
#include "utils.h"

void init_line_enemy(GameState* game) {
    LineEnemy* line = &game->line_enemy;
    line->size = PLAYER_SIZE;
    line->speed = 200.0f; // Adjust as needed
    line->active = false; // Starts inactive until first set collected
    line->shrinking = false;
    line->collectible_sets = 0;
}

void spawn_line_enemy(GameState* game) {
    LineEnemy* line = &game->line_enemy;
    line->active = true;
    line->shrinking = false;

    // Randomly pick a side (0: left, 1: right, 2: top, 3: bottom)
    int side = rand() % 4;
    switch (side) {
        case 0: // Left
            line->start_x = BORDER_SIZE;
            line->start_y = BORDER_SIZE + (rand() % (WINDOW_HEIGHT - 2 * BORDER_SIZE - line->size));
            line->target_x = WINDOW_WIDTH - BORDER_SIZE - line->size;
            line->target_y = line->start_y;
            break;
        case 1: // Right
            line->start_x = WINDOW_WIDTH - BORDER_SIZE - line->size;
            line->start_y = BORDER_SIZE + (rand() % (WINDOW_HEIGHT - 2 * BORDER_SIZE - line->size));
            line->target_x = BORDER_SIZE;
            line->target_y = line->start_y;
            break;
        case 2: // Top
            line->start_x = BORDER_SIZE + (rand() % (WINDOW_WIDTH - 2 * BORDER_SIZE - line->size));
            line->start_y = BORDER_SIZE;
            line->target_x = line->start_x;
            line->target_y = WINDOW_HEIGHT - BORDER_SIZE - line->size;
            break;
        case 3: // Bottom
            line->start_x = BORDER_SIZE + (rand() % (WINDOW_WIDTH - 2 * BORDER_SIZE - line->size));
            line->start_y = WINDOW_HEIGHT - BORDER_SIZE - line->size;
            line->target_x = line->start_x;
            line->target_y = BORDER_SIZE;
            break;
    }
    line->end_x = line->start_x;
    line->end_y = line->start_y;
}

void update_line_enemy(GameState* game) {
    LineEnemy* line = &game->line_enemy;
    if (!line->active) return;

    if (!line->shrinking) {
        // Move end point toward target
        float dx = line->target_x - line->end_x;
        float dy = line->target_y - line->end_y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance > 0) {
            dx /= distance; // Normalize
            dy /= distance;
            float move = line->speed * game->delta_time;
            line->end_x += dx * move;
            line->end_y += dy * move;

            // Check if reached target
            if (distance <= move) {
                line->end_x = line->target_x;
                line->end_y = line->target_y;
                line->shrinking = true;
            }
        }
    } else {
        // Shrink from start toward end
        float dx = line->end_x - line->start_x;
        float dy = line->end_y - line->start_y;
        float length = sqrt(dx * dx + dy * dy);
        if (length > 0) {
            dx /= length;
            dy /= length;
            float shrink = line->speed * game->delta_time;
            line->start_x += dx * shrink;
            line->start_y += dy * shrink;

            // Check if fully shrunk
            if (length <= shrink) {
                line->active = false; // Deactivate until respawn
                spawn_line_enemy(game); // Respawn from new position
            }
        }
    }

    // Collision with player
    SDL_Rect line_rect = {(int)line->start_x, (int)line->start_y, 
                          (int)(line->end_x - line->start_x), (int)(line->end_y - line->start_y)};
    if (line_rect.w < 0) {
        line_rect.x += line_rect.w;
        line_rect.w = -line_rect.w;
    }
    if (line_rect.h < 0) {
        line_rect.y += line_rect.h;
        line_rect.h = -line_rect.h;
    }
    if (game->player.x < line_rect.x + line_rect.w &&
        game->player.x + game->player.size > line_rect.x &&
        game->player.y < line_rect.y + line_rect.h &&
        game->player.y + game->player.size > line_rect.y) {
        game->game_over = true;
    }
}

void render_line_enemy(GameState* game) {
    LineEnemy* line = &game->line_enemy;
    if (!line->active) return;

    SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255); // Green for visibility
    SDL_RenderDrawLine(game->renderer, (int)line->start_x, (int)line->start_y, 
                       (int)line->end_x, (int)line->end_y);
}