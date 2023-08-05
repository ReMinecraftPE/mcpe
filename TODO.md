## What is there left to do?

### World gen features
* `LargeFeature`
* `LargeCaveFeature` [unused]

### Unused entities and their renderers
* [DONE] `FallingTile`
* [DONE] `ItemEntity`
* `ItemSpriteRenderer`

### Sound system:
* Mutex [used for SoundSystemSL, most of it is inlined]
* [DONE] `SoundEngine`
* [DONE] `SoundRepository`
* [DONE] `SoundSystem`
* `SoundSystemSL`
* [DONE] `SoundSystemWindows` (not original, but needed if you want sound on Windows)

### Miscellanea
* `StopwatchHandler` -- `Performance::watches`. Unused?! Don't know.

### GUI stuff
* [DONE] `ChatScreen`
* [DONE] `ConfirmScreen`
* [DONE] `DeleteWorldScreen`
* [DONE] `InvalidLicenseScreen`
* [DONE] `SmallButton`
* [DONE] `ScrolledSelectionList`
* [DONE] `AvailableGamesList`
* [DONE] `JoinGameScreen`
* [DONE] `RenameMPLevelScreen`

### Save data
* [DONE] `PlayerData`
* [DONE] `RegionFile`

### Multiplayer
* [DONE] `ServerSideNetworkHandler`
* [DONE] `ClientSideNetworkHandler`
* [DONE] `MinecraftPackets`
* [DONE] `NetEventCallback`
* [DONE] `Packet`
* [DONE] `AddPlayerPacket`
* [DONE] `ChunkDataPacket`
* [DONE] `LoginPacket`
* [DONE] `MessagePacket`
* [DONE] `MovePlayerPacket`
* [DONE] `PlaceBlockPacket`
* [DONE] `PlayerEquipmentPacket`
* [DONE] `RemoveBlockPacket`
* [DONE] `RemoveEntityPacket`
* [DONE] `RequestChunkPacket`
* [DONE] `StartGamePacket`
* [DONE] `UpdateBlockPacket`

### Xperia Play features
* [DONE] `Controller`
* [DONE] `ControllerTurnInput`

### From the non-demo version
* [DONE] `ExternalFileLevelStorage`
* [DONE] `ExternalFileLevelStorageSource`

## More generic things left to do:

* The level generator is inaccurate in terms of features. Not sure why yet.
  The land and ice generation is accurate, though :)

* Untangle all the control flow and variables. Some functions have nonsense variable names like v3, x4,
  and sometimes gotos are used (like label_3). It would be nice to untangle those.

* Instead of having a list of directories to include from, have only one and use relative paths instead.

* Add the assertions from v0.1.1j (unoptimized debug build)

* Attempt to recreate the project structure from Mojang. See the [Reconstructed project structure](#reconstructed-project-structure)

## Reconstructed project structure
Info extracted from:
* v0.1.0 demo - [D]
* v0.1.1j alpha - [J]
* v0.1.0 touch prototype - [T]

Obviously, this is VERY incomplete. This is what we know:

* Root: `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld`

```
project/android/jni/Android.mk (possibly)             [D]
project/android/jni/Application.mk (possibly)         [D]
src/raknet/*                                          [D]
src/client/gui/components/RolledSelectionList.cpp     [T]
src/client/gui/components/ScrolledSelectionList.cpp   [T]
src/client/gui/screens/IngameBlockSelectionScreen.cpp [T]
src/client/gui/screens/ProgressScreen.cpp
src/client/gui/Font.cpp
src/client/gui/Gui.cpp
src/client/gui/GuiComponent.cpp
src/client/gui/Screen.cpp
src/client/model/Cube.cpp
src/client/renderer/Chunk.cpp
src/client/renderer/GameRenderer.cpp
src/client/renderer/ItemInHandRenderer.cpp
src/client/renderer/LevelRenderer.cpp
src/client/renderer/RenderList.cpp
src/client/renderer/Tesselator.cpp
src/client/renderer/Textures.cpp
src/client/renderer/gles.cpp
src/client/renderer/entity/EntityRenderer.cpp
src/client/renderer/entity/HumanoidMobRenderer.cpp
src/client/renderer/entity/ItemRenderer.cpp
src/client/renderer/entity/ItemSpriteRenderer.cpp
src/client/renderer/entity/MobRenderer.cpp
src/client/renderer/entity/TntRenderer.cpp
src/client/renderer/entity/TripodCameraRenderer.cpp
src/player/input - maybe

project/                                           [D]
  android_java/                                    [D]
    jni/                                           [D]
      Possibly: Android.mk, Application.mk
src/                                               [D]
  raknet/                                          [D]
    The RakNet source code resides here.           [D]
  client/                                          [T]
    gui/                                           [T]
      components/                                  [T]
	    RolledSelectionList.cpp                    [T]
	    ScrolledSelectionList.cpp                  [T]
	  screens/                                     
	  Font.cpp                                     [T]
	  Gui.cpp                                      [T]
	  GuiComponent.cpp                             [T]
	  Screen.cpp                                   [T]
  world/                                           [J]
    level/                                         [J]
      storage/                                     [J]
        RegionFile.cpp                             [J]
  NinecraftApp.cpp                                 [T]
```

* [1] - In v0.1.1j, the RakNet source files are located at: `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld/project/lib_projects//raknet/jni/RakNetSources/`.
  In v0.1.0 demo, they're at `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld/project/android/jni/../../../src/raknet/`.
