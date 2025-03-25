#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mechanics.h"
#include "constants.h"
#include "utils.h"

void init_line_enemies(GameState* game) {
    game->line_enemy_count = 0;
    game->total_collectible_sets = 0;
    for (int i = 0; i < 10; i++) { 
        game->line_enemies[i].size = PLAYER_SIZE;
        game->line_enemies[i].speed = 200.0f;
        game->line_enemies[i].active = false;
        game->line_enemies[i].shrinking = false;
        game->line_enemies[i].collectible_sets = 0;
    }
}

void spawn_line_enemy(GameState* game) {
    if (game->line_enemy_count >= 10) return; // overflow

    LineEnemy* line = &game->line_enemies[game->line_enemy_count];
    line->active = true;
    line->shrinking = false;
    line->collectible_sets = game->total_collectible_sets;

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
    game->line_enemy_count++;
    printf("Line enemy spawned, total active: %d\n", game->line_enemy_count);
}

void update_line_enemies(GameState* game) {
    for (int i = 0; i < game->line_enemy_count; i++) {
        LineEnemy* line = &game->line_enemies[i];
        if (!line->active) continue;

        if (!line->shrinking) {
            float dx = line->target_x - line->end_x;
            float dy = line->target_y - line->end_y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance > 0) {
                dx /= distance;
                dy /= distance;
                float move = line->speed * game->delta_time;
                line->end_x += dx * move;
                line->end_y += dy * move;

                if (distance <= move) {
                    line->end_x = line->target_x;
                    line->end_y = line->target_y;
                    line->shrinking = true;
                }
            }
        } else {
            float dx = line->end_x - line->start_x;
            float dy = line->end_y - line->start_y;
            float length = sqrt(dx * dx + dy * dy);
            if (length > 0) {
                dx /= length;
                dy /= length;
                float shrink = line->speed * game->delta_time;
                line->start_x += dx * shrink;
                line->start_y += dy * shrink;

                if (length <= shrink) {
                    line->active = false;
                    for (int j = i; j < game->line_enemy_count - 1; j++) {
                        game->line_enemies[j] = game->line_enemies[j + 1];
                    }
                    game->line_enemy_count--;
                    i--; 
                    spawn_line_enemy(game); 
                }
            }
        }

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
}

void render_line_enemies(GameState* game) {
    for (int i = 0; i < game->line_enemy_count; i++) {
        LineEnemy* line = &game->line_enemies[i];
        if (!line->active) continue;

        SDL_SetRenderDrawColor(game->renderer, 224, 16, 186, 255);
        SDL_RenderDrawLine(game->renderer, (int)line->start_x, (int)line->start_y, 
                           (int)line->end_x, (int)line->end_y);
    }
}