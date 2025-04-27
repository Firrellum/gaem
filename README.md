# SDL2 Game Project

## Overview

This is a side project built using SDL2 in C, designed as a learning exercise to create a reusable game template. The game, titled *F Cubed*, features a borderless 1280x720 window with multiple game states (start screen, playing, paused, settings), a player-controlled white cube with a particle trail, collectibles, enemies, and a dynamic pause menu. The project is modular, split across multiple source files for maintainability, and includes logging to `log.txt` for debugging.

## Release - V0.9.0
[Release - Latest](https://github.com/Firrellum/gaem/releases/tag/v0.9.0)

## Gameplay
- Navigate a white cube to collect gold collectibles (+10 score each) and green health pickups (+25 HP).
- Avoid the red enemy cube (deals 20 damage/second on contact) and purple line enemies (instant game over on contact).
- Collecting all 5 collectibles spawns a new set, a new line enemy, and a 10% chance for a health pickup.
- Line enemies spawn from a random screen edge, move across the screen, and shrink back after reaching their target.
- The red enemy’s speed increases with your score (150 * (1 + score/500)).
- Particle trails follow the player, turning red during collisions with the red enemy.
- Health and score UI highlights briefly when updated (yellow for score, red for health).

![Interface](https://i.imgur.com/1ftUDKr.gif)

## Controls

- **Start Screen**:
  - `ENTER`: Start the game.
  - `ESC`: Quit to desktop.
  - `M`: Toggle music mute.
- **Playing State**:
  - `WASD` or `Arrow Keys`: Move the player cube (up, down, left, right).
  - `ESC`: Toggle pause menu.
- **Pause Menu**:
  - `Up/Down Arrows`: Navigate options (Resume, Settings, Quit to Main Menu, Quit to Desktop).
  - `ENTER`: Select the highlighted option.
  - `ESC`: Resume game.
- **Settings Menu**:
  - `Up/Down Arrows`: Navigate options (Music Volume, SFX Volume, Frame Rate, Back).
  - `Left/Right Arrows`: Adjust volume (0-128 in steps of 8) or frame rate (60 FPS, 120 FPS, Uncapped).
  - `ENTER`: Select "Back" to return to pause menu.
  - `ESC`: Return to pause menu.
- **Game Over Screen**:
  - `R`: Restart the game.
  - `ESC`: Return to start screen.
- **Notes**:
  - Movement momentum is preserved when pausing/resuming mid-motion.
  - Diagonal movement is normalized to maintain consistent speed.

## Current Features

- **Game States**: Start screen, playing, paused, settings, and game over states with smooth transitions.
- **Player**: A white cube (12x12 pixels) with 300 units/second speed, 100 HP, and screen boundary constraints.
- **Particle System**: Up to 100 particles (6x6 pixels, 0.8-second lifetime) trail the player, fading from cyan to white (or red shades during collisions).
- **Collectibles**: 5 gold collectibles (6x6 pixels) spawn randomly, avoiding UI areas. Collecting all triggers respawn and a new line enemy.
- **Health Pickups**: Green pickups (6x6 pixels, max 3 at a time) restore 25 HP, capped at 100 HP.
- **Enemies**:
  - **Red Cube**: A single 12x12 pixel enemy chases the player, dealing 20 damage/second on contact.
  - **Line Enemies**: Up to 10 purple lines (3 pixels thick) spawn from a random edge, move to a target, then shrink back. Contact triggers instant game over.
- **Pause Menu**: Four options (Resume, Settings, Quit to Main Menu, Quit to Desktop) with cyan highlight for selected option.
- **Settings Menu**: Adjust music volume (0-128), SFX volume (0-128), and frame rate (60 FPS, 120 FPS, uncapped).
- **UI**: Displays score (top-left) and health (top-right), with color changes for low health (<30 HP: red) and recent updates.
- **Audio**:
  - Background music loop with mute toggle (`M`).
  - Sound effects for pickup, hit, death, and game start.
  - Configurable music and SFX volumes.
- **Visuals**:
  - Cyan border (3 pixels) around the 1280x720 window.
  - Semi-transparent grid overlay (12x12 pixel cells).
  - Animated start screen title with sine-wave motion.
- **Logging**: Debug info (FPS, initialization, menu actions, enemy spawns, collisions) written to `log.txt`.

## Config

- Frame rate: Configurable (60 FPS, 120 FPS, or uncapped), default 120 FPS.
- Window: 1280x720 pixels, borderless with a 3-pixel cyan border.
- Player: 12x12 pixels, 300 units/second speed, 100 max HP.
- Particle System: 100 max particles, 6x6 pixels, 0.8-second lifetime, 100 units/second speed, 0.016-second spawn rate.
- Collectibles: 5 active, 6x6 pixels, +10 score each.
- Health Pickups: Max 3, 6x6 pixels, +25 HP each.
- Enemies:
  - Red Cube: 12x12 pixels, base speed 150 units/second, scales with score.
  - Line Enemies: 3-pixel thickness, 200 units/second speed, max 10 active.
- Audio: Default music volume 16/128, SFX volume 64/128.

## Build Prerequisites

- [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.32.2) development libraries
- [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.24.0) development libraries
- [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/releases/tag/release-2.8.1) development libraries
- [SDL2_image](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.8) development libraries
- A compiler that supports C (GCC recommended)
- Make (for using the provided Makefile)

---

## Credits

- **Sounds**:
  - [Ambient Loop](https://soundimage.org/sfx-scifi-amb/)
  - [Death, Start, Pickup, and Hit Sounds](https://sfbgames.itch.io/chiptone)

---

## Future Improvements

- **Settings Menu**: Add toggles for particle effects, grid overlay, or other visual options.
- **UI Polish**: Implement fade transitions, semi-transparent pause/settings menu backgrounds, or additional sound cues.
- **Dynamic Particles**: Replace fixed particle array with a linked list for better memory management.
- **Gameplay Enhancements**:
  - Add more enemy types or behaviors.
  - Introduce power-ups beyond health pickups (e.g., speed boosts, shields).
  - Implement high score tracking or leaderboards.
- **Visuals**: Add animations for collectible pickups or enemy spawns.
- **Audio**: Support for multiple music tracks or dynamic sound effects based on game state.

---

## Building the Game

To build the game, ensure you have the prerequisite libraries, then run:

```sh
make build