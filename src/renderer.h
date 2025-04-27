#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include "entities.h"

// renders the entire game, including game state-specific elements like the start screen, gameplay, or pause screen
void render_game(GameState* game);

// renders the gameplay UI such as score, health, and other in-game stats
void render_gameplay_ui(GameState* game);

// renders the start screen, typically including the title and start button
void render_start_screen(GameState* game);

// renders the text at a specific (x, y) position, with an option to center the text
void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, bool centered);

// renders the border around the game screen
void render_border(GameState* game);

// renders text as a texture using a specified font and color
SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color fg);

// renders a grid overlay on the screen, typically for alignment or visual effects
void render_grid_overlay(GameState* game);

void render_settings_menu(GameState* game);

#endif 