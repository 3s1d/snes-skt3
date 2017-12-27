/*
 * hw_keys.c
 *
 *  Created on: Dec 27, 2017
 *      Author: sid
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "sys/i2c.h"
#include "sys/gpio.h"
#include "hw_keys.h"

uint32_t hw_key_eval(void)
{
	static uint8_t keyState = 0x1F;
	uint8_t rx;

	if(i2cRead(I2C0, I2C_ADDR_PWRMNG, &rx, 1) != 1)
		return 0;

	int change = rx ^ keyState;

	/* keys */
	uint32_t keys = 0;
	if (change & (1 << KEY_MENUE_BIT))
		keys |= (2 | !(rx & (1 << KEY_MENUE_BIT))) << HW_KEY_MENU;
	if (change & (1 << KEY_OK_BIT))
		keys |= (2 | !(rx & (1 << KEY_OK_BIT))) << HW_KEY_OK;
	if (change & (1 << KEY_PLUS_BIT))
		keys |= (2 | !(rx & (1 << KEY_PLUS_BIT))) << HW_KEY_PLUS;
	if (change & (1 << KEY_MINUS_BIT))
		keys |= (2 | !(rx & (1 << KEY_MINUS_BIT))) << HW_KEY_MINUS;

#if 0
	/* charger stuff */
	if (change & (1 << KEY_CHG_STAT))
		battery_updateState();

	if (change & (1 << KEY_PWR_LDO))
	{
		if (rx & (1 << KEY_PWR_LDO))
			battery_charger_connected();
		else
			battery_charger_disconnected();
	}
#endif

	keyState = rx;

	return keys;
}

uint32_t hw_keys_poll(void)
{
	unsigned int value = 0;
	gpio_get_value(KEY_INT, &value);
	if(value == 0)
		return 0;

	return hw_key_eval();
}

bool hw_key_init(void)
{
	if(i2cOpen() < 0)
		return false;

	if(gpio_export(KEY_INT) < 0 || gpio_set_dir(KEY_INT, DIRECTION_INPUT) < 0)
		return false;

	hw_key_eval();
	return true;
}

void hw_key_deinit(void)
{
	i2cClose();

	gpio_unexport(KEY_INT);
}
