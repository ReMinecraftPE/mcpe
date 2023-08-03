# Minecraft PE Reverse Engineering Project

This project is an attempt to recreate one of the first released builds of Minecraft: Pocket Edition -- mcpe01_canada.apk -- and port it to other platforms,
via binary reverse engineering. This project is similar in terms of goals to [Minecraft Pi](https://minecraft.net/edition/pi), except playable on more devices.

An Android build will come soon.

### The default build is **enhanced**. If you want the original experience, comment every define and uncomment the demo define from GameMods.hpp.

* Note: The original mcpe01_canada.apk does not work on newer Android devices. A port of this likely will.

Eventually, I plan on creating a new repository, `mcpe01_canada`, which will include just the port of the Canada demo.

### This is the main modded version. For the original decompilation, see [ReMinecraftPE/mcped](https://github.com/ReMinecraftPE/mcped)

## WANT TO HELP?

Want to help this project? [Here's a list of things left to do.](TODO.md)

## DISCLAIMER

This project **requires** you to have a copy of the `mcpe01_canada.apk` (sha256: ``), before you can build.

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
2. Due to `SHAPE_FIRE` not being implemented, fire is invisible in this version. Fire rendering was backported from v0.7.1 and can be disabled with the `ORIGINAL_CODE` define
3. Due to `GL_BLEND` being disabled, the hotbar renders as fully opaque.
4. Memory is leaked when leaving a world with particles in it.
5. Stairs are invisible when held. To fix this, a fix has been backported from v0.1.1j
6. If the game is left on for approximately 1,242 days, the level will freeze, but the GUI will still be interactable.

The bugs can be "un-patched" by defining `ORIGINAL_CODE`. This allows compilation of an incomplete and mostly accurate recreation of MCPE for Xperia Play.

### Unpatched bugs
1. Particles with render texture 3 are invisible.
2. Potential freeze in `Entity::move`. If z_1 is between -0.04f and 0.04f, the game may freeze.
3. In `Particle::render`, the offset (`C_MAGIC_1`) is slightly larger than 1.0f / 16.0f. It's probably meant to be _smaller_ than 1.0f / 16.0f.
4. Ice doesn't properly cull faces, except on the y axis.
5. Fire is not extinguished on the server side.
6. Due to inaccuracies in level generation, some blocks may be different in multiplayer between a player on this version, and a player on the original MCPE.

## Enhancements
The original Minecraft: Pocket Edition had some missing details that make it feel like MCPE. Here are some enhancements that have been done:

* (`ENH_ENTITY_SHADING`) Make entity models have primitive shading
* (`ENH_SHADE_HELD_TILES`) Make in-hand block items have shading
* (`ENH_FIX_INVIS_STAIRS`) Fix 16x16x16 chunks of stairs rendering as invisible
* (`ENH_ALLOW_AO`) Allow enabling ambient occlusion (smooth lighting) using a hotkey (F4)
* (`ENH_TRANSPARENT_HOTBAR`) Allow the hotbar to be transparent. (See patched bugs #3)
* (`ENH_INSTA_BREAK`) Allow instant breaking of blocks
* (`ENH_CAMERA_NO_PARTICLES`) Hide particles from a camera's view. This hides the smoke particles that the camera emits, creating a clearer picture
* (`ENH_USE_JAVA_LIGHT_RAMP`) Use Minecraft: Java Edition Beta 1.6's light ramp code instead of the original (max brightness was only 0.8 in early versions of MC:PE for whatever reason)
* (`ENH_RUN_DAY_NIGHT_CYCLE`) Allow the day/night cycle to run
* (`ENH_ENABLE_9TH_SLOT`) Allow the 9th slot of the hotbar to be used instead of the unusable elipses (...)
* (`ENH_USE_OWN_AO`) Use a custom ambient occlusion engine (looks the same but fixes some corners)
* (`ENH_ADD_OPTIONS_PAUSE`) Add an "Options" button to the pause menu
* (`ENH_EXTRA_ITEMS_IN_INV`) Add extra items to a 5th row in the creative inventory
* (`ENH_HIGHLIGHT_BY_HOVER`) Highlight buttons when hovering over them instead of when using arrow keys
* (`ENH_ALLOW_SAND_GRAVITY`) Allow sand to fall
* (`ENH_USE_GUI_SCALE_2`) Use a GUI scale of 2 instead of 3 which looks better on PC (temporary)
* (`ENH_ALLOW_SCROLL_WHEEL`) Allow use of the scroll wheel to change selected inventory slots

They can be turned on/off in `GameMods.hpp`. All enhancements are enabled by default unless explicitly disabled or `ORIGINAL_CODE` is defined.

### Known bugs with enhancements
1. The camera's white overlay is fully opaque in shaded entity mode.

## Mods
In addition to enhancements, optional modifications to the base game were made. These mods are disabled by default but can be enabled in `GameMods.hpp`.

* (`MOD_USE_FLAT_WORLD`) Use a flat world instead of regular world generation
* (`MOD_USE_BIGGER_SCREEN_SIZE`) Use a bigger screen size of 1280x720 instead of 854x480
* (`MOD_DONT_COLOR_GRASS`) Don't tint the top of grass tiles with a different color (like Classic)
