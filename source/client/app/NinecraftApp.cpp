/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "NinecraftApp.hpp"
#include "world/item/Item.hpp"
#include "world/entity/MobCategory.hpp"
#include "client/player/input/Multitouch.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"
#include "client/renderer/FoliageColor.hpp"
#include "client/renderer/GrassColor.hpp"
#include "client/renderer/Lighting.hpp"
#include "client/renderer/PatchManager.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/GlobalConstantBufferManager.hpp"
#include "renderer/GlobalConstantBuffers.hpp"
#include "renderer/ConstantBufferMetaDataManager.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/RenderMaterial.hpp"

#ifdef DEMO
#include "world/level/storage/MemoryLevelStorageSource.hpp"
#else
#include "world/level/storage/ExternalFileLevelStorageSource.hpp"
#endif

bool NinecraftApp::_hasInitedStatics;

void NinecraftApp::_initOptions()
{
	// Must be loaded before options, certain options states are forced based on this
	_reloadOptionalFeatures();
	_reloadPatchData();

	if (platform()->hasFileSystemAccess())
		m_pOptions = new Options(m_externalStorageDir);
	else
		m_pOptions = new Options();
}

void NinecraftApp::_initTextures()
{
	m_pTextures = new Textures(getOptions(), platform());

	m_pTextures->addDynamicTexture(new WaterTexture);
	m_pTextures->addDynamicTexture(new WaterSideTexture);
	m_pTextures->addDynamicTexture(new LavaTexture);
	m_pTextures->addDynamicTexture(new LavaSideTexture);
	m_pTextures->addDynamicTexture(new FireTexture(0));
	m_pTextures->addDynamicTexture(new FireTexture(1));

	//m_pTextures->loadList("startup.images");
	//m_pTextures->loadList("background.images");
	//m_pTextures->loadList("ingame.images", ...);

	_reloadTextures();

	if (GrassColor::isAvailable())
	{
        TextureData textureData = m_pPlatform->loadTexture("misc/grasscolor.png", true);
		GrassColor::init(textureData);
	}
	if (FoliageColor::isAvailable())
	{
        TextureData textureData = m_pPlatform->loadTexture("misc/foliagecolor.png", true);
		FoliageColor::init(textureData);
	}
}

void NinecraftApp::_initRenderMaterials()
{
	mce::RenderMaterial::InitContext();

	mce::RenderMaterialGroup::common.loadList("materials/common.json");
	_reloadFancy(getOptions()->m_bFancyGraphics);
}

void NinecraftApp::_initInput()
{
	m_bIsTouchscreen = platform()->isTouchscreen();
	getOptions()->m_bUseController = platform()->hasGamepad();
	getOptions()->loadControls();
	_reloadInput();
}

void NinecraftApp::_updateStats()
{
	/*
	int timeMs = getTimeMs();
	if (timeMs > field_2B0 + 999)
	{
		if (m_pLocalPlayer)
		{
			Vec3 &pos = m_pLocalPlayer->m_pos;
			LOG_I("%d fps\t%3d chunk updates.   (%.2f, %.2f, %.2f)", m_fps, Chunk::updates, pos.x, pos.y, pos.z);
			LOG_I("%s", m_pLevelRenderer->gatherStats1().c_str());
			Chunk::updates = 0;
		}
		else
		{
			LOG_I("%d fps", m_fps);
		}

		field_2B0 = timeMs;
		m_fps = 0;
	}
	*/
}

void NinecraftApp::_reloadTextures()
{
	TextureData* pTexture;
	pTexture = m_pTextures->loadAndBindTexture(C_TERRAIN_NAME, true);
	GetPatchManager()->PatchTextures(*pTexture, TYPE_TERRAIN);
	pTexture = m_pTextures->loadAndBindTexture(C_ITEMS_NAME, true);
	GetPatchManager()->PatchTextures(*pTexture, TYPE_ITEMS);

	GetPatchManager()->PatchTiles();
}

void NinecraftApp::_reloadFancy(bool isFancy)
{
	std::string listPath = isFancy ? "materials/fancy.json" : "materials/sad.json";
	mce::RenderMaterialGroup::switchable.loadList(listPath);
}

void NinecraftApp::_reloadOptionalFeatures()
{
	// Optional features that you really should be able to get away with not including.
	Screen::setIsMenuPanoramaAvailable(platform()->doesTextureExist("gui/background/panorama_0.png"));
	LevelRenderer::setAreCloudsAvailable(platform()->doesTextureExist("environment/clouds.png"));
	LevelRenderer::setArePlanetsAvailable(platform()->doesTextureExist("terrain/sun.png") && platform()->doesTextureExist("terrain/moon.png"));
	GrassColor::setIsAvailable(platform()->doesTextureExist("misc/grasscolor.png"));
	FoliageColor::setIsAvailable(platform()->doesTextureExist("misc/foliagecolor.png"));
	Gui::setIsVignetteAvailable(platform()->doesTextureExist("misc/vignette.png"));
	EntityRenderer::setAreShadowsAvailable(platform()->doesTextureExist("misc/shadow.png"));
}

void NinecraftApp::_reloadPatchData()
{
	GetPatchManager()->LoadPatchData(platform()->getPatchData());
}

void NinecraftApp::_initAll()
{
	Mth::initMth();

	if (!_hasInitedStatics)
	{
		_hasInitedStatics = true;
		Material::initMaterials();
		EntityTypeDescriptor::initDescriptors(); // custom
		MobCategory::initMobCategories();
		Tile::initTiles();
		Item::initItems();
		Biome::initBiomes();
		//TileEntity::initTileEntities();
	}

	_initOptions();
	setupRenderer();
	_initRenderMaterials();
	_initTextures();
	Minecraft::init();
	Tesselator::instance.init();

#ifdef DEMO
	m_pLevelStorageSource = new MemoryLevelStorageSource;
#else
	m_pLevelStorageSource = new ExternalFileLevelStorageSource(m_externalStorageDir);
#endif

	m_pGui = new Gui(this);
	// "Default.png" for the launch image overwrites "default.png" for the font during app packaging
	std::string font = "font/default8.png";
	if (!platform()->doesTextureExist(font))
		font = "font/default.png";
	m_pFont = new Font(getOptions(), font, m_pTextures);
	m_pLevelRenderer = new LevelRenderer(this, m_pTextures);
	m_pGameRenderer = new GameRenderer(this);
	m_pParticleEngine = new ParticleEngine(m_pLevel, m_pTextures);
	m_pUser = new User(getOptions()->m_playerName, "");

	_initInput();

	platform()->initSoundSystem();
	m_pSoundEngine = new SoundEngine(platform()->getSoundSystem(), 20.0f); // 20.0f on 0.7.0
	m_pSoundEngine->init(getOptions(), platform());

	field_D9C = 0;

	setScreen(new StartMenuScreen);
}

bool NinecraftApp::handleBack(bool b)
{
	if (m_bPreparingLevel)
		return true;

	if (!m_pLevel)
	{
		if (!m_pScreen)
			return false;

		return m_pScreen->handleBackEvent(b);
	}

	if (b)
		return true;

	if (!m_pScreen)
	{
		pauseGame();
		return false;
	}

	if (m_pScreen->handleBackEvent(b))
		return true;

	if (isGamePaused())
	{
		resumeGame();
		return true;
	}


	setScreen(nullptr);
	return true;
}

int NinecraftApp::getFpsIntlCounter()
{
	int ofps = m_fps;
	m_fps = 0;
	return ofps;
}

void NinecraftApp::onAppResumed()
{
	// Needs to be called before materials are reloaded
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.lostContext();
	
	Tesselator::instance.init();
    
	m_pTextures->clear();
	_reloadTextures();
	m_pFont->onGraphicsReset();
    
	if (m_pGameRenderer)
		m_pGameRenderer->onGraphicsReset();
    
	EntityRenderDispatcher::getInstance()->onGraphicsReset();
}

void NinecraftApp::onAppFocusLost()
{
    //releaseMouse();
}

void NinecraftApp::onAppFocusGained()
{
    //if (getScreen()->shouldStealMouse())
    //    grabMouse();
}

void NinecraftApp::onAppSuspended()
{
    m_pTextures->unloadAll();
    mce::Mesh::clearGlobalBuffers();
}

void NinecraftApp::init()
{
	// We have no way to debug with libXenon, and aborting will kill our logs
#ifdef XENON
	try
	{
		_initAll();
	}
	catch (...)
	{
		LOG_E("NinecraftApp encountered an exception during initialization!");
		exit(EXIT_FAILURE);
	}
#else
	_initAll();
#endif
}

void NinecraftApp::setupRenderer()
{
	mce::GlobalConstantBufferManager::createInstance();
	mce::GlobalConstantBuffers::createInstance();
	if (mce::ConstantBufferMetaDataManager::createInstance())
	{
#ifdef FEATURE_GFX_SHADERS
		mce::ConstantBufferMetaDataManager& metaDataManager = mce::ConstantBufferMetaDataManager::getInstance();
		std::string fileContents = AppPlatform::singleton()->readAssetFileStr("shaders/uniforms.json", false);
		metaDataManager.loadJsonFile(fileContents);
#endif
	}
	mce::RenderDevice::createInstance();
	mce::GlobalConstantBufferManager::getInstance().allocateAndSetupConstantBuffersFromMetadata(mce::RenderContextImmediate::get());
}

void NinecraftApp::onGraphicsReset()
{
	platform()->_fireAppSuspended();
	platform()->_fireAppResumed();
}

void NinecraftApp::teardown()
{
	teardownRenderer();
}

void NinecraftApp::teardownRenderer()
{
	mce::GlobalConstantBuffers::deleteInstance();
	mce::GlobalConstantBufferManager::deleteInstance();
}

void NinecraftApp::reloadFancy(bool isFancy)
{
	m_pLevelRenderer->allChanged();
	EntityRenderDispatcher::instance->onAppSuspended();
	_reloadFancy(isFancy);
}

void NinecraftApp::update()
{
	++m_fps;
	Multitouch::commit();
	Minecraft::update();
	Mouse::reset2();
	_updateStats();
}

NinecraftApp::NinecraftApp()
{
	field_DBC = 0;
	field_DC0 = 1;
	m_fps = 0;
}

NinecraftApp::~NinecraftApp()
{
	teardown();
}
