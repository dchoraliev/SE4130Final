/*
 * SnakeGameScreen.c
 *
 *  Created on: Dec 11, 2025
 *      Author: Tirth
 */

#include "SnakeGameScreen.h"

// Main Menu option positions
#define MENU_START_X 50
#define MENU_START_Y 100
#define MENU_EXIT_X 50
#define MENU_EXIT_Y 150

// Resume Menu / Game Over Menu option positions
#define RESUME_X 50
#define RESUME_Y 100
#define EXIT_TO_HOME_X 50
#define EXIT_TO_HOME_Y 150
#define RESTART_X 50
#define RESTART_Y 100


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
	    BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"SNAKE GAME", CENTER_MODE);

	    // Menu options (Start / Exit) with button outlines
	    BSP_LCD_SetFont(&Font16);
	    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	    int16_t BUTTON_WIDTH = SCREEN_WIDTH - 100;
	    int16_t BUTTON_HEIGHT = 24;

	    // "Start New Game" button + text
	    BSP_LCD_DrawRect(MENU_START_X, MENU_START_Y - 4, BUTTON_WIDTH, BUTTON_HEIGHT);
	    BSP_LCD_DisplayStringAt(0, MENU_START_Y, (uint8_t *)"START NEW GAME", CENTER_MODE);

	    // "Exit" button + text
	    BSP_LCD_DrawRect(MENU_EXIT_X, MENU_EXIT_Y - 4, BUTTON_WIDTH, BUTTON_HEIGHT);
	    BSP_LCD_DisplayStringAt(0, MENU_EXIT_Y,  (uint8_t *)"EXIT", CENTER_MODE);

	    // Draw pointer for default selection (Start)
	    UpdateMainMenu(MENU_START);

	    // Instructions at the bottom of the screen
	    int16_t yPos = SCREEN_HEIGHT - 30;
	    BSP_LCD_DisplayStringAt(0, yPos, (uint8_t *)"Joystick: Move  ||  Button: Select", CENTER_MODE);
}

// Update which main menu item is selected (draw filled circle next to it)
// NOTE: 'choice = MENU_START' is a C++ style default argument.
void UpdateMainMenu(MainMenuChoice choice /* = MENU_START */){
    // Clear the circle next to the non-selected option and draw a new one next to the current option

	if ( choice == MENU_START ){
	    // Clear previous circle near EXIT
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	    BSP_LCD_FillCircle(28, MENU_EXIT_Y - 2, 20);

	    // Draw new circle near START
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	    BSP_LCD_FillCircle(30, MENU_START_Y, 16);
	}

	if ( choice == MENU_EXIT ){
	    // Clear previous circle near START
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	    BSP_LCD_FillCircle(28, MENU_START_Y - 2, 20);

	    // Draw new circle near EXIT
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	    BSP_LCD_FillCircle(30, MENU_EXIT_Y, 16);
	}
}

// Draw the main game screen (score text + play area + snake)
void DrawGameScreen(void){
	// Clear whole screen
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	// Show score at the top
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font12);
	BSP_LCD_DisplayStringAtLine(1, (int8_t*)"Score : 0");

	// Draw the game play area (border rectangle)
	BSP_LCD_DrawRect(16, 0, X_GRIDS * GRID_SIZE, Y_GRIDS * GRID_SIZE);

	// Draw the snake at its current position
	UpdateSnakePosition();
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
void UpdateSnakePosition(void){
	// TODO: draw snake body here based on your data structure
}

// Draw pause screen (full-screen, not overlay)
void DrawPauseMenu(void){
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

    // Title "Game Paused"
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"GAME PAUSED", CENTER_MODE);

    // Menu options (Resume / Exit to Home)
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

    int16_t BUTTON_WIDTH = SCREEN_WIDTH - 100;
    int16_t BUTTON_HEIGHT = 24;

    // "Resume Game" button
    BSP_LCD_DrawRect(RESUME_X, RESUME_Y - 4, BUTTON_WIDTH, BUTTON_HEIGHT);
    BSP_LCD_DisplayStringAt(0, RESUME_Y, (uint8_t *)"RESUME GAME", CENTER_MODE);

    // "Exit to Home Page" button
    BSP_LCD_DrawRect(EXIT_TO_HOME_X, EXIT_TO_HOME_Y - 4, BUTTON_WIDTH, BUTTON_HEIGHT);
    BSP_LCD_DisplayStringAt(0, EXIT_TO_HOME_Y,  (uint8_t *)"EXIT TO HOME PAGE", CENTER_MODE);

    // Default pointer at RESUME
    UpdatePauseMenu(RESUME);
}

// Update which pause menu item is selected
void UpdatePauseMenu(PauseMenuChoice choice /* = RESUME */){
    // Clear the circle next to the other option and draw new one next to current option

	if ( choice == RESUME ){
	    // Clear previous circle near EXIT TO HOME
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	    BSP_LCD_FillCircle(28, EXIT_TO_HOME_Y - 2, 20);

	    // Draw circle near RESUME
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	    BSP_LCD_FillCircle(30, RESUME_Y, 16);
	}

	if ( choice == EXIT_TO_HOME ){
	    // Clear previous circle near RESUME
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	    BSP_LCD_FillCircle(28, RESUME_Y - 2, 20);

	    // Draw circle near EXIT TO HOME
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	    BSP_LCD_FillCircle(30, EXIT_TO_HOME_Y, 16);
	}
}

// Draw game-over screen (Restart / Exit to home)
void DrawGameOverMenu(void){
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

    // Title "GAME OVER"
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"GAME OVER", CENTER_MODE);

    // Menu options
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

    int16_t BUTTON_WIDTH = SCREEN_WIDTH - 100;
    int16_t BUTTON_HEIGHT = 24;

    // "Restart Game" button
    BSP_LCD_DrawRect(RESTART_X, RESTART_Y - 4, BUTTON_WIDTH, BUTTON_HEIGHT);
    BSP_LCD_DisplayStringAt(0, RESTART_Y, (uint8_t *)"RESTART GAME", CENTER_MODE);

    // "Exit to Home Page" button
    BSP_LCD_DrawRect(MENU_EXIT_X, MENU_EXIT_Y - 4, BUTTON_WIDTH, BUTTON_HEIGHT);
    BSP_LCD_DisplayStringAt(0, EXIT_TO_HOME_Y,  (uint8_t *)"EXIT TO HOME PAGE", CENTER_MODE);

    // Default pointer at RESTART
    UpdateGameOverMenu(GAME_OVER_RESTART);
}

// Update which game-over menu item is selected
void UpdateGameOverMenu(GameOverMenuChoice choice /* = GAME_OVER_RESTART */){
    // Clear the circle next to the other option and draw new one next to current option

	if ( choice == GAME_OVER_RESTART ){
	    // Clear previous circle near EXIT TO HOME
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	    BSP_LCD_FillCircle(28, EXIT_TO_HOME_Y - 2, 20);

	    // Draw circle near RESTART
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	    BSP_LCD_FillCircle(30, RESTART_Y, 16);
	}

	if ( choice == GAME_OVER_EXIT_TO_HOME ){
	    // Clear previous circle near RESTART
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	    BSP_LCD_FillCircle(28, RESTART_Y - 2, 20);

	    // Draw circle near EXIT TO HOME
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	    BSP_LCD_FillCircle(30, EXIT_TO_HOME_Y, 16);
	}
}
