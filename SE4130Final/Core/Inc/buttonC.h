/*
 * buttonC.h
 *
 *  Created on: Dec 9, 2025
 *      Author: breuv
 */

#ifndef INC_BUTTONC_H_
#define INC_BUTTONC_H_

#include "stm32f4xx_hal.h"

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
} Button_HandleTypeDef;

void Button_Init(Button_HandleTypeDef *button, GPIO_TypeDef* port, uint16_t pin);

#endif /* INC_BUTTONC_H_ */
