/*
 * hw_keys.h
 *
 *  Created on: Dec 27, 2017
 *      Author: sid
 */

#ifndef HAL_HW_KEYS_H_
#define HAL_HW_KEYS_H_

//hardware
#define KEY_OK_BIT		1
#define KEY_PLUS_BIT		3
#define KEY_MINUS_BIT		0
#define KEY_MENUE_BIT		2

#define KEY_CHG_STAT		4
#define KEY_PWR_LDO		5

#define KEY_INT			90		//S0-DIMM Pin 188, signal PM_INT

// two bits per key. LSB: 1 pressed, 0 released. MSB: changed
#define HW_KEY_OK		0
#define HW_KEY_MINUS		2
#define HW_KEY_PLUS		4
#define HW_KEY_MENU		6

uint32_t hw_keys_poll(void);
bool hw_key_init(void);
void hw_key_deinit(void);

#endif /* HAL_HW_KEYS_H_ */
