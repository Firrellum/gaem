#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"
#include "entities.h"
#include "player.h"

FILE *file;

bool init_game(GameState* game);
void write_to_file(const char *text);
void setup(GameState* game);
void calculate_delta_time(GameState* game);
void render_game(GameState* game);
void handle_inputs(GameState* game);
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

void handle_inputs(GameState* game){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
            game->running = false;
        }
    }
}

void render_game(GameState* game){
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255); // Set colot
    SDL_RenderClear(game->renderer); // Clear Screen
    render_player_cube(&game->player, game->renderer);
    SDL_RenderPresent(game->renderer); // Present render
}

void calculate_delta_time(GameState* game){
    double current_time = SDL_GetTicks() / FRAME_TARGET_TIME;
    game->delta_time = current_time - game->last_frame_time;
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
    game->last_frame_time = SDL_GetTicks() / FRAME_TARGET_TIME;
    game->delta_time = 0.0;
    game->score = 0; 
    game->game_over = false;

    return true;
}

int main(int argc, char *argv[]){
    // srand(time(NULL));

    // create a ew gamestate with no defined parameters
    GameState game = {0};
    game.running = true;
    if(!init_game(&game)){
        return 1;
    }

    setup(&game); // spawn Fcube

    // game loop
    while(game.running){
        // calculate delta time and set frame rate
        calculate_delta_time(&game);
        // handle inputs
        handle_inputs(&game); // Just esc for now
        render_game(&game); // render game
        
    }

    cleanup_and_quit(&game);
    return 1;
}

