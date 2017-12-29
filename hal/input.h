/*
 * input.h
 *
 *  Created on: Dec 27, 2017
 *      Author: sid
 */

#ifndef HAL_INPUT_H_
#define HAL_INPUT_H_

#define KEY_JOYPAD_UP       	0
#define KEY_JOYPAD_DOWN    	1
#define KEY_JOYPAD_LEFT     	2
#define KEY_JOYPAD_RIGHT    	3
#define KEY_JOYPAD_A        	4
#define KEY_JOYPAD_B        	5
#define KEY_JOYPAD_X        	6
#define KEY_JOYPAD_Y        	7
#define KEY_JOYPAD_L       	8
#define KEY_JOYPAD_R       	9
#define KEY_JOYPAD_SELECT   	10
#define KEY_JOYPAD_START    	11

#define KEY_PRESSED(v, k)	((v&(3<<(k)))>>(k) == 3)
#define KEY_RELEASED(v, k)	((v&(3<<(k)))>>(k) == 2)

bool input_init(void);
void input_deinit(void);

void input_poll(void);

#endif /* HAL_INPUT_H_ */
