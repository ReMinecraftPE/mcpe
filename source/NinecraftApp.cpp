/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "NinecraftApp.hpp"
#include "world/item/Item.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"

#ifdef DEMO
#include "world/level/storage/MemoryLevelStorageSource.hpp"
#else
#include "world/level/storage/ExternalFileLevelStorageSource.hpp"
#endif

bool NinecraftApp::_hasInitedStatics;

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
		return 1;

	if (!m_pScreen) return false;

	if (m_pScreen->handleBackEvent(b))
		return true;

	setScreen(nullptr);
	return true;
}

void NinecraftApp::initGLStates()
{
	GL_TEXTURE_2D;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glCullFace(GL_NONE);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}

void NinecraftApp::init()
{
	Mth::initMth();

	if (!_hasInitedStatics)
	{
		_hasInitedStatics = true;
		Material::initMaterials();
		Tile::initTiles();
		Item::initItems();
		Biome::initBiomes();
	}

	initGLStates();
	Tesselator::instance.init();
	Minecraft::init();

#ifdef DEMO
	m_pLevelStorageSource = new MemoryLevelStorageSource;
#else
	m_pLevelStorageSource = new ExternalFileLevelStorageSource(m_externalStorageDir);
#endif

	field_D9C = 0;

	setScreen(new StartMenuScreen);
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
	Minecraft::update();

#ifdef ORIGINAL_CODE
	eglSwapBuffers(field_8, field_10);
#endif

	Mouse::_xOld = Mouse::_x;
	Mouse::_yOld = Mouse::_y;
	updateStats();
}

void NinecraftApp::updateStats()
{
	int timeMs = getTimeMs();
	if (timeMs > field_2B0 + 999)
	{
		if (m_pLocalPlayer)
		{
			Vec3 &pos = m_pLocalPlayer->m_pos;
			printf("%d fps\t%3d chunk updates.   (%.2f, %.2f, %.2f)\n", m_fps, Chunk::updates, pos.x, pos.y, pos.z);
			printf("%s", m_pLevelRenderer->gatherStats1().c_str());
			Chunk::updates = 0;
		}
		else
		{
			printf("%d fps\n", m_fps);
		}

		field_2B0 = timeMs;
		m_fps = 0;
	}
}

NinecraftApp::~NinecraftApp()
{
	teardown();
}
