#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "constants.h"
#include "entities.h"
#include "player.h"
#include "utils.h"

FILE *file; // use write_to_file(); to output to the log.txt

bool init_game(GameState* game);
void write_to_file(const char *text);
void setup(GameState* game);
void calculate_delta_time(GameState* game);
void render_game(GameState* game, TTF_Font* font);
void render_start_screen(GameState* game, TTF_Font* font);
SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color fg);
void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);
void render_border(GameState* game);
void update_game(GameState* game);
void handle_inputs(GameState* game, Player* player);
void cleanup_and_quit(GameState* game, TTF_Font* font);

void write_to_file(const char* text){
    fprintf(file,"%s\n", text);
}

void cleanup_and_quit(GameState* game, TTF_Font* font){
    write_to_file("Destroying renderer.");
    SDL_DestroyRenderer(game->renderer);
    write_to_file("Destroying window.");
    SDL_DestroyWindow(game->window);
    write_to_file("Closing ttf.");
    TTF_CloseFont(font);
    TTF_Quit();
    write_to_file("Exiting..0/");
    fclose(file);
    SDL_Quit();
}

void handle_inputs(GameState* game, Player* player) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                if (game->mode == STATE_PLAYING) {
                    game->mode = STATE_PAUSED;  // pause on escape
                    game->paused_dx = player->dx;
                    game->paused_dy = player->dy; // stor velocity
                    player->dx = 0; //
                    player->dy = 0; // stop cube moving on menu
                } else if (game->mode == STATE_PAUSED) {
                    game->mode = STATE_PLAYING; // unpause on escape
                    player->dx = game->paused_dx;
                    player->dy = game->paused_dy; // restore velocity
                }
            }
            // start on enter
            if (game->mode == STATE_START_SCREEN && event.key.keysym.sym == SDLK_RETURN) {
                game->mode = STATE_PLAYING;  
            }
            // quit on escape from start screen or desktop option
            if ((game->mode == STATE_START_SCREEN && event.key.keysym.sym == SDLK_ESCAPE) ||
                (game->mode == STATE_PAUSED && game->pause_menu.selected_index == 3 && event.key.keysym.sym == SDLK_RETURN)) {
                game->running = false;
            }
        }
    }

    if (game->mode == STATE_PLAYING) {
         // get keyboard state
        const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
        // scope player
        Player updated_player = *player;
        updated_player.dx = 0;
        updated_player.dy = 0;
        // update movement
        if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP]) updated_player.dy = -1;
        if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN]) updated_player.dy = 1;
        if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT]) updated_player.dx = -1;
        if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) updated_player.dx = 1;

        // keep diag movement at the same speed as x and y movement
        if (updated_player.dx != 0 && updated_player.dy != 0) {
            float length = sqrt(updated_player.dx * updated_player.dx + updated_player.dy * updated_player.dy);
            updated_player.dx /= length;
            updated_player.dy /= length;
        }
        // update global player
        *player = updated_player;

    } else if (game->mode == STATE_PAUSED) {
        // get kb state
        const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

        const float MENU_DELAY_LOCAL = MENU_DELAY;
        // decreae cooldown per fram
        if (game->menu_cooldown > 0){
            game->menu_cooldown -= game->delta_time;
        }
        // move selection with up arrow keys
        if (keyboard_state[SDL_SCANCODE_UP] && game->pause_menu.selected_index > 0 && game->menu_cooldown <= 0) {
            game->pause_menu.options[game->pause_menu.selected_index].selected = false; // deselect current
            game->pause_menu.selected_index--; // move selection up
            game->pause_menu.options[game->pause_menu.selected_index].selected = true; // select new option
            game->menu_cooldown = MENU_DELAY_LOCAL;
            // printf("Moving up, %i.", game->pause_menu.selected_index );
        } // or move selection with down key
        if (keyboard_state[SDL_SCANCODE_DOWN] && game->pause_menu.selected_index < game->pause_menu.option_count - 1 && game->menu_cooldown <= 0) {
            game->pause_menu.options[game->pause_menu.selected_index].selected = false; // "
            game->pause_menu.selected_index++; // "
            game->pause_menu.options[game->pause_menu.selected_index].selected = true; //
            game->menu_cooldown = MENU_DELAY_LOCAL;
            // printf("Moving down, %i.", game->pause_menu.selected_index ); 
        }
        // confirm selection w enter
        if (keyboard_state[SDL_SCANCODE_RETURN] && game->menu_cooldown <= 0) {
            switch (game->pause_menu.selected_index) {
                case 0:  // resume 0
                    game->mode = STATE_PLAYING;
                    break;
                case 1:  // settings
                    write_to_file("Settings selected (not implemented yet).");
                    break;
                case 2:  // quit to main 2
                    // reset cube and particles
                    game->mode = STATE_START_SCREEN;
                    game->player.x = WINDOW_WIDTH / 2 - PLAYER_SIZE / 2;  
                    game->player.y = WINDOW_HEIGHT / 2 - PLAYER_SIZE / 2;
                    game->particles.count = 0;  
                    break;
                case 3:  // qtd 3
                    break;
            }
            game->menu_cooldown = MENU_DELAY_LOCAL;
        }
    }
}

void update_game(GameState* game){
    // get frame time and delay update by it
    int frame_time = SDL_GetTicks() - game->last_frame_time;
        if (frame_time < (FRAME_TARGET_TIME)) {
            SDL_Delay(FRAME_TARGET_TIME - frame_time);
        }
    // calculate delta time
    calculate_delta_time(game);
    
    // update player
    update_player(&game->player, game->delta_time);

    // particles | spawn on move | only update while in playing state
    if (game->mode == STATE_PLAYING) {  
        update_player(&game->player, game->delta_time);
        if (game->particles.spawn_timer >= SPAWN_RATE) {
            game->particles.spawn_timer = 0;
            if (game->player.dx != 0 || game->player.dy != 0) {
                spawn_particle(&game->particles, &game->player);
            }
        }
        update_particles(&game->particles, game->delta_time);
    }
}

void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y) {
    // check the texture
    if (!texture) return; 

    // get dimentions
    int text_width, text_height;
    SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height);

    SDL_Rect text_rect = {
        x - text_width / 2,  
        y - text_height / 2, 
        text_width,
        text_height
    };

    // draw the texture
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
}

void render_start_screen(GameState* game, TTF_Font* font) {
    // set colours
    SDL_Color button_color = {255, 255, 255, 255};
    SDL_Color title_color = {0, 255, 255, 255};

    // create the textures with the fonts
    SDL_Texture* game_title_texture = render_text(game->renderer, "F Cubed (f³)", font, title_color);
    SDL_Texture* button_text_texture = render_text(game->renderer, "ENTER to Start", font, button_color);

    // render 'button'
    render_text_at(game->renderer, button_text_texture, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    // render title text
    render_text_at(game->renderer, game_title_texture, WINDOW_WIDTH / 2, 100);

    // TODO refactor the repetition here
    SDL_DestroyTexture(game_title_texture);
    SDL_DestroyTexture(button_text_texture);
}

void render_border(GameState* game){
    SDL_SetRenderDrawColor(game->renderer, 0, 255, 255, 75); // color border

    // border || SDL defaults to one pixel wide | for loop
    for (int i = 0; i < BORDER_SIZE; i++){
        SDL_Rect border_rect = {i, i, WINDOW_WIDTH - (2 * i), WINDOW_HEIGHT - (2 * i)};
        SDL_RenderDrawRect(game->renderer, &border_rect);
    }
    // SDL_Rect border_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    // NEED TO SUPTRACT BORDER FROM PLAYER BOUNDS
    
}

void render_pause_menu(GameState* game, TTF_Font* font) {
    // color highlights and main
    SDL_Color selected_color = {0, 255, 255, 255};  
    SDL_Color unselected_color = {255, 255, 255, 255};  

    int y_offset = WINDOW_HEIGHT / 2 - (game->pause_menu.option_count * 50) / 2;  // menu pos
    // loop for each item
    for (int i = 0; i < game->pause_menu.option_count; i++) {
        SDL_Color color = game->pause_menu.options[i].selected ? selected_color : unselected_color;
        SDL_Texture* option_texture = render_text(game->renderer, game->pause_menu.options[i].text, font, color);
        render_text_at(game->renderer, option_texture, WINDOW_WIDTH / 2, y_offset + i * 50);
        SDL_DestroyTexture(option_texture);  // cleaning
    }
}

void render_game(GameState* game, TTF_Font* font){
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255); // Set colot 
    SDL_RenderClear(game->renderer); // clear Screen
    
    if(game->mode == STATE_START_SCREEN){
        render_start_screen(game, font);
        render_border(game); // render border
    }else if (game->mode == STATE_PLAYING){
        render_border(game); // render border
        render_particles(&game->particles, game->renderer); // pender particles
        render_player_cube(&game->player, game->renderer); // render player
    } else if (game->mode == STATE_PAUSED) {
        render_border(game);
        render_particles(&game->particles, game->renderer);  // keep particles visible but frozen
        render_player_cube(&game->player, game->renderer);
        render_pause_menu(game, font);  // overlay the pause menu
    }
    
    SDL_RenderPresent(game->renderer); // present render

    // order matters here
}

void calculate_delta_time(GameState* game) {
    Uint32 current_time = SDL_GetTicks();  
    game->delta_time = (current_time - game->last_frame_time) / 1000.0f; 
    game->last_frame_time = current_time;
}

void setup(GameState* game){
    // set up the cube init variables
    game->player.x = WINDOW_WIDTH / 2 - PLAYER_SIZE / 2;
    game->player.y = WINDOW_HEIGHT / 2 - PLAYER_SIZE / 2;
    game->player.size = PLAYER_SIZE;
    game->player.dx = 0;
    game->player.dy = 0;
    game->player.alive = true;
    game->player.speed = MOVE_SPEED;

    game->pause_menu.options[0] = (MenuOption){"Resume", false};
    game->pause_menu.options[1] = (MenuOption){"Settings", false};
    game->pause_menu.options[2] = (MenuOption){"Quit to Main Menu", false};
    game->pause_menu.options[3] = (MenuOption){"Quit to Desktop", false};
    game->pause_menu.option_count = 4;
    game->pause_menu.selected_index = 0;  // starting option 
    game->pause_menu.options[game->pause_menu.selected_index].selected = true;
    
    game->menu_cooldown = 0.0f;

    write_to_file("Spawn cube and menus.");

}

bool init_game(GameState* game){

    file = fopen("log.txt", "w");
    write_to_file("Hello Game!");

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        write_to_file("Error initializing SDL.");
        return false;
    } else { write_to_file("Initialized SDL | VIDEO");};

    if (TTF_Init() == -1){
        write_to_file("Error initializing SDL_TTF.");
        return false;
    } else { write_to_file("Initialized SDL_TTF.");};

    game->font = TTF_OpenFont("./src/assets/font.ttf", 48);
    if(!game->font){
        write_to_file("Error loading font.");
        return false;
    }else{ write_to_file("Font loaded");};

    game->window = SDL_CreateWindow(
        NULL, // window name
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, // window screen position
        WINDOW_WIDTH, WINDOW_HEIGHT, // window size
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS); // borderless

    if(!game->window){
        write_to_file("Error creating SDL window!");
        return false;
    }else{write_to_file("Created window.");};

    game->renderer = SDL_CreateRenderer(game->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); //gpu accelaratoin and vsync

    if(!game->renderer){
        write_to_file("Error creating SDL renderer!");
        return false;
    }else{ write_to_file("Created renderer.");};

    // Enable alpha blending for the particles
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    // game state variables
    game->running = true;
    game->mode = STATE_START_SCREEN;
    game->last_frame_time = SDL_GetTicks() / FRAME_TARGET_TIME;
    game->delta_time = 0.0;
    game->score = 0; 
    game->game_over = false;

    return true;
}

int main(int argc, char *argv[]){
    srand(time(NULL));

    // create a new gamestate with no defined parameters
    GameState game = {0};
    game.running = true;
    if(!init_game(&game)){
        return 1;
    }

    setup(&game); // spawn Fcube

    // game loop
    while(game.running){
        handle_inputs(&game, &game.player); // handle inputs
        update_game(&game); // update game
        render_game(&game, game.font); // render game
        
    }

    cleanup_and_quit(&game, game.font);
    return 0;
}

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