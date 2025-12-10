/*
 * buttonC.h
 *
 *  Created on: Dec 9, 2025
 *      Author: breuv
 */

#ifndef INC_BUTTONC_H_
#define INC_BUTTONC_H_

#include "stm32f4xx_hal.h"

// How often your button task will call Button_Update (in ms)
#define BUTTON_UPDATE_PERIOD_MS      10U

// Debounce time (ms)
#define BUTTON_DEBOUNCE_TIME_MS      50U

// Long-press threshold (ms)
#define BUTTON_LONG_PRESS_TIME_MS    1000U

typedef enum
{
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_CLICK,       // short press: press + release
    BUTTON_EVENT_LONG_PRESS   // held longer than threshold, then released
} ButtonEvent_t;

typedef enum
{
    BUTTON_STATE_IDLE = 0,
    BUTTON_STATE_DEBOUNCE,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_HELD
} ButtonState_t;

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t      pin;

    ButtonState_t state;
    uint32_t      lastChangeTick;   // last time we changed state
    uint32_t      pressTick;        // time when button was confirmed pressed

    ButtonEvent_t pendingEvent;     // event waiting to be read
} Button_HandleTypeDef;

// Initialize the button struct
void Button_Init(Button_HandleTypeDef *button, GPIO_TypeDef* port, uint16_t pin);

// Call this periodically from a task (e.g., every 10 ms)
void Button_Update(Button_HandleTypeDef *button, uint32_t currentTick);

// Get the latest event (if any). Returns BUTTON_EVENT_NONE if no new event.
ButtonEvent_t Button_GetEvent(Button_HandleTypeDef *button);

// Convenience function: returns 1 if the raw pin is currently pressed (active low)
uint8_t Button_IsPressedRaw(Button_HandleTypeDef *button);

#endif /* INC_BUTTONC_H_ */

