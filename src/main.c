#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "constants.h"
#include "entities.h"
#include "player.h"

FILE *file; // use write_to_file(); to output to the log.txt
 
bool init_game(GameState* game);
void write_to_file(const char *text);
void setup(GameState* game);
void calculate_delta_time(GameState* game);
void render_game(GameState* game);
void render_start_screen(GameState* game);
void render_border(GameState* game);
void update_game(GameState* game);
void handle_inputs(GameState* game, Player* player);
void cleanup_and_quit(GameState* game);

void write_to_file(const char* text){
    fprintf(file,"%s\n", text);
}

void cleanup_and_quit(GameState* game){
    write_to_file("Destroying renderer.");
    SDL_DestroyRenderer(game->renderer);
    write_to_file("Destroying widow.");
    SDL_DestroyWindow(game->window);
    write_to_file("Exiting..0/");
    fclose(file);
    SDL_Quit();
}

void handle_inputs(GameState* game, Player* player){
    SDL_Event event;

    // exit on escape
    while(SDL_PollEvent(&event)){
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
            game->running = false;
        }
        if (game->mode == STATE_START_SCREEN && event.key.keysym.sym == SDLK_RETURN) {
            game->mode = STATE_PLAYING; // Switch to gameplay when Enter is pressed
        }
    }


    if(game->mode == STATE_PLAYING){
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

    // particles | spawn on move
    if (game->particles.spawn_timer >= SPAWN_RATE){
        game->particles.spawn_timer = 0;
        if((game->player.dx != 0 || game->player.dy != 0)){
            spawn_particle(&game->particles, &game->player);
        }
    }
    // update spawend particles
    update_particles(&game->particles, game->delta_time);
}

void render_start_screen(GameState* game){
    // SDL_SetRenderDrawColor(game->renderer, 0, 0, 50, 175);
    SDL_RenderClear(game->renderer);
    SDL_SetRenderDrawColor(game->renderer, 0, 255, 255, 175); // {000, 255, 255}
    SDL_Rect start_button_rect = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 25, 200, 50};
    SDL_RenderDrawRect(game->renderer, &start_button_rect);
    // TODO: get the DLFttf to render text
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

void render_game(GameState* game){
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255); // Set colot 
    SDL_RenderClear(game->renderer); // clear Screen
    if(game->mode == STATE_START_SCREEN){
        render_start_screen(game);
    }else if (game->mode == STATE_PLAYING){
        render_border(game); // render border
        render_particles(&game->particles, game->renderer); // pender particles
        render_player_cube(&game->player, game->renderer); // render player
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

    write_to_file("Spawn cube.");

}

bool init_game(GameState* game){

    file = fopen("log.txt", "w");
    write_to_file("Hello Game!");

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        write_to_file("Error initializing SDL.");
        return false;
    } else { write_to_file("Initialized SDL | VIDEO");};

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
    // srand(time(NULL));

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

    cleanup_and_quit(&game);
    return 0;
}

