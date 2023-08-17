/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "App.hpp"
#include "client/common/CThread.hpp"
#include "client/common/Mth.hpp"
#include "client/common/Timer.hpp"
#include "client/gui/Gui.hpp"
#include "client/gui/Screen.hpp"
#include "client/network/RakNetInstance.hpp"
#include "client/network/NetEventCallback.hpp"
#include "client/player/input/ITurnInput.hpp"
#include "client/renderer/GameRenderer.hpp"
#include "client/renderer/LevelRenderer.hpp"
#include "client/renderer/entity/EntityRenderDispatcher.hpp"
#include "client/sound/SoundEngine.hpp"
#include "world/level/Level.hpp"
#include "world/entity/LocalPlayer.hpp"
#include "world/gamemode/GameMode.hpp"
#include "world/particle/ParticleEngine.hpp"

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
	void saveOptions();
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
	void tickMouse();
	void handleCharInput(char chr);
	void sendMessage(const std::string& message);
	void resetPlayer(Player* player);
	void respawnPlayer(Player* player);
	std::string getVersionString();

	virtual void onGraphicsReset();
	virtual void update() override;
	virtual void init() override;
	virtual void sizeUpdate(int newWidth, int newHeight) override;

	float getBestScaleForThisScreenSize(int width, int height);
	void generateLevel(const std::string& unused, Level* pLevel);
	void prepareLevel(const std::string& unused);
	void _levelGenerated();
	bool isOnline();
	bool isOnlineClient();
	static void* prepareLevel_tspawn(void* pMinecraft);
    static void setDisplayProperties(
        int drawWidth, int drawHeight,
        int windowWidth, int windowHeight);
    static const int getWindowWidth() { return _windowWidth; }
    static const int getWindowHeight() { return _windowHeight; }
    static const int getDrawWidth() { return width; }
    static const int getDrawHeight() { return height; }
    static const int getDrawScale() { return _drawScale; }

	const char* getProgressMessage();
	LevelStorageSource* getLevelSource();

	ItemInstance* getSelectedItem();

	virtual int getFpsIntlCounter();

private:
    static int _windowWidth, _windowHeight;
    //static int _drawWidth, _drawHeight;
    static float _drawScale;
public:
    // DEPRECATED: Use getDrawWidth() & getDrawHeight() instead
	static int width, height;
	static bool useAmbientOcclusion;
	static const char* progressMessages[];
	static const bool DEADMAU5_CAMERA_CHEATS;
	static int customDebugId;

public:
	bool field_18;
	Options m_options;
	bool field_288;
	LevelRenderer* m_pLevelRenderer;
	GameRenderer* m_pGameRenderer;
	ParticleEngine* m_pParticleEngine;
	SoundEngine* m_pSoundEngine;
	GameMode* m_pGameMode;
	Textures* m_pTextures;
	Font* m_pFont;
	RakNetInstance* m_pRakNetInstance;
	NetEventCallback* m_pNetEventCallback;
	int field_2B0;
	int field_2B4;
	int field_2B8;
	User* m_pUser;
	Level* m_pLevel;
	LocalPlayer* m_pLocalPlayer;
	Mob* m_pMobPersp; // why is there a duplicate?
	Gui m_gui;
	int field_D0C;
	CThread* m_pPrepThread;
	Screen* m_pScreen;
	int field_D18;
	ITurnInput* m_pTurnInput;
	float field_D20;
	float field_D24;
	bool m_bGrabbedMouse;
	HitResult m_hitResult;
	int m_progressPercent;
	std::string m_externalStorageDir;
	Timer m_timer;
	bool m_bPreparingLevel;
	LevelStorageSource* m_pLevelStorageSource; // TODO
	int field_D9C;
	int field_DA0;
	int field_DA4;
	int field_DA8;
	int field_DAC;
	bool m_bUsingScreen;
	bool m_bHasQueuedScreen;
	Screen* m_pQueuedScreen;
	int m_licenseID;
	ItemInstance m_CurrItemInstance;

	// in 0.8. Offset 3368
	double m_fDeltaTime, m_fLastUpdated;
};

