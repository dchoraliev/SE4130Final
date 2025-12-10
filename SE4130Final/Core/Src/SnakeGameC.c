/*
 * SnakeGameC.c
 *
 *  Created on: Dec 10, 2025
 *      Author: dchor
 */


// SnakeGameC.c
#include "SnakeGameC.h"
#include <stdlib.h>  // for rand()
// #include <time.h> // optional if we want to seed srand() with time

typedef struct {
    uint8_t x;
    uint8_t y;
} SnakePosition;

// Internal game state
static SnakePosition snakeBody[SNAKE_MAX_LENGTH];
static uint16_t snakeLength = 0;
static SnakeDirection currentDirection = SNAKE_DIR_RIGHT;
static uint8_t gameOver = 0;
static SnakePosition foodPos;
static uint16_t score = 0;

// High-level state (menu / playing / paused / game over)
static SnakeGameState_t gameState = SNAKE_GAME_STATE_MENU;

// Forward declarations of internal helpers
static void SnakeGame_InternalResetBoard(void);
static void SnakeGame_SpawnFood(void);
static uint8_t SnakeGame_IsSnakeAt(uint8_t x, uint8_t y);
static uint8_t SnakeGame_CheckSelfCollision(uint8_t newHeadX, uint8_t newHeadY);

// -----------------------------------------------------------
// Public API
// -----------------------------------------------------------

void SnakeGame_Init(void)
{
    // If you want, you can seed RNG here:
    // srand((unsigned int)time(NULL));

    score = 0;
    gameOver = 0;
    gameState = SNAKE_GAME_STATE_MENU;

    // Prepare a valid board (snake in the middle) even in menu state.
    SnakeGame_InternalResetBoard();
}

void SnakeGame_Reset(void)
{
    // Always reset score and board when starting a fresh game
    score = 0;
    gameOver = 0;

    SnakeGame_InternalResetBoard();
}

void SnakeGame_SetDirection(SnakeDirection dir)
{
    // Prevent 180° reversal
    if ((currentDirection == SNAKE_DIR_UP    && dir == SNAKE_DIR_DOWN) ||
        (currentDirection == SNAKE_DIR_DOWN  && dir == SNAKE_DIR_UP)   ||
        (currentDirection == SNAKE_DIR_LEFT  && dir == SNAKE_DIR_RIGHT)||
        (currentDirection == SNAKE_DIR_RIGHT && dir == SNAKE_DIR_LEFT))
    {
        // Ignore invalid reverse direction
        return;
    }

    currentDirection = dir;
}

void SnakeGame_Tick(void)
{
    // Only update game when we are actively playing
    if (gameState != SNAKE_GAME_STATE_PLAYING) {
        return;
    }

    if (gameOver) {
        return;
    }

    // Current head position
    SnakePosition head = snakeBody[0];
    uint8_t newHeadX = head.x;
    uint8_t newHeadY = head.y;

    // Move head based on currentDirection
    switch (currentDirection) {
        case SNAKE_DIR_UP:
            if (newHeadY == 0) {
                gameOver = 1;
                gameState = SNAKE_GAME_STATE_GAME_OVER;
                return;
            }
            newHeadY--;
            break;

        case SNAKE_DIR_DOWN:
            if (newHeadY >= (SNAKE_BOARD_HEIGHT - 1)) {
                gameOver = 1;
                gameState = SNAKE_GAME_STATE_GAME_OVER;
                return;
            }
            newHeadY++;
            break;

        case SNAKE_DIR_LEFT:
            if (newHeadX == 0) {
                gameOver = 1;
                gameState = SNAKE_GAME_STATE_GAME_OVER;
                return;
            }
            newHeadX--;
            break;

        case SNAKE_DIR_RIGHT:
            if (newHeadX >= (SNAKE_BOARD_WIDTH - 1)) {
                gameOver = 1;
                gameState = SNAKE_GAME_STATE_GAME_OVER;
                return;
            }
            newHeadX++;
            break;

        default:
            break;
    }

    // Check self-collision
    if (SnakeGame_CheckSelfCollision(newHeadX, newHeadY)) {
        gameOver = 1;
        gameState = SNAKE_GAME_STATE_GAME_OVER;
        return;
    }

    // Check if eating food
    uint8_t ateFood = (newHeadX == foodPos.x && newHeadY == foodPos.y);

    // Shift body: move tail forward
    for (int16_t i = (int16_t)(snakeLength - 1); i > 0; i--) {
        snakeBody[i] = snakeBody[i - 1];
    }

    // Place new head
    snakeBody[0].x = newHeadX;
    snakeBody[0].y = newHeadY;

    if (ateFood) {
        // If we have capacity, grow the snake (duplicate tail)
        if (snakeLength < SNAKE_MAX_LENGTH) {
            snakeBody[snakeLength] = snakeBody[snakeLength - 1];
            snakeLength++;
        }
        score++;
        SnakeGame_SpawnFood();
    }
}

SnakeCellType SnakeGame_GetCell(uint8_t x, uint8_t y)
{
    if (x >= SNAKE_BOARD_WIDTH || y >= SNAKE_BOARD_HEIGHT) {
        return SNAKE_CELL_EMPTY;
    }

    // Check snake body
    for (uint16_t i = 0; i < snakeLength; i++) {
        if (snakeBody[i].x == x && snakeBody[i].y == y) {
            return SNAKE_CELL_SNAKE;
        }
    }

    // Check food
    if (foodPos.x == x && foodPos.y == y) {
        return SNAKE_CELL_FOOD;
    }

    return SNAKE_CELL_EMPTY;
}

uint8_t SnakeGame_IsGameOver(void)
{
    return (gameState == SNAKE_GAME_STATE_GAME_OVER) ? 1U : 0U;
}

uint16_t SnakeGame_GetScore(void)
{
    return score;
}

SnakeGameState_t SnakeGame_GetState(void)
{
    return gameState;
}

void SnakeGame_HandleButtonEvent(ButtonEvent_t evt)
{
    if (evt == BUTTON_EVENT_NONE) {
        return;
    }

    switch (gameState)
    {
    case SNAKE_GAME_STATE_MENU:
        if (evt == BUTTON_EVENT_CLICK) {
            // Start a new game from the menu
            SnakeGame_Reset();                      // score -> 0, board reset
            gameState = SNAKE_GAME_STATE_PLAYING;   // begin playing
        }
        break;

    case SNAKE_GAME_STATE_PLAYING:
        if (evt == BUTTON_EVENT_CLICK) {
            // Pause the game on click
            gameState = SNAKE_GAME_STATE_PAUSED;
        }
        // (You could also map long-press here to quit directly, if you want)
        break;

    case SNAKE_GAME_STATE_PAUSED:
        if (evt == BUTTON_EVENT_CLICK) {
            // Resume the game
            gameState = SNAKE_GAME_STATE_PLAYING;
        } else if (evt == BUTTON_EVENT_LONG_PRESS) {
            // Quit: go back to menu and reset score/board
            SnakeGame_Reset();
            gameState = SNAKE_GAME_STATE_MENU;
        }
        break;

    case SNAKE_GAME_STATE_GAME_OVER:
        if (evt == BUTTON_EVENT_CLICK) {
            // From game over, a click returns to menu with fresh board
            SnakeGame_Reset();
            gameState = SNAKE_GAME_STATE_MENU;
        }
        break;

    default:
        break;
    }
}

// -----------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------

static void SnakeGame_InternalResetBoard(void)
{
    // Initial snake in middle of board, length = 3
    uint8_t startX = SNAKE_BOARD_WIDTH / 2;
    uint8_t startY = SNAKE_BOARD_HEIGHT / 2;

    snakeLength = 3;
    currentDirection = SNAKE_DIR_RIGHT;
    gameOver = 0;

    // Head at [0], body follows
    snakeBody[0].x = startX;
    snakeBody[0].y = startY;

    snakeBody[1].x = startX - 1;
    snakeBody[1].y = startY;

    snakeBody[2].x = startX - 2;
    snakeBody[2].y = startY;

    SnakeGame_SpawnFood();
}

static void SnakeGame_SpawnFood(void)
{
    // Simple approach: random cell that is not occupied by the snake
    while (1) {
        uint8_t x = (uint8_t)(rand() % SNAKE_BOARD_WIDTH);
        uint8_t y = (uint8_t)(rand() % SNAKE_BOARD_HEIGHT);

        if (!SnakeGame_IsSnakeAt(x, y)) {
            foodPos.x = x;
            foodPos.y = y;
            return;
        }
    }
}

static uint8_t SnakeGame_IsSnakeAt(uint8_t x, uint8_t y)
{
    for (uint16_t i = 0; i < snakeLength; i++) {
        if (snakeBody[i].x == x && snakeBody[i].y == y) {
            return 1U;
        }
    }
    return 0U;
}

static uint8_t SnakeGame_CheckSelfCollision(uint8_t newHeadX, uint8_t newHeadY)
{
    // If new head position overlaps any existing segment -> collision
    for (uint16_t i = 0; i < snakeLength; i++) {
        if (snakeBody[i].x == newHeadX && snakeBody[i].y == newHeadY) {
            return 1U;
        }
    }
    return 0U;
}


