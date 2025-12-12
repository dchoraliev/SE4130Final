/*
 * SnakeGameC.c
 *
 *  Created on: Dec 10, 2025
 *      Author: dchor
 */


#include "SnakeGameC.h"
#include <stdlib.h>  // rand()
#include <string.h>  // memcpy

// Internal buffer of positions (ring buffer)
static SnakePos_t snakeBuf[SNAKE_MAX_LENGTH];
static size_t snakeLen = 0;        // number of occupied entries
static size_t headIdx = 0;         // index in snakeBuf where the head sits
static SnakeDirection currentDirection = SNAKE_DIR_RIGHT;

static SnakePos_t foodPos;
static uint16_t score = 0;

static SnakeGameState_t gameState = SNAKE_GAME_STATE_MENU;
static uint8_t gameOver = 0;

// Forward declarations
static void internal_placeInitialSnake(void);
static void internal_spawnFood(void);
static int internal_isSnakeAt(uint8_t x, uint8_t y);
static int internal_checkSelfCollision(uint8_t newX, uint8_t newY);

// ---------------- Public API ----------------

void SnakeGame_Init(void)
{
    // srand seed choice left to user or hardware RNG; default is deterministic.
    score = 0;
    gameOver = 0;
    gameState = SNAKE_GAME_STATE_MENU;
    internal_placeInitialSnake();
}

void SnakeGame_Reset(void)
{
    score = 0;
    gameOver = 0;
    internal_placeInitialSnake();
}

void SnakeGame_SetDirection(SnakeDirection dir)
{
    // Prevent 180-degree reversal based on currentDirection
    if ((currentDirection == SNAKE_DIR_UP && dir == SNAKE_DIR_DOWN) ||
        (currentDirection == SNAKE_DIR_DOWN && dir == SNAKE_DIR_UP) ||
        (currentDirection == SNAKE_DIR_LEFT && dir == SNAKE_DIR_RIGHT) ||
        (currentDirection == SNAKE_DIR_RIGHT && dir == SNAKE_DIR_LEFT)) {
        return;
    }
    currentDirection = dir;
}

void SnakeGame_Tick(void)
{
    if (gameState != SNAKE_GAME_STATE_PLAYING) return;
    if (gameOver) return;

    // current head position
    SnakePos_t head = snakeBuf[headIdx];
    int newX = head.x;
    int newY = head.y;

    switch (currentDirection) {
        case SNAKE_DIR_UP:
            if (newY == 0) { gameOver = 1; gameState = SNAKE_GAME_STATE_GAME_OVER; return; }
            newY--; break;
        case SNAKE_DIR_DOWN:
            if (newY >= (SNAKE_BOARD_HEIGHT - 1)) { gameOver = 1; gameState = SNAKE_GAME_STATE_GAME_OVER; return; }
            newY++; break;
        case SNAKE_DIR_LEFT:
            if (newX == 0) { gameOver = 1; gameState = SNAKE_GAME_STATE_GAME_OVER; return; }
            newX--; break;
        case SNAKE_DIR_RIGHT:
            if (newX >= (SNAKE_BOARD_WIDTH - 1)) { gameOver = 1; gameState = SNAKE_GAME_STATE_GAME_OVER; return; }
            newX++; break;
        default: break;
    }

    // self-collision check: note that moving into the current tail cell is allowed
    // because the tail will move unless we are growing.
    if (internal_checkSelfCollision((uint8_t)newX, (uint8_t)newY)) {
        gameOver = 1;
        gameState = SNAKE_GAME_STATE_GAME_OVER;
        return;
    }

    // did we eat food?
    int ate = (newX == foodPos.x && newY == foodPos.y);

    if (ate) {
        // Grow: we will add a new head without removing tail (increase length)
        // head index moves backward one step in buffer
        headIdx = (headIdx == 0) ? (SNAKE_MAX_LENGTH - 1) : (headIdx - 1);
        snakeBuf[headIdx].x = (uint8_t)newX;
        snakeBuf[headIdx].y = (uint8_t)newY;
        if (snakeLen < SNAKE_MAX_LENGTH) snakeLen++;
        score++;
        internal_spawnFood();
    } else {
        // Move: overwrite the tail entry with new head and advance headIdx to that entry
        // tail index is (headIdx + snakeLen - 1) % SNAKE_MAX_LENGTH
        size_t tailIdx = (headIdx + snakeLen - 1) % SNAKE_MAX_LENGTH;
        // write new head into tail slot (reusing memory) then make it the new head
        snakeBuf[tailIdx].x = (uint8_t)newX;
        snakeBuf[tailIdx].y = (uint8_t)newY;
        headIdx = tailIdx; // new head
        // snakeLen unchanged
    }
}

SnakeCellType SnakeGame_GetCell(uint8_t x, uint8_t y)
{
    if (x >= SNAKE_BOARD_WIDTH || y >= SNAKE_BOARD_HEIGHT) return SNAKE_CELL_EMPTY;

    if (foodPos.x == x && foodPos.y == y) return SNAKE_CELL_FOOD;

    // iterate snake segments (snakeLen entries starting at headIdx)
    for (size_t i = 0; i < snakeLen; i++) {
        size_t idx = (headIdx + i) % SNAKE_MAX_LENGTH;
        if (snakeBuf[idx].x == x && snakeBuf[idx].y == y) return SNAKE_CELL_SNAKE;
    }
    return SNAKE_CELL_EMPTY;
}

uint8_t SnakeGame_IsGameOver(void) { return (gameState == SNAKE_GAME_STATE_GAME_OVER) ? 1U : 0U; }
uint16_t SnakeGame_GetScore(void) { return score; }
SnakeGameState_t SnakeGame_GetState(void) { return gameState; }

void SnakeGame_HandleButtonEvent(ButtonEvent_t evt)
{
    if (evt == BUTTON_EVENT_NONE) return;

    switch (gameState) {
        case SNAKE_GAME_STATE_MENU:
            if (evt == BUTTON_EVENT_CLICK) {
                SnakeGame_Reset();
                gameState = SNAKE_GAME_STATE_PLAYING;
            }
            break;
        case SNAKE_GAME_STATE_PLAYING:
            if (evt == BUTTON_EVENT_CLICK) {
                gameState = SNAKE_GAME_STATE_PAUSED;
            }
            break;
        case SNAKE_GAME_STATE_PAUSED:
            if (evt == BUTTON_EVENT_CLICK) {
                gameState = SNAKE_GAME_STATE_PLAYING;
            } else if (evt == BUTTON_EVENT_LONG_PRESS) {
                SnakeGame_Reset();
                gameState = SNAKE_GAME_STATE_MENU;
            }
            break;
        case SNAKE_GAME_STATE_GAME_OVER:
            if (evt == BUTTON_EVENT_CLICK) {
                SnakeGame_Reset();
                gameState = SNAKE_GAME_STATE_MENU;
            }
            break;
        default: break;
    }
}

// Efficient accessor: copy snake positions into out_array
size_t SnakeGame_GetSnakePositions(SnakePos_t *out_array, size_t max_slots)
{
    if (!out_array || max_slots == 0) return 0;
    size_t toCopy = (snakeLen < max_slots) ? snakeLen : max_slots;
    // copy entries starting from headIdx, for snakeLen items
    for (size_t i = 0; i < toCopy; i++) {
        size_t idx = (headIdx + i) % SNAKE_MAX_LENGTH;
        out_array[i] = snakeBuf[idx];
    }
    return toCopy;
}

size_t SnakeGame_GetSnakeLength(void) { return snakeLen; }
SnakePos_t SnakeGame_GetFoodPosition(void) { return foodPos; }

// ---------------- Internal helpers ----------------

static void internal_placeInitialSnake(void)
{
    // center-ish start
    uint8_t sx = SNAKE_BOARD_WIDTH / 2;
    uint8_t sy = SNAKE_BOARD_HEIGHT / 2;

    // reset buffer
    snakeLen = 3;
    headIdx = 0;
    currentDirection = SNAKE_DIR_RIGHT;
    snakeBuf[0].x = sx;
    snakeBuf[0].y = sy;
    // following segments: right->left
    snakeBuf[1].x = sx - 1;
    snakeBuf[1].y = sy;
    snakeBuf[2].x = sx - 2;
    snakeBuf[2].y = sy;

    internal_spawnFood();
}

static void internal_spawnFood(void)
{
    // naive random free cell selection
    // careful: in degenerate states this loops; it's OK for normal gameplay
    while (1) {
        uint8_t x = (uint8_t)(rand() % SNAKE_BOARD_WIDTH);
        uint8_t y = (uint8_t)(rand() % SNAKE_BOARD_HEIGHT);
        if (!internal_isSnakeAt(x, y)) {
            foodPos.x = x;
            foodPos.y = y;
            return;
        }
    }
}

static int internal_isSnakeAt(uint8_t x, uint8_t y)
{
    for (size_t i = 0; i < snakeLen; i++) {
        size_t idx = (headIdx + i) % SNAKE_MAX_LENGTH;
        if (snakeBuf[idx].x == x && snakeBuf[idx].y == y) return 1;
    }
    return 0;
}

// Note: This version allows moving into the current tail cell (unless we are about to grow)
// To implement that correctly: if new head == tail and !ateFood -> it's allowed
static int internal_checkSelfCollision(uint8_t newX, uint8_t newY)
{
    // If new head equals food, we will grow so tail won't move -> check entire body for collision
    int willGrow = (newX == foodPos.x && newY == foodPos.y);

    // iterate through segments
    for (size_t i = 0; i < snakeLen; i++) {
        size_t idx = (headIdx + i) % SNAKE_MAX_LENGTH;
        // if we are not growing, the tail will vacate its cell so allow stepping into tail
        if (!willGrow && (i == (snakeLen - 1))) {
            // tail index, allowed
            continue;
        }
        if (snakeBuf[idx].x == newX && snakeBuf[idx].y == newY) return 1;
    }
    return 0;
}


