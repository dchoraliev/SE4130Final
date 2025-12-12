/*
 * SnakeGameScreen.h
 *
 *  Created on: Dec 11, 2025
 *      Author: Tirth
 */

#ifndef INC_SNAKEGAMESCREEN_H_
#define INC_SNAKEGAMESCREEN_H_

#include <stm32f413h_discovery_lcd.h>
#include "string.h"

#define SCREEN_HEIGHT BSP_LCD_GetYSize();
#define SCREEN_WIDTH BSP_LCD_GetXSize();

#define GRID_SIZE 4
#define X_GRIDS 60
#define Y_GRIDS 60

typedef enum{
	MENU_START,
	MENU_EXIT
} MainMenuChoice;

typedef enum{
	RESUME,
	EXIT_TO_HOME
} PauseMenuChoice;

typedef enum{
	GAME_OVER_RESTART,
	GAME_OVER_EXIT_TO_HOME
} GameOverMenuChoice;

void SnakeGameScreen_Init(void);

void DrawMainMenu(void);

void UpdateMainMenu(MainMenuChoice choice);

void DrawGameScreen(void);

void UpdateGameScore(int8_t newScore);

void UpdateSnakePosition(void);

void DrawPauseMenu(void);

void UpdatePauseMenu(PauseMenuChoice choice);

void DrawGameOverMenu(void);

void UpdateGameOverMenu(GameOverMenuChoice choice);

#endif /* INC_SNAKEGAMESCREEN_H_ */
