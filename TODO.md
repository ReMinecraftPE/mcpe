## What is there left to do?

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
* `RegionFile`

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

### World gen features
* `LargeFeature`
* `LargeCaveFeature` [unused]

### Sound system:
* Mutex [used for SoundSystemSL, most of it is inlined]
* [DONE] `SoundEngine`
* [DONE] `SoundRepository`
* [DONE] `SoundSystem`
* `SoundSystemSL`
* `SoundSystemWindows` (not original, but needed if you want sound on Windows)

### Unused entities and their renderers
* [DONE] `FallingTile`
* [DONE] `ItemEntity`
* `ItemSpriteRenderer`

### From the non-demo version
* `ExternalFileLevelStorage`
* `ExternalFileLevelStorageSource`

### Miscellanea
* `StopwatchHandler` -- `Performance::watches`. Unused?! Don't know.

## More generic things left to do:

* The level generator is inaccurate in terms of features. Not sure why yet.
  The land and ice generation is accurate, though :)

* Untangle all the control flow and variables. Some functions have nonsense variable names like v3, x4,
  and sometimes gotos are used (like label_3). It would be nice to untangle those.

* Instead of having a list of directories to include from, have only one and use relative paths instead.

* Add the assertions from v0.1.1j (unoptimized debug build)

* Attempt to recreate the project structure from Mojang. See the [Reconstructed project structure](#reconstructed-project-structure)

## Reconstructed project structure
(note: some info is present in v0.1.0demo, some in v0.1.1j. The latter will be marked as [J].)

Obviously, this is VERY incomplete. This is what we know:

* Root: `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld`

```
project/
  android_java/
    jni/
      Possibly: Android.mk, Application.mk
src/
  raknet/
    The RakNet source code resides here.[1]
  world/ [J]
    level/ [J]
      storage/ [J]
        RegionFile.cpp [J]
```

* [1] - In v0.1.1j, the RakNet source files are located at: `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld/project/lib_projects//raknet/jni/RakNetSources/`.
  In v0.1.0 demo, they're at `C:/dev/subversion/mojang/minecraftcpp/trunk/handheld/project/android/jni/../../../src/raknet/`.
