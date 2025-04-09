#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
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
#include "mechanics.h"

FILE *file; 
// use write_to_file(); to output to the log.txt
// writes a line of text to the log file
void write_to_file(const char *text);
// sets up sdl and other subsystems
bool init_game(GameState* game);
// loads the main game font
bool load_font(GameState* game);
// initializes game state variables
void setup(GameState* game);
// updates delta time based on frame duration
void calculate_delta_time(GameState* game);
// handles logic updates per frame
void update_game(GameState* game);
// processes keyboard and controller input
void handle_inputs(GameState* game, Player* player);
// draws everything to the screen
void render_game(GameState* game);
// resets everything to initial state
void reset_game(GameState* game);
// frees resources and exits cleanly
void cleanup_and_quit(GameState* game, TTF_Font* font, TTF_Font* ui_font);


void write_to_file(const char* text){
    // write text to log file with newline
    fprintf(file,"%s\n", text);
}

void cleanup_and_quit(GameState* game, TTF_Font* font, TTF_Font* ui_font){
    // destroy renderer
    write_to_file("Destroying renderer.");
    SDL_DestroyRenderer(game->renderer);

    // destroy window
    write_to_file("Destroying window.");
    SDL_DestroyWindow(game->window);

    // close fonts
    write_to_file("Closing ttfS.");
    TTF_CloseFont(font);
    TTF_CloseFont(ui_font);

    // quit ttf
    TTF_Quit();

    // free sound
    Mix_FreeChunk(game->pick_up_sound);

    // close audio
    Mix_CloseAudio();

    // quit mixer
    Mix_Quit();

    // quit image
    IMG_Quit();

    // final log message
    write_to_file("Exiting..0/");

    // close log file
    fclose(file);

    // quit sdl
    SDL_Quit();
}

void reset_game(GameState* game) {
    // spawn a fresh enemy
    spawn_enemy(game);

    // reset player position to center
    game->player.x = WINDOW_WIDTH / 2 - PLAYER_SIZE / 2;
    game->player.y = WINDOW_HEIGHT / 2 - PLAYER_SIZE / 2;

    // stop player movement
    game->player.dx = 0;
    game->player.dy = 0;

    // restore player health
    game->player.hp = 100;

    // clear collision flag
    game->isCollided = false;

    // mark player as alive
    game->player.alive = true;

    // reset score
    game->score = 0;

    // clear game over flag
    game->game_over = false;

    // clear particles
    game->particles.count = 0;

    // reset particle timer
    game->particles.spawn_timer = 0;

    // spawn collectibles
    spawn_collectibles(game);

    // spawn another enemy
    spawn_enemy(game);

    // reinit line enemies
    init_line_enemies(game);
}

bool isMuted = false;

void handle_inputs(GameState* game, Player* player) {
    // event variable for polling
    SDL_Event event;

    // process all sdl events
    while (SDL_PollEvent(&event)) {

        // check for key presses
        if (event.type == SDL_KEYDOWN) {

            // toggle music mute
            if(event.key.keysym.sym == SDLK_m){
                if (!isMuted){
                    Mix_VolumeMusic(0);
                    isMuted = true;
                } else {
                    Mix_VolumeMusic(MIX_MAX_VOLUME / 8);
                    isMuted = false;
                }
            }

            // pause or unpause with escape
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                if (game->mode == STATE_PLAYING) {
                    game->mode = STATE_PAUSED;
                    game->paused_dx = player->dx;
                    game->paused_dy = player->dy;
                    player->dx = 0;
                    player->dy = 0;
                } else if (game->mode == STATE_PAUSED) {
                    game->mode = STATE_PLAYING;
                    player->dx = game->paused_dx;
                    player->dy = game->paused_dy;
                }
            }

            // start game from start screen
            if (game->mode == STATE_START_SCREEN && event.key.keysym.sym == SDLK_RETURN) {
                game->mode = STATE_PLAYING;
                if (Mix_PlayChannel(-1, game->start_sound, 0) == -1) {
                    printf("Failed to play start sound! Mix_Error: %s\n", Mix_GetError());
                }
            }

            // quit from start or pause menu
            if ((game->mode == STATE_START_SCREEN && event.key.keysym.sym == SDLK_ESCAPE) ||
                (game->mode == STATE_PAUSED && game->pause_menu.selected_index == 3 && event.key.keysym.sym == SDLK_RETURN)) {
                game->running = false;
            }

            // restart on 'r' if game over
            if (game->game_over && event.key.keysym.sym == SDLK_r) {
                if (Mix_PlayChannel(-1, game->start_sound, 0) == -1) {
                    printf("Failed to play start sound! Mix_Error: %s\n", Mix_GetError());
                }
                game->restart_requested = true;
            }
            // return to start screen from game over
            else if (game->game_over && event.key.keysym.sym == SDLK_ESCAPE){
                game->mode = STATE_START_SCREEN;
                game->restart_requested = true;
            }
        }
    }

    // reset game if flagged
    if (game->restart_requested) {
        reset_game(game);
        game->restart_requested = false;
    }

    // player movement if playing
    if (game->mode == STATE_PLAYING) {
        // get key state
        const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

        // make temp player for changes
        Player updated_player = *player;
        updated_player.dx = 0;
        updated_player.dy = 0;

        // apply directional input
        if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP]) updated_player.dy = -1;
        if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN]) updated_player.dy = 1;
        if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT]) updated_player.dx = -1;
        if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) updated_player.dx = 1;

        // normalize diagonal speed
        if (updated_player.dx != 0 && updated_player.dy != 0) {
            float length = sqrt(updated_player.dx * updated_player.dx + updated_player.dy * updated_player.dy);
            updated_player.dx /= length;
            updated_player.dy /= length;
        }

        // apply changes
        *player = updated_player;

    } else if (game->mode == STATE_PAUSED) {
        // get key state for menu nav
        const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

        // local menu cooldown constant
        const float MENU_DELAY_LOCAL = MENU_DELAY;

        // decrease cooldown
        if (game->menu_cooldown > 0){
            game->menu_cooldown -= game->delta_time;
        }

        // navigate up
        if (keyboard_state[SDL_SCANCODE_UP] && game->pause_menu.selected_index > 0 && game->menu_cooldown <= 0) {
            game->pause_menu.options[game->pause_menu.selected_index].selected = false;
            game->pause_menu.selected_index--;
            game->pause_menu.options[game->pause_menu.selected_index].selected = true;
            game->menu_cooldown = MENU_DELAY_LOCAL;
        }

        // navigate down
        if (keyboard_state[SDL_SCANCODE_DOWN] && game->pause_menu.selected_index < game->pause_menu.option_count - 1 && game->menu_cooldown <= 0) {
            game->pause_menu.options[game->pause_menu.selected_index].selected = false;
            game->pause_menu.selected_index++;
            game->pause_menu.options[game->pause_menu.selected_index].selected = true;
            game->menu_cooldown = MENU_DELAY_LOCAL;
        }

        // select menu option
        if (keyboard_state[SDL_SCANCODE_RETURN] && game->menu_cooldown <= 0) {
            switch (game->pause_menu.selected_index) {
                case 0:
                    game->mode = STATE_PLAYING;
                    break;
                case 1:
                    write_to_file("Settings selected (not implemented yet).");
                    break;
                case 2:
                    game->mode = STATE_START_SCREEN;
                    game->player.x = WINDOW_WIDTH / 2 - PLAYER_SIZE / 2;
                    game->player.y = WINDOW_HEIGHT / 2 - PLAYER_SIZE / 2;
                    game->particles.count = 0;
                    break;
                case 3:
                    // exit handled earlier
                    break;
            }
            game->menu_cooldown = MENU_DELAY_LOCAL;
        }
    }
}

void update_game(GameState* game){
    
    // get time since last frame
    int frame_time = SDL_GetTicks() - game->last_frame_time;

    // delay to cap frame rate
    if (frame_time < (FRAME_TARGET_TIME)) {
        SDL_Delay(FRAME_TARGET_TIME - frame_time);
    }
    
    // compute delta time
    calculate_delta_time(game);
    
    // advance animation timer
    game->animation_time += game->delta_time;

    // move title up and down with sine wave
    game->title_y = 200.0f + sinf(game->animation_time * 1.0f) * 30.0f;

    // update logic if game is active
    if (game->mode == STATE_PLAYING && !game->game_over) {
        
        // move player
        update_player(&game->player, game->delta_time);

        // update collectibles
        update_collectibles(game);

        // handle enemy behavior
        update_enemy(game);

        // update line enemies
        update_line_enemies(game);

        // check particle timer
        if (game->particles.spawn_timer >= SPAWN_RATE) {
            game->particles.spawn_timer = 0;

            // spawn if player moved
            if (game->player.dx != 0 || game->player.dy != 0) {
                spawn_particle(game, &game->particles, &game->player);
            }
        }

        // update existing particles
        update_particles(&game->particles, game->delta_time);
    }
}

void calculate_delta_time(GameState* game) {
    // get current time in ms
    Uint32 current_time = SDL_GetTicks();  

    // compute seconds since last frame
    game->delta_time = (current_time - game->last_frame_time) / 1000.0f; 

    // set last frame time to now
    game->last_frame_time = current_time;
}

void setup(GameState* game){
    // set initial player values
    game->player.x = WINDOW_WIDTH / 2 - PLAYER_SIZE / 2;
    game->player.y = WINDOW_HEIGHT / 2 - PLAYER_SIZE / 2;
    game->player.size = PLAYER_SIZE;
    game->player.dx = 0;
    game->player.dy = 0;
    game->player.alive = true;
    game->isCollided = false;
    game->player.speed = MOVE_SPEED;
    game->player.hp = 100;
    game->player.last_hp = 100;
    game->player.last_collide_update_time = 1;

    // set other initial values
    game->last_score = 0;
    game->title_y = 200.0f; 
    game->animation_time = 0.0f;
    game->restart_requested = false;

    // spawn collectibles and enemies
    spawn_collectibles(game);
    spawn_enemy(game);

    // initialize line enemies
    init_line_enemies(game);

    // fill out pause menu options
    game->pause_menu.options[0] = (MenuOption){"Resume", false};
    game->pause_menu.options[1] = (MenuOption){"Settings", false};
    game->pause_menu.options[2] = (MenuOption){"Quit to Main Menu", false};
    game->pause_menu.options[3] = (MenuOption){"Quit to Desktop", false};
    game->pause_menu.option_count = 4;
    game->pause_menu.selected_index = 0;
    game->pause_menu.options[game->pause_menu.selected_index].selected = true;

    // reset cooldown for menu input
    game->menu_cooldown = 0.0f;

    // note setup finished
    write_to_file("Spawn cube and menus.");
}

bool load_font(GameState* game){

    // load main font at size 48
    game->font = TTF_OpenFont("./src/assets/font.ttf", 48);
    if(!game->font){
        write_to_file("Error loading font.");
        return false;
    } else {
        write_to_file("Font loaded");
    }

    // load ui font at size 72
    game->ui_font = TTF_OpenFont("./src/assets/font.ttf", 72);
    if(!game->ui_font){
        write_to_file("Error loading ui_font.");
        return false;
    } else {
        write_to_file("UIFont loaded");
    }

    // load title font at size 96
    game->title_font = TTF_OpenFont("./src/assets/font.ttf", 96);
    if(!game->title_font){
        write_to_file("Error loading title_font.");
        return false;
    } else {
        write_to_file("title_ Font loaded");
    }

    return true;
}

bool init_game(GameState* game){

    // open log file for writing
    file = fopen("log.txt", "w");
    // log initial message
    write_to_file("Hello Game!");

    // init sdl with video, audio, and png support
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | IMG_INIT_PNG) < 0){
        write_to_file("Error initializing SDL.");
        return false;
    } else { write_to_file("Initialized SDL | VIDEO");};

    // init sdl_ttf for font handling
    if (TTF_Init() == -1){
        write_to_file("Error initializing SDL_TTF.");
        return false;
    } else { write_to_file("Initialized SDL_TTF.");};

    // load game fonts
    load_font(game);
    
    printf("Initializing SDL_mixer...\n");
    // set up audio with 44100hz, default format, stereo, 2048 buffer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
        return 1;
    }
    printf("SDL_mixer initialized successfully\n");

    // load pickup sound effect
    game->pick_up_sound = Mix_LoadWAV("./src/assets/pick_up.wav");
    if (game->pick_up_sound == NULL) {
        write_to_file("Failed to load pick_up.wav! Mix_Error");
        return false;
    }else{write_to_file("Loaded pickup noise.");};

    // load death sound effect
    game->dead_sound = Mix_LoadWAV("./src/assets/dead_sound.wav");
    if (game->dead_sound == NULL) {
        write_to_file("Failed to load dead_sound.wav! Mix_Error");
        return false;
    }else{write_to_file("Loaded dead noise.");};

    // load start sound effect
    game->start_sound = Mix_LoadWAV("./src/assets/start_sound.wav");
    if (game->start_sound == NULL) {
        write_to_file("Failed to load start_sound.wav! Mix_Error");
        return false;
    }else{write_to_file("Loaded start noise.");};

    // load hit sound effect
    game->hit_sound = Mix_LoadWAV("./src/assets/hit_sound.wav");
    if (game->hit_sound == NULL) {
        write_to_file("Failed to load hit_sound.wav! Mix_Error");
        return false;
    }else{write_to_file("Loaded hit noise.");};

    // load main background music
    game->main_sound_loop = Mix_LoadMUS("./src/assets/main_sound_loop.mp3");
    if (game->main_sound_loop == NULL) {
        write_to_file("Failed to load main_sound_loop.mp3! Mix_Error");
        return false;
    }else{write_to_file("Loaded start main_sound_loop noise.");};

    // set music volume to 1/8th max
    Mix_VolumeMusic(MIX_MAX_VOLUME / 8);
    // play music on loop, -1 means infinite
    if (Mix_PlayMusic(game->main_sound_loop, -1) == -1) {  
        write_to_file("Failed to play main_sound_loop! Mix_Error");
        return false;
    } 

    // create game window, centered, borderless
    game->window = SDL_CreateWindow(
        NULL, // window name
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, // window screen position
        WINDOW_WIDTH, WINDOW_HEIGHT, // window size
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS); // borderless

    if(!game->window){
        write_to_file("Error creating SDL window!");
        return false;
    }else{write_to_file("Created window.");};

    // create renderer with gpu acceleration and vsync
    game->renderer = SDL_CreateRenderer(game->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(!game->renderer){
        write_to_file("Error creating SDL renderer!");
        return false;
    }else{ write_to_file("Created renderer.");};

    // load window icon
    SDL_Surface* icon = IMG_Load("./src/assets/F_icon_transparent.ico");
    if (!icon){
        write_to_file("F Icon loaded.");
    }else{write_to_file("Failed to load F icon.");};

    // set icon and free surface
    SDL_SetWindowIcon(game->window, icon);
    SDL_FreeSurface(icon);

    // enable alpha blending for particles
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    // set render scaling quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    // set initial game state values
    game->running = true;
    game->mode = STATE_START_SCREEN;
    game->last_frame_time = SDL_GetTicks() / FRAME_TARGET_TIME;
    game->delta_time = 0.0;
    game->score = 0; 
    game->game_over = false;

    return true;
}

int main(int argc, char *argv[]){
    // initialize random seed with current time for randomization
    srand(time(NULL));

    // create a new gamestate with no defined parameters
    // start with a zeroed-out gamestate
    GameState game = {0};
    // set the game to active
    game.running = true;
    
    // attempt to initialize game settings
    if(!init_game(&game)){
        // return error if initialization fails
        return 1;
    }

    // set up initial game state, spawn cube and other elements
    setup(&game);

    // game loop
    // run while the game is active
    while(game.running){
        // process player inputs
        handle_inputs(&game, &game.player);
        
        // update game logic and state
        update_game(&game);
        
        // render the current frame
        render_game(&game);
    }

    // clean up resources and quit the game
    cleanup_and_quit(&game, game.font, game.ui_font);
    return 0;
}


