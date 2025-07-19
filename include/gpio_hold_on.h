#pragma once

#include <zephyr/drivers/gpio.h>

#define GPIO_HOLD_ON_BHV 1
#define GPIO_HOLD_OFF_BHV 0

/**
 * @brief Control external power channel
 * 
 * @param channel Channel number (1-4)
 * @param enable true to turn on, false to turn off
 * @return int 0 on success, negative errno on error
 */
int gpio_hold_control(int channel, bool enable);

/**
 * @brief Initialize external power channels
 * 
 * @return int 0 on success, negative errno on error
 */
int gpio_hold_init(void);