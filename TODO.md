## What is there left to do?

### Classes to decompile: 

#### Sound system:
* `SoundSystemSL`

#### Miscellanea
* `StopwatchHandler` -- `Performance::watches`. Unused?! Don't know.

## Other things left to do:

* The level generator is inaccurate:
	- Clay patches don't seem to generate properly. (example: In seed 123456, at (104,11), there are supposed to be a few clay blocks exposed on the surface)

* Untangle all the control flow and variables. Some functions have nonsense variable names like v3, x4,
  and sometimes gotos are used (like label_3). It would be nice to untangle those.

* Finish the texture pack I've been working on and use that as a placeholder for the actual Minecraft assets.

## Reconstructed project structure
Currently, ReMinecraftPE's directory structure imitates the structure found in 

Info extracted from:
* v0.1.0 demo - [D]
* v0.1.1j alpha - [J]
* v0.1.0 touch prototype - [T]

Obviously, this is VERY incomplete. Here's what we know:

* Root: `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld`

```
project/android/jni/Android.mk (possibly)             [D]
project/android/jni/Application.mk (possibly)         [D]
src/raknet/*                                          [D] *
src/NinecraftApp.cpp                                  [T]
src/client/gui/components/RolledSelectionList.cpp     [T]
src/client/gui/components/ScrolledSelectionList.cpp   [T]
src/client/gui/screens/IngameBlockSelectionScreen.cpp [T]
src/client/gui/screens/ProgressScreen.cpp             [T]
src/client/gui/Font.cpp                               [T]
src/client/gui/Gui.cpp                                [T]
src/client/gui/GuiComponent.cpp                       [T]
src/client/gui/Screen.cpp                             [T]
src/client/model/Cube.cpp                             [T]
src/client/renderer/Chunk.cpp                         [T]
src/client/renderer/GameRenderer.cpp                  [T]
src/client/renderer/ItemInHandRenderer.cpp            [T]
src/client/renderer/LevelRenderer.cpp                 [T]
src/client/renderer/RenderList.cpp                    [T]
src/client/renderer/Tesselator.cpp                    [T]
src/client/renderer/Textures.cpp                      [T]
src/client/renderer/gles.cpp                          [T]
src/client/renderer/entity/EntityRenderer.cpp         [T]
src/client/renderer/entity/HumanoidMobRenderer.cpp    [T]
src/client/renderer/entity/ItemRenderer.cpp           [T]
src/client/renderer/entity/ItemSpriteRenderer.cpp     [T]
src/client/renderer/entity/MobRenderer.cpp            [T]
src/client/renderer/entity/TntRenderer.cpp            [T]
src/client/renderer/entity/TripodCameraRenderer.cpp   [T]
src/client/player/input - maybe                       [T]
src/world/level/storage/RegionFile.cpp                [J]
```

* [1] - In v0.1.1j, the RakNet source files are located at: `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld/project/lib_projects//raknet/jni/RakNetSources/`.
  In v0.1.0 demo, they're at `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld/project/android/jni/../../../src/raknet/`.
