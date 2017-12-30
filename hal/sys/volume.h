/*
 * volume.h
 *
 *  Created on: Dec 30, 2017
 *      Author: sid
 */

#ifndef HAL_SYS_VOLUME_H_
#define HAL_SYS_VOLUME_H_

typedef enum {
	AUDIO_VOLUME_SET,
	AUDIO_VOLUME_GET,
	AUDIO_VOLUME_MIN,
	AUDIO_VOLUME_MAX
} audio_volume_action;

void volume_chg(bool inc);

#endif /* HAL_SYS_VOLUME_H_ */
