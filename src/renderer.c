#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "constants.h"
#include "utils.h"
#include "entities.h"
#include "player.h"
#include "renderer.h"

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
    // check the texture
    if (!texture) return; 

    // get dimentions
    int text_width, text_height;
    SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height);

    SDL_Rect text_rect = {
        centered ? x - text_width / 2 : x,  
        centered ? y - text_height / 2 : y,  
        text_width,
        text_height
    };

    // draw the texture
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
}

void render_gameplay_ui(GameState* game){
    SDL_Color ui_color = {255,255,255,255};

    SDL_Texture* player_score_texture = render_text(game->renderer, "Score :", game->ui_font, ui_color);
    SDL_Texture* player_hp_texture = render_text(game->renderer, "HP :", game->ui_font, ui_color);

    render_text_at(game->renderer, player_score_texture, 48, WINDOW_HEIGHT - 24, false); 
    render_text_at(game->renderer, player_hp_texture, 48, WINDOW_HEIGHT - 48, false); 

    SDL_DestroyTexture(player_score_texture);
    SDL_DestroyTexture(player_hp_texture);
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

void render_game(GameState* game){
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255); // Set colot 
    SDL_RenderClear(game->renderer); // clear Screen
    
    if(game->mode == STATE_START_SCREEN){
        render_start_screen(game);
        render_border(game); // render border
    }else if (game->mode == STATE_PLAYING){

        render_border(game); // render border
        render_gameplay_ui(game);
        render_particles(&game->particles, game->renderer); // pender particles
        render_player_cube(&game->player, game->renderer); // render player
    } else if (game->mode == STATE_PAUSED) {
        render_particles(&game->particles, game->renderer);  // keep particles visible but frozen
        render_player_cube(&game->player, game->renderer);
        render_pause_menu(game);  // overlay the pause menu
        render_border(game);
    }
    render_grid_overlay(game); // testing|align grid
    SDL_RenderPresent(game->renderer); // present render
   
    // order matters here
}

