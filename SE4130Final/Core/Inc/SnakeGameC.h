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

// Board dimensions — adjust as needed
#define SNAKE_BOARD_WIDTH   20
#define SNAKE_BOARD_HEIGHT  15
#define SNAKE_MAX_LENGTH    (SNAKE_BOARD_WIDTH * SNAKE_BOARD_HEIGHT)

// Simple position type (exposed so draw task can use it)
typedef struct {
    uint8_t x;
    uint8_t y;
} SnakePos_t;

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

// High-level game states (for menu / pause / playing / game over)
typedef enum {
    SNAKE_GAME_STATE_MENU = 0,
    SNAKE_GAME_STATE_PLAYING,
    SNAKE_GAME_STATE_PAUSED,
    SNAKE_GAME_STATE_GAME_OVER
} SnakeGameState_t;

// -------------------------------------------------------
// Public API – called from tasks and button handling
// -------------------------------------------------------

// Initializes game subsystem (call once at startup)
void SnakeGame_Init(void);

// Reset game board + score to initial state.
// (Does not change game state; button handler controls transitions)
void SnakeGame_Reset(void);

// Change snake direction (e.g., from joystick task)
void SnakeGame_SetDirection(SnakeDirection dir);

// Advance one game tick – call from GameLogic task (e.g., every 200 ms).
// This only updates the game if state == PLAYING.
void SnakeGame_Tick(void);

// Query cell contents – used by DrawScreen task to draw LCD.
// This is convenient for tile mapping but iterates snake segments internally.
SnakeCellType SnakeGame_GetCell(uint8_t x, uint8_t y);

// Check whether the game is over
uint8_t SnakeGame_IsGameOver(void);

// Get current score
uint16_t SnakeGame_GetScore(void);

// Get current game state
SnakeGameState_t SnakeGame_GetState(void);

// Handle button events from the button task (click / long press)
void SnakeGame_HandleButtonEvent(ButtonEvent_t evt);

// -------------------------------------------------------
// Efficient accessors for drawing (ring-buffer friendly)
// -------------------------------------------------------

// Copies up to max_slots snake segment positions into out_array.
// Returns the number of segments copied (i.e., current snake length).
// Caller must allocate out_array with capacity >= SNAKE_MAX_LENGTH if desired.
size_t SnakeGame_GetSnakePositions(SnakePos_t *out_array, size_t max_slots);

// Convenience: get current snake length
size_t SnakeGame_GetSnakeLength(void);

// Convenience: get food position
SnakePos_t SnakeGame_GetFoodPosition(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SNAKEGAMEC_H
