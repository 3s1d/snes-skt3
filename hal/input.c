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
#include "usb_keys.h"
#include "../main.h"
#include "../snes2010/snes9x.h"
#include "../snes2010/controls.h"
#include "sys/volume.h"
#include "input.h"


bool input_init(void)
{
	if(hw_key_init() == false)
		return false;

	//note: this will block until a device is connected
	if(usb_keys_init() == false)
		return false;

	S9xUnmapAllControls();

	/* controller 0+1 */
	S9xMapButton(KEY_JOYPAD_A, S9xGetCommandT("Joypad1 A"));
	S9xMapButton(KEY_JOYPAD_B, S9xGetCommandT("Joypad1 B"));
	S9xMapButton(KEY_JOYPAD_X, S9xGetCommandT("Joypad1 X"));
	S9xMapButton(KEY_JOYPAD_Y, S9xGetCommandT("Joypad1 Y"));
	S9xMapButton(KEY_JOYPAD_UP, S9xGetCommandT("Joypad1 Up"));
	S9xMapButton(KEY_JOYPAD_DOWN, S9xGetCommandT("Joypad1 Down"));
	S9xMapButton(KEY_JOYPAD_LEFT, S9xGetCommandT("Joypad1 Left"));
	S9xMapButton(KEY_JOYPAD_RIGHT, S9xGetCommandT("Joypad1 Right"));
	S9xMapButton(KEY_JOYPAD_L, S9xGetCommandT("Joypad1 L"));
	S9xMapButton(KEY_JOYPAD_R, S9xGetCommandT("Joypad1 R"));
	S9xMapButton(KEY_JOYPAD_START, S9xGetCommandT("Joypad1 Start"));
	S9xMapButton(KEY_JOYPAD_SELECT, S9xGetCommandT("Joypad1 Select"));
	S9xSetController(0, CTL_JOYPAD, 0, 0, 0, 0);
	S9xMapButton(KEY_JOYPAD_A, S9xGetCommandT("Joypad2 A"));
	S9xMapButton(KEY_JOYPAD_B, S9xGetCommandT("Joypad2 B"));
	S9xMapButton(KEY_JOYPAD_X, S9xGetCommandT("Joypad2 X"));
	S9xMapButton(KEY_JOYPAD_Y, S9xGetCommandT("Joypad2 Y"));
	S9xMapButton(KEY_JOYPAD_UP, S9xGetCommandT("Joypad2 Up"));
	S9xMapButton(KEY_JOYPAD_DOWN, S9xGetCommandT("Joypad2 Down"));
	S9xMapButton(KEY_JOYPAD_LEFT, S9xGetCommandT("Joypad2 Left"));
	S9xMapButton(KEY_JOYPAD_RIGHT, S9xGetCommandT("Joypad2 Right"));
	S9xMapButton(KEY_JOYPAD_L, S9xGetCommandT("Joypad2 L"));
	S9xMapButton(KEY_JOYPAD_R, S9xGetCommandT("Joypad2 R"));
	S9xMapButton(KEY_JOYPAD_START, S9xGetCommandT("Joypad2 Start"));
	S9xMapButton(KEY_JOYPAD_SELECT, S9xGetCommandT("Joypad2 Select"));
	S9xSetController(1, CTL_JOYPAD, 1, 0, 0, 0);

	return true;
}

void input_deinit(void)
{
	hw_key_deinit();
	usb_keys_deinit();	//requires initialized hw_keys

	S9xUnmapAllControls();
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
		if(KEY_PRESSED(hw_keys, HW_KEY_MENU))
			quit();
		if(KEY_PRESSED(hw_keys, HW_KEY_PLUS))
			volume_chg(false);
		if(KEY_PRESSED(hw_keys, HW_KEY_MINUS))
			volume_chg(true);
	}

	/*uint32_t usb_keys = */ usb_keys_poll();
	//printf("k %08X\n", usb_keys);

}
