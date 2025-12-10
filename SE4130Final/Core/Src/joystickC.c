/*
 * joystickC.c
 *
 *  Created on: Dec 9, 2025
 *      Author: breuv
 */

#include "joystickC.h"
#include <math.h>

#define JOYSTICK_CENTER_VALUE 2048.0f
#define JOYSTICK_MAX_VALUE    4095.0f
#define DEADZONE              0.10f


//Helper function for reading the joystick position
static uint16_t Joystick_ReadChannel(ADC_HandleTypeDef *hadc, uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    HAL_ADC_ConfigChannel(hadc, &sConfig);

    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 10);

    uint16_t value = HAL_ADC_GetValue(hadc);

    HAL_ADC_Stop(hadc);

    return value;
}

//Initialization
void Joystick_Init(Joystick_HandleTypeDef *joystick,
                   ADC_HandleTypeDef *hadc,
                   uint32_t x_channel,
                   uint32_t y_channel)
{
	joystick->hadc = hadc;
	joystick->x_channel = x_channel;
	joystick->y_channel = y_channel;
}

//Gets the raw x position
uint16_t Joystick_ReadRawX(Joystick_HandleTypeDef *joystick)
{
    return Joystick_ReadChannel(joystick->hadc, joystick->x_channel);
}

//Gets the raw y position
uint16_t Joystick_ReadRawY(Joystick_HandleTypeDef *joystick)
{
    return Joystick_ReadChannel(joystick->hadc, joystick->y_channel);
}

//Normalizes values to a -1->1 range
static float Normalize_Value(uint16_t raw)
{
    //Normalizes the value
    float centered = (float)raw - JOYSTICK_CENTER_VALUE;
    float normalized = centered / JOYSTICK_CENTER_VALUE;

    //Caps the output range to [-1, 1]
    if (normalized > 1.0f)
    {
    	normalized = 1.0f;
    }
    else if (normalized < -1.0f)
    {
    	normalized = -1.0f;
    }

    //Applies the deadzone
    if (fabsf(normalized) < DEADZONE)
        normalized = 0.0f;

    return normalized;
}

//Gets the normalized x value
float Joystick_GetX(Joystick_HandleTypeDef *joystick)
{
    return Normalize_Value(Joystick_ReadRawX(joystick));
}

//Gets the normalized y value
float Joystick_GetY(Joystick_HandleTypeDef *joystick)
{
    return Normalize_Value(Joystick_ReadRawY(joystick));
}

//Gets the largest direction
JoystickDirection Joystick_GetDirection(Joystick_HandleTypeDef *joystick)
{
	//Gets the normalized values
    float x = Joystick_GetX(joystick);
    float y = Joystick_GetY(joystick);

    //Determines the direction
    JoystickDirection direction;

    if(x == 0.0f && y == 0.0f)//Center
    {
    	direction = CENTER;
    }
    else
    {
    	if(fabsf(x) > fabsf(y))//Greater x
    	{
    		if(x > 0.0f)//Positive x
    		{
    			direction = RIGHT;
    		}
    		else//Negative x
    		{
    			direction = LEFT;
    		}
    	}
    	else//Greater y
    	{
    		if(y > 0.0f)//Positive y
    		{
    			direction = DOWN;
    		}
    		else//Negative y
    		{
    			direction = UP;
    		}
    	}
    }

    return direction;
}


