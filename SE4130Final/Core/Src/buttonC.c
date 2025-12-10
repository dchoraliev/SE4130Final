/*
 * buttonC.c
 *
 *  Created on: Dec 9, 2025
 *      Author: breuv
 */

#include <buttonC.h>

void Button_Init(Button_HandleTypeDef *button, GPIO_TypeDef* port, uint16_t pin)
{
	button->port = port;
	button->pin = pin;
}
