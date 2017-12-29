/*
 * usb_keys.c
 *
 *  Created on: Dec 29, 2017
 *      Author: sid
 */

#define _DEFAULT_SOURCE			//usleep

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include "../snes2010/snes9x.h"
#include "../snes2010/controls.h"

#include "input.h"
#include "hw_keys.h"
#include "usb_keys.h"

int fd_usb0 = -1;
int fd_usb1 = -1;

//yep, SNES emu is hacky...
extern uint16_t joypad[8];

inline void usb_joypad(int port, uint16_t button_press, bool pressed)
{
	      if (pressed)
		      joypad[port] |= button_press;
	      else
		      joypad[port] &= ~button_press;
}

void usb_key_eval(struct input_event *ev)
{
	/*
	 * for now its hard coded...
	 */

	if(ev->type == 1)
	{
		/* A */
		if(ev->code == 289)
			usb_joypad(0, SNES_A_MASK, ev->value);
		/* B */
		else if(ev->code == 290)
			usb_joypad(0, SNES_B_MASK, ev->value);
		/* Y */
		else if(ev->code == 291)
			usb_joypad(0, SNES_Y_MASK, ev->value);
		/* X */
		else if(ev->code == 288)
			usb_joypad(0, SNES_X_MASK, ev->value);
		/* R */
		else if(ev->code == 293)
			usb_joypad(0, SNES_TR_MASK, ev->value);
		/* L */
		else if(ev->code == 292)
			usb_joypad(0, SNES_TL_MASK, ev->value);
		/* START */
		else if(ev->code == 297)
			usb_joypad(0, SNES_START_MASK, ev->value);
		/* SELECT */
		else if(ev->code == 296)
			usb_joypad(0, SNES_SELECT_MASK, ev->value);

		//todo keyboard p1+2

	}
	else if(ev->type == 3)
	{
		/* UP/DOWN */
		if(ev->code == 1)
		{
			if(ev->value < 127)
			{
				joypad[0] |= SNES_UP_MASK;
				joypad[0] &= ~SNES_DOWN_MASK;
			}
			else if(ev->value > 127)
			{
				joypad[0] |= SNES_DOWN_MASK;
				joypad[0] &= ~SNES_UP_MASK;
			}
			else
			{
				joypad[0] &= ~(SNES_DOWN_MASK|SNES_UP_MASK);
			}
		}
		/* LEFT/RIGHT */
		else if(ev->code == 0)
		{
			if(ev->value < 127)
			{
				joypad[0] |= SNES_LEFT_MASK;
				joypad[0] &= ~SNES_RIGHT_MASK;
			}
			else if(ev->value > 127)
			{
				joypad[0] |= SNES_RIGHT_MASK;
				joypad[0] &= ~SNES_LEFT_MASK;
			}
			else
			{
				joypad[0] &= ~(SNES_RIGHT_MASK|SNES_LEFT_MASK);
			}
		}
	}

}

bool usb_keys_init()
{
	/* wait for devices */
	//note: it is assumes that USB is powered up
	printf("Waiting for USB device\n");
	while(access(USB_KEY_INPUTDEV0, F_OK) == -1 && access(USB_KEY_INPUTDEV1, F_OK) == -1)
	{
		usleep(100000);
		uint32_t hw_keys = hw_keys_poll();
		if(KEY_PRESSED(hw_keys, HW_KEY_MENU))
			return false;

	}
	sleep(1);	//let usb settle

	/* present */
	if(!access(USB_KEY_INPUTDEV0, F_OK))
		fd_usb0 = open(USB_KEY_INPUTDEV0, O_RDWR | O_NONBLOCK);
	if(!access(USB_KEY_INPUTDEV1, F_OK))
		fd_usb1 = open(USB_KEY_INPUTDEV1, O_RDWR | O_NONBLOCK);

	printf("USB: event0:%d event1:%d\n", (fd_usb0!=-1)?1:0, (fd_usb1!=-1)?1:0);

	return true;
}

uint32_t usb_keys_poll(void)
{

	int rd;
	struct input_event ev[16];

	if(fd_usb0 > -1 && (rd=read(fd_usb0, ev, sizeof(ev))) > 0)
	{
		for(int i =0; i< rd/sizeof(struct input_event); i++)
		{
			usb_key_eval(&ev[i]);
			//printf("type%d code%d state%i\n", ev[i].type, ev[i].code, ev[i].value);
		}
	}
	if(fd_usb1 > -1 && (rd=read(fd_usb1, ev, sizeof(ev))) > 0)
	{
		//printf("rd1 %d\n", rd);
	}

	return 0;
}

void usb_keys_deinit()
{
	//todo

}
