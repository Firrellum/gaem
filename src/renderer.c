#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include "constants.h"
#include "entities.h"
#include "player.h"
#include "renderer.h"
#include "mechanics.h"

// writes a string to a file
void write_to_file(const char *text);
// renders the entire game scene
void render_game(GameState* game);
// renders the gameplay UI elements like score, health, etc.
void render_gameplay_ui(GameState* game);
// renders the start screen before gameplay starts
void render_start_screen(GameState* game);
// renders text at a specific position
// if centered is true, the text is centered at the given position
void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, bool centered);
// renders the game border on screen
void render_border(GameState* game);
// renders the given text into a texture, using the provided font and color
SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color fg);
// renders a grid overlay, useful for debugging or showing grid-based layouts
void render_grid_overlay(GameState* game);
void render_pause_menu(GameState* game);

SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color fg) {
    // create a surface from the text using the font and color
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text, fg);
    if (!text_surface) {
        return NULL; // return null if surface creation fails
    }

    // create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface); // free surface after texture creation
    if (!texture) {
        write_to_file("Error creating texture from surface.");
        return NULL; // return null if texture creation fails
    }

    // set texture scale mode to linear for smooth scaling
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    return texture; // return the created texture
}

void render_grid_overlay(GameState* game) {
    // set blend mode to allow transparency
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);

    // set the grid color
    SDL_SetRenderDrawColor(game->renderer, GRID_COLOR.r, GRID_COLOR.g, GRID_COLOR.b, GRID_COLOR.a);

    // draw vertical grid lines
    for (int x = GRID_CELL_SIZE; x < WINDOW_WIDTH; x += GRID_CELL_SIZE) {
        SDL_RenderDrawLine(game->renderer, x, 0, x, WINDOW_HEIGHT);
    }

    // draw horizontal grid lines
    for (int y = GRID_CELL_SIZE; y < WINDOW_HEIGHT; y += GRID_CELL_SIZE) {
        SDL_RenderDrawLine(game->renderer, 0, y, WINDOW_WIDTH, y);
    }
}

void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, bool centered) {
    // if the texture is not valid, exit the function
    if (!texture) return;

    int text_width, text_height;
    // query the texture for its width and height
    if (SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height) < 0) {
        write_to_file("Failed to query texture.");
        return;
    }

    // create the rectangle for rendering text
    SDL_Rect text_rect = {
        centered ? x - text_width / 2 : x,  // center the text if needed
        centered ? y - text_height / 2 : y, // center the text if needed
        text_width,
        text_height
    };
    
    // render the texture to the screen
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
}

void render_gameplay_ui(GameState* game) {
    Uint32 current_time = SDL_GetTicks();
    
    // color setup for score and health UI
    SDL_Color ui_score_color = {0, 122, 122, 200};
    SDL_Color ui_hp_color;

    // check player's health and adjust UI color accordingly
    if(game->player.hp <= 30){
        ui_hp_color = (SDL_Color){126, 33, 2, 200}; // low health (red)
    } else {
        ui_hp_color = (SDL_Color){76, 126, 12, 200}; // good health (green)
    }

    char score_text[32];
    char hp_text[32];

    // update last score time when the score changes
    if(game->score != game->last_score){
        game->player.last_score_update_time = current_time;
        game->last_score = game->score;
    }

    // temporarily highlight score if it has recently changed
    if (current_time - game->player.last_score_update_time < 300){
        ui_score_color = (SDL_Color){255, 215, 0, 200}; // highlight score (yellow)
    }

    // update last health time when the health changes
    if(game->player.hp != game->player.last_hp){
        game->player.last_hp_update_time = current_time;
        game->player.last_hp = game->player.hp;
    }

    // temporarily highlight health if it has recently changed
    if(current_time - game->player.last_hp_update_time < 300){
        ui_hp_color = (SDL_Color){126, 33, 2, 200}; // highlight health (red)
    }

    // convert score and health to text
    sprintf(score_text, "%d", game->score);
    sprintf(hp_text, "%d", game->player.hp);

    // render score and health text as textures
    SDL_Texture* player_score_texture = render_text(game->renderer, score_text, game->ui_font, ui_score_color);
    SDL_Texture* player_hp_texture = render_text(game->renderer, hp_text, game->ui_font, ui_hp_color);

    // draw the score and health on screen at specified positions
    render_text_at(game->renderer, player_score_texture, 12, 6, false);
    render_text_at(game->renderer, player_hp_texture, WINDOW_WIDTH - 130, 6, false);
    
    // clean up textures after rendering
    SDL_DestroyTexture(player_score_texture);
    SDL_DestroyTexture(player_hp_texture);
}

void render_start_screen(GameState* game) {
    // set colours for title and button text
    SDL_Color button_color = {255, 255, 255, 255}; // white
    SDL_Color title_color = {0, 255, 255, 255};  // cyan

    // create the textures for title and button text using the fonts
    SDL_Texture* game_title_texture = render_text(game->renderer, "F Cubed", game->title_font, title_color);
    SDL_Texture* button_text_texture = render_text(game->renderer, "Enter to start..", game->font, button_color);

    // render the button text (centered)
    render_text_at(game->renderer, button_text_texture, WINDOW_WIDTH / 2, 400, true);

    // render the game title text (centered)
    render_text_at(game->renderer, game_title_texture, WINDOW_WIDTH / 2, game->title_y, true);

    // cleanup textures after rendering
    SDL_DestroyTexture(game_title_texture);
    SDL_DestroyTexture(button_text_texture);
}

void render_border(GameState* game) {
    // set the border color
    SDL_SetRenderDrawColor(game->renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);

    // draw the border with a loop, reducing size for each iteration
    for (int i = 0; i < BORDER_SIZE; i++) {
        SDL_Rect border_rect = {i, i, WINDOW_WIDTH - (2 * i), WINDOW_HEIGHT - (2 * i)};
        SDL_RenderDrawRect(game->renderer, &border_rect);
    }
}

void render_pause_menu(GameState* game) {
    // set blend mode for smooth transparency
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);

    // set the background color and draw the backdrop
    SDL_SetRenderDrawColor(game->renderer, MENU_BACKDROP.r, MENU_BACKDROP.g, MENU_BACKDROP.b, MENU_BACKDROP.a);
    SDL_Rect backdrop = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(game->renderer, &backdrop);
    SDL_RenderClear(game->renderer);

    // define colors for selected and unselected options
    SDL_Color selected_color = SELECTED_COLOR;  
    SDL_Color unselected_color = BASE_COLOR;  

    // calculate menu positioning based on number of options
    int y_offset = WINDOW_HEIGHT / 2 - (game->pause_menu.option_count * 50) / 2;

    // loop through each menu option
    for (int i = 0; i < game->pause_menu.option_count; i++) {
        // choose color based on whether the option is selected
        SDL_Color color = game->pause_menu.options[i].selected ? selected_color : unselected_color;
        SDL_Texture* option_texture = render_text(game->renderer, game->pause_menu.options[i].text, game->font, color);
        render_text_at(game->renderer, option_texture, WINDOW_WIDTH / 2, y_offset + i * 50, true);
        SDL_DestroyTexture(option_texture);  // clean up texture
    }
}

void render_settings_menu(GameState* game) {
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(game->renderer, MENU_BACKDROP.r, MENU_BACKDROP.g, MENU_BACKDROP.b, MENU_BACKDROP.a);
    SDL_Rect backdrop = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(game->renderer, &backdrop);
    SDL_RenderClear(game->renderer);

    SDL_Color selected_color = SELECTED_COLOR;
    SDL_Color unselected_color = BASE_COLOR;

    int y_offset = WINDOW_HEIGHT / 2 - (game->settings_menu.option_count * 50) / 2;

    for (int i = 0; i < game->settings_menu.option_count; i++) {
        char option_text[64];
        if (i == 0) {
            snprintf(option_text, sizeof(option_text), "Music Volume: %d", game->music_volume);
        } else if (i == 1) {
            snprintf(option_text, sizeof(option_text), "SFX Volume: %d", game->sfx_volume);
        } else if (i == 2) {
            const char* fps_text = game->target_fps == 0 ? "Uncapped" : (game->target_fps == 60 ? "60 FPS" : "120 FPS");
            snprintf(option_text, sizeof(option_text), "Frame Rate: %s", fps_text);
        } else {
            snprintf(option_text, sizeof(option_text), "%s", game->settings_menu.options[i].text); 
        }

        SDL_Color color = game->settings_menu.options[i].selected ? selected_color : unselected_color;
        SDL_Texture* option_texture = render_text(game->renderer, option_text, game->font, color);
        render_text_at(game->renderer, option_texture, WINDOW_WIDTH / 2, y_offset + i * 50, true);
        SDL_DestroyTexture(option_texture);
    }
}

void render_game(GameState* game) {
    
    // clear screen with black
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    // render start screen
    if (game->mode == STATE_START_SCREEN) {
        render_grid_overlay(game);
        render_start_screen(game); 
        render_border(game);
        // render_enemy(game);  
        // render_player_cube(&game->player, game->renderer);  

    // render gameplay
    } else if (game->mode == STATE_PLAYING) {
        render_grid_overlay(game);
        render_border(game);
        render_particles(&game->particles, game->renderer);
        render_player_cube(&game->player, game->renderer);
        render_collectibles(game);
        render_enemy(game);
        render_line_enemies(game);
        render_gameplay_ui(game);

        // render game over screen
        if (game->game_over) {
            render_grid_overlay(game);
            
            SDL_Texture* game_over_texture = render_text(game->renderer, "Game Over", game->font, SELECTED_COLOR);
            render_text_at(game->renderer, game_over_texture, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50, true);
            SDL_DestroyTexture(game_over_texture);
            
            char score_text[32];
            snprintf(score_text, sizeof(score_text), "Score : %d", game->score);
            SDL_Texture* game_over_score = render_text(game->renderer, score_text, game->font, BASE_COLOR);
            render_text_at(game->renderer, game_over_score, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, true);
            SDL_DestroyTexture(game_over_score);

            SDL_Texture* restart_texture = render_text(game->renderer, "Press R to Restart", game->font, BASE_COLOR);
            render_text_at(game->renderer, restart_texture, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 50, true);
            SDL_DestroyTexture(restart_texture);
        }
    } 

    // render pause menu
    else if (game->mode == STATE_PAUSED) {
        render_particles(&game->particles, game->renderer);
        render_player_cube(&game->player, game->renderer);
        render_collectibles(game);
        render_enemy(game);
        render_pause_menu(game);
        render_grid_overlay(game);
        render_border(game);
    } else if (game->mode == STATE_SETTINGS) {
        render_particles(&game->particles, game->renderer);
        render_player_cube(&game->player, game->renderer);
        render_collectibles(game);
        render_enemy(game);
        render_settings_menu(game);
        render_grid_overlay(game);
        render_border(game);
    }

    // present the final render
    SDL_RenderPresent(game->renderer);
}

