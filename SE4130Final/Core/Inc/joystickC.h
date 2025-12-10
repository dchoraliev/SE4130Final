/*
 * joystickC.h
 *
 *  Created on: Dec 9, 2025
 *      Author: breuv
 */

#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include "stm32f4xx_hal.h"

typedef enum
{
    CENTER = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT
} JoystickDirection;

typedef struct
{
	ADC_HandleTypeDef *hadc;
    uint32_t x_channel;
    uint32_t y_channel;
} Joystick_HandleTypeDef;

void Joystick_Init(Joystick_HandleTypeDef *joystick,
				   ADC_HandleTypeDef *hadc,
                   uint32_t x_channel,
                   uint32_t y_channel);

uint16_t Joystick_GetRawX(Joystick_HandleTypeDef *joystick);
uint16_t Joystick_GetRawY(Joystick_HandleTypeDef *joystick);
float Joystick_GetX(Joystick_HandleTypeDef *joystick);
float Joystick_GetY(Joystick_HandleTypeDef *joystick);
JoystickDirection Joystick_GetDirection(Joystick_HandleTypeDef *joystick);

#endif /* INC_JOYSTICKC_H_ */
