/*
 * SnakeGameScreen.c
 *
 *  Created on: Dec 11, 2025
 *      Author: Tirth
 */

#include "SnakeGameScreen.h"

// Title and option positions
#define TITLE_X 40
#define TITLE_Y 50
#define OPTION1_X 40
#define OPTION1_Y 120

// Button Size
#define BUTTON_WIDTH (SCREEN_WIDTH - 80)
#define BUTTON_HEIGHT 16							// Font size for options is 12 keeping margin of 2

// Play Board Pos
#define BOARD_X ((SCREEN_WIDTH - (GRID_SIZE * SNAKE_BOARD_WIDTH))/2)
#define BOARD_Y 30

// Initialize LCD and show the main menu first
void SnakeGameScreen_Init(void){
	BSP_LCD_Init();

	// Draw the main menu when the game starts
	DrawMainMenu();
}

// Draw the main menu screen (title + buttons + default pointer)
void DrawMainMenu(void){
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

	// Title text
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(0, TITLE_Y, (uint8_t *)"SNAKE GAME", CENTER_MODE);

	// Menu options (Start / Exit) with button outlines
	BSP_LCD_SetFont(&Font12);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	// "Start New Game" button + text
	BSP_LCD_DrawRect(OPTION1_X, OPTION1_Y - 2, BUTTON_WIDTH, BUTTON_HEIGHT);
	BSP_LCD_DisplayStringAt(0, OPTION1_Y, (uint8_t *)"START NEW GAME", CENTER_MODE);

	// Draw new circle next to START
	BSP_LCD_FillCircle(OPTION1_X - 20, OPTION1_Y + 6, BUTTON_HEIGHT/2);

	// Instructions at the bottom of the screen
	int16_t yPos = SCREEN_HEIGHT - 30;
	BSP_LCD_DisplayStringAt(0, yPos, (uint8_t *)"Joystick: Move | Button: Select", CENTER_MODE);
}

// Draw the main game screen (score text + play area + snake)
void DrawGameScreen(void){
	// Clear whole screen
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	// Show score at the top
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font12);
	BSP_LCD_DisplayStringAtLine(1, (int8_t*)"Score : 0");

	// Draw the game board play area (border rectangle)
	BSP_LCD_DrawRect(BOARD_X - 1, BOARD_Y - 1, SNAKE_BOARD_WIDTH * GRID_SIZE + 2, SNAKE_BOARD_HEIGHT * GRID_SIZE + 2);
	BSP_LCD_DrawRect(BOARD_X - 2, BOARD_Y - 2, SNAKE_BOARD_WIDTH * GRID_SIZE + 4, SNAKE_BOARD_HEIGHT * GRID_SIZE + 4);
	UpdateGameBoard();
}

// Update the score text line with the new score value
void UpdateGameScore(int8_t newScore){
	// Clear previous score text
	BSP_LCD_ClearStringLine(1);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	char buff[32];
	sprintf(buff, (uint8_t*)"Score : %d", newScore);
	BSP_LCD_DisplayStringAtLine(1, buff);
}

// Redraw snake segments based on the game logic (to be implemented)
void UpdateGameBoard(void){
	int8_t snakeLen = SnakeGame_GetSnakeLength();
	SnakePos_t snakePos[snakeLen];
	SnakeGame_GetSnakePositions(&snakePos, snakeLen);

	// Redraw the board
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(BOARD_X, BOARD_Y, SNAKE_BOARD_WIDTH * GRID_SIZE, SNAKE_BOARD_HEIGHT * GRID_SIZE);

	// Redraw the snake
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	for(int i=0; i < sizeof(snakePos); i++){
		int gridX = BOARD_X + (snakePos[i].x * GRID_SIZE);
		int gridY = BOARD_Y + (snakePos[i].y * GRID_SIZE);
		if(i == 0){
			// It is the head
			BSP_LCD_FillRect(gridX, gridY, GRID_SIZE, GRID_SIZE);
		}
		else{
			// It is the body
			BSP_LCD_FillRect(gridX + 1, gridY + 1, GRID_SIZE - 2, GRID_SIZE - 2);
		}
	}

	// Redraw the food pos
	SnakePos_t foodPos = SnakeGame_GetFoodPosition();
	int gridX = BOARD_X + (foodPos.x * GRID_SIZE) + GRID_SIZE/2;
	int gridY = BOARD_Y + (foodPos.y * GRID_SIZE) + GRID_SIZE/2;
	BSP_LCD_FillCircle(gridX, gridY, GRID_SIZE/2);
}

// Draw pause screen (full-screen, not overlay)
void DrawPauseMenu(void){
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

    // Title "Game Paused"
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAt(0, TITLE_Y, (uint8_t *)"GAME PAUSED", CENTER_MODE);

    // Menu options (Resume / Exit to Home)
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

    // "Resume Game" button
    BSP_LCD_DrawRect(OPTION1_X, OPTION1_Y - 2, BUTTON_WIDTH, BUTTON_HEIGHT);
    BSP_LCD_DisplayStringAt(0, OPTION1_Y, (uint8_t *)"RESUME GAME", CENTER_MODE);

//    // "Exit to Home Page" button
//    BSP_LCD_DrawRect(OPTION2_X, OPTION2_Y - 2, BUTTON_WIDTH, BUTTON_HEIGHT);
//    BSP_LCD_DisplayStringAt(0, OPTION2_Y, (uint8_t *)"EXIT TO HOME PAGE", CENTER_MODE);

    // Default pointer at RESUME
//    UpdatePauseMenu(PAUSE_RESUME);
	BSP_LCD_FillCircle(OPTION1_X - 20, OPTION1_Y + 6, BUTTON_HEIGHT/2);

	// Instructions at the bottom of the screen
	int16_t yPos = SCREEN_HEIGHT - 30;
	BSP_LCD_DisplayStringAt(0, yPos, (uint8_t *)"Long press to EXIT TO HOME", CENTER_MODE);
}

// Update which pause menu item is selected
void UpdatePauseMenu(PauseMenuChoice choice /* = RESUME */){
//    // Clear the circle next to the other option and draw new one next to current option
//	if ( choice == PAUSE_RESUME ){
//	    // Clear previous circle near EXIT TO HOME
//		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
//	    BSP_LCD_FillCircle(OPTION2_X - 20, OPTION2_Y + 6, BUTTON_HEIGHT/2);
//
//	    // Draw circle near RESUME
//		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//		BSP_LCD_FillCircle(OPTION1_X - 20, OPTION1_Y + 6, BUTTON_HEIGHT/2);
//	}
//
//	if ( choice == PAUSE_EXIT_TO_HOME ){
//	    // Clear previous circle near EXIT TO HOME
//		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
//	    BSP_LCD_FillCircle(OPTION1_X - 20, OPTION1_Y + 6, BUTTON_HEIGHT/2);
//
//	    // Draw circle near RESUME
//		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//		BSP_LCD_FillCircle(OPTION2_X - 20, OPTION2_Y + 6, BUTTON_HEIGHT/2);
//	}
}

// Draw game-over screen (Restart / Exit to home)
void DrawGameOverMenu(void){
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

    // Title "Game Paused"
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAt(0, TITLE_Y, (uint8_t *)"GAME OVER", CENTER_MODE);

    // Menu options (Resume / Exit to Home)
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

    // "Resume Game" button
    BSP_LCD_DrawRect(OPTION1_X, OPTION1_Y - 2, BUTTON_WIDTH, BUTTON_HEIGHT);
    BSP_LCD_DisplayStringAt(0, OPTION1_Y, (uint8_t *)"EXIT TO HOME", CENTER_MODE);

//    // "Exit to Home Page" button
//    BSP_LCD_DrawRect(OPTION2_X, OPTION2_Y - 2, BUTTON_WIDTH, BUTTON_HEIGHT);
//    BSP_LCD_DisplayStringAt(0, OPTION2_Y, (uint8_t *)"EXIT TO HOME PAGE", CENTER_MODE);

    // Default pointer at EXIT TO HOME
//    UpdateGameOverMenu(GAME_OVER_RESTART);
	BSP_LCD_FillCircle(OPTION1_X - 20, OPTION1_Y + 6, BUTTON_HEIGHT/2);
}

// Update which game-over menu item is selected
void UpdateGameOverMenu(GameOverMenuChoice choice /* = GAME_OVER_RESTART */){
//    // Clear the circle next to the other option and draw new one next to current option
//	if ( choice == GAME_OVER_RESTART ){
//	    // Clear previous circle near EXIT TO HOME
//		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
//	    BSP_LCD_FillCircle(OPTION2_X - 20, OPTION2_Y + 6, BUTTON_HEIGHT/2);
//
//	    // Draw circle near RESUME
//		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//		BSP_LCD_FillCircle(OPTION1_X - 20, OPTION1_Y + 6, BUTTON_HEIGHT/2);
//	}
//
//	if ( choice == GAME_OVER_EXIT_TO_HOME ){
//	    // Clear previous circle near EXIT TO HOME
//		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
//	    BSP_LCD_FillCircle(OPTION1_X - 20, OPTION1_Y + 6, BUTTON_HEIGHT/2);
//
//	    // Draw circle near RESUME
//		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//		BSP_LCD_FillCircle(OPTION2_X - 20, OPTION2_Y + 6, BUTTON_HEIGHT/2);
//	}
}
