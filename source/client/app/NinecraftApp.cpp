/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "NinecraftApp.hpp"
#include "world/item/Item.hpp"
#include "world/entity/MobCategory.hpp"
#include "client/player/input/GameControllerHandler.hpp"
#include "client/player/input/Multitouch.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"
#include "client/renderer/FoliageColor.hpp"
#include "client/renderer/GrassColor.hpp"
#include "client/renderer/Lighting.hpp"
#include "client/renderer/PatchManager.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "client/resources/Resource.hpp"
#include "client/resources/AppResourceLoader.hpp"
#include "client/resources/ResourcePackManager.hpp"
#include "client/locale/Language.hpp"
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

void NinecraftApp::_initResourceLoaders()
{
	Resource::registerLoader(new AppResourceLoader(ResourceLocation::APP_PACKAGE));
	//Resource::registerLoader(ResourceLocation::DATA_DIR, new AppResourceLoader(platform()->getDataUrl()));
	//Resource::registerLoader(ResourceLocation::USER_DIR, new AppResourceLoader(dontevenknow));
	//Resource::registerLoader(ResourceLocation::SETTINGS_DIR, new AppResourceLoader(dontevenknow));
	Resource::registerLoader(new AppResourceLoader(ResourceLocation::EXTERNAL_DIR));
	Resource::registerLoader(new AppResourceLoader(ResourceLocation::RAW_PATH));
	//Resource::registerLoader(ResourceLocation::WORLD_DIR, new ScreenshotLoader(this));
	m_pResourceLoader = new ResourcePackManager();
	Resource::registerLoader(m_pResourceLoader);
}

void NinecraftApp::_initOptions()
{
	// Must be loaded before options, certain options states are forced based on this
	_reloadPatchData();

	if (platform()->hasFileSystemAccess())
		m_pOptions = new Options(this, platform()->m_externalStorageDir);
	else
		m_pOptions = new Options(this);

	// kind of a hack, just so we can keep everything centralized in options
	m_pResourceLoader->m_pPacks = &m_pOptions->m_resourcePacks;
	_reloadOptionalFeatures();
	m_pOptions->initResourceDependentOptions();
}

void NinecraftApp::_initTextures()
{
	m_pTextures = new Textures();

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

	if (GrassColor::isAvailable()) GrassColor::init();
	if (FoliageColor::isAvailable()) FoliageColor::init();
}

void NinecraftApp::_initRenderMaterials()
{
	mce::RenderMaterial::InitContext();

	mce::RenderMaterialGroup::common.loadList("materials/common.json");
	_reloadFancy(getOptions()->m_fancyGraphics.get());
}

void NinecraftApp::_initInput()
{
	m_bIsTouchscreen = platform()->isTouchscreen();
	getOptions()->m_bUseController.set(platform()->hasGamepad());
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
	Screen::setIsMenuPanoramaAvailable(Resource::hasTexture("gui/background/panorama_0.png"));
	LevelRenderer::setAreCloudsAvailable(Resource::hasTexture("environment/clouds.png"));
	LevelRenderer::setArePlanetsAvailable(Resource::hasTexture("terrain/sun.png") && Resource::hasTexture("terrain/moon.png"));
	GrassColor::setIsAvailable(Resource::hasTexture("misc/grasscolor.png"));
	FoliageColor::setIsAvailable(Resource::hasTexture("misc/foliagecolor.png"));
	Gui::setIsVignetteAvailable(Resource::hasTexture("misc/vignette.png"));
	EntityRenderer::setAreShadowsAvailable(Resource::hasTexture("misc/shadow.png"));
}

void NinecraftApp::_reloadPatchData()
{
	std::string patchData;
	Resource::load("patches/patch_data.txt", patchData);
	GetPatchManager()->LoadPatchData(patchData);
}

void NinecraftApp::_initAll()
{
	Mth::initMth();
	_initResourceLoaders();

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
	m_pLevelStorageSource = new ExternalFileLevelStorageSource(platform()->m_externalStorageDir);
#endif

	m_pGui = new Gui(this);
	m_pFont = new Font(getOptions(), "font/default.png", m_pTextures);
	m_pLevelRenderer = new LevelRenderer(this, m_pTextures);
	m_pGameRenderer = new GameRenderer(this);
	m_pParticleEngine = new ParticleEngine(m_pLevel, m_pTextures);
	m_pUser = new User(getOptions()->m_playerName.get(), "");

	_initInput();

	platform()->initSoundSystem();
	m_pSoundEngine = new SoundEngine(platform()->getSoundSystem(), 20.0f); // 20.0f on 0.7.0
	m_pSoundEngine->init(getOptions());

	Language::singleton().init(getOptions());

	field_D9C = 0;

	gotoMainMenu();
}

bool NinecraftApp::handleBack(bool b)
{
	if (m_bPreparingLevel)
		return true;

	if (!m_pLevel)
	{
		if (!m_pScreen)
			return false;

		return m_pScreen->onBack(b);
	}

	if (b)
		return true;

	if (!m_pScreen)
	{
		pauseGame();
		return false;
	}

	if (m_pScreen->onBack(b))
		return true;

	if (isGamePaused())
	{
		resumeGame();
		return true;
	}

	m_pScreen->onClose();
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
		std::string fileContents;
		Resource::load("shaders/uniforms.json", fileContents);
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
	Resource::teardownLoaders();
	// Stop our SoundSystem before we nuke our sound buffers and cause it to implode
	platform()->getSoundSystem()->stopEngine();
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

	if (getOptions()->m_bUseController.get())
	{
		GameControllerHandler* pControllerHandler = platform()->getGameControllerHandler();
		if (pControllerHandler)
		{
			pControllerHandler->refresh();
		}
	}

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
