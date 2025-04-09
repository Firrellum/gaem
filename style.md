
# Code Style Guidelines

## General Principles

- **Clarity over brevity**: Always write code that is easy to read and understand. The goal is to make the code self-explanatory so others (and your future self) can quickly grasp its purpose.
- **Consistency**: Follow consistent naming conventions, formatting, and style choices across the entire codebase to ensure uniformity.

## Code Formatting

- **Indentation**: Use 4 spaces for indentation. Do not mix tabs and spaces.
- **Line Length**: Limit lines to 80 characters. This ensures code readability, especially on smaller screens or in split windows.
- **Braces**: Always use braces `{}` for control structures (if, for, while, etc.), even for single-line blocks. This helps prevent errors when adding more code later.
  
    ```c
    if (condition) {
        // code
    }
    ```

- **Blank Lines**: Use blank lines to separate logical blocks of code. This enhances readability and makes it easier to follow the flow of the program.

    ```c
    int a = 5;

    // Do something
    int b = 10;
    ```

## Comments

- **Comment Style**: Keep comments simple and clear. They should be concise, without over-explaining.
- **Lowercase**: All comments should be written in lowercase to maintain uniformity.
- **Comment Placement**: Place comments on separate lines above the code they describe. Do not comment at the end of a line.
  
    Example:
    ```c
    // initialize game variables
    GameState game = {0};
    ```

- **Avoid Over-Commenting**: Don’t add comments for obvious things. If the code is straightforward and self-explanatory, you don't need a comment for every line. Focus on describing complex or non-obvious logic.
- **Explain 'Why' and 'What'**: Prefer comments that explain *why* something is done, or what the purpose is, rather than explaining what each line does.
  
    Example:
    ```c
    // toggle mute with 'm' key
    if(event.key.keysym.sym == SDLK_m) {
        // mute or unmute based on current state
    }
    ```

- **Function Headers**: For each function, provide a brief comment explaining its purpose. This is particularly important for functions that perform non-trivial operations.

    Example:
    ```c
    // loads necessary game assets (fonts, sounds, etc.)
    bool load_game_assets(GameState* game) {
        // function implementation
    }
    ```

## Naming Conventions

- **Variables**: Use descriptive names that reflect the purpose of the variable. Use camelCase for local variables and functions, and PascalCase for types and structs.
  
    Example:
    ```c
    int playerHealth;
    float playerSpeed;
    ```

- **Constants**: Constants should be in uppercase, separated by underscores.

    Example:
    ```c
    #define MAX_PLAYER_HEALTH 100
    ```

- **Function Names**: Use verbs to describe the action that the function performs, followed by the object it affects (if applicable).

    Example:
    ```c
    void initialize_game(GameState* game);
    void reset_player(Player* player);
    ```

## Code Structure

- **Modularity**: Break down large functions into smaller, manageable ones. Each function should have a single responsibility.
- **Game Loop**: Structure your game loop with clear separation between input handling, game logic updates, and rendering.

    Example:
    ```c
    while (game.running) {
        handle_inputs(&game);
        update_game_logic(&game);
        render_game(&game);
    }
    ```

## Error Handling

- **Check for Errors**: Always check for errors after operations that can fail (e.g., file opening, memory allocation, SDL initialization).
  
    Example:
    ```c
    if (!file) {
        // handle error
    }
    ```

- **Error Logging**: When handling errors, log a meaningful message to help with debugging. 

    Example:
    ```c
    write_to_file("Error initializing SDL: %s", SDL_GetError());
    ```

## File Organization

- **Modularization**: Separate your code into multiple files based on functionality. For example:
  - `main.c` for the main game loop
  - `game.c` for game state management
  - `player.c` for player-specific logic
  - `render.c` for rendering-related functions

- **Header Files**: Define function prototypes and necessary structs in corresponding header files (`.h`). Include header guards to prevent multiple inclusions.

    Example:
    ```c
    #ifndef GAME_H
    #define GAME_H

    void initialize_game(GameState* game);

    #endif // GAME_H
    ```

## Debugging and Logging

- **Logging**: Use logs to track important events and errors. For example, use a log file to store errors or important information about game state transitions. Avoid printing to the console during production.

    Example:
    ```c
    write_to_file("Game started.");
    ```

- **Conditional Debugging**: Use conditional compilation to include debug code, like logging, only in debug builds.

    Example:
    ```c
    #ifdef DEBUG
    printf("Debugging info: %d
", variable);
    #endif
    ```

