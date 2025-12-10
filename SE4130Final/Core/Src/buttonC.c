/*
 * buttonC.c
 *
 *  Created on: Dec 9, 2025
 *      Author: breuv
 */

#include "buttonC.h"

// Assume button is wired as: pressed = pin LOW (pull-up enabled on EXTI)
// If yours is active-high, just flip the logic in Button_IsPressedRaw().
uint8_t Button_IsPressedRaw(Button_HandleTypeDef *button)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(button->port, button->pin);
    return (state == GPIO_PIN_RESET) ? 1U : 0U;  // active LOW
}

void Button_Init(Button_HandleTypeDef *button, GPIO_TypeDef* port, uint16_t pin)
{
    button->port          = port;
    button->pin           = pin;
    button->state         = BUTTON_STATE_IDLE;
    button->lastChangeTick = HAL_GetTick();
    button->pressTick     = 0U;
    button->pendingEvent  = BUTTON_EVENT_NONE;
}

// Debounced state machine
void Button_Update(Button_HandleTypeDef *button, uint32_t currentTick)
{
    uint8_t rawPressed = Button_IsPressedRaw(button);

    switch (button->state)
    {
    case BUTTON_STATE_IDLE:
        if (rawPressed)
        {
            // Possible press: start debounce
            button->state = BUTTON_STATE_DEBOUNCE;
            button->lastChangeTick = currentTick;
        }
        break;

    case BUTTON_STATE_DEBOUNCE:
        // Wait for debounce time to confirm press
        if ((currentTick - button->lastChangeTick) >= BUTTON_DEBOUNCE_TIME_MS)
        {
            if (rawPressed)
            {
                // Confirmed press
                button->state = BUTTON_STATE_PRESSED;
                button->pressTick = currentTick;
            }
            else
            {
                // False alarm, back to idle
                button->state = BUTTON_STATE_IDLE;
            }
        }
        break;

    case BUTTON_STATE_PRESSED:
        if (!rawPressed)
        {
            // Released before long press threshold -> CLICK
            if ((currentTick - button->pressTick) < BUTTON_LONG_PRESS_TIME_MS)
            {
                button->pendingEvent = BUTTON_EVENT_CLICK;
            }
            button->state = BUTTON_STATE_IDLE;
            button->lastChangeTick = currentTick;
        }
        else
        {
            // Still pressed: check if it becomes a long press
            if ((currentTick - button->pressTick) >= BUTTON_LONG_PRESS_TIME_MS)
            {
                button->state = BUTTON_STATE_HELD;
                // We don't fire the event yet; we'll do it on release
            }
        }
        break;

    case BUTTON_STATE_HELD:
        if (!rawPressed)
        {
            // Released after long press threshold -> LONG PRESS
            button->pendingEvent = BUTTON_EVENT_LONG_PRESS;
            button->state = BUTTON_STATE_IDLE;
            button->lastChangeTick = currentTick;
        }
        // If still held, do nothing; we only generate event when released
        break;

    default:
        button->state = BUTTON_STATE_IDLE;
        break;
    }
}

ButtonEvent_t Button_GetEvent(Button_HandleTypeDef *button)
{
    ButtonEvent_t evt = button->pendingEvent;
    button->pendingEvent = BUTTON_EVENT_NONE;
    return evt;
}
