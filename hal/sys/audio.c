/*
 * audio.c
 *
 *  Created on: Dec 27, 2017
 *      Author: sid
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>

#include "audio.h"
#include "../../snes2010/snes9x.h"
#include "../../snes2010/apu.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

snd_pcm_t *pcm_handle = NULL;
static const char *device = "default";						/* playback device */
static const snd_pcm_access_t pcm_access = SND_PCM_ACCESS_RW_INTERLEAVED; 	/* PCM access type */
static const snd_pcm_format_t format = SND_PCM_FORMAT_S16; 			/* sample format */
static const unsigned int pcm_channels = 1; 					/* count of channels */
snd_pcm_uframes_t pcm_buffer_size = 8192;	//must be x*2048		/* ring buffer length in samples */
static const int pcm_resample = 1; 						/* enable alsa-lib resampling */
static unsigned int pcm_period_time = 50000;     //as fast as possible		/* period time in us */
static int period_event = 0; /* produce poll event after each period */

snd_output_t *output = NULL;
bool play_state = false;

static snd_pcm_sframes_t buffer_size;
static snd_pcm_sframes_t period_size;

int16_t *sound_buffer = NULL;
size_t sound_buffer_size = 0;

int set_hwparams(snd_pcm_t *handle, snd_pcm_hw_params_t *params, snd_pcm_access_t access)
{
	snd_pcm_uframes_t size;
	int err, dir;

	/* choose all parameters */
	if ((err = snd_pcm_hw_params_any(handle, params)) < 0)
	{
		printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
		return err;
	}
	/* set hardware resampling */
	if ((err = snd_pcm_hw_params_set_rate_resample(handle, params, pcm_resample)) < 0)
	{
		printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the interleaved read/write format */
	if ((err = snd_pcm_hw_params_set_access(handle, params, access)) < 0)
	{
		printf("Access type not available for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the sample format */
	if ((err = snd_pcm_hw_params_set_format(handle, params, format))< 0)
	{
		printf("Sample format not available for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the count of channels */
	if ((err = snd_pcm_hw_params_set_channels(handle, params, pcm_channels)) < 0)
	{
		printf("Channels count (%i) not available for playbacks: %s\n", pcm_channels, snd_strerror(err));
		return err;
	}
	/* set the stream rate */
	unsigned int rrate = Settings.SoundPlaybackRate; //pcm_rate;
	if ((err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0)) < 0)
	{
		printf("Rate %iHz not available for playback: %s\n", Settings.SoundPlaybackRate, snd_strerror(err));
		return err;
	}
	if (rrate != Settings.SoundPlaybackRate)
	{
		printf("Rate doesn't match (requested %iHz, get %iHz)\n", Settings.SoundPlaybackRate, err);
		return -EINVAL;
	}
	/* set the buffer size */
	//err = snd_pcm_hw_params_set_buffer_time_near(handle, params, &buffer_time, &dir);
	if ((err = snd_pcm_hw_params_set_buffer_size_near(handle, params, &pcm_buffer_size)) < 0)
	{
		printf("Unable to set buffer size %i for playback: %s\n", (int)pcm_buffer_size,	snd_strerror(err));
		return err;
	}
	if ((err = snd_pcm_hw_params_get_buffer_size(params, &size))< 0)
	{
		printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
		return err;
	}
	buffer_size = size;
	/* set the period time */
	if ((err = snd_pcm_hw_params_set_period_time_near(handle, params, &pcm_period_time, &dir))< 0)
	{
		printf("Unable to set period time %i for playback: %s\n", pcm_period_time, snd_strerror(err));
		return err;
	}
	if ((err = snd_pcm_hw_params_get_period_size(params, &size, &dir)) < 0)
	{
		printf("Unable to get period size for playback: %s\n", snd_strerror(err));
		return err;
	}
	period_size = size;
	/* write the parameters to device */
	if ((err = snd_pcm_hw_params(handle, params))< 0)
	{
		printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}

int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams)
{
	int err;
	/* get the current swparams */
	if ((err = snd_pcm_sw_params_current(handle, swparams)) < 0)
	{
		printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* start the transfer when the buffer is almost full: */
	/* (buffer_size / avail_min) * avail_min */
	if((err=snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size)) < 0)
	{
		printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* allow the transfer when at least period_size samples can be processed */
	/* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
	if ((err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size)) < 0)
	{
		printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* enable period events when requested */
	if (period_event && (err = snd_pcm_sw_params_set_period_event(handle, swparams, 1)) < 0)
	{
		printf("Unable to set period event: %s\n", snd_strerror(err));
		return err;
	}
	/* write the parameters to the playback device */
	if ((err = snd_pcm_sw_params(handle, swparams)) < 0)
	{
		printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}

void sound_cb(void)
{
	S9xFinalizeSamples ();

	snd_pcm_sframes_t frames = snd_pcm_avail_update (pcm_handle);
	if (frames < 0)
		frames = snd_pcm_recover (pcm_handle, frames, 1);

	frames = MIN (frames, S9xGetSampleCount () >> 1);

	if (sound_buffer_size < frames*4 || sound_buffer == NULL)
	{
		sound_buffer_size = frames*8;
		sound_buffer = (int16_t *) realloc (sound_buffer, sound_buffer_size*sizeof(int16_t));
	}

	S9xMixSamples (sound_buffer, frames << 1);
	for(int i=0; i<frames; i++)
		sound_buffer[i] = (int16_t) ((((int32_t)sound_buffer[i*2])+((int32_t)sound_buffer[i*2+1])) / 2);

	snd_pcm_sframes_t frames_written = 0;
	while (frames_written < frames)
	{
		int result;

		result = snd_pcm_writei (pcm_handle, sound_buffer + (frames_written<<1), frames - frames_written);

		if (result < 0)
		{
			result = snd_pcm_recover (pcm_handle, result, 1);

			if (result < 0)
				break;
		}
		else
		{
			frames_written += result;
		}
	}

	/* get state */
	snd_pcm_state_t pcm_state = snd_pcm_state(pcm_handle);

	/* prepare */
	if(pcm_state == SND_PCM_STATE_SETUP)
	{
		snd_pcm_prepare(pcm_handle);
		pcm_state = snd_pcm_state(pcm_handle);
	}

	if(pcm_state == SND_PCM_STATE_PREPARED && snd_pcm_avail_update(pcm_handle) < 2*period_size)
	{
		int err;
		if((err = snd_pcm_start(pcm_handle))< 0)
		{
			printf("Start error: %s\n", snd_strerror(err));
		}
	}

	return;
}


bool audio_init(void)
{
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	int err;

	/* set parameter */
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	if ((err=snd_output_stdio_attach(&output, stdout, 0)) < 0)
	{
		printf("Output failed: %s\n", snd_strerror(err));
		return false;
	}


	if ((err = snd_pcm_open(&pcm_handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
		printf("Playback open error: %s\n", snd_strerror(err));
		return false;
	}

	if ((err = set_hwparams(pcm_handle, hwparams, pcm_access)) < 0)
	{
		printf("Setting of hwparams failed: %s\n", snd_strerror(err));
		return false;
	}

	if ((err = set_swparams(pcm_handle, swparams)) < 0)
	{
		printf("Setting of swparams failed: %s\n", snd_strerror(err));
		return false;
	}

	printf("Playback device is %s\n", device);
	printf("Stream parameters are %iHz, %s, %i channels\n",
			Settings.SoundPlaybackRate, snd_pcm_format_name(format), pcm_channels);
	snd_pcm_dump(pcm_handle, output);

	if(S9xInitAPU() == false)
	{
		perror("Apu failed\n");
		exit(EXIT_FAILURE);
	}
	if(S9xInitSound(100, 0) == false)
	{
		perror("Sound init failed\n");
		exit(EXIT_FAILURE);
	}
	S9xSetSamplesAvailableCallback(sound_cb);

	return true;
}


void audio_deinit(void)
{
	S9xDeinitAPU();

	if(sound_buffer != NULL)
	{
		free(sound_buffer);
		sound_buffer = NULL;
		sound_buffer_size = 0;
	}

	/* stop pcm handle */
	snd_pcm_close(pcm_handle);
	pcm_handle = NULL;
}
