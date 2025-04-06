#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mechanics.h"
#include "constants.h"

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

    const int GRID_SIZE = 24;

    int side = rand() % 4;
    switch (side) {
        case 0: // Left
            line->start_x = BORDER_SIZE;
            // Random Y position, snapped to grid
            line->start_y = BORDER_SIZE + (rand() % (WINDOW_HEIGHT - 2 * BORDER_SIZE));
            line->start_y = BORDER_SIZE + round((line->start_y - BORDER_SIZE) / (float)GRID_SIZE) * GRID_SIZE;
            line->start_y -= 1.5;
            line->target_x = WINDOW_WIDTH - BORDER_SIZE; // Reach the edge
            line->target_y = line->start_y;
            break;
        case 1: // Right
            line->start_x = WINDOW_WIDTH - BORDER_SIZE; // Start at the edge
            // Random Y position, snapped to grid
            line->start_y = BORDER_SIZE + (rand() % (WINDOW_HEIGHT - 2 * BORDER_SIZE));
            line->start_y = BORDER_SIZE + round((line->start_y - BORDER_SIZE) / (float)GRID_SIZE) * GRID_SIZE;
            line->start_y -= 1.5;
            line->target_x = BORDER_SIZE; // Reach the edge
            line->target_y = line->start_y;
            break;
        case 2: // Top
            // Random X position, snapped to grid
            line->start_x = BORDER_SIZE + (rand() % (WINDOW_WIDTH - 2 * BORDER_SIZE));
            line->start_x = BORDER_SIZE + round((line->start_x - BORDER_SIZE) / (float)GRID_SIZE) * GRID_SIZE;
            line->start_x -= 1.5;
            line->start_y = BORDER_SIZE;
            line->target_x = line->start_x;
            line->target_y = WINDOW_HEIGHT - BORDER_SIZE; // Reach the edge
            break;
        case 3: // Bottom
            // Random X position, snapped to grid
            line->start_x = BORDER_SIZE + (rand() % (WINDOW_WIDTH - 2 * BORDER_SIZE));
            line->start_x = BORDER_SIZE + round((line->start_x - BORDER_SIZE) / (float)GRID_SIZE) * GRID_SIZE;
            line->start_x -= 1.5;
            line->start_y = WINDOW_HEIGHT - BORDER_SIZE;
            line->target_x = line->start_x;
            line->target_y = BORDER_SIZE; // Reach the edge
            break;
    }
    line->end_x = line->start_x;
    line->end_y = line->start_y;
    game->line_enemy_count++;
    printf("Line enemy spawned, total active: %d, side: %d, start: (%.2f, %.2f), target: (%.2f, %.2f)\n", 
           game->line_enemy_count, side, line->start_x, line->start_y, line->target_x, line->target_y);
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
                dx /= distance; // Normalize
                dy /= distance;
                float move = line->speed * game->delta_time;
                float new_end_x = line->end_x + dx * move;
                float new_end_y = line->end_y + dy * move;

                // reached or passed the target
                bool reached = false;
                if (dx > 0 && new_end_x >= line->target_x) reached = true; // right
                else if (dx < 0 && new_end_x <= line->target_x) reached = true; //  left
                if (dy > 0 && new_end_y >= line->target_y) reached = true; // down
                else if (dy < 0 && new_end_y <= line->target_y) reached = true; // up 

                if (reached) {
                    line->end_x = line->target_x;
                    line->end_y = line->target_y;
                    line->shrinking = true;
                    printf("Line %d reached target: (%.2f, %.2f)\n", i, line->end_x, line->end_y);
                } else {
                    line->end_x = new_end_x;
                    line->end_y = new_end_y;
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

        float dx = line->end_x - line->start_x;
        float dy = line->end_y - line->start_y;
        float length = sqrt(dx * dx + dy * dy);
        if (length == 0) continue; 

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

        int thickness = 3; 
        float perp_x = -dy / length; 
        float perp_y = dx / length;
    
        int offset_x = (int)(perp_x * thickness);
        int offset_y = (int)(perp_y * thickness);
        line_rect.x -= offset_x;
        line_rect.y -= offset_y;
        line_rect.w += 2 * offset_x;
        line_rect.h += 2 * offset_y;

       
        if (line_rect.w < 0) {
            line_rect.x += line_rect.w;
            line_rect.w = -line_rect.w;
        }
        if (line_rect.h < 0) {
            line_rect.y += line_rect.h;
            line_rect.h = -line_rect.h;
        }

       
        printf("Line %d collision rect: x=%d, y=%d, w=%d, h=%d\n", 
               i, line_rect.x, line_rect.y, line_rect.w, line_rect.h);

        if (game->player.x < line_rect.x + line_rect.w &&
            game->player.x + game->player.size > line_rect.x &&
            game->player.y < line_rect.y + line_rect.h &&
            game->player.y + game->player.size > line_rect.y) {
            printf("Player hit line %d - Game Over\n", i);
            game->player.hp = 0;
            game->game_over = true;
            if (Mix_PlayChannel(-1, game->dead_sound, 0) == -1) {
                printf("Failed to play dead sound! Mix_Error: %s\n", Mix_GetError());
            } 
        }
    }
}

void render_line_enemies(GameState* game) {
    for (int i = 0; i < game->line_enemy_count; i++) {
        LineEnemy* line = &game->line_enemies[i];
        if (!line->active) continue;

        SDL_SetRenderDrawColor(game->renderer, 224, 16, 186, 255);
        int thickness = 3;
        for (int offset = -thickness; offset <= thickness; offset++) {
            float dx = line->end_x - line->start_x;
            float dy = line->end_y - line->start_y;
            float length = sqrt(dx * dx + dy * dy);
            if (length == 0) continue;
            float perp_x = -dy / length;
            float perp_y = dx / length;
            int start_x = (int)(line->start_x + perp_x * offset);
            int start_y = (int)(line->start_y + perp_y * offset);
            int end_x = (int)(line->end_x + perp_x * offset);
            int end_y = (int)(line->end_y + perp_y * offset);
            SDL_RenderDrawLine(game->renderer, start_x, start_y, end_x, end_y);
        }
    }
}