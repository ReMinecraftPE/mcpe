cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-core)

# Build
add_library(reminecraftpe-core STATIC
    common/Random.cpp
    common/Utils.cpp
    common/Matrix.cpp
    common/Mth.cpp
    common/Timer.cpp
    common/CThread.cpp
    common/Util.cpp
    common/Logger.cpp
    common/SmoothFloat.cpp
    client/app/App.cpp
    client/app/AppPlatform.cpp
    client/app/Minecraft.cpp
    client/app/NinecraftApp.cpp
    client/options/Options.cpp
    client/renderer/LevelRenderer.cpp
    client/renderer/Culler.cpp
    client/renderer/entity/TntRenderer.cpp
    client/renderer/entity/MobRenderer.cpp
    client/renderer/entity/FallingTileRenderer.cpp
    client/renderer/entity/EntityRenderer.cpp
    client/renderer/entity/ItemRenderer.cpp
    client/renderer/entity/TripodCameraRenderer.cpp
    client/renderer/entity/ItemSpriteRenderer.cpp
    client/renderer/entity/EntityRenderDispatcher.cpp
    client/renderer/entity/HumanoidMobRenderer.cpp
    client/renderer/entity/SpiderRenderer.cpp
    client/renderer/entity/ZombieRenderer.cpp
    client/renderer/entity/SkeletonRenderer.cpp
    client/renderer/entity/SheepRenderer.cpp
    client/renderer/entity/SheepFurRenderer.cpp
    client/renderer/entity/CreeperRenderer.cpp
    client/renderer/entity/CowRenderer.cpp
    client/renderer/entity/PigRenderer.cpp
    client/renderer/entity/ChickenRenderer.cpp
    client/renderer/entity/RocketRenderer.cpp
    client/renderer/RenderList.cpp
    client/renderer/Chunk.cpp
    client/renderer/RenderChunk.cpp
    client/renderer/Frustum.cpp
    client/renderer/ItemInHandRenderer.cpp
    client/renderer/DynamicTexture.cpp
    client/renderer/GameRenderer.cpp
    client/renderer/Textures.cpp
    client/renderer/FrustumCuller.cpp
    client/renderer/LightUpdate.cpp
    client/renderer/Font.cpp
    client/renderer/WaterSideTexture.cpp
    client/renderer/Tesselator.cpp
    client/renderer/TileRenderer.cpp
    client/renderer/LightLayer.cpp
    client/renderer/WaterTexture.cpp
    client/renderer/PatchManager.cpp
    client/renderer/LavaTexture.cpp
    client/renderer/LavaSideTexture.cpp
    client/renderer/FireTexture.cpp
    client/renderer/FoliageColor.cpp
    client/renderer/GrassColor.cpp
    client/sound/SoundData.cpp
    client/sound/SoundSystem.cpp
    client/sound/SoundRepository.cpp
    client/sound/SoundEngine.cpp
    client/gui/Screen.cpp
    client/gui/screens/OptionsScreen.cpp
    client/gui/screens/StartMenuScreen.cpp
    client/gui/screens/CreateWorldScreen.cpp
    client/gui/screens/DirectConnectScreen.cpp
    client/gui/screens/SelectWorldScreen.cpp
    client/gui/screens/SavingWorldScreen.cpp
    client/gui/screens/InvalidLicenseScreen.cpp
    client/gui/screens/ConfirmScreen.cpp
    client/gui/screens/DeleteWorldScreen.cpp
    client/gui/screens/DeathScreen.cpp
    client/gui/screens/ChatScreen.cpp
    client/gui/screens/RenameMPLevelScreen.cpp
    client/gui/screens/ProgressScreen.cpp
    client/gui/screens/JoinGameScreen.cpp
    client/gui/screens/IngameBlockSelectionScreen.cpp
    client/gui/screens/PauseScreen.cpp
    client/gui/screens/DeathScreen.cpp
    client/gui/components/ScrolledSelectionList.cpp
    client/gui/components/AvailableGamesList.cpp
    client/gui/components/RolledSelectionList.cpp
    client/gui/components/Button.cpp
    client/gui/components/TextInputBox.cpp
    client/gui/components/SmallButton.cpp
    client/gui/components/WorldSelectionList.cpp
    client/gui/components/OptionList.cpp
    client/gui/Gui.cpp
    client/gui/GuiComponent.cpp
    client/model/PolygonQuad.cpp
    client/model/Model.cpp
    client/model/Cube.cpp
    client/model/ModelPart.cpp
    client/model/QuadrupedModel.cpp
    client/model/PigModel.cpp
    client/model/HumanoidModel.cpp
    client/model/SpiderModel.cpp
    client/model/ZombieModel.cpp
    client/model/SkeletonModel.cpp
    client/model/SheepModel.cpp
    client/model/SheepFurModel.cpp
    client/model/CreeperModel.cpp
    client/model/CowModel.cpp
    client/model/ChickenModel.cpp
    client/player/input/ControllerTurnInput.cpp
    client/player/input/Controller.cpp
    client/player/input/Mouse.cpp
    client/player/input/Keyboard.cpp
    client/player/input/MouseTurnInput.cpp
    client/player/input/KeyboardInput.cpp
    client/player/input/ITurnInput.cpp
    client/player/input/IBuildInput.cpp
    client/player/input/IncludeExcludeArea.cpp
    client/player/input/MouseHandler.cpp
    client/player/input/PolygonArea.cpp
    client/player/input/RectangleArea.cpp
    client/player/input/MouseDevice.cpp
    client/player/input/Multitouch.cpp
    client/player/input/CustomInputHolder.cpp
    client/player/input/IInputHolder.cpp
    client/player/input/IMoveInput.cpp
    client/player/input/ITouchScreenModel.cpp
    client/player/input/TouchAreaModel.cpp
    client/player/input/TouchInputHolder.cpp
    client/player/input/TouchscreenInput_TestFps.cpp
    client/player/input/UnifiedTurnBuild.cpp
    client/network/ClientSideNetworkHandler.cpp
    network/packets/UpdateBlockPacket.cpp
    network/packets/RequestChunkPacket.cpp
    network/packets/PlayerEquipmentPacket.cpp
    network/packets/ChunkDataPacket.cpp
    network/packets/LevelDataPacket.cpp
    network/packets/PlaceBlockPacket.cpp
    network/packets/LoginPacket.cpp
    network/packets/StartGamePacket.cpp
    network/packets/RemoveEntityPacket.cpp
    network/packets/AddPlayerPacket.cpp
    network/packets/RemoveBlockPacket.cpp
    network/packets/MovePlayerPacket.cpp
    network/packets/MessagePacket.cpp
    network/ServerSideNetworkHandler.cpp
    network/RakNetInstance.cpp
    network/MinecraftPackets.cpp
    network/NetEventCallback.cpp
    world/level/levelgen/synth/Synth.cpp
    world/level/levelgen/synth/ImprovedNoise.cpp
    world/level/levelgen/synth/PerlinNoise.cpp
    world/level/path/Node.cpp
    world/level/path/Path.cpp
    world/level/path/BinaryHeap.cpp
    world/level/path/PathFinder.cpp
    world/phys/HitResult.cpp
    world/phys/Vec3.cpp
    world/phys/AABB.cpp
    world/gamemode/SurvivalMode.cpp
    world/gamemode/GameMode.cpp
    world/gamemode/CreativeMode.cpp
    world/entity/Mob.cpp
    world/entity/LocalPlayer.cpp
    world/entity/Player.cpp
    world/entity/PrimedTnt.cpp
    world/entity/Entity.cpp
    world/entity/FallingTile.cpp
    world/entity/TripodCamera.cpp
    world/entity/ItemEntity.cpp
    world/entity/PathfinderMob.cpp
    world/entity/Animal.cpp
    world/entity/WaterAnimal.cpp
    world/entity/Monster.cpp
    world/entity/Rocket.cpp
    world/level/Dimension.cpp
    world/level/Material.cpp
    world/level/LevelListener.cpp
    world/level/TickNextTickData.cpp
    world/level/Explosion.cpp
    world/level/storage/LevelStorageSource.cpp
    world/level/storage/MemoryLevelStorageSource.cpp
    world/level/storage/LevelData.cpp
    world/level/storage/ExternalFileLevelStorage.cpp
    world/level/storage/RegionFile.cpp
    world/level/storage/LevelStorage.cpp
    world/level/storage/MemoryLevelStorage.cpp
    world/level/storage/ChunkStorage.cpp
    world/level/storage/LevelSource.cpp
    world/level/storage/MemoryChunkStorage.cpp
    world/level/storage/ExternalFileLevelStorageSource.cpp
    world/level/path/Node.cpp
    world/level/path/Path.cpp
    world/level/path/PathFinder.cpp
    world/level/path/BinaryHeap.cpp
    world/level/levelgen/feature/BirchFeature.cpp
    world/level/levelgen/feature/LargeFeature.cpp
    world/level/levelgen/feature/Feature.cpp
    world/level/levelgen/feature/LargeCaveFeature.cpp
    world/level/levelgen/feature/SpringFeature.cpp
    world/level/levelgen/feature/TreeFeature.cpp
    world/level/levelgen/feature/PineFeature.cpp
    world/level/levelgen/feature/ReedsFeature.cpp
    world/level/levelgen/feature/OreFeature.cpp
    world/level/levelgen/feature/ClayFeature.cpp
    world/level/levelgen/feature/FlowerFeature.cpp
    world/level/levelgen/feature/SpruceFeature.cpp
    world/level/levelgen/biome/Biome.cpp
    world/level/levelgen/biome/BiomeSource.cpp
    world/level/levelgen/chunk/RandomLevelSource.cpp
    world/level/levelgen/chunk/LevelChunk.cpp
    world/level/levelgen/chunk/ChunkCache.cpp
    world/level/levelgen/chunk/ChunkSource.cpp
    world/level/levelgen/chunk/PerformanceTestChunkSource.cpp
    world/level/levelgen/chunk/TestChunkSource.cpp
    world/level/Level.cpp
    world/level/Region.cpp
    world/item/TilePlanterItem.cpp
    world/item/CameraItem.cpp
    world/item/TileItem.cpp
    world/item/Inventory.cpp
    world/item/DoorItem.cpp
    world/item/ItemInstance.cpp
    world/item/RocketItem.cpp
    world/item/RedstoneItem.cpp
    world/item/Item.cpp
    world/particle/RedDustParticle.cpp
    world/particle/TerrainParticle.cpp
    world/particle/BubbleParticle.cpp
    world/particle/ExplodeParticle.cpp
    world/particle/ParticleEngine.cpp
    world/particle/FlameParticle.cpp
    world/particle/SmokeParticle.cpp
    world/particle/Particle.cpp
    world/particle/LavaParticle.cpp
    world/particle/FireworkParticle.cpp
    world/tile/InvisibleTile.cpp
    world/tile/Sapling.cpp
    world/tile/TreeTile.cpp
    world/tile/GrassTile.cpp
    world/tile/HalfTransparentTile.cpp
    world/tile/ClothTile.cpp
    world/tile/TorchTile.cpp
    world/tile/MetalTile.cpp
    world/tile/SpongeTile.cpp
    world/tile/GlassTile.cpp
    world/tile/SandTile.cpp
    world/tile/Tile.cpp
    world/tile/ClayTile.cpp
    world/tile/StoneTile.cpp
    world/tile/LadderTile.cpp
    world/tile/IceTile.cpp
    world/tile/TopSnowTile.cpp
    world/tile/ReedTile.cpp
    world/tile/Bush.cpp
    world/tile/RedStoneOreTile.cpp
    world/tile/DirtTile.cpp
    world/tile/LiquidTileStatic.cpp
    world/tile/BookshelfTile.cpp
    world/tile/TntTile.cpp
    world/tile/OreTile.cpp
    world/tile/StairTile.cpp
    world/tile/SandStoneTile.cpp
    world/tile/FireTile.cpp
    world/tile/StoneSlabTile.cpp
    world/tile/LiquidTile.cpp
    world/tile/GravelTile.cpp
    world/tile/LiquidTileDynamic.cpp
    world/tile/TransparentTile.cpp
    world/tile/LeafTile.cpp
    world/tile/ObsidianTile.cpp
    world/tile/FarmTile.cpp
    world/tile/DoorTile.cpp
    world/tile/RocketLauncherTile.cpp
    world/tile/WireTile.cpp
    world/tile/RedStoneTorchTile.cpp
    world/tile/LeverTile.cpp
    world/tile/ButtonTile.cpp
    world/tile/PressurePlateTile.cpp
    world/tile/DiodeTile.cpp
    renderer/GL/GL.cpp
)
target_include_directories(reminecraftpe-core PUBLIC . ..)

# RakNet
add_subdirectory(../thirdparty/raknet raknet)
target_link_libraries(reminecraftpe-core PUBLIC raknet)

# zlib
add_library(zlib INTERFACE)
if(EMSCRIPTEN)
    # Use Emscripten's ZLib
    set(ZLIB_FLAG -sUSE_ZLIB=1)
    target_compile_options(zlib INTERFACE "${ZLIB_FLAG}")
    target_link_options(zlib INTERFACE "${ZLIB_FLAG}")
else()
    # Use System ZLib
    find_package(ZLIB REQUIRED)
    target_link_libraries(zlib INTERFACE ZLIB::ZLIB)
endif()
target_link_libraries(reminecraftpe-core PUBLIC zlib)

# Platform Dependencies
if(USE_SDL)
    # SDL
    add_library(SDL INTERFACE)
    if(ANDROID)
        # Use Vendored SDL2 (Only For Android)
        add_subdirectory(../thirdparty/SDL2/src SDL EXCLUDE_FROM_ALL)
        target_link_libraries(SDL INTERFACE SDL2::SDL2)
    elseif(EMSCRIPTEN)
        # Use Emscripten's SDL2
        set(SDL_FLAG -sUSE_SDL=2)
        target_compile_options(SDL INTERFACE "${SDL_FLAG}")
        target_link_options(SDL INTERFACE "${SDL_FLAG}")
    else()
        # Use System SDL2
        find_package(SDL2 REQUIRED)
        target_link_libraries(SDL INTERFACE SDL2::SDL2)
    endif()
    target_link_libraries(reminecraftpe-core PUBLIC SDL)

    # OpenGL
    if(NOT EMSCRIPTEN AND NOT ANDROID)
        option(USE_GLES1_COMPATIBILITY_LAYER "Whether To Enable The GLESv1_CM Compatibility Layer" TRUE)
    else()
        set(USE_GLES1_COMPATIBILITY_LAYER TRUE CACHE BOOL "" FORCE)
    endif()
    if(USE_GLES1_COMPATIBILITY_LAYER)
        set(GLES_COMPATIBILITY_LAYER_USE_SDL TRUE CACHE BOOL "" FORCE)
        set(GLES_COMPATIBILITY_LAYER_DEPENDENCY SDL CACHE STRING "" FORCE)
        set(GLES_COMPATIBILITY_LAYER_USE_ES3 FALSE CACHE BOOL "" FORCE)
        add_subdirectory(../thirdparty/gles-compatibility-layer gles-compatibility-layer)
        target_link_libraries(reminecraftpe-core PUBLIC gles-compatibility-layer)
        target_compile_definitions(reminecraftpe-core PUBLIC USE_GLES1_COMPATIBILITY_LAYER)
        if(EMSCRIPTEN)
            # Use WebGL 2
            target_link_options(reminecraftpe-core PUBLIC -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2)
        endif()
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(reminecraftpe-core PUBLIC OpenGL::OpenGL)
    endif()
elseif(USE_NATIVE_ANDROID)
    # OpenGL
    target_link_libraries(reminecraftpe-core PUBLIC EGL GLESv1_CM)
endif()

# Android Logging
if(ANDROID)
    target_link_libraries(reminecraftpe-core PUBLIC log)
endif()

# Sound Data
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../sound_data/sounds.h")
    if(NOT DEFINED ENV{CI})
        message(WARNING "Missing sound data! Did you run tools/grabsounds.py?")
    endif()
    target_compile_definitions(reminecraftpe-core PRIVATE MISSING_SOUND_DATA)
endif()
