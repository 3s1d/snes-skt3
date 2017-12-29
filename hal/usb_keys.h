/*
 * usb_keys.h
 *
 *  Created on: Dec 29, 2017
 *      Author: sid
 */

#ifndef HAL_USB_KEYS_H_
#define HAL_USB_KEYS_H_

#define USB_KEY_INPUTDEV0		"/dev/input/event0"
#define USB_KEY_INPUTDEV1		"/dev/input/event1"

bool usb_keys_init();
void usb_keys_deinit();

uint32_t usb_keys_poll(void);

#endif /* HAL_USB_KEYS_H_ */
