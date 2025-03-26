# SDL2 Game Project

## Overview

This is a simple game side project built using SDL2 in C, designed as a learning exercise alongside the CS50 course. The game initializes an SDL2 window and renderer, manages multiple game states (start screen, playing, paused), and handles player input with a focus on creating a reusable game template. It features a movable player cube with a particle trail, a pause menu with navigation, and momentum preservation. The project is structured into multiple source files for maintainability and includes basic logging for debugging.

## Gameplay
- Avoid red cube and purple lines, collect pickups.

## Controls

- **Start Screen**:
  - `ENTER`: Start the game...
  - `ESC`: Quit to desktop.
- **Playing State**:
  - `WASD` or `Arrow Keys`: Move the player cube (up, down, left, right).
  - `ESC`: Toggle pause menu.
- **Pause Menu**:
  - `Up/Down Arrows`: Navigate menu options (Resume, Settings, Quit to Main Menu, Quit to Desktop).
  - `ENTER`: Select the highlighted option.
  - `ESC`: Resume game.
- **Reset State**
  - `R` : Reset
  - `Esc` : Start Screen
- **Notes**: 
  - Movement momentum is preserved when pausing and resuming mid-motion.
  - Menu navigation and selection are rate-limited to 0.2-second intervals.

## Current Features

- **Game States**: Start screen, playing, and paused states with smooth transitions.
- **Player**: A white cube that moves with normalized diagonal speed and stays within screen bounds.
- **Particle System**: Trails the player with fading, colored particles (cyan-to-white gradient).
- **Pause Menu**: Fully functional with four options:
  - Resume: Returns to gameplay with preserved momentum.
  - Settings: Placeholder (logs selection) WIP.
  - Quit to Main Menu: Resets to start screen and clears game state.
  - Quit to Desktop: Exits the game.
- **Logging**: Outputs debug info to `log.txt` (e.g., initialization, menu actions, velocity changes).

## Config

- Game targets and caps at 120 FPS using a frame time limiter.
- Window size: 1280x720 pixels, borderless with a 3-pixel cyan border.
- Player movement speed: 300 units/second.
- Particle system: Up to 100 particles with a 0.8-second lifetime and trail effect.

## Future Improvements

- **Scoring System**: Track distance moved or time survived (CS50 Week 3: Algorithms practice).
- **Settings Menu**: Expand the "Settings" option with toggles (e.g., particle effects, sound).
- **Enemy AI**: Add moving obstacles or enemies with basic pathfinding (CS50 Week 5: Data Structures).
- **UI Polish**: Enhance with fade effects, sound cues, or a semi-transparent pause menu background using SDL_ttf.
- **Dynamic Particles**: Replace fixed array with a linked list (CS50 Week 5 prep).

## Build Prerequisites

- SDL2 development libraries installed (`libsdl2-dev` on Ubuntu, or equivalent)
- SDL2_ttf development libraries installed (`libsdl2-ttf-dev` on Ubuntu)
- A compiler that supports C (GCC recommended)
- Make (for using the provided Makefile)

---

## Building the Game

To build the game, ensure SDL2 and SDL2_ttf are installed, then run these from the working dir:

```sh
make build
```

## Running the Game

After building, run the game using:

```sh
make run
```

## Cleaning Up

To remove the built game binary:

```sh
make clean
```