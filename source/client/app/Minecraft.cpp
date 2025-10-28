/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "client/app/Minecraft.hpp"
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

#include "client/player/input/Controller.hpp"
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

#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"

// custom:
#include "client/renderer/PatchManager.hpp"

float Minecraft::_renderScaleMultiplier = 1.0f;

int Minecraft::width  = C_DEFAULT_SCREEN_WIDTH;
int Minecraft::height = C_DEFAULT_SCREEN_HEIGHT;
bool Minecraft::useAmbientOcclusion = false;
int Minecraft::customDebugId = 0;

//@HUH: For the demo, this is defined as TRUE.
//@HUH: deadmau5 had camera cheats? That's interesting.
const bool Minecraft::DEADMAU5_CAMERA_CHEATS = true;

const char* Minecraft::progressMessages[] =
{
	"Locating server",
	"Building terrain",
	"Preparing",
	"Saving chunks",
};

Minecraft::Minecraft() :
	m_gui(this)
{
	m_options = nullptr;
	field_18 = false;
	m_bIsGamePaused = false;
	m_pLevelRenderer = nullptr;
	m_pGameRenderer = nullptr;
	m_pParticleEngine = nullptr;
	m_pSoundEngine = nullptr;
	m_pGameMode = nullptr;
	m_pTextures = nullptr;
	m_pFont = nullptr;
	m_pRakNetInstance = nullptr;
	m_pNetEventCallback = nullptr;
	field_2B0 = 0;
	m_pUser = nullptr;
	m_pLevel = nullptr;
	m_pLocalPlayer = nullptr;
	m_pMobPersp = nullptr; // why is there a duplicate?
	field_D0C = 0;
	m_pPrepThread = nullptr;
	m_pScreen = nullptr;
	field_D18 = 10;
	m_pInputHolder = nullptr;
	m_bGrabbedMouse = false; // this was true by default. why? we do not start off in-game...
	m_bIsTouchscreen = false;
	m_progressPercent = 0;
	m_bPreparingLevel = false;
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
}

Minecraft::~Minecraft()
{
	SAFE_DELETE(m_options);
	SAFE_DELETE(m_pNetEventCallback);
	SAFE_DELETE(m_pRakNetInstance);
	SAFE_DELETE(m_pLevelRenderer);
	SAFE_DELETE(m_pGameRenderer);
	SAFE_DELETE(m_pParticleEngine);
    SAFE_DELETE(EntityRenderDispatcher::instance);
	m_pSoundEngine->destroy();
	SAFE_DELETE(m_pSoundEngine);
	SAFE_DELETE(m_pGameMode);
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pTextures);
    
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

int Minecraft::getLicenseId()
{
	if (m_licenseID < 0)
		m_licenseID = m_pPlatform->checkLicense();

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
	if (!useController() && !isTouchscreen())
		platform()->recenterMouse(); // don't actually try to grab or release the mouse
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

	if (useController() || isTouchscreen())
		return; // don't actually try to grab the mouse

	platform()->setMouseGrabbed(true);
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

	if (pScreen && pScreen->isErrorScreen())
	{
		// not in original
		delete pScreen;
		return;
	}

	if (m_pScreen)
	{
		m_pScreen->removed();
		delete m_pScreen;
	}

	Mouse::reset();
	Multitouch::reset();
	Controller::reset();
	Multitouch::resetThisUpdate();

	m_pScreen = pScreen;
	if (pScreen)
	{
		releaseMouse();
		// the ceil prevents under-drawing
		pScreen->init(this, ceil(width * Gui::InvGuiScale), ceil(height * Gui::InvGuiScale));

		if (pScreen->isPauseScreen())
		{
			if (m_pLevel && isLevelGenerated())
				return m_pLevel->saveGame();
		}
	}
	else
	{
		platform()->recenterMouse();
		grabMouse();
	}
}

void Minecraft::onGraphicsReset()
{
	m_pTextures->clear();
	_initTextures();
	m_pFont->onGraphicsReset();

	if (m_pLevelRenderer)
		m_pLevelRenderer->onGraphicsReset();

	if (m_pGameRenderer)
		m_pGameRenderer->onGraphicsReset();

	EntityRenderDispatcher::getInstance()->onGraphicsReset();
}

void Minecraft::saveOptions()
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

bool Minecraft::isTouchscreen() const
{
	return m_bIsTouchscreen;
}

bool Minecraft::useSplitControls() const
{
	return !m_bIsTouchscreen || m_options->m_bSplitControls;
}

bool Minecraft::useController() const
{
	return m_pPlatform->hasGamepad() && m_options->m_bUseController;
}

GameMode* Minecraft::createGameMode(GameType gameType, Level& level)
{
	switch (gameType)
	{
	case GAME_TYPE_SURVIVAL:
		return new SurvivalMode(this, level);
	case GAME_TYPE_CREATIVE:
		return new CreativeMode(this, level);
	default:
		return nullptr;
	}
}

void Minecraft::setGameMode(GameType gameType)
{
	if (m_pLevel)
	{
        SAFE_DELETE(m_pGameMode);
		m_pGameMode = createGameMode(gameType, *m_pLevel);
		m_pGameMode->initLevel(m_pLevel);
	}
}

void Minecraft::handleBuildAction(const BuildActionIntention& action)
{
	LocalPlayer* player = m_pLocalPlayer;
	bool canInteract = getTimeMs() - m_lastInteractTime >= 200;
	// This logic is present in 0.9.0, but just does not make any sense being here.
	//if (player->isUsingItem()) return;

	if (action.isDestroyStart() || action.isAttack())
	{
		player->swing();
	}
	
	if (!action.isDestroy() && !m_pGameMode->field_8) // from Minecraft::handleMouseDown
	{
		m_pGameMode->stopDestroyBlock();
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
				m_pRakNetInstance->send(new InteractPacket(player->m_EntityID, pTarget->m_EntityID, InteractPacket::ATTACK));
				m_pGameMode->attack(player, pTarget);
				m_lastBlockBreakTime = getTimeMs();
			}
			else if (action.isInteract() && canInteract)
			{
				if (pTarget->interactPreventDefault())
					bInteract = false;

				m_pRakNetInstance->send(new InteractPacket(player->m_EntityID, pTarget->m_EntityID, InteractPacket::INTERACT));
				m_pGameMode->interact(player, pTarget);
				m_lastInteractTime = getTimeMs();
			}
			break;
		}
		case HitResult::TILE:
		{
			Tile* pTile = Tile::tiles[m_pLevel->getTile(m_hitResult.m_tilePos)];

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

				if (pTile != Tile::unbreakable || (player->field_B94 >= 100 && !m_hitResult.m_bUnk24))
				{
					bool destroyed = false;
					if (action.isDestroyStart())
					{
						destroyed = m_pGameMode->startDestroyBlock(player, m_hitResult.m_tilePos, m_hitResult.m_hitSide);
						player->startDestroying();
					}

					bool contDestory = m_pGameMode->continueDestroyBlock(player, m_hitResult.m_tilePos, m_hitResult.m_hitSide);

					destroyed = destroyed || contDestory;
					m_pParticleEngine->crack(m_hitResult.m_tilePos, m_hitResult.m_hitSide);

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
				int auxValue = m_pLevel->getData(m_hitResult.m_tilePos);
				player->m_pInventory->selectItemByIdAux(pTile->m_ID, auxValue, C_MAX_HOTBAR_ITEMS);
			}
			else if (action.isPlace() && canInteract)
			{
				ItemInstance* pItem = getSelectedItem();
				if (m_pGameMode->useItemOn(player, m_pLevel, pItem, m_hitResult.m_tilePos, m_hitResult.m_hitSide))
				{
					bInteract = false;

					player->swing();

					m_lastInteractTime = getTimeMs();

					if (isOnline())
					{
						if (ItemInstance::isNull(pItem) || !pItem->getTile())
							return;

						TilePos tp(m_hitResult.m_tilePos);

						Facing::Name hitSide = m_hitResult.m_hitSide;

						if (m_pLevel->getTile(m_hitResult.m_tilePos) == Tile::topSnow->m_ID)
						{
							hitSide = Facing::DOWN;
						}

						m_pRakNetInstance->send(new PlaceBlockPacket(player->m_EntityID, tp.relative(hitSide, 1), (TileID)pItem->getId(), hitSide, pItem->getAuxValue()));
					}
				}
			}
			break;
		}
	}

	if (bInteract && action.isInteract() && canInteract)
	{
		ItemInstance* pItem = getSelectedItem();
		if (pItem && player->isUsingItem())
		{
			m_lastInteractTime = getTimeMs();
			if (m_pGameMode->useItem(player, m_pLevel, pItem))
				m_pGameRenderer->m_pItemInHandRenderer->itemUsed();
		}
	}
}

void Minecraft::tickInput()
{
	if (m_pScreen)
	{
		if (!m_pScreen->field_10)
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

	if (!m_pLocalPlayer)
		return;

	bool bIsInGUI = m_gui.isInside(Mouse::getX(), Mouse::getY());

	while (Mouse::next())
	{
		if (getTimeMs() - field_2B4 > 200)
			continue;

		if (Mouse::isButtonDown(BUTTON_LEFT))
		{
			// @HACK: on SDL1, we don't recenter the mouse every tick, meaning the user can
			// unintentionally click the hotbar while swinging their fist
			if (platform()->getRecenterMouseEveryTick() || m_pScreen)
				m_gui.handleClick(1, Mouse::getX(), Mouse::getY());
		}

		MouseButtonType buttonType = Mouse::getEventButton();
		bool bPressed = Mouse::getEventButtonState() == true;

#ifdef ENH_ALLOW_SCROLL_WHEEL
		if (buttonType == BUTTON_SCROLLWHEEL)
			m_gui.handleScroll(bPressed);
#endif
	}

	while (Keyboard::next())
	{
		int keyCode = Keyboard::getEventKey();
		bool bPressed = Keyboard::getEventKeyState() == 1;

		m_pLocalPlayer->m_pMoveInput->setKey(keyCode, bPressed);

		if (bPressed)
		{
			m_gui.handleKeyPressed(keyCode);

			for (int i = 0; i < m_gui.getNumUsableSlots(); i++)
			{
				if (getOptions()->isKey(eKeyMappingIndex(KM_SLOT_1 + i), keyCode))
					m_pLocalPlayer->m_pInventory->selectSlot(i);
			}

			if (getOptions()->isKey(KM_TOGGLE3RD, keyCode))
			{
				getOptions()->m_bThirdPerson = !getOptions()->m_bThirdPerson;
			}
			else if (getOptions()->isKey(KM_MENU_CANCEL, keyCode))
			{
				handleBack(false);
			}
			else if (getOptions()->isKey(KM_DROP, keyCode))
			{
				ItemInstance *item = m_pLocalPlayer->m_pInventory->getSelected();
				if (!ItemInstance::isNull(item) && item->m_count > 0)
				{
					ItemInstance itemDrop(*item);
					itemDrop.m_count = 1;

					if (m_pLocalPlayer->isSurvival())
						item->remove(1);

					m_pLocalPlayer->drop(itemDrop);
				}
			}
			else if (getOptions()->isKey(KM_TOGGLEGUI, keyCode))
			{
				getOptions()->m_bDontRenderGui = !getOptions()->m_bDontRenderGui;
			}
			else if (getOptions()->isKey(KM_TOGGLEDEBUG, keyCode))
			{
				getOptions()->m_bDebugText = !getOptions()->m_bDebugText;
			}
#ifdef ENH_ALLOW_AO_TOGGLE
			else if (getOptions()->isKey(KM_TOGGLEAO, keyCode))
			{
				// Toggle ambient occlusion.
				getOptions()->m_bAmbientOcclusion = !getOptions()->m_bAmbientOcclusion;
				Minecraft::useAmbientOcclusion = getOptions()->m_bAmbientOcclusion;
				m_pLevelRenderer->allChanged();
			}
#endif
		}

		if (getOptions()->field_19)
			continue;

		// @TODO: Replace with KeyboardBuildInput
		if (!useController() && getTimeMs() - field_2B4 <= 200)
		{
			if (getOptions()->getKey(KM_DESTROY) == keyCode && bPressed)
			{
				BuildActionIntention intention(BuildActionIntention::KEY_DESTROY);
				handleBuildAction(intention);
			}

			if (getOptions()->getKey(KM_PLACE) == keyCode && bPressed)
			{
				BuildActionIntention intention(BuildActionIntention::KEY_USE);
				handleBuildAction(intention);
			}
		}
	}

	BuildActionIntention bai;
	IBuildInput* buildInput = m_pInputHolder->getBuildInput();
	if (buildInput && buildInput->tickBuild(m_pLocalPlayer, &bai))
		handleBuildAction(bai);

	field_2B4 = getTimeMs();

	Keyboard::reset();
	Mouse::reset();
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

	if (useController() || isTouchscreen())
		return; // don't actually try to recenter the mouse

    if (platform()->getRecenterMouseEveryTick()) // just for SDL1
        platform()->recenterMouse();
}

void Minecraft::handleCharInput(char chr)
{
	if (m_pScreen)
		m_pScreen->keyboardNewChar(chr);
}

void Minecraft::resetInput()
{
	Keyboard::reset();
	Mouse::reset();
	Controller::reset();
	Multitouch::resetThisUpdate();
}

void Minecraft::sendMessage(const std::string& message)
{
	if (isOnlineClient())
	{
		// send the server a message packet
		if (m_pRakNetInstance)
			m_pRakNetInstance->send(new MessagePacket(message));
		else
			m_gui.addMessage("You aren't actually playing multiplayer!");
	}
	else
	{
		// fake the server having received a packet
		MessagePacket mp(message);
		if (m_pNetEventCallback && m_pRakNetInstance)
			m_pNetEventCallback->handle(m_pRakNetInstance->m_pRakPeerInterface->GetMyGUID(), &mp);
		else
			m_gui.addMessage("You aren't hosting a multiplayer server!");
	}
}

void Minecraft::respawnPlayer()
{
	_resetPlayer(m_pLocalPlayer);

	// Allows client to dictate respawn position. Why?
	m_pRakNetInstance->send(new RespawnPacket(m_pLocalPlayer->m_EntityID, m_pLocalPlayer->m_pos));
}

std::string Minecraft::getVersionString(const std::string& str) const
{
	return "v0.2.0" + str + " alpha";
}

void Minecraft::_reloadInput()
{
	if (m_pInputHolder)
		delete m_pInputHolder;

	if (isTouchscreen())
	{
		m_pInputHolder = new TouchInputHolder(this, m_options);
	}
	else if (useController())
	{
		m_pInputHolder = new CustomInputHolder(
			new ControllerMoveInput(m_options),
			new ControllerTurnInput(),
			new ControllerBuildInput()
		);
	}
	else
	{
		m_pInputHolder = new CustomInputHolder(
			new KeyboardInput(m_options),
			new MouseTurnInput(this),
			new MouseBuildInput()
		);
	}

	m_mouseHandler.setTurnInput(m_pInputHolder->getTurnInput());

	if (m_pLevel && m_pLocalPlayer)
	{
		m_pLocalPlayer->m_pMoveInput = m_pInputHolder->getMoveInput();
	}

	m_options->field_19 = !isTouchscreen();
}

void Minecraft::_levelGenerated()
{
	if (m_pNetEventCallback)
		m_pNetEventCallback->levelGenerated(m_pLevel);
}

void Minecraft::_initTextures()
{
	m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
	GetPatchManager()->PatchTextures(platform(), TYPE_TERRAIN);
	m_pTextures->loadAndBindTexture(C_ITEMS_NAME);
	GetPatchManager()->PatchTextures(platform(), TYPE_ITEMS);
	
	GetPatchManager()->PatchTiles();	
}

void Minecraft::_resetPlayer(Player* player)
{
	m_pLevel->validateSpawn();
	player->reset();

	TilePos pos = m_pLevel->getSharedSpawnPos();
	player->setPos(pos);
	player->resetPos();
}

void Minecraft::tick()
{
	if (!m_pScreen)
	{
		if (m_pLocalPlayer && m_pLocalPlayer->m_health <= 0)
		{
			setScreen(new DeathScreen);
		}
	}

	tickInput();

	m_gui.tick();

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
            m_pLevel->m_difficulty = m_options->m_difficulty;
            if (m_pLevel->m_bIsClientSide)
            {
                m_pLevel->m_difficulty = 3;
            }
            
			m_pGameMode->tick();
			m_pGameRenderer->tick();
			m_pLevelRenderer->tick();
			m_pLevel->tickEntities();
			m_pLevel->tick();

			if (m_pLocalPlayer)
			{
				m_pLevel->animateTick(m_pLocalPlayer->m_pos);
			}
		}

		m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

		if (!isGamePaused())
		{
			m_pTextures->tick();
			m_pParticleEngine->tick();

#ifndef ORIGINAL_CODE
			m_pSoundEngine->update(m_pMobPersp, m_timer.m_renderTicks);
#endif
		}

		if (m_pScreen)
			m_pScreen->tick();

		Multitouch::reset();
	}
}

void Minecraft::update()
{
	if (isGamePaused() && m_pLevel)
	{
		// Don't advance renderTicks when we're paused
		float x = m_timer.m_renderTicks;
		m_timer.advanceTime();
		m_timer.m_renderTicks = x;
	}
	else
	{
		m_timer.advanceTime();
	}

	if (m_pRakNetInstance)
	{
		m_pRakNetInstance->runEvents(*m_pNetEventCallback);
	}

	for (int i = 0; i < m_timer.m_ticks; i++)
	{
		// @BUG?: Minecraft::tick() also calls tickInput(), so we're doing input handling potentially more than once
		tick(); // tick(i, m_timer.m_ticks - 1); // 0.9.2
		field_DA8++;
	}

	if (m_pLevel && !m_bPreparingLevel)
	{
		m_pLevel->updateLights();
	}

#ifndef ORIGINAL_CODE
	tickMouse();
#endif

	m_pGameRenderer->render(m_timer.m_renderTicks);

	// Added by iProgramInCpp
	if (m_pGameMode)
		m_pGameMode->render(m_timer.m_renderTicks);

	double time = getTimeS();
	m_fDeltaTime = time - m_fLastUpdated;
	m_fLastUpdated = time;
}

void Minecraft::init()
{
	// Optional features that you really should be able to get away with not including.
	Screen::setIsMenuPanoramaAvailable(platform()->doesTextureExist("gui/background/panorama_0.png"));
	LevelRenderer::setAreCloudsAvailable(platform()->doesTextureExist("environment/clouds.png"));
	LevelRenderer::setArePlanetsAvailable(platform()->doesTextureExist("terrain/sun.png") && platform()->doesTextureExist("terrain/moon.png"));
	GrassColor::setIsAvailable(platform()->doesTextureExist("misc/grasscolor.png"));
	FoliageColor::setIsAvailable(platform()->doesTextureExist("misc/foliagecolor.png"));
	Gui::setIsVignetteAvailable(platform()->doesTextureExist("misc/vignette.png"));
	EntityRenderer::setAreShadowsAvailable(platform()->doesTextureExist("misc/shadow.png"));

	GetPatchManager()->LoadPatchData(platform()->getPatchData());

	m_bIsTouchscreen = platform()->isTouchscreen();

	m_pRakNetInstance = new RakNetInstance;

	m_pTextures = new Textures(m_options, platform());
	m_pTextures->addDynamicTexture(new WaterTexture);
	m_pTextures->addDynamicTexture(new WaterSideTexture);
	m_pTextures->addDynamicTexture(new LavaTexture);
	m_pTextures->addDynamicTexture(new LavaSideTexture);
	m_pTextures->addDynamicTexture(new FireTexture(0));
	m_pTextures->addDynamicTexture(new FireTexture(1));

	if (platform()->hasFileSystemAccess())
		m_options = new Options(m_externalStorageDir);
	else
		m_options = new Options();

	m_options->m_bUseController = platform()->hasGamepad();
	m_options->loadControls();

	_reloadInput();
	_initTextures();

	m_pSoundEngine = new SoundEngine(platform()->getSoundSystem(), 20.0f); // 20.0f on 0.7.0
	m_pSoundEngine->init(m_options, platform());

	m_pLevelRenderer = new LevelRenderer(this, m_pTextures);
	m_pGameRenderer = new GameRenderer(this);
	m_pParticleEngine = new ParticleEngine(m_pLevel, m_pTextures);
	m_pUser = new User(getOptions()->m_playerName, "");

	// "Default.png" for the launch image overwrites "default.png" for the font during app packaging
	m_pFont = new Font(m_options, "font/default8.png", m_pTextures);

	if (GrassColor::isAvailable())
	{
		GrassColor::init(m_pPlatform->loadTexture("misc/grasscolor.png", true));
	}
	if (FoliageColor::isAvailable())
	{
		FoliageColor::init(m_pPlatform->loadTexture("misc/foliagecolor.png", true));
	}
}

void Minecraft::prepareLevel(const std::string& unused)
{
	field_DA0 = 1;

	float startTime = float(getTimeS());
	Level* pLevel = m_pLevel;

	if (!pLevel->field_B0C)
	{
		pLevel->setUpdateLights(0);
	}

	for (int i = 8, i2 = 0; i != 8 + C_MAX_CHUNKS_X * 16; i += 16)
	{
		for (int j = 8; j != 8 + C_MAX_CHUNKS_Z * 16; j += 16, i2 += 100)
		{
			// this looks like some kind of progress tracking
			m_progressPercent = i2 / (C_MAX_CHUNKS_X * C_MAX_CHUNKS_Z);

			float time1 = float(getTimeS());

			// generating all the chunks at once
			(void)pLevel->getTile(TilePos(i, (C_MAX_Y + C_MIN_Y) / 2, j));

			if (time1 != -1.0f)
				getTimeS();

			float time2 = getTimeS();
			if (pLevel->field_B0C)
			{
				while (pLevel->updateLights());
			}

			if (time2 != -1.0f)
				getTimeS();
		}
	}

	if (startTime != -1.0f)
		getTimeS();

	pLevel->setUpdateLights(1);

	startTime = float(getTimeS());

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

	if (startTime != -1.0f)
		getTimeS();

	field_DA0 = 3;

	if (pLevel->field_B0C)
	{
		pLevel->setInitialSpawn();
		pLevel->saveLevelData();
		pLevel->getChunkSource()->saveAll();
		pLevel->saveGame();
	}
	else
	{
		pLevel->saveLevelData();
		pLevel->loadEntities();
	}

	m_progressPercent = -1;
	field_DA0 = 2;

	startTime = float(getTimeS());

	pLevel->prepare();

	if (startTime != -1.0f)
		getTimeS();

	// These strings are initialized and then removed quickly afterwards. Probably some debug leftover
	// "Generate level:";
	// " - light: ";
	// " - getTl: ";
	// " - clear: ";
	// " - prepr: ";
}

void Minecraft::sizeUpdate(int newWidth, int newHeight)
{
	// re-calculate the GUI scale.
	Gui::InvGuiScale = getBestScaleForThisScreenSize(newWidth, newHeight) / getRenderScaleMultiplier();

	// The ceil gives an extra pixel to the screen's width and height, in case the GUI scale doesn't
	// divide evenly into width or height, so that none of the game screen is uncovered.
	if (m_pScreen)
		m_pScreen->setSize(
			int(ceilf(Minecraft::width * Gui::InvGuiScale)),
			int(ceilf(Minecraft::height * Gui::InvGuiScale))
		);

	if (m_pInputHolder)
		m_pInputHolder->setScreenSize(Minecraft::width, Minecraft::height);
}

float Minecraft::getBestScaleForThisScreenSize(int width, int height)
{
//#define USE_JAVA_SCREEN_SCALING
#ifdef USE_JAVA_SCREEN_SCALING
	int scale;
	for (scale = 1; width / (scale + 1) >= 320 && height / (scale + 1) >= 240; ++scale)
	{
	}
	return 1.0f / scale;
#endif

	if (height > 1800)
		return 1.0f / 8.0f;

	if (isTouchscreen())
	{
		if (height > 1100)
			return 1.0f / 6.0f;

		if (height > 900)
			return 1.0f / 5.0f;

		if (height > 700)
			return 1.0f / 4.0f;

		if (height > 500)
			return 1.0f / 3.0f;

		if (height > 300)
			return 1.0f / 2.0f;
	}
	else
	{
		if (height > 1600)
			return 1.0f / 4.0f;

		if (height > 800)
			return 1.0f / 3.0f;

		if (height > 400)
			return 1.0f / 2.0f;
	}

	return 1.0f;
}

void Minecraft::generateLevel(const std::string& unused, Level* pLevel)
{
	float time = float(getTimeS()); //@UNUSED

	prepareLevel(unused);

	if (time != -1.0f)
		getTimeS(); //@QUIRK: unused return value

	// std::string("Level generated: "); //@QUIRK: unused string instance

	LocalPlayer* pLocalPlayer = m_pLocalPlayer;
	if (!pLocalPlayer)
	{
		pLocalPlayer = m_pGameMode->createPlayer(pLevel);
		m_pLocalPlayer = pLocalPlayer;
		m_pGameMode->initPlayer(pLocalPlayer);
	}

	if (pLocalPlayer)
		pLocalPlayer->m_pMoveInput = m_pInputHolder->getMoveInput();

	if (m_pLevelRenderer)
		m_pLevelRenderer->setLevel(pLevel);

	if (m_pParticleEngine)
		m_pParticleEngine->setLevel(pLevel);

	m_pGameMode->adjustPlayer(m_pLocalPlayer);

	// was after loadPlayer for some reason
	if (m_pLocalPlayer)
		m_pLocalPlayer->resetPos();

	pLevel->validateSpawn();
	pLevel->loadPlayer(*m_pLocalPlayer);

	m_pMobPersp = m_pLocalPlayer;
	m_pLevel = pLevel;

	m_bPreparingLevel = false;

	if (m_pRakNetInstance && m_pRakNetInstance->m_bIsHost)
		m_pRakNetInstance->announceServer(m_pUser->field_0);
}

void* Minecraft::prepareLevel_tspawn(void* ptr)
{
	Minecraft* pMinecraft = (Minecraft*)ptr;

	pMinecraft->generateLevel("Currently not used", pMinecraft->m_pLevel);

	return nullptr;
}

bool Minecraft::pauseGame()
{
	if (isGamePaused() || m_pScreen) return false;

	if (!isOnline())
	{
		// Actually pause the game, because fuck bedrock edition
		m_bIsGamePaused = true;
	}
	m_pLevel->savePlayerData();
	setScreen(new PauseScreen);

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
	m_pMobPersp = nullptr;

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

		if (m_pLocalPlayer)
			m_pLocalPlayer->resetPos();

		m_pLevel = pLevel;
		m_bPreparingLevel = true;
		m_pPrepThread = new CThread(&Minecraft::prepareLevel_tspawn, this);

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
    if (ls.m_storageVersion != LEVEL_STORAGE_VERSION_DEFAULT && !forceConversion)
    {
        setScreen(new ConvertWorldScreen(ls));
        return;
    }
    
    selectLevel(ls.m_fileName, ls.m_levelName, LevelSettings(), forceConversion);
}

void Minecraft::selectLevel(const std::string& levelDir, const std::string& levelName, const LevelSettings& levelSettings, bool forceConversion)
{
	LevelStorage* pStor = m_pLevelStorageSource->selectLevel(levelDir, false, forceConversion);
	Dimension* pDim = Dimension::getNew(0);

	m_pLevel = new Level(pStor, levelName, levelSettings, LEVEL_STORAGE_VERSION_DEFAULT, pDim);
	setLevel(m_pLevel, "Generating level", nullptr);

	field_D9C = 1;
    
    hostMultiplayer();
    setScreen(new ProgressScreen);
}

const char* Minecraft::getProgressMessage()
{
	return progressMessages[field_DA0];
}

LevelStorageSource* Minecraft::getLevelSource()
{
	return m_pLevelStorageSource;
}

ItemInstance* Minecraft::getSelectedItem()
{
	ItemInstance* pInst = m_pLocalPlayer->getSelectedItem();

	if (ItemInstance::isNull(pInst))
		return nullptr;

	if (m_pGameMode->isCreativeType())
	{
		// Create new "unlimited" ItemInstance for Creative mode
		m_CurrItemInstance = ItemInstance(pInst->getId(), 999, pInst->getAuxValue());
		return &m_CurrItemInstance;
	}

	return pInst;
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
	m_pMobPersp = nullptr;
	m_pLevelRenderer->setLevel(nullptr);
	m_pParticleEngine->setLevel(nullptr);

#ifndef ORIGINAL_CODE
	// @BUG: Deleting ServerSideNetworkHandler too late! This causes
	// access to invalid memory in the destructor seeing as we already deleted the level.
	delete m_pNetEventCallback;
#endif

#ifdef ENH_IMPROVED_SAVING
	m_bIsGamePaused = true;
	setScreen(new SavingWorldScreen(bCopyMap/*, m_pLocalPlayer*/));
#else
	if (m_pLevel)
	{
		LevelStorage* pStorage = m_pLevel->getLevelStorage();
		SAFE_DELETE(pStorage);
		SAFE_DELETE(m_pLevel);

		m_pLevel = nullptr;
	}
#endif

#ifdef ORIGINAL_CODE
	delete m_pNetEventCallback;
#endif

	m_pLocalPlayer = nullptr;
	m_pNetEventCallback = nullptr;
	field_D9C = 0;

#ifndef ENH_IMPROVED_SAVING
	// this is safe to do, since on destruction, we don't actually delete it.
	SAFE_DELETE(m_pLocalPlayer);

	if (bCopyMap)
		setScreen(new RenameMPLevelScreen("_LastJoinedServer"));
	else
		setScreen(new StartMenuScreen);
#endif
}

void Minecraft::hostMultiplayer()
{
	if (!m_pRakNetInstance)
		return;

#ifndef __EMSCRIPTEN__
	m_pRakNetInstance->host(m_pUser->field_0, C_DEFAULT_PORT, C_MAX_CONNECTIONS);
	m_pNetEventCallback = new ServerSideNetworkHandler(this, m_pRakNetInstance);
#endif
}

void Minecraft::joinMultiplayer(const PingedCompatibleServer& serverInfo)
{
	if (!m_pRakNetInstance)
		return;

#ifndef __EMSCRIPTEN__
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

#ifndef __EMSCRIPTEN__
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

#ifndef __EMSCRIPTEN__
	field_18 = true;
	m_pRakNetInstance->pingForHosts(C_DEFAULT_PORT);
	m_pNetEventCallback = new ClientSideNetworkHandler(this, m_pRakNetInstance);
#endif
}
