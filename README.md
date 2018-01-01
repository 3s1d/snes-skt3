# snes-skt3
SNES Emulator for the Skytraxx 3.0

# Requirements
- Skytraxx 3.0 with v3.0.11 or later installed. 
- Joypad or keyboard

# Usage
Create the folder 'app' on the sd-card and place the 'snes' binary ([from here](https://github.com/3s1d/snes-skt3/raw/master/Release/snes)) into it. Create an folder 'app/snes.data' (same name as binary + .data) and place unziped ROM cartridge(s) into it  (Sapces inside the filename are not yet supported).

In the Skytraxx menu navigate to 'Programs -> 3rd-Party Applications -> SNES'

For keyboard layout see [usb_keys.c](https://github.com/3s1d/snes-skt3/blob/master/hal/usb_keys.c).

# Disclaimer
- Do not violate any copyrights.
- Don't ask me where to get the ROMs from. 
- ... 

# Development
Cross compiler and toolchain for x64 can be found [here](https://www.skytraxx.eu/skytraxx3/angstrom-glibc-x86_64-armv7at2hf-vfp-neon-v2015.06-toolchain.sh).
