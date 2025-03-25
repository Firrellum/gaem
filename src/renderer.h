#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include "entities.h"

void render_game(GameState* game);
void render_gameplay_ui(GameState* game);
void render_start_screen(GameState* game);
void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, bool centered);
void render_border(GameState* game);
SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color fg);
void render_grid_overlay(GameState* game);

#endif 