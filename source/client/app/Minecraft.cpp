/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Minecraft.hpp"
#include "GameMods.hpp"
#include "compat/PlatformDefinitions.h"
#include "client/gui/screens/PauseScreen.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"
#include "client/gui/screens/RenameMPLevelScreen.hpp"
#include "client/gui/screens/SavingWorldScreen.hpp"
#include "client/gui/screens/DeathScreen.hpp"
#include "client/gui/screens/ProgressScreen.hpp"
#include "client/gui/screens/ConvertWorldScreen.hpp"

#include "client/network/ClientSideNetworkHandler.hpp"
#include "server/ServerSideNetworkHandler.hpp"
#include "network/packets/PlaceBlockPacket.hpp"
#include "network/packets/MessagePacket.hpp"

#include "world/gamemode/SurvivalMode.hpp"
#include "world/gamemode/CreativeMode.hpp"

#include "client/player/input/GameControllerManager.hpp"
#include "client/player/input/ControllerBuildInput.hpp"
#include "client/player/input/ControllerMoveInput.hpp"
#include "client/player/input/ControllerTurnInput.hpp"
#include "client/player/input/MouseTurnInput.hpp"
#include "client/player/input/KeyboardInput.hpp"
#include "client/player/input/MouseBuildInput.hpp"
#include "client/player/input/CustomInputHolder.hpp"
#include "client/player/input/TouchInputHolder.hpp"
#include "client/player/input/Multitouch.hpp"

#include "world/tile/SandTile.hpp"
#include "world/level/TileSource.hpp"

#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"
#include "client/renderer/PatchManager.hpp"

#include "renderer/RenderContextImmediate.hpp"
#include "client/renderer/LogoRenderer.hpp"
#include "client/renderer/Font_Console.hpp"

Minecraft* Minecraft::_singletonPtr;
float Minecraft::_renderScaleMultiplier = 1.0f;

ViewportSize Minecraft::_viewportSize =
    ViewportSize(
        C_DEFAULT_SCREEN_WIDTH, C_DEFAULT_SCREEN_HEIGHT,
        C_DEFAULT_SCREEN_WIDTH, C_DEFAULT_SCREEN_HEIGHT
    );
bool Minecraft::useAmbientOcclusion = true;
int Minecraft::customDebugId = 0;
InputMethod::Type Minecraft::_inputMethod = InputMethod::KEYBOARD;

//@HUH: For the demo, this is defined as TRUE.
//@HUH: deadmau5 had camera cheats? That's interesting.
const bool Minecraft::DEADMAU5_CAMERA_CHEATS = true;

const char* Minecraft::progressMessages[] =
{
	"Locating server",
	"Building terrain",
	"Preparing", // "Simulating world for a bit" on Java
	"Saving chunks",
};

static AppPlatform* platform()
{
	return AppPlatform::singleton();
}

Minecraft::Minecraft()
{
	m_pOptions = nullptr;
	m_pScreenChooser = nullptr;
	field_18 = false;
	m_bIsGamePaused = false;
	m_pResourceLoader = nullptr;
	m_pLevelRenderer = nullptr;
	m_pGameRenderer = nullptr;
	m_pParticleEngine = nullptr;
	m_pSoundEngine = nullptr;
	memset(m_gameModes, 0, sizeof(GameMode*) * GAME_TYPES_COUNT);
	m_pTextures = nullptr;
	m_pFont = nullptr;
	m_pRakNetInstance = nullptr;
	m_pNetEventCallback = nullptr;
	field_2B0 = 0;
	m_pUser = nullptr;
	m_pLevel = nullptr;
	m_pLocalPlayer = nullptr;
	m_pCameraEntity = nullptr; // why is there a duplicate?
	m_pGui = nullptr;
	field_D0C = 0;
	m_pPrepThread = nullptr;
	m_pScreen = nullptr;
	field_D18 = 10;
	m_pInputHolder = nullptr;
	m_bGrabbedMouse = false; // this was true by default. why? we do not start off in-game...
	m_bIsTouchscreen = false;
	m_progressPercent = 0;
	m_bPreparingLevel = false;
	m_bPendingResize = false;
	m_pLevelStorageSource = nullptr; // TODO
	field_D9C = 0;
	field_DA0 = 0;
	m_lastBlockBreakTime = 0;
	field_DA8 = 0;
	field_DAC = 0;
	m_bUsingScreen = false;
	m_bHasQueuedScreen = false;
	m_pQueuedScreen = nullptr;
	m_licenseID = -2;
	m_fLastUpdated = 0;
	m_fDeltaTime = 0;
	m_lastInteractTime = 0;

	_singletonPtr = this;
}

Minecraft::~Minecraft()
{
	SAFE_DELETE(m_pGui);
	SAFE_DELETE(m_pOptions);
	SAFE_DELETE(m_pNetEventCallback);
	SAFE_DELETE(m_pRakNetInstance);
	//SAFE_DELETE(m_pResourceLoader); // not ours
	SAFE_DELETE(m_pLevelRenderer);
	SAFE_DELETE(m_pGameRenderer);
	SAFE_DELETE(m_pParticleEngine);
    SAFE_DELETE(EntityRenderDispatcher::instance);
	m_pSoundEngine->destroy();
	SAFE_DELETE(m_pSoundEngine);
	for (unsigned int gameType = GAME_TYPES_MIN; gameType <= GAME_TYPES_MAX; gameType++)
	{
		SAFE_DELETE(m_gameModes[gameType]);
	}
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pTextures);
	SAFE_DELETE(m_pScreenChooser);
    
	if (m_pLevel)
	{
		LevelStorage* pStor = m_pLevel->getLevelStorage();
		SAFE_DELETE(pStor);
		delete m_pLevel;
	}
    
	SAFE_DELETE(m_pUser);
	SAFE_DELETE(m_pLevelStorageSource);
	SAFE_DELETE(m_pInputHolder);
    
	//@BUG: potentially leaking a CThread instance if this is destroyed early?
}

void Minecraft::_levelGenerated()
{
	onClientStartedLevel(m_pLevel, m_pLocalPlayer);

	if (m_pNetEventCallback)
		m_pNetEventCallback->levelGenerated(m_pLevel);
}

void Minecraft::_resetPlayer(Player* player)
{
	m_pLevel->validateSpawn();
	player->reset();

	TilePos pos = m_pLevel->getSharedSpawnPos();
	player->setPos(pos);
	player->resetPos();
}

GameMode* Minecraft::_createGameMode(GameType gameType)
{
	switch (gameType)
	{
	case GAME_TYPE_SURVIVAL:
		return new SurvivalMode(this);
	case GAME_TYPE_CREATIVE:
		return new CreativeMode(this);
	default:
		return nullptr;
	}
}

void Minecraft::_initGameModes()
{
	for (unsigned int gameType = GAME_TYPES_MIN; gameType <= GAME_TYPES_MAX; gameType++)
	{
		delete m_gameModes[gameType];
		m_gameModes[gameType] = _createGameMode((GameType)gameType);
	}
}

void Minecraft::reloadInput()
{
	if (m_pInputHolder)
		delete m_pInputHolder;

	if (useTouchscreen())
	{
		m_pInputHolder = new TouchInputHolder(this, getOptions());

		if (m_bGrabbedMouse)
			platform()->setMouseGrabbed(false);
	}
	else if (useController())
	{
		m_pInputHolder = new CustomInputHolder(
			new ControllerMoveInput(getOptions()),
			new ControllerTurnInput(),
			new ControllerBuildInput(),
			true
		);

		if (m_bGrabbedMouse)
			platform()->setMouseGrabbed(false);
	}
	else
	{
		m_pInputHolder = new CustomInputHolder(
			new KeyboardInput(getOptions()),
			new MouseTurnInput(),
			new MouseBuildInput()
		);

		if (m_bGrabbedMouse)
			platform()->setMouseGrabbed(true);
	}

	m_mouseHandler.setTurnInput(m_pInputHolder->getTurnInput());

	if (m_pLevel && m_pLocalPlayer)
	{
		m_pLocalPlayer->m_pMoveInput = m_pInputHolder->getMoveInput();
	}

	getOptions()->m_bUseMouseForDigging = !useTouchscreen();
}

void Minecraft::resetInputMethod()
{
	if (platform()->hasGamepad())
		SetInputMethod(InputMethod::CONTROLLER);
	else if (platform()->isTouchscreen())
		SetInputMethod(InputMethod::TOUCHSCREEN);
	else
		SetInputMethod(InputMethod::KEYBOARD);
}

int Minecraft::getLicenseId()
{
	if (m_licenseID < 0)
		m_licenseID = platform()->checkLicense();

	return m_licenseID;
}

void Minecraft::releaseMouse()
{
	if (!m_bGrabbedMouse)
		return;

	if (m_pLocalPlayer)
		m_pLocalPlayer->m_pMoveInput->releaseAllKeys();

	m_bGrabbedMouse = false;
	m_mouseHandler.release();

	// Note, normally the platform stuff would be located within
	// the mouse handler, but we don't have access to the platform
	// from there!
	recenterMouse();
	platform()->setMouseGrabbed(false);
}

void Minecraft::grabMouse()
{
	if (m_bGrabbedMouse)
		return;

	m_bGrabbedMouse = true;
	m_mouseHandler.grab();

	// This will call grabMouse again, so why are we calling it here?
	//setScreen(nullptr);

	if (useController() || useTouchscreen())
		return; // don't actually try to grab the mouse

	platform()->setMouseGrabbed(true);
}

void Minecraft::recenterMouse()
{
	if (useController() || useTouchscreen())
		return;

	platform()->recenterMouse();
}

void Minecraft::setScreen(Screen* pScreen)
{
#ifndef ORIGINAL_CODE
	if (pScreen == nullptr && !isLevelGenerated())
	{
		return;
	}
#endif

	if (m_bUsingScreen)
	{
		m_bHasQueuedScreen = true;
		m_pQueuedScreen = pScreen;
		return;
	}

	if (pScreen && (pScreen->isErrorScreen() || !pScreen->validate(this)))
	{
		// not in original
		delete pScreen;
		return;
	}

	if (m_pScreen)
	{
		m_pScreen->removed();
		if (!pScreen || (pScreen && pScreen->m_bDeletePrevious))
			delete m_pScreen;
	}

	Mouse::reset();
	Multitouch::reset();
	GameControllerManager::reset();
	Multitouch::resetThisUpdate();

	m_pScreen = pScreen;

	if (pScreen)
	{
		pScreen->init(this, Gui::GuiWidth, Gui::GuiHeight);
	}

	sizeUpdate();

	if (pScreen)
	{
		releaseMouse();

		if (pScreen->isPauseScreen())
		{
			if (m_pLevel && isLevelGenerated())
				m_pLevel->saveGame();
		}
	}
	else
	{
		recenterMouse();
		grabMouse();
	}
}

void Minecraft::saveOptions()
{
	if (platform()->hasFileSystemAccess())
		getOptions()->save().join();
}

void Minecraft::saveOptionsAsync()
{
	if (platform()->hasFileSystemAccess())
		getOptions()->save();
}

bool Minecraft::isLevelGenerated() const
{
	return m_pLevel && !m_bPreparingLevel;
}

bool Minecraft::isOnline() const
{
	return m_pNetEventCallback != nullptr;
}

bool Minecraft::isOnlineClient() const
{
	if (!m_pLevel)
		return false;

	return m_pLevel->m_bIsClientSide;
}

bool Minecraft::useTouchscreen() const
{
	return GetInputMethod() == InputMethod::TOUCHSCREEN;
}

bool Minecraft::useSplitControls() const
{
	return !useTouchscreen() || getOptions()->m_splitControls.get();
}

bool Minecraft::useController() const
{
	return GetInputMethod() == InputMethod::CONTROLLER;
}

void Minecraft::setGameMode(GameType gameType)
{
	if (m_pLevel)
	{
		GameMode* pGameMode = m_gameModes[gameType];
		pGameMode->initLevel(m_pLevel);
	}
}

GameMode* Minecraft::getLevelGameMode() const
{
	if (!m_pLevel)
		return nullptr;

	GameType gameType = m_pLevel->getDefaultGameType();

	return m_gameModes[gameType];
}

GameMode* Minecraft::getPlayerGameMode(Player& player) const
{
	GameType gameType = player.getPlayerGameType();

	return m_gameModes[gameType];
}

GameMode* Minecraft::getLocalPlayerGameMode() const
{
	if (m_pLocalPlayer)
		return getPlayerGameMode(*m_pLocalPlayer);
	else if (m_pLevel)
		return m_gameModes[m_pLevel->getLoadedPlayerGameType()];

	return nullptr;
}

void Minecraft::handleBuildAction(const BuildActionIntention& action)
{
	LocalPlayer* player = m_pLocalPlayer;
	GameMode* pGameMode = getLocalPlayerGameMode();
	bool canInteract = getTimeMs() - m_lastInteractTime >= 200;
	// This logic is present in 0.9.0, but just does not make any sense being here.
	//if (player->isUsingItem()) return;

	if (action.isDestroyStart() || action.isAttack())
	{
		player->swing();
	}
	
	if (!action.isDestroy() && !pGameMode->m_bInstaBuild) // from Minecraft::handleMouseDown
	{
		pGameMode->stopDestroyBlock();
	}

	if (!action.isInteract()) m_lastInteractTime = 0;

	bool bInteract = true;
	switch (m_hitResult.m_hitType)
	{
		case HitResult::ENTITY:
		{
			Entity* pTarget = m_hitResult.m_pEnt;
			if (action.isAttack())
			{
				InteractPacket pkt(player->m_EntityID, pTarget->m_EntityID, InteractPacket::ATTACK);

				pkt.m_reliability = RELIABLE_ORDERED;
				pkt.m_channel = CHANNEL_PLAYER_EVENTS;
				m_pRakNetInstance->send(pkt);

				pGameMode->attack(*player, *pTarget);
				m_lastBlockBreakTime = getTimeMs();
			}
			else if (action.isInteract() && canInteract)
			{
				if (pTarget->interactPreventDefault())
					bInteract = false;

				InteractPacket pkt(player->m_EntityID, pTarget->m_EntityID, InteractPacket::INTERACT);
				m_pRakNetInstance->send(pkt);

				pGameMode->interact(*player, *pTarget);
				m_lastInteractTime = getTimeMs();
			}
			break;
		}
		case HitResult::TILE:
		{
			TileSource& source = player->getTileSource();

			Tile* pTile = Tile::tiles[source.getTile(m_hitResult.m_tilePos)];

			if (action.isDestroy())
			{
				if (!pTile) return;
				//if (pTile->isLiquidTile()) return;

				player->swing();

				// @BUG: This is only done on the client side.
				//bool extinguished = m_pLevel->extinguishFire(player, m_hitResult.m_tilePos, m_hitResult.m_hitSide);

				// Allows fire to be extinguished *without* destroying blocks
				// @BUG: Hits sometimes pass through fire when done from above
				//if (extinguished) break;

				if (pTile != Tile::unbreakable || (player->m_userType >= 100 && !m_hitResult.m_bUnk24))
				{
					bool destroyed = false;
					if (action.isDestroyStart())
					{
						destroyed = pGameMode->startDestroyBlock(*player, m_hitResult.m_tilePos, m_hitResult.m_hitSide);
						player->startDestroying();
					}

					bool contDestory = pGameMode->continueDestroyBlock(*player, m_hitResult.m_tilePos, m_hitResult.m_hitSide);

					destroyed = destroyed || contDestory;
					m_pParticleEngine->crack(*player, m_hitResult.m_tilePos, m_hitResult.m_hitSide);

					m_lastBlockBreakTime = getTimeMs();

					if (destroyed)
					{
						/*if (isVibrateOnBlockBreakOptionEnabledOrWhatever)
							platform()->vibrate(24);*/
					}
				}
			}
			else if (action.isPick())
			{
				// Try to pick the tile.
				TileData auxValue = source.getData(m_hitResult.m_tilePos);
				player->m_pInventory->pickItem(pTile->m_ID, auxValue, C_MAX_HOTBAR_ITEMS);
			}
			else if (action.isPlace() && canInteract)
			{
				ItemStack& item = getSelectedItem();
				if (pGameMode->useItemOn(*player, item, m_hitResult.m_tilePos, m_hitResult.m_hitSide))
				{
					bInteract = false;

					player->swing();

					m_lastInteractTime = getTimeMs();
				}
			}
			break;
		}
		default:
			break;
	}

	if (bInteract && action.isInteract() && canInteract)
	{
		ItemStack& item = getSelectedItem();
		if (!item.isEmpty())
		{
			m_lastInteractTime = getTimeMs();
			if (pGameMode->useItem(*player, item))
				m_pGameRenderer->m_pItemInHandRenderer->itemUsed();
		}
	}
}

void Minecraft::tickInput()
{
	if (!platform()->hasGamepad() && useController())
		resetInputMethod();

	if (!m_pInputHolder->allowsInputMethod(GetInputMethod()))
		reloadInput();
    
    m_virtualKeyboardManager.tick();
    
	if (m_pScreen)
	{
		if (!m_pScreen->m_bPassEvents)
		{
			m_bUsingScreen = true;
			m_pScreen->updateEvents();
			m_bUsingScreen = false;

			if (m_bHasQueuedScreen)
			{
				setScreen(m_pQueuedScreen);
				m_pQueuedScreen = nullptr;
				m_bHasQueuedScreen = false;
			}
			return;
		}
	}

	if (!m_pLocalPlayer || m_bPreparingLevel)
		return;

	//bool bIsInGUI = m_pGui->isInside(Mouse::getX(), Mouse::getY());

	while (Mouse::next())
	{
		if (getTimeMs() - field_2B4 > 200)
			continue;

		MouseButtonType buttonType = Mouse::getEventButton();
		bool bPressed = Mouse::getEventButtonState() == true;

#ifdef ENH_ALLOW_SCROLL_WHEEL
		if (buttonType == MOUSE_BUTTON_SCROLLWHEEL)
			m_pGui->handleScrollWheel(bPressed);
#endif

		if (Mouse::isButtonDown(MOUSE_BUTTON_LEFT))
		{
			// @HACK: on SDL1, we don't recenter the mouse every tick, meaning the user can
			// unintentionally click the hotbar while swinging their fist
			if (platform()->getRecenterMouseEveryTick() || m_pScreen)
			{
				m_pGui->handleClick(1, Mouse::getX(), Mouse::getY());
				// Gui:handleClick() pops the current mouse event from the stack, and continuing would mean an over-read
				continue;
			}
		}
	}

	if (useController())
	{
		//Clearing, so the events don't accumulate
		Keyboard::reset();

		while (GameControllerManager::next())
		{
			GameController::EngineButtonID button = GameControllerManager::getEventButton();
			bool bPressed = GameControllerManager::getEventButtonState() == GameController::BTN_STATE_DOWN;

			if (bPressed)
				m_pGui->handleUserAction(ActionInfo(-1, button));

			for (size_t i = 0; i < AID_COUNT; ++i)
			{
				UserActionID ctrl = (UserActionID)i;
				if (!getOptions()->getAction(ctrl).isControllerButton(button)) continue;

				m_pLocalPlayer->m_pMoveInput->setKey(ctrl, bPressed);
				if (bPressed)
					getOptions()->getInputMapping(ctrl).pressed();
				else
					getOptions()->getInputMapping(ctrl).reset();
			}
		}
	}
	else
	{
		// Clearing, so the events don't accumulate
		GameControllerManager::clear();
		while (Keyboard::next())
		{
			int keyCode = Keyboard::getEventKey();
			bool bPressed = Keyboard::getEventKeyState() == Keyboard::DOWN;

			if (bPressed)
				m_pGui->handleUserAction(ActionInfo(keyCode, GameController::BUTTON_NONE));

			for (size_t i = 0; i < AID_COUNT; ++i)
			{
				UserActionID ctrl = (UserActionID)i;
				if (!getOptions()->isKey(ctrl, keyCode)) continue;

				m_pLocalPlayer->m_pMoveInput->setKey(ctrl, bPressed);
				if (bPressed)
					getOptions()->getInputMapping(ctrl).pressed();
				else
					getOptions()->getInputMapping(ctrl).reset();
			}

			if (getOptions()->m_bUseMouseForDigging)
				continue;

			// @TODO: Replace with KeyboardBuildInput
			if (getTimeMs() - field_2B4 <= 200)
			{
				if (getOptions()->isKey(AID_DESTROY, keyCode) && bPressed)
				{
					BuildActionIntention intention(BuildActionIntention::KEY_DESTROY);
					handleBuildAction(intention);
				}

				if (getOptions()->isKey(AID_PLACE, keyCode) && bPressed)
				{
					BuildActionIntention intention(BuildActionIntention::KEY_USE);
					handleBuildAction(intention);
				}
			}
		}
	}

	for (int i = 0; i < m_pGui->getNumUsableSlots(); i++)
	{
		while (getOptions()->getInputMapping(UserActionID(AID_SLOT_1 + i)).consume())
			m_pLocalPlayer->m_pInventory->selectSlot(i);
	}

	while (getOptions()->getInputMapping(AID_TOGGLE3RD).consume())
	{
		getOptions()->m_thirdPerson.toggle();
	}

	while (getOptions()->getInputMapping(AID_MENU_PAUSE).consume())
	{
		handleBack(false);
	}

	while (getOptions()->getInputMapping(AID_DROP).consume())
	{
		m_pLocalPlayer->drop();
	}

	while (getOptions()->getInputMapping(AID_TOGGLEGUI).consume())
	{
		getOptions()->m_hideGui.toggle();
	}

	while (getOptions()->getInputMapping(AID_TOGGLEDEBUG).consume())
	{
		getOptions()->m_debugText.toggle();
	}
#ifdef ENH_ALLOW_AO_TOGGLE
	while (getOptions()->getInputMapping(AID_TOGGLEAO).consume())
	{
		// Toggle ambient occlusion.
		getOptions()->m_ambientOcclusion.toggle();
		m_pLevelRenderer->allChanged();
	}
#endif

	BuildActionIntention bai;
	IBuildInput* buildInput = m_pInputHolder->getBuildInput();
	if (buildInput && buildInput->tickBuild(m_pLocalPlayer, &bai))
		handleBuildAction(bai);

	field_2B4 = getTimeMs();
}

void Minecraft::tickMouse()
{
	if (!m_bGrabbedMouse)
		return;

	/**
	 * iProgramInCpp's explanation on why we're recentering the mouse constantly when it's grabbed:
	 * we would recenter the mouse every frame to avoid it being stopped by the edges of the window
	 * when we grab the mouse, the Win32 AppPlatform calls ClipCursor(hWnd) which clips the cursor within the confines of the window and does not let it escape
	 * if we weren't doing that, we'd
	 * not receive WM_MOUSEMOVE events outside of the window and
	 * even if we would, we'd have the exact same problem with the edges of the screen
	 * so to fix these issues we clip the cursor within the window, and every frame we set its position back to the center.
	 * This is exactly what Minecraft Java does too
	**/

	if (useController() || useTouchscreen())
		return; // don't actually try to recenter the mouse

    if (platform()->getRecenterMouseEveryTick()) // just for SDL1
        recenterMouse();
}

void Minecraft::handleCharInput(char chr)
{
	if (m_pScreen)
		m_pScreen->handleTextChar(chr);
}

void Minecraft::handleTextPaste(const std::string& text)
{
	if (m_pScreen)
		m_pScreen->keyboardTextPaste(text);
}

void Minecraft::handleTextPaste()
{
	std::string text = platform()->getClipboardText();
	if (!text.empty())
		handleTextPaste(text);
}

void Minecraft::handlePointerLocation(MenuPointer::Unit x, MenuPointer::Unit y)
{
	if (m_pScreen)
		m_pScreen->handleRawPointerLocation(x, y);
}

void Minecraft::handlePointerPressedButtonPress()
{
	// m_pGui->handleClick();
	if (m_pScreen)
		m_pScreen->handlePointerPressed(true);
}

void Minecraft::handlePointerPressedButtonRelease()
{
	if (m_pScreen)
		m_pScreen->handlePointerPressed(false);
}

void Minecraft::handleKeyboardClosed()
{
	if (m_pScreen)
		m_pScreen->handleKeyboardClosed();
}

void Minecraft::resetInput()
{
	Keyboard::reset();
	Mouse::reset();
	GameControllerManager::reset();
	Multitouch::resetThisUpdate();
}

void Minecraft::sendMessage(const std::string& message)
{
	if (isOnlineClient())
	{
		// send the server a message packet
		if (m_pRakNetInstance)
		{
			MessagePacket pkt(message);
			m_pRakNetInstance->send(pkt);
		}
		else
		{
			m_pGui->addMessage("You aren't actually playing multiplayer!");
		}
	}
	else
	{
		// fake the server having received a packet
		MessagePacket mp(message);
		if (m_pNetEventCallback && m_pRakNetInstance)
			m_pNetEventCallback->handle(m_pRakNetInstance->m_pRakPeerInterface->GetMyGUID(), &mp);
		else
			m_pGui->addMessage("You aren't hosting a multiplayer server!");
	}
}

void Minecraft::respawnPlayer()
{
	_resetPlayer(m_pLocalPlayer);

	// Allows client to dictate respawn position. Why?
	m_pRakNetInstance->send(new RespawnPacket(m_pLocalPlayer->m_EntityID, m_pLocalPlayer->m_pos));
}

void Minecraft::freeResources(bool bCopyMap)
{
	m_pLevelRenderer->setLevel(nullptr);
	m_pParticleEngine->setLevel(nullptr);

	m_pCameraEntity = nullptr;
	m_pLocalPlayer = nullptr;

#ifndef ENH_IMPROVED_SAVING
	if (bCopyMap)
		setScreen(new RenameMPLevelScreen("_LastJoinedServer"));
	else
		gotoMainMenu();
#endif

	m_pGameRenderer->setLevel(nullptr, nullptr);

	delete m_pNetEventCallback;
	m_pNetEventCallback = nullptr;

#ifdef ENH_IMPROVED_SAVING
	m_bIsGamePaused = true;
	getScreenChooser()->pushSavingScreen(bCopyMap/*, m_pLocalPlayer*/);
#else
	if (m_pLevel)
	{
		LevelStorage* pStorage = m_pLevel->getLevelStorage();
		SAFE_DELETE(pStorage);
		SAFE_DELETE(m_pLevel);

		m_pLevel = nullptr;
	}
#endif

	field_D9C = 0;
}

void Minecraft::unloadLevel(bool bCopyMap)
{
	if (m_pLevel)
	{
		if (!m_pLevel->m_bIsClientSide || bCopyMap)
		{
			m_progressPercent = 33;
			m_pLevel->saveUnsavedChunks();

			m_progressPercent = 66;
			m_pLevel->saveLevelData();

			m_progressPercent = 100;
			m_pLevel->savePlayerData();
		}

		LevelStorage* pStorage = m_pLevel->getLevelStorage();
		SAFE_DELETE(pStorage);
		SAFE_DELETE(m_pLevel);

		m_pLevel = nullptr;
	}

	// this is safe to do, since on destruction, nothing accesses the parent level or anything
	//SAFE_DELETE(m_pEntityToDeleteAfterSave);
	// already done by the Level

	m_pCameraEntity = m_pLocalPlayer = nullptr;

	m_bUsingScreen = true;

	if (bCopyMap)
		setScreen(new RenameMPLevelScreen("_LastJoinedServer"));
	else
		gotoMainMenu();

	m_bUsingScreen = false;

	m_bIsGamePaused = false;
}

std::string Minecraft::getVersionString(const std::string& str) const
{
	return "v0.3.3" + str + " alpha";
}

void Minecraft::tick()
{
	if (m_bPendingResize)
	{
		m_bPendingResize = false;
		sizeUpdate();
	}

	if (!m_pScreen)
	{
		if (m_pLocalPlayer && m_pLocalPlayer->m_health <= 0)
		{
			getScreenChooser()->pushDeathScreen();
		}
	}
	else
	{
		m_pScreen->validate(this);
	}

	tickInput();

	m_pGui->tick();

	LogoRenderer::singleton().tick();

	// if the level has been prepared, delete the prep thread
	if (!m_bPreparingLevel)
	{
		if (m_pPrepThread)
		{
			delete m_pPrepThread;
			m_pPrepThread = nullptr;
			_levelGenerated();
		}

		SandTile::instaFall = false;

		if (m_pLevel && !isGamePaused())
		{
            m_pLevel->m_difficulty = getOptions()->m_difficulty.get();
            if (m_pLevel->m_bIsClientSide)
            {
                m_pLevel->m_difficulty = 3;
            }
            
			GameMode* pGameMode = getLocalPlayerGameMode();
			if (pGameMode)
				pGameMode->tick();

			m_pGameRenderer->tick();
			m_pLevelRenderer->tick();
			m_pLevel->tickEntities();
			m_pLevel->tick();

			if (m_pLocalPlayer)
			{
				m_pLevel->animateTick(m_pLocalPlayer->getPos());
			}
		}

		m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

		if (!isGamePaused())
		{
			m_pTextures->tick();
			m_pParticleEngine->tick();

			m_pSoundEngine->updateListener(m_pCameraEntity, m_timer.m_renderTicks);
		}

		if (m_pScreen)
			m_pScreen->tick();

		Multitouch::reset();

		// Actually pause the game, because fuck bedrock edition
		m_bIsGamePaused = (!isOnline() || (m_pLevel && m_pLevel->m_players.size() == 1)) && (m_pScreen && m_pScreen->isPauseScreen());
	}
}

void Minecraft::update()
{
	m_timer.advanceTime(isGamePaused() && m_pLevel);

	platform()->tick();

	if (m_pRakNetInstance && m_pNetEventCallback)
	{
		m_pRakNetInstance->runEvents(*m_pNetEventCallback);
	}

	for (int i = 0; i < m_timer.m_ticks; i++)
	{
		// @BUG?: Minecraft::tick() also calls tickInput(), so we're doing input handling potentially more than once
		tick(); // tick(i, m_timer.m_ticks - 1); // 0.9.2
		field_DA8++;
	}

	m_pSoundEngine->update();

	if (m_pLevel && !m_bPreparingLevel)
	{
		m_pLevel->updateLights();
	}

	tickMouse();

	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	renderContext.beginRender();

	m_pGameRenderer->render(m_timer);

	renderContext.endRender();

	double time = getTimeS();
	m_fDeltaTime = time - m_fLastUpdated;
	m_fLastUpdated = time;
}

void Minecraft::init()
{
	m_pRakNetInstance = new RakNetInstance;
}

void Minecraft::prepareLevel(const std::string& unused)
{
	field_DA0 = 1;

	//float startTime = float(getTimeS());
	Level* pLevel = m_pLevel;

	if (!pLevel->field_B0C)
	{
		pLevel->setUpdateLights(false);
	}

	for (int i = 8, i2 = 0; i != 8 + C_MAX_CHUNKS_X * 16; i += 16)
	{
		for (int j = 8; j != 8 + C_MAX_CHUNKS_Z * 16; j += 16, i2 += 100)
		{
			// this looks like some kind of progress tracking
			m_progressPercent = i2 / (C_MAX_CHUNKS_X * C_MAX_CHUNKS_Z);

			//float time1 = float(getTimeS());

			// generating all the chunks at once
			(void)pLevel->getTile(TilePos(i, (C_MAX_Y + C_MIN_Y) / 2, j));

			//if (time1 != -1.0f)
			//	getTimeS();

			//float time2 = getTimeS();
			if (pLevel->field_B0C)
			{
				while (pLevel->updateLights());
			}

			//if (time2 != -1.0f)
			//	getTimeS();
		}
	}

	//if (startTime != -1.0f)
	//	getTimeS();

	pLevel->setUpdateLights(true);

	//startTime = float(getTimeS());

	ChunkPos cp(0, 0);
	for (cp.x = 0; cp.x < C_MAX_CHUNKS_X; cp.x++)
	{
		for (cp.z = 0; cp.z < C_MAX_CHUNKS_Z; cp.z++)
		{
			LevelChunk* pChunk = pLevel->getChunk(cp);
			if (!pChunk)
				continue;

			if (pChunk->field_237)
				continue;

			pChunk->m_bUnsaved = false;
			pChunk->clearUpdateMap();
		}
	}

	//if (startTime != -1.0f)
	//	getTimeS();

	field_DA0 = 3;

	if (pLevel->field_B0C)
	{
		pLevel->setInitialSpawn();
		pLevel->saveLevelData();
		pLevel->getChunkSource().saveAll();
		pLevel->saveGame();
	}
	else
	{
		pLevel->saveLevelData();
		pLevel->loadEntities();
	}

	m_progressPercent = -1;
	field_DA0 = 2;

	//startTime = float(getTimeS());

	pLevel->prepare();

	//if (startTime != -1.0f)
	//	getTimeS();

	// These strings are initialized and then removed quickly afterwards. Probably some debug leftover
	// "Generate level:";
	// " - light: ";
	// " - getTl: ";
	// " - clear: ";
	// " - prepr: ";
}

void Minecraft::sizeUpdate()
{
	const ViewportSize& size = GetViewportSize();

    // re-calculate the GUI scale.
	Gui::GuiScale = 1.0f / getBestScaleForThisScreenSize(size.logical.width, size.logical.height);

	// The ceil gives an extra pixel to the screen's width and height, in case the GUI scale doesn't
	// divide evenly into width or height, so that none of the game screen is uncovered.
	float newGuiWidth  = ceilf(size.logical.width  * Gui::GuiScale);
	float newGuiHeight = ceilf(size.logical.height * Gui::GuiScale);
	
	// GuiSize did not change, bail out
	if (newGuiWidth == Gui::GuiWidth && newGuiHeight == Gui::GuiHeight)
		return;
	
	Gui::GuiWidth  = newGuiWidth;
	Gui::GuiHeight = newGuiHeight;

	LogoRenderer::singleton().build(Gui::GuiWidth);

	if (m_pScreen)
	{
		m_pScreen->setSize(
			Gui::GuiWidth,
			Gui::GuiHeight
		);
		m_pScreen->initMenuPointer();
	}

	if (m_pInputHolder)
		m_pInputHolder->setScreenSize(size);
}

void Minecraft::setTextboxText(const std::string& text)
{
	if (m_pScreen)
		m_pScreen->setTextboxText(text);
}

float Minecraft::getBestScaleForThisScreenSize(unsigned int width, unsigned int height)
{
	if (m_pScreen)
	{
		float scale = m_pScreen->getScale(width, height);
		if (scale > 0)
			return scale;
	}
	else if (m_pOptions->getUiTheme() == UI_CONSOLE)
		return Screen::GetConsoleScale(height);

#if MC_PLATFORM_XBOX
#define USE_JAVA_SCREEN_SCALING
#endif
#ifdef USE_JAVA_SCREEN_SCALING
	if (getUiTheme() == UI_JAVA)
	{
		int scale;
		for (scale = 1; width / (scale + 1) >= 320 && height / (scale + 1) >= 240; ++scale)
		{
		}
		return scale;
	}
#endif

    if (height >= 1800)
		return 8.0f;

	if (useTouchscreen())
	{
		/*
        // Our custom (broken) logic
        if (height >= 1100)
			return 6.0f;

		if (height >= 900)
			return 5.0f;

		if (height >= 700)
			return 4.0f;

		if (height >= 500)
			return 3.0f;

		if (height >= 300)
			return 2.0f;
        */
        
        // PE 0.3.3 logic
        if (width >= 1000)
            return 4.0f;
        
        if (width >= 800)
            return 3.0f;
        
        if (width >= 400)
            return 2.0f;
	}
	else
	{
		// @PARITY-JAVA: This is the screen scaling we use on non-touchscreen devices (minus Xboxes)
		if (height >= 1600)
			return 4.0f;

		if (height >= 800)
			return 3.0f;

		if (height >= 400)
			return 2.0f;
	}

	return 1.0f;
}

void Minecraft::setupLevelRendering(Level* pLevel, Dimension* pDimension, Mob* pCamera)
{
	m_pCameraEntity = pCamera;

	m_pParticleEngine->setLevel(pLevel);
	m_pGameRenderer->setLevel(pLevel, pDimension);
	m_pLevelRenderer->setLevel(pLevel);
	m_pLevelRenderer->setDimension(pDimension);
}

void Minecraft::onClientStartedLevel(Level* pLevel, LocalPlayer* pLocalPlayer)
{
	// already added in Level::loadPlayer()
	//pLevel->addEntity(pLocalPlayer); // addPlayer on 0.12.1
	setupLevelRendering(pLevel, &pLocalPlayer->getDimension(), pLocalPlayer);
}

void Minecraft::generateLevel(const std::string& unused, Level& level)
{
	//float time = float(getTimeS()); //@UNUSED

	prepareLevel(unused);

	//if (time != -1.0f)
	//	getTimeS(); //@QUIRK: unused return value

	// std::string("Level generated: "); //@QUIRK: unused string instance

	GameMode* pGameMode = getLocalPlayerGameMode();

	LocalPlayer* pLocalPlayer = m_pLocalPlayer;
	if (!pLocalPlayer)
	{
		pLocalPlayer = pGameMode->createPlayer(level);
		pLocalPlayer->resetPos();
		pGameMode->initPlayer(*pLocalPlayer);
	}

	if (pLocalPlayer)
		pLocalPlayer->m_pMoveInput = m_pInputHolder->getMoveInput();

	pGameMode->adjustPlayer(pLocalPlayer);

	level.validateSpawn();
	level.loadPlayer(*pLocalPlayer);

	m_pLocalPlayer = pLocalPlayer;

	m_bPreparingLevel = false;

	if (m_pRakNetInstance && m_pRakNetInstance->m_bIsHost)
		m_pRakNetInstance->announceServer(m_pUser->m_name);
}

void* Minecraft::prepareLevel_tspawn(void* ptr)
{
	Minecraft* pMinecraft = (Minecraft*)ptr;

	pMinecraft->generateLevel("Currently not used", *pMinecraft->m_pLevel);

	return nullptr;
}

bool Minecraft::pauseGame()
{
	if (isGamePaused() || m_pScreen) return false;

	m_pLevel->savePlayerData();
	getScreenChooser()->pushPauseScreen();

	return true;
}

bool Minecraft::resumeGame()
{
	m_bIsGamePaused = false;
	if (m_pScreen)
		setScreen(nullptr);

	return true;
}

void Minecraft::setLevel(Level* pLevel, const std::string& text, LocalPlayer* pLocalPlayer)
{
	m_pCameraEntity = nullptr;

	if (pLevel)
	{
		pLevel->m_pRakNetInstance = m_pRakNetInstance;
		if (pLocalPlayer && m_pLocalPlayer == nullptr)
		{
			// We're getting a LocalPlayer from a server
			m_pLocalPlayer = pLocalPlayer;
		}
		else if (m_pLocalPlayer)
		{
			// We're not on any server
			pLevel->addEntity(m_pLocalPlayer);
		}

		m_pLevel = pLevel;
		m_bPreparingLevel = true;
		m_pPrepThread = new CThread(&Minecraft::prepareLevel_tspawn, this);

		_initGameModes();

		if (m_pLocalPlayer)
			setGameMode(m_pLocalPlayer->getPlayerGameType());
		else
			setGameMode(pLevel->getDefaultGameType());
	}
	else
	{
		m_pLocalPlayer = nullptr;
	}

	if (!m_pLocalPlayer)
	{
		// pLocalPlayer went unused, and *someone* needs to clean it up
		SAFE_DELETE(pLocalPlayer);
	}
}

void Minecraft::selectLevel(const LevelSummary& ls, bool forceConversion)
{
    if (ls.m_storageVersion < LEVEL_STORAGE_VERSION_DEFAULT && !forceConversion)
    {
        setScreen(new ConvertWorldScreen(ls));
        return;
    }
    
    selectLevel(ls.m_fileName, ls.m_levelName, LevelSettings(), forceConversion);
}

void Minecraft::selectLevel(const std::string& levelDir, const std::string& levelName, const LevelSettings& levelSettings, bool forceConversion)
{
	LevelStorage* pStor = m_pLevelStorageSource->selectLevel(levelDir, false, forceConversion);
	Dimension* pDim = Dimension::createNew(DIMENSION_OVERWORLD);

	m_pLevel = new Level(pStor, levelName, levelSettings, LEVEL_STORAGE_VERSION_DEFAULT, pDim);
	setLevel(m_pLevel, "Generating level", nullptr);

	field_D9C = 1;
    
    hostMultiplayer();
	getScreenChooser()->pushProgressScreen();
}

const char* Minecraft::getProgressMessage()
{
	return progressMessages[field_DA0];
}

LevelStorageSource* Minecraft::getLevelSource()
{
	return m_pLevelStorageSource;
}

ItemStack& Minecraft::getSelectedItem()
{
	ItemStack& item = m_pLocalPlayer->getSelectedItem();

	if (item.isEmpty())
		return item;

	GameMode* pGameMode = getLocalPlayerGameMode();
	if (pGameMode && pGameMode->isCreativeType())
	{
		// Create new "unlimited" ItemStack for Creative mode
		m_CurrItemStack = ItemStack(item.getId(), 999, item.getAuxValue());
		return m_CurrItemStack;
	}

	return item;
}

ScreenChooser* Minecraft::getScreenChooser()
{
	if (!m_pScreenChooser || m_pScreenChooser->m_uiTheme != getOptions()->getUiTheme())
	{
		SAFE_DELETE(m_pScreenChooser);
		m_pScreenChooser = ScreenChooser::Create(this);
	}
	return m_pScreenChooser;
}

UITheme Minecraft::getUiTheme()
{
	return m_pScreen ? m_pScreen->m_uiTheme : getOptions()->getUiTheme();
}

Font* Minecraft::_createFont(const std::string& fileName) const
{
	assert(m_pOptions);

	switch (m_pOptions->getUiTheme())
	{
	case UI_CONSOLE:
		return new Font_Console(m_pOptions, fileName, m_pTextures);

	default:
		return new Font(m_pOptions, fileName, m_pTextures);
	}
}

void Minecraft::_reloadFontInternal()
{
	delete m_pFont;
	m_pFont = _createFont("font/default.png");
}

void Minecraft::reloadFont()
{
	if (!m_initialized)
		return;

	_reloadFontInternal();
}

void Minecraft::reloadFancy(bool isFancy)
{
}

int Minecraft::getFpsIntlCounter()
{
	return 0;
}

void Minecraft::leaveGame(bool bCopyMap)
{
	m_bPreparingLevel = false;

	if (m_pRakNetInstance)
		m_pRakNetInstance->disconnect();

	freeResources(bCopyMap);
}

void Minecraft::gotoMainMenu()
{
#if MC_PLATFORM_CONSOLE
	m_pSoundEngine->playMusic();
#else
	if (getOptions()->getUiTheme() == UI_CONSOLE)
		m_pSoundEngine->playMusic();
#endif
	getScreenChooser()->pushStartScreen();
}

void Minecraft::hostMultiplayer()
{
	if (!m_pRakNetInstance)
		return;

#ifdef FEATURE_NETWORKING
	m_pRakNetInstance->host(m_pUser->m_name, C_DEFAULT_PORT, C_MAX_CONNECTIONS);
	m_pNetEventCallback = new ServerSideNetworkHandler(this, m_pRakNetInstance);
#endif
}

void Minecraft::joinMultiplayer(const PingedCompatibleServer& serverInfo)
{
	if (!m_pRakNetInstance)
		return;

#ifdef FEATURE_NETWORKING
	if (field_18 && m_pNetEventCallback)
	{
		field_18 = false;
		m_pRakNetInstance->connect(serverInfo.m_address.ToString(), serverInfo.m_address.GetPort());
	}
#endif
}

void Minecraft::cancelLocateMultiplayer()
{
	if (!m_pRakNetInstance)
		return;

#ifdef FEATURE_NETWORKING
	field_18 = false;
	m_pRakNetInstance->stopPingForHosts();
	delete m_pNetEventCallback;
	m_pNetEventCallback = nullptr;
#endif
}

void Minecraft::locateMultiplayer()
{
	if (!m_pRakNetInstance)
		return;

#ifdef FEATURE_NETWORKING
	field_18 = true;
	m_pRakNetInstance->pingForHosts(C_DEFAULT_PORT);
	m_pNetEventCallback = new ClientSideNetworkHandler(this, m_pRakNetInstance);
#endif
}

void Minecraft::SetViewportSize(unsigned int widthP, unsigned int heightP)
{
	SetViewportSize(
		widthP,
		heightP,
		widthP  / GetRenderScaleMultiplier(),
		heightP / GetRenderScaleMultiplier()
	);
}

void Minecraft::SetViewportSize(unsigned int widthP, unsigned int heightP, unsigned int widthL, unsigned int heightL)
{
	Minecraft::_viewportSize.physical.width  = widthP;
	Minecraft::_viewportSize.physical.height = heightP;

	Minecraft::_viewportSize.logical.width   = widthL;
	Minecraft::_viewportSize.logical.height  = heightL;

	// recalculate the point to pixel scale.
	// This currently assumes that the aspect ratio is the same.
	SetRenderScaleMultiplier(float(widthP) / float(widthL));
}
