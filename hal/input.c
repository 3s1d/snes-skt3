/*
 * input.c
 *
 *  Created on: Dec 27, 2017
 *      Author: sid
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "hw_keys.h"
#include "../main.h"



bool input_init(void)
{
	if(hw_key_init() == false)
		return false;

	return true;
}

void input_deinit(void)
{
	hw_key_deinit();
}

void input_poll(void)
{
	static uint32_t cnt = 0;
	cnt++;

	/* only do this every 250ms, as those buttons are of minor interest */
	//note: this should be interrupt based... todo
	if((cnt & 0x0000000F) == 0)
	{
		uint32_t hw_keys = hw_keys_poll();
		if(HW_KEY_PRESSED(hw_keys, HW_KEY_MENU))
			quit();
	}
}
