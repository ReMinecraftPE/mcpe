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
#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"
#include "client/renderer/PatchManager.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/GlobalConstantBufferManager.hpp"
#include "renderer/GlobalConstantBuffers.hpp"
#include "renderer/ConstantBufferMetaDataManager.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/platform/ogl/Extensions.hpp"

#ifdef DEMO
#include "world/level/storage/MemoryLevelStorageSource.hpp"
#else
#include "world/level/storage/ExternalFileLevelStorageSource.hpp"
#endif

bool NinecraftApp::_hasInitedStatics;

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

void NinecraftApp::initGLStates()
{
#ifdef MC_GL_DEBUG_OUTPUT
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	xglDebugMessageCallback(&mce::Platform::OGL::DebugMessage, nullptr);
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int NinecraftApp::getFpsIntlCounter()
{
	int ofps = m_fps;
	m_fps = 0;
	return ofps;
}

void NinecraftApp::init()
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

	// Must be loaded before options, certain options states are forced based on this
	_reloadOptionalFeatures();
	_reloadPatchData();

	if (platform()->hasFileSystemAccess())
		m_pOptions = new Options(m_externalStorageDir);
	else
		m_pOptions = new Options();

	setupRenderer();

	// Load materials
	mce::RenderMaterialGroup::common.loadList("materials/common.json");
	Options* pOptions = getOptions();
	_reloadFancy(pOptions->m_bFancyGraphics);

	// Do this after, since material loading messed with the GL state, particularly depth
	initGLStates();
	Tesselator::instance.init();
	platform()->initSoundSystem();

	Minecraft::init();

#ifdef DEMO
	m_pLevelStorageSource = new MemoryLevelStorageSource;
#else
	m_pLevelStorageSource = new ExternalFileLevelStorageSource(m_externalStorageDir);
#endif

	field_D9C = 0;

	setScreen(new StartMenuScreen);
}

void NinecraftApp::setupRenderer()
{
	mce::GlobalConstantBufferManager::createInstance();
	mce::GlobalConstantBuffers::createInstance();
	if (mce::ConstantBufferMetaDataManager::createInstance())
	{
#ifdef FEATURE_SHADERS
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
	initGLStates();
	Tesselator::instance.init();
	Minecraft::onGraphicsReset();
}

void NinecraftApp::teardown()
{

}

void NinecraftApp::update()
{
	++m_fps;
	Multitouch::commit();
	Minecraft::update();
	Mouse::reset2();
	updateStats();
}

void NinecraftApp::updateStats()
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
