# SDL2 Game Project

## Overview

This is a simple game side project built using SDL2. Currently the game initializes an SDL2 window and renderer, manages game states, and handles basic input. The project is structured into multiple source files for maintainability.

## Prerequisites

- SDL2 development libraries installed
- A compiler that supports C (GCC recommended)
- Make (for using the provided Makefile)

## Config

Game aims and caps at 120 FPS

## Controls

- `ESC`: Close the window
- `WSAD || ARROWS` : Move cube 

## Future Improvements

- Add enemy AI
- Border
- UI 

---

## Building the Game

To build the game, use:

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