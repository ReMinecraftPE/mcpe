# ReMinecraftPE

This project aims to create a custom Minecraft experience based on Minecraft: Pocket Edition as of 2011. It's
based on Minecraft PE v0.1.3.

**Its goals are:**
* To add Quality-of-Life features to Minecraft: Pocket Edition, such as a brighter color gamut, an in-game
  options menu, etc.
* To add features that were in Minecraft Alpha and the early Betas in 2011. (before and during Minecraft PE's
  development - this excludes hunger, for example, as it was added in Beta 1.8)
* To keep the source code layout similar to the original Minecraft PE (reconstructed from clues hidden within
  certain versions of the game, such as the 0.1.0 touch prototype/debug build)
* To port the game to more platforms, such as Windows (including older versions), Xbox 360, Wii, and more.
  Currently there are ports for:
	* Windows XP-11
	* Android (thanks to [Stom](https://github.com/Stommm) for the help)
	* Linux
	* WebGL
	* Mac OS (port by [BrentDaMage](https://github.com/BrentDaMage))
	* iOS (3.0 and above; port by [BrentDaMage](https://github.com/BrentDaMage))
	* HaikuOS (thanks to [SanyaSho](https://github.com/SanyaSho))
	* Xbox 360 (work in progress; port by [BrentDaMage](https://github.com/BrentDaMage))

**We will not do the following:**
* Add features added in or after Java Edition Beta 1.8
* Backport features from versions newer than 0.9.0
* Add The Nether or The End (probably)
* Add Herobrine
* Add support for Minecraft Java or Minecraft Bedrock servers. (although LAN play with original MCPE clients
  will be supported for as long as possible)
* Modify the project structure

Note: While the original Minecraft PE v0.1.3 may not work on newer devices, ReMinecraftPE works on all Android devices.

### This is the main modded version. For the original decompilation, see [ReMinecraftPE/mcped](https://github.com/ReMinecraftPE/mcped)

## License information

This project is licensed under the [BSD 1 clause license](LICENSE.md). However, it contains third party
software with different but compatible licenses:

- [RakNet](https://github.com/facebookarchive/RakNet): [Licensed under the BSD 2 clause license](thirdparty/raknet/LICENSE)
- [zlib](https://github.com/madler/zlib): [View license](thirdparty/zlib/LICENSE)
- [GLES compatibility layer](https://github.com/TheBrokenRail/gles-compatibility-layer): [View license](https://github.com/TheBrokenRail/gles-compatibility-layer/blob/master/LICENSE). Embedded as a submodule.
- [coi-serviceworker](https://github.com/gzuidhof/coi-serviceworker): [View license](https://github.com/gzuidhof/coi-serviceworker/blob/master/LICENSE). Embedded as a submodule.

## WANT TO HELP?

Want to help this project? [Here's a list of things left to do.](TODO.md)

## DISCLAIMER

This project **requires** you to have a copy of the Minecraft PE v0.1.3 apk (sha256 of the `libminecraftpe.so`
inside: `157af341d13a54cc935bbe24c5e1cf3d02d7e40ec20f9859b9853c2e996ebd81`), before you can build.

## Setup

Before trying to build:

1. Run the following `git` commands, or perform the following actions with Git:
```
git submodule init
git submodule update
```

This fetches the three dependencies we have:
- [zlib](https://github.com/madler/zlib)
- [coi-serviceworker](https://github.com/gzuidhof/coi-serviceworker)
- [gles-compatibility-layer](https://github.com/TheBrokenRail/gles-compatibility-layer.git)

2. Load the sound assets into the `sound_data/` folder in the root of the project
by **running the following command**:
* `tools/grabsounds.py /path/to/the/apk/lib/armeabi-v7a/libminecraftpe.so`.

After that, **prepare the assets folder** from the apk. You will need it for the platform specific
build.

## Building

### Windows

Click the thumbnail below to watch a video guide showcasing how to build ReMCPE.
<a href="https://youtu.be/Tx1u7C2DCPI" target="_blank">
  <img alt="How to Compile ReMCPE for Windows" src="http://i.ytimg.com/vi/Tx1u7C2DCPI/maxresdefault.jpg" />
</a>

The project is configured to target Windows XP by default by using "v141_xp" build tools. If you would like
to build with Windows XP support, please follow the guide [here](https://learn.microsoft.com/en-us/cpp/build/configuring-programs-for-windows-xp?view=msvc-170#install-the-windows-xp-platform-toolset)
to obtain the build tools via the Visual Studio installer.

**If you would like to strictly target more modern versions of Windows, you can change the build toolset by
doing the following:**

1. Right-click the project and click "Properties".

![Visual Studio Project](screenshots/visualstudio/mcpe_project.png)
![Visual Studio Project Properties Option](screenshots/visualstudio/properties_contextmenu.png)

2. Click the dropdown for the "Platform Toolset" property and select "Visual Studio 2022 (v143)" or newer.
![Selecting v143 Platform Toolset](screenshots/visualstudio/mcpe_project_properties_v143.png)

3. Click "Apply".

Install the [DirectX SDK (June 2010)](https://www.microsoft.com/en-US/download/details.aspx?id=6812), unless:
- You opt to use OpenAL instead (but that will require some hacking)
- You are using the latest platform toolset

Once you have the proper build tools installed (v140_xp or newer), you can now build it.

After building, place the `assets` folder you have prepared in the working directory of the output executable.
(if running from VS, it's in `game/`, otherwise, where your executable is)

### WebAssembly

**Make sure you have CMake and Ninja installed. On Windows, the ninja executable must be in your PATH.**
On Linux, the package names are `cmake` and `ninja-build` respectively if you are using apt.

The game will use the assets from within the `game/` directory. So put your assets there if you want to build
for wasm. Then run the `build-wasm` script corresponding to your platform (`-.sh` on linux/WSL, `-.bat` on
windows).

The output files will be in `./wasm/dist`, but you need to upload them to a web host (localhost works too) to
use. (problem with Emscripten)

**NOTE:** If you are using nginx, make sure the `*.wasm` file is served as `application/wasm`, and not
`application/octet-stream`. This can be done by opening `/etc/nginx/mime.types` as root and adding
`application/wasm wasm;` to the types block.

### Linux

This project uses CMake on Linux. Just like WebAssembly, the game assets must be placed in the `game/` directory.

#### Dependencies (Ubuntu/Debian)

- `build-essential` (C/C++ Toolchain)
- `cmake` (CMake)
- `ninja-build` (Ninja)
- `libsdl2-dev` (SDL2)
- `libopenal-dev` (OpenAL)
- `zlib1g-dev` (ZLib)

#### How To Build

```sh
cd platforms/sdl
mkdir build && cd build
cmake -GNinja ..
cmake --build .
# Run
./reminecraftpe
```

### HaikuOS

Dependencies:
- `libsdl2_devel` (SDL2)
- `glu_devel mesa_devel` (Mesa)
- `cmake` (CMake)
- `ninja` (Ninja)

To build, perform the same steps as on Linux.
 
## Have seams when playing?

I've had texture seams when playing Minecraft Classic, ClassiCube and this recreation of Minecraft PE, on a
fresh new laptop. If seams bother you, and you are using an NVIDIA graphics card, go to the NVIDIA Control
Panel, then in "Manage 3D Settings", change "Antialiasing - Mode" to "Application Controlled".

## Notes on assets

The terrain.png and related textures appear to have stayed the same between the E3 demo and the final release
for Xperia PLAY. It appears to have been fetched before Java Edition Beta 1.4's release. This can be seen
because the cookie's texture is missing. (it was added in Java Edition Beta 1.4)

There are plans to create a custom script which downloads and sets up all assets needed for the game.

## Screenshots (from the decomp)

![Title screen](screenshots/title_screen.png)
![Generating world](screenshots/loading.png)
![In-game](screenshots/ingame.png)
![Inventory](screenshots/inventory.png)
![Pause menu](screenshots/pause_screen.png)

## Enhancements

ReMinecraftPE is an enhanced version of the original. To see the original, (mostly) as it was back in the day,
[see the mcped repository](https://github.com/ReMinecraftPE/mcped).

