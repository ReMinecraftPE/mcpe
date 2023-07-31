/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "App.hpp"
#include "Mth.hpp"
#include "Gui.hpp"
#include "Screen.hpp"
#include "Timer.hpp"
#include "GameRenderer.hpp"
#include "LevelRenderer.hpp"
#include "Level.hpp"
#include "CThread.hpp"
#include "LocalPlayer.hpp"
#include "SurvivalMode.hpp"
#include "ITurnInput.hpp"
#include "EntityRenderDispatcher.hpp"
#include "ParticleEngine.hpp"
#include "SoundEngine.hpp"
#include "RakNetInstance.hpp"
#include "NetEventCallback.hpp"

class Screen; // in case we're included from Screen.hpp

class Minecraft : public App
{
public:
	Minecraft();
	virtual ~Minecraft();

	int getLicenseId();
	void setScreen(Screen * pScreen);
	void releaseMouse();
	void grabMouse();
	void tick();
	void tickInput();
	void reloadOptions();
	void handleMouseClick(int type);
	void handleMouseDown(int type, bool b);
	bool isLevelGenerated();
	void selectLevel(const std::string&, const std::string&, int);
	void setLevel(Level*, const std::string&, LocalPlayer*);
	void pauseGame();
	void leaveGame(bool bCopyMap);
	void hostMultiplayer();
	void joinMultiplayer(const PingedCompatibleServer& serverInfo);
	void cancelLocateMultiplayer();
	void locateMultiplayer();

#ifndef ORIGINAL_CODE
	void tickMouse();
#endif

	virtual void onGraphicsReset();
	virtual void update() override;
	virtual void init() override;

	static void* prepareLevel_tspawn(void* pMinecraft);
	void generateLevel(const std::string& unused, Level* pLevel);
	void prepareLevel(const std::string& unused);
	void _levelGenerated();
	bool isOnline();
	bool isOnlineClient();

	const char* getProgressMessage();
	LevelStorageSource* getLevelSource();

public:
	static int width, height;
	static bool useAmbientOcclusion;
	static const char* progressMessages[];
	static const bool DEADMAU5_CAMERA_CHEATS;
	static int customDebugId;

public:
	bool field_18 = false;
	Options m_options;
	bool field_288 = false;
	LevelRenderer* m_pLevelRenderer = nullptr;
	GameRenderer* m_pGameRenderer = nullptr;
	ParticleEngine* m_pParticleEngine = nullptr;
	SoundEngine* m_pSoundEngine = nullptr;
	GameMode* m_pGameMode = nullptr;
	Textures* m_pTextures = nullptr;
	Font* m_pFont = nullptr;
	RakNetInstance* m_pRakNetInstance = nullptr;
	NetEventCallback* m_pNetEventCallback = nullptr;
	int field_2B0 = 0;
	int field_2B4;
	int field_2B8;
	User* m_pUser = nullptr;
	Level* m_pLevel = nullptr;
	LocalPlayer* m_pLocalPlayer = nullptr;
	Mob* m_pMobPersp = nullptr; // why is there a duplicate?
	Gui m_gui;
	int field_D0C = 0;
	CThread* m_pPrepThread = nullptr;
	Screen* field_D14 = nullptr;
	int field_D18 = 10;
	ITurnInput* m_pTurnInput = nullptr;
	float field_D20 = 0.0f;
	float field_D24 = 0.0f;
	bool m_bGrabbedMouse = true;
	HitResult m_hitResult;
	int m_progressPercent = 0;
	std::string field_D58;
	Timer m_timer;
	bool m_bPreparingLevel = false;
	LevelStorageSource* m_pLevelStorageSource = nullptr; // TODO
	int field_D9C = 0;
	int field_DA0 = 0;
	int field_DA4 = 0;
	int field_DA8 = 0;
	int field_DAC = 0;
	bool field_DB0 = 0;
	bool field_DB1 = 0;
	Screen* m_pScreen = nullptr;
	int m_licenseID = -2;
};

