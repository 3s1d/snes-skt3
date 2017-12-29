/*
 * main.c
 *
 *  Created on: Dec 26, 2017
 *      Author: sid
 */

#define __USE_MISC

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>

#include "hal/fb.h"
#include "hal/sys/audio.h"
#include "hal/input.h"
#include "snes2010/snes9x.h"
#include "snes2010/display.h"
#include "snes2010/cpuexec.h"
#include "snes2010/fxemu.h"

/*
 * TODO: Battery
 */
bool running;
void quit(void)
{
	running = false;
}

void S9xMessage(int type, int number, const char *message)
{
	if(message != NULL)
		printf("SNES %d,%d: %s\n", type, number, message);
	else
		printf("SNES %d,%d (NULL)\n", type, number);
}

const char *S9xGetFilename(const char *extension, enum s9x_getdirtype dirtype) { printf("A\n"); return NULL; }
const char *S9xGetDirectory (enum s9x_getdirtype dirtype) { printf("B\n"); return NULL; }
const char *S9xChooseFilename (bool8 read_only) { printf("C\n"); return NULL; }

int load_file_to_memory(const char *filename, uint8_t **result)
{
	int size = 0;
	FILE *f = fopen(filename, "rb");
	if (f == NULL)
	{
		*result = NULL;
		return -1; // -1 means file opening fail
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = (uint8_t *)malloc(size+1);
	if (size != fread(*result, sizeof(uint8_t), size, f))
	{
		free(*result);
		return -2; // -2 means file reading fail
	}
	fclose(f);
	(*result)[size] = 0;
	return size;
}

/*
 * Main
 */
int main(int argc, char *argv[])
{
	/* settings */
	memset(&Settings, 0, sizeof(Settings));
	Settings.FrameTimePAL = 20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.SoundPlaybackRate = 22050;
	Settings.SoundInputRate = 22050;
	Settings.HDMATimingHack = 100;
	Settings.BlockInvalidVRAMAccessMaster = true;

	Settings.PAL = 1;
	Settings.ForcePAL = 1;

	if(fb_init() < 0)
	{
		perror("fb\n");
		exit(EXIT_FAILURE);
	}

	/* init */
	if(Init() == false)
	{
		perror("Init failed\n");
		exit(EXIT_FAILURE);
	}

	/* screen */
	GFX.Screen = calloc(512 * 478, sizeof(uint16_t));	//max resolution
	GFX.Pitch = sizeof(uint16_t) * 512;
	if(GFX.Screen == NULL)
	{
		perror("Malloc\n");
		exit(EXIT_FAILURE);
	}
	if(S9xGraphicsInit() == false)
	{
		perror("S9x init failed\n");
		exit(EXIT_FAILURE);
	}

	/* sound */
	if(audio_init() == false)
	{
		perror("audio failed\n");
		exit(EXIT_FAILURE);
	}

	/* load cartridge */
	uint8_t *rom;
	size_t rom_size = load_file_to_memory("/run/media/mmcblk0p3/smk.smc", &rom);		//todo argv
	if(rom == NULL || rom_size == 0)
	{
		perror("load file \n");
		exit(EXIT_FAILURE);
	}
	/* Hack. S9x cannot do stuff from RAM. <_< */
	memstream_set_buffer(rom, rom_size);

	if(LoadROM("") == false)
	{
		perror("loadrom\n");
		exit(EXIT_FAILURE);
	}

	/* keys, will block until a deviceis connected */
	if(input_init() == false)
	{
		perror("input\n");
		exit(EXIT_FAILURE);
	}

	/* main loop */
	running = true;
	while(running)
	{
		S9xMainLoop();

		/* buttons */
		input_poll();

		//button
		//S9xReportButton(k1P_A_Button, (key_is_pressed ? true : false));
	}

	Deinit();
	free(GFX.Screen);
	S9xGraphicsDeinit();

	free(rom);

	fb_deinit();
	audio_deinit();
	input_deinit();

	/* exit anyway */
	exit(EXIT_SUCCESS);
}

