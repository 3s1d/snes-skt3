/*
 * volume.c
 *
 *  Created on: Dec 30, 2017
 *      Author: sid
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>

#include "volume.h"

static const char *device = "default";      					/* playback device */

/*
 * Change volume
 */
int audio_volume(audio_volume_action action, long* outvol)
{
	int ret;
	snd_mixer_t* mix_handle;
	snd_mixer_elem_t* elem;
	snd_mixer_selem_id_t* sid;

	static const char* mix_name = "Playback";
	static int mix_index = 0;


	snd_mixer_selem_id_alloca(&sid);

	//sets simple-mixer index and name
	snd_mixer_selem_id_set_index(sid, mix_index);
	snd_mixer_selem_id_set_name(sid, mix_name);

	if ((snd_mixer_open(&mix_handle, 0)) < 0)
		return -1;
	if ((snd_mixer_attach(mix_handle, device)) < 0)
	{
		snd_mixer_close(mix_handle);
		return -2;
	}
	if ((snd_mixer_selem_register(mix_handle, NULL, NULL)) < 0)
	{
		snd_mixer_close(mix_handle);
		return -3;
	}
	ret = snd_mixer_load(mix_handle);
	if (ret < 0)
	{
		snd_mixer_close(mix_handle);
		return -4;
	}
	elem = snd_mixer_find_selem(mix_handle, sid);
	if (!elem)
	{
		snd_mixer_close(mix_handle);
		return -5;
	}

	if (action == AUDIO_VOLUME_GET)
	{
		if (snd_mixer_selem_get_playback_volume(elem, 0, outvol) < 0)
		{
			snd_mixer_close(mix_handle);
			return -6;
		}
	}
	else if (action == AUDIO_VOLUME_SET)
	{
		if (snd_mixer_selem_set_playback_volume(elem, 0, *outvol) < 0)
		{
			snd_mixer_close(mix_handle);
			return -8;
		}
		if (snd_mixer_selem_set_playback_volume(elem, 1, *outvol) < 0)
		{
			snd_mixer_close(mix_handle);
			return -9;
		}
		printf("Set volume %li\n", *outvol);
	}
	else if(action == AUDIO_VOLUME_MAX || action == AUDIO_VOLUME_MIN)
	{
		long minv, maxv;

		snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);
		*outvol = (action==AUDIO_VOLUME_MAX)? maxv : minv;
	}

	snd_mixer_close(mix_handle);
	return 0;

}

void volume_chg(bool inc)
{
	if(inc)
	{
		/* get current values */
		long vol, vol_max;
		audio_volume(AUDIO_VOLUME_GET, &vol);
		audio_volume(AUDIO_VOLUME_MAX, &vol_max);

		/* Increase */
		vol++;
		if(vol <= vol_max)
			audio_volume(AUDIO_VOLUME_SET, &vol);
	}
	else
	{
		/* get current values */
		long vol, vol_min;
		audio_volume(AUDIO_VOLUME_GET, &vol);
		audio_volume(AUDIO_VOLUME_MIN, &vol_min);

		/* Increase */
		vol--;
		if(vol >= vol_min)
			audio_volume(AUDIO_VOLUME_SET, &vol);
	}
}

