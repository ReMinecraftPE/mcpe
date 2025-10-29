/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "App.hpp"
#include "common/CThread.hpp"
#include "common/Mth.hpp"
#include "common/Timer.hpp"
#include "client/gui/Gui.hpp"
#include "client/gui/Screen.hpp"
#include "network/RakNetInstance.hpp"
#include "network/NetEventCallback.hpp"
#include "client/player/input/IInputHolder.hpp"
#include "client/player/input/MouseHandler.hpp"
#include "client/player/input/BuildActionIntention.hpp"
#include "client/player/LocalPlayer.hpp"
#include "client/renderer/GameRenderer.hpp"
#include "client/renderer/LevelRenderer.hpp"
#include "client/renderer/entity/EntityRenderDispatcher.hpp"
#include "client/sound/SoundEngine.hpp"
#include "world/level/Level.hpp"
#include "world/gamemode/GameMode.hpp"
#include "world/gamemode/GameType.hpp"
#include "world/particle/ParticleEngine.hpp"

class Screen; // in case we're included from Screen.hpp

class Minecraft : public App
{
public:
	Minecraft();
	virtual ~Minecraft();

public:
	int getLicenseId();
	void setScreen(Screen * pScreen);
	void releaseMouse();
	void grabMouse();
	void tick();
	void tickInput();
	void saveOptions();
	void handleBuildAction(const BuildActionIntention& action);
	bool isLevelGenerated() const;
    void selectLevel(const LevelSummary& ls, bool forceConversion = false);
	void selectLevel(const std::string& levelDir, const std::string& levelName, const LevelSettings& levelSettings, bool forceConversion = false);
	void setLevel(Level*, const std::string&, LocalPlayer*);
	bool pauseGame();
	bool resumeGame();
	void leaveGame(bool bCopyMap);
	void hostMultiplayer();
	void joinMultiplayer(const PingedCompatibleServer& serverInfo);
	void cancelLocateMultiplayer();
	void locateMultiplayer();
	void tickMouse();
	void handleCharInput(char chr);
	void resetInput();
	void sendMessage(const std::string& message);
	void respawnPlayer();
	std::string getVersionString(const std::string& str = Util::EMPTY_STRING) const;
	bool isTouchscreen() const;
	bool useSplitControls() const;
	bool useController() const;

	void setGameMode(GameType gameType);

	virtual void update() override;
	virtual void init() override;
	virtual void onGraphicsReset();
	virtual void sizeUpdate(int newWidth, int newHeight) override;
	virtual int getFpsIntlCounter();

	float getBestScaleForThisScreenSize(int width, int height);
	void generateLevel(const std::string& unused, Level* pLevel);
	void prepareLevel(const std::string& unused);
	bool isOnline() const;
	bool isOnlineClient() const;
	bool isGamePaused() const { return m_bIsGamePaused; }
	static void* prepareLevel_tspawn(void* pMinecraft);

	const char* getProgressMessage();
	LevelStorageSource* getLevelSource();
	ItemInstance* getSelectedItem();
	Options* getOptions() const { return m_options; }
	
private:
	void _reloadInput();
	void _levelGenerated();
	void _initTextures();
	void _resetPlayer(Player* player);
	GameMode* createGameMode(GameType gameType, Level& level);

private:
    // Value provided by the OS
    static float _renderScaleMultiplier;
public:
    static float getRenderScaleMultiplier() { return _renderScaleMultiplier; }
    static void setRenderScaleMultiplier(float value) { _renderScaleMultiplier = value; }
    
public:
	static int width, height;
	static bool useAmbientOcclusion;
	static const char* progressMessages[];
	static const bool DEADMAU5_CAMERA_CHEATS;
	static int customDebugId;

private:
	Options *m_options;

public:
	bool field_18;
	bool m_bIsGamePaused;
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
	IInputHolder* m_pInputHolder;
	MouseHandler m_mouseHandler;
	bool m_bGrabbedMouse;
	bool m_bIsTouchscreen;
	HitResult m_hitResult;
	int m_progressPercent;
	std::string m_externalStorageDir;
	Timer m_timer;
	bool m_bPreparingLevel;
	LevelStorageSource* m_pLevelStorageSource; // TODO
	int field_D9C;
	int field_DA0;
	int m_lastBlockBreakTime;
	int field_DA8;
	int field_DAC;
	bool m_bUsingScreen;
	bool m_bHasQueuedScreen;
	Screen* m_pQueuedScreen;
	int m_licenseID;
	ItemInstance m_CurrItemInstance;

	// in 0.8. Offset 3368
	double m_fDeltaTime, m_fLastUpdated;
	int m_lastInteractTime;
};

