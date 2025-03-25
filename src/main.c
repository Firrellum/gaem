#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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
void render_game(GameState* game);
void render_gameplay_ui(GameState* game);
void render_start_screen(GameState* game);
SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color fg);
void render_text_at(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);
void render_border(GameState* game);
void update_game(GameState* game);
void handle_inputs(GameState* game, Player* player);
void cleanup_and_quit(GameState* game, TTF_Font* font, TTF_Font* ui_font);
void render_grid_overlay(GameState* game);

void write_to_file(const char* text){
    fprintf(file,"%s\n", text);
}

void cleanup_and_quit(GameState* game, TTF_Font* font, TTF_Font* ui_font){
    write_to_file("Destroying renderer.");
    SDL_DestroyRenderer(game->renderer);
    write_to_file("Destroying window.");
    SDL_DestroyWindow(game->window);
    write_to_file("Closing ttfS.");
    TTF_CloseFont(font);
    TTF_CloseFont(ui_font);
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

    game->ui_info.options[0] = (UiOptions){"Score: "};
    game->ui_info.options[0] = (UiOptions){"HP: "};
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

    game->ui_font = TTF_OpenFont("./src/assets/font.ttf", 24);
    if(!game->ui_font){
        write_to_file("Error loading ui_font.");
        return false;
    }else{ write_to_file("UIFont loaded");};

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

    SDL_Surface* icon = IMG_Load("./src/assets/F_icon_transparent.ico");
    if (!icon){
        write_to_file("Failed to load F icon.");
    }else{write_to_file("F Icon loaded.");};

    SDL_SetWindowIcon(game->window, icon);
    SDL_FreeSurface(icon);

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
        render_game(&game); // render game
        
    }

    cleanup_and_quit(&game, game.font, game.ui_font);
    return 0;
}

