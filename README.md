# RobotGame

Small prototype for a 2D platformer game.

<img width="1922" height="1107" alt="image" src="https://github.com/user-attachments/assets/613fc9b0-0b44-4f14-b5a7-cd6162011bce" />

## Game Premise and Description
Premise of the game prototype is that you control a drone with limited power. Running out of power makes you unable to control the player drone character. Reaching another energy station/socket will recharge your power.

## Features
- Built in C + raylib + raygui
- Simple level editor with UI, properties inspector, and game object movement and scaling handles
- ID-based entity system with swap removal and id-index mapping
- Switch between editor and game mode

## Dependencies
- Raylib and raygui

## Controls
- ESC to quit
- F1 to switch between game and editor mode
### Gameplay
- Arrow keys to boost
- Jump key to double jump boost
- Release jump key to thrust away from socket
- R to reset level
### Editor
- Delete key to delete the current selected entity
- Arrow keys to switch entity selection
- 1, 2, 3 keys to switch between movement, scaling, and no handles
- Left click to select objects
- Right click to create a new entity
- Hold control while dragging handles to snap to grid size
