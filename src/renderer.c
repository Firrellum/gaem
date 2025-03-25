#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "constants.h"
#include "utils.h"
#include "entities.h"
#include "player.h"
#include "renderer.h"
#include "mechanics.h"

void write_to_file(const char *text);
void render_game(GameState* game);
void render_gameplay_ui(GameState* game);
void render_start_screen(GameState* game);
void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, bool centered);
void render_border(GameState* game);
SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color fg);
void render_grid_overlay(GameState* game);

SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color fg){
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text, fg);
    if (!text_surface) {
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);
    if (!texture) {
        write_to_file("Error creating texture from surface.");
        return NULL;
    }

    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    return texture;
}

void render_grid_overlay(GameState* game){
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(game->renderer, GRID_COLOR.r,GRID_COLOR.g,GRID_COLOR.b,GRID_COLOR.a);

    for (int x = GRID_CELL_SIZE; x < WINDOW_WIDTH; x += GRID_CELL_SIZE){
        SDL_RenderDrawLine(game->renderer, x, 0, x, WINDOW_HEIGHT);
    }

    for (int y = GRID_CELL_SIZE; y < WINDOW_HEIGHT; y += GRID_CELL_SIZE){
        SDL_RenderDrawLine(game->renderer, 0, y, WINDOW_WIDTH, y);
    }
}

void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, bool centered) {
    if (!texture) return;

    int text_width, text_height;
    if (SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height) < 0) {
        write_to_file("Failed to query texture.");
        return;
    }

    SDL_Rect text_rect = {
        centered ? x - text_width / 2 : x,
        centered ? y - text_height / 2 : y,
        text_width,
        text_height
    };
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
}

void render_gameplay_ui(GameState* game) {
    SDL_Color ui_color = {255, 255, 255, 255};
    char score_text[32];
    // char hp_text[32];
    // printf("Formatting score text\n");
    sprintf(score_text, "Score: %d", game->score);
    // printf("Formatting HP text\n");
    // sprintf(hp_text, "HP: %d", game->player.hp);

    // printf("Creating score texture\n");
    SDL_Texture* player_score_texture = render_text(game->renderer, score_text, game->ui_font, ui_color);
    // printf("Creating HP texture\n");
    // SDL_Texture* player_hp_texture = render_text(game->renderer, hp_text, game->ui_font, ui_color);

    // printf("Rendering score text\n");
    render_text_at(game->renderer, player_score_texture, 12, 48, false);
    // printf("Rendering HP text\n");
    // render_text_at(game->renderer, player_hp_texture, 48, WINDOW_HEIGHT - 48, false);

    // printf("Destroying score texture\n");
    SDL_DestroyTexture(player_score_texture);
    // printf("Destroying HP texture\n");
    // SDL_DestroyTexture(player_hp_texture);
}

void render_start_screen(GameState* game) {
    // set colours
    SDL_Color button_color = {255, 255, 255, 255};
    SDL_Color title_color = {0, 255, 255, 255};

    // create the textures with the fonts
    SDL_Texture* game_title_texture = render_text(game->renderer, "F Cubed (f³)", game->font, title_color);
    SDL_Texture* button_text_texture = render_text(game->renderer, "ENTER to Start", game->font, button_color);

    // render 'button'
    render_text_at(game->renderer, button_text_texture, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, true);

    // render title text
    render_text_at(game->renderer, game_title_texture, WINDOW_WIDTH / 2, 100, true);

    // TODO refactor the repetition here
    SDL_DestroyTexture(game_title_texture);
    SDL_DestroyTexture(button_text_texture);
}

void render_border(GameState* game){
    SDL_SetRenderDrawColor(game->renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a); // color border

    // border || SDL defaults to one pixel wide | for loop
    for (int i = 0; i < BORDER_SIZE; i++){
        SDL_Rect border_rect = {i, i, WINDOW_WIDTH - (2 * i), WINDOW_HEIGHT - (2 * i)};
        SDL_RenderDrawRect(game->renderer, &border_rect);
    }
    // SDL_Rect border_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    // NEED TO SUPTRACT BORDER FROM PLAYER BOUNDS
    
}

void render_pause_menu(GameState* game) {
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(game->renderer, MENU_BACKDROP.r, MENU_BACKDROP.g, MENU_BACKDROP.b, MENU_BACKDROP.a);
    SDL_Rect backdrop = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(game->renderer, &backdrop);
    SDL_RenderClear(game->renderer);
    // color highlights and main
    SDL_Color selected_color = SELECTED_COLOR;  
    SDL_Color unselected_color = BASE_COLOR;  

    int y_offset = WINDOW_HEIGHT / 2 - (game->pause_menu.option_count * 50) / 2;  // menu pos
    // loop for each item
    for (int i = 0; i < game->pause_menu.option_count; i++) {
        SDL_Color color = game->pause_menu.options[i].selected ? selected_color : unselected_color;
        SDL_Texture* option_texture = render_text(game->renderer, game->pause_menu.options[i].text, game->font, color);
        render_text_at(game->renderer, option_texture, WINDOW_WIDTH / 2, y_offset + i * 50, true);
        SDL_DestroyTexture(option_texture);  // cleaning
    }
}

void render_game(GameState* game) {
    // printf("Starting render_game\n");
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    if (game->mode == STATE_START_SCREEN) {
        // printf("Rendering start screen\n");
        render_start_screen(game);
        render_border(game);
    } else if (game->mode == STATE_PLAYING) {
        // printf("Rendering border\n");
        render_border(game);
        // printf("Rendering gameplay UI\n");
        render_gameplay_ui(game);
        // printf("Rendering particles\n");
        render_particles(&game->particles, game->renderer);
        // printf("Rendering player\n");
        render_player_cube(&game->player, game->renderer);
        // printf("Rendering collectibles\n");
        render_collectibles(game);
        // printf("Rendering enemy\n");
        render_enemy(game);
        render_line_enemies(game);
        if (game->game_over) {
            // printf("Rendering game over\n");
            SDL_Texture* game_over_texture = render_text(game->renderer, "Game Over", game->font, SELECTED_COLOR);
            render_text_at(game->renderer, game_over_texture, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, true);
            SDL_DestroyTexture(game_over_texture);
            SDL_Texture* restart_texture = render_text(game->renderer, "Press R to Restart", game->ui_font, BASE_COLOR);
            render_text_at(game->renderer, restart_texture, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 50, true);
            SDL_DestroyTexture(restart_texture);
        }
    } else if (game->mode == STATE_PAUSED) {
        // printf("Rendering paused state\n");
        render_particles(&game->particles, game->renderer);
        render_player_cube(&game->player, game->renderer);
        render_collectibles(game);
        render_enemy(game);
        render_pause_menu(game);
        render_border(game);
    }
    // printf("Rendering grid overlay\n");
    render_grid_overlay(game);
    // printf("Presenting render\n");
    SDL_RenderPresent(game->renderer);
}
