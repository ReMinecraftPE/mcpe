# Minecraft PE Reverse Engineering Project

This project is an attempt to recreate one of the first released builds of Minecraft: Pocket Edition -- mcpe01_canada.apk -- and port it to other platforms,
via binary reverse engineering.

An Android build will come soon.

### The default build is **enhanced**. If you want the original experience, comment every define and uncomment the demo define from GameMods.hpp.

* Note: The original mcpe01_canada.apk does not work on newer Android devices. A port of this likely will.

Eventually, I plan on creating a new repository, `mcpe01_canada`, which will include just the port of the Canada demo.

## WANT TO HELP?

Want to help this project? [Here's a list of things left to do.](TODO.md)

## Setup

Before opening the VS2022 project or trying to build, load the sound assets into the `sound_data/` folder in the root of the project
by **running the following command**:
* `tools/grabsounds.py /path/to/the/mcpe01_canada/lib/armeabi-v7a/libminecraftpe.so`.

After that, **copy the assets folder** from mcpe01_canada.apk, such that there is a folder called `assets` in the current directory of the executable
(`windows_vs` if you are running from VS, or the same dir as the executable otherwise)

## Have seams when playing?

I've had texture seams when playing Minecraft Classic, ClassiCube and this recreation of Minecraft PE. If seams bother you, and you are using an NVIDIA graphics card,
go to the NVIDIA Control Panel, then in "Manage 3D Settings", change "Antialiasing - Mode" to "Application Controlled".

## Notes on assets
The terrain.png and related textures appear to have stayed the same between the E3 demo and the final release for Xperia PLAY. It appears to have been fetched before
Java Edition Beta 1.4's release. This can be seen because the cookie's texture is missing. (it was added in Java Edition Beta 1.4)

## Screenshots
![Title screen](screenshots/title_screen.png)
![Generating world](screenshots/loading.png)
![In-game](screenshots/ingame.png)
![Inventory](screenshots/inventory.png)
![Pause menu](screenshots/pause_screen.png)

## Known bugs

### Patched bugs
1. Due to lack of initialization of memory, fire can potentially burn normally inflammable blocks. See [this video](https://www.youtube.com/watch?v=3hrz7KK2EJs) for a demo.
2. Due to SHAPE_FIRE not being implemented, fire is invisible in this version. Fire rendering was backported from v0.7.1 and can be disabled with the ORIGINAL_CODE define
3. Due to `GL_BLEND` being disabled, the hotbar renders as fully opaque.
4. Memory is leaked when leaving a world with particles in it.
5. Stairs are invisible when held. To fix this, a fix has been backported from v0.1.1j
6. If the game is left on for approximately 1,242 days, the level will freeze, but the GUI will still be interactable.

The bugs can be "un-patched" by defining ORIGINAL_CODE. This allows compilation of an incomplete and mostly accurate recreation of MCPE for Xperia Play.

### Unpatched bugs
1. Particles with render texture 3 are invisible.
2. Potential freeze in `Entity::move`. If z_1 is between -0.04f and 0.04f, the game may freeze.
3. In `Particle::render`, the offset (`C_MAGIC_1`) is slightly larger than 1.0f / 16.0f. It's probably meant to be _smaller_ than 1.0f / 16.0f.
4. Ice doesn't properly cull faces, except on the y axis.
5. Fire is not extinguished on the server side.
6. Due to inaccuracies in level generation, some blocks may be different in multiplayer between a player on this version, and a player on the original MCPE.

## Enhancements
The original Minecraft: Pocket Edition had some missing details that make it feel like MCPE. Here are some enhancements that I've done (these are all turned off with the
ORIGINAL_CODE define)

* Make fire burn properly (see Patched bug #1)
* Allow fire to be rendered (see Patched bug #2)
* Allow instant breaking of blocks
* Make entity models have primitive shading
* Make in-hand items have shading
* Use a GUI scale of 2 instead of 3 (temporary)
* Allow enabling ambient occlusion (smooth lighting), using a hotkey (F4)
* Allow the hotbar to be transparent. Fixes Patched bug #3
* Use Minecraft Java Edition Beta 1.6's light ramp code instead of the original (Max brightness was only 0.8 in early versions of PE for whatever reason)
* Hide particles from a camera's view. This hides the smoke particles that the camera emits, creating a clearer picture

They can be turned on/off in `source/Base/Utils.hpp`. They're disabled by default.

### Known bugs with enhancements
1. The camera's white overlay is fully opaque in shaded entity mode.
