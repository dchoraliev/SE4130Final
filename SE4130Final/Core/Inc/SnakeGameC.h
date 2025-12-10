/*
 * SnakeGameC.h
 *
 *  Created on: Dec 10, 2025
 *      Author: dchor
 */

#ifndef SNAKEGAMEC_H
#define SNAKEGAMEC_H

#include <stdint.h>
#include <stddef.h>
#include "buttonC.h"   // for ButtonEvent_t

#ifdef __cplusplus
extern "C" {
#endif

// Adjust these to match your LCD grid / mapping
#define SNAKE_BOARD_WIDTH   20
#define SNAKE_BOARD_HEIGHT  15
#define SNAKE_MAX_LENGTH    (SNAKE_BOARD_WIDTH * SNAKE_BOARD_HEIGHT)

// Direction used by the snake game
typedef enum {
    SNAKE_DIR_UP = 0,
    SNAKE_DIR_DOWN,
    SNAKE_DIR_LEFT,
    SNAKE_DIR_RIGHT
} SnakeDirection;

// What is at a given cell (for drawing)
typedef enum {
    SNAKE_CELL_EMPTY = 0,
    SNAKE_CELL_SNAKE,
    SNAKE_CELL_FOOD
} SnakeCellType;

// High-level game states (for menu / pause / playing)
typedef enum {
    SNAKE_GAME_STATE_MENU = 0,
    SNAKE_GAME_STATE_PLAYING,
    SNAKE_GAME_STATE_PAUSED,
    SNAKE_GAME_STATE_GAME_OVER
} SnakeGameState_t;

// -------------------------------------------------------
// Public API – called from tasks and button handling
// -------------------------------------------------------

// Call once at startup (e.g., before starting tasks)
void SnakeGame_Init(void);

// Reset game board + score to initial state.
// NOTE: State transition (MENU/PLAYING/etc.) is handled in
// SnakeGame_HandleButtonEvent, not here.
void SnakeGame_Reset(void);

// Change snake direction (e.g., from joystick task)
// Invalid 180° reversals are ignored inside the implementation.
void SnakeGame_SetDirection(SnakeDirection dir);

// Advance one game tick – call from GameLogic task (e.g., every 200 ms).
// This only updates the game if state == PLAYING.
void SnakeGame_Tick(void);

// Query cell contents – used by DrawScreen task to draw LCD.
SnakeCellType SnakeGame_GetCell(uint8_t x, uint8_t y);

// Check whether the game is over (wrapper around game state).
uint8_t SnakeGame_IsGameOver(void);

// Get current score
uint16_t SnakeGame_GetScore(void);

// Get current game state (MENU / PLAYING / PAUSED / GAME_OVER)
SnakeGameState_t SnakeGame_GetState(void);

// Handle button events from the button task.
// Typical mapping (as we discussed):
//  - MENU + CLICK          -> start game  (PLAYING, score=0)
//  - PLAYING + CLICK       -> PAUSED
//  - PAUSED + CLICK        -> resume (PLAYING)
//  - PAUSED + LONG_PRESS   -> quit to MENU (score reset)
//  - GAME_OVER + CLICK     -> back to MENU (score reset)
void SnakeGame_HandleButtonEvent(ButtonEvent_t evt);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SNAKEGAMEC_H

