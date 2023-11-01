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
#include "network/ServerSideNetworkHandler.hpp"
#include "client/network/ClientSideNetworkHandler.hpp"

#include "world/gamemode/SurvivalMode.hpp"
#include "world/gamemode/CreativeMode.hpp"

#include "client/player/input/ControllerTurnInput.hpp"
#include "client/player/input/MouseTurnInput.hpp"
#include "client/player/input/KeyboardInput.hpp"
#include "client/player/input/IBuildInput.hpp"
#include "client/player/input/CustomInputHolder.hpp"
#include "client/player/input/TouchInputHolder.hpp"
#include "client/player/input/Multitouch.hpp"

#include "world/tile/SandTile.hpp"

// custom:
#include "client/renderer/PatchManager.hpp"

int Minecraft::width  = C_DEFAULT_SCREEN_WIDTH;
int Minecraft::height = C_DEFAULT_SCREEN_HEIGHT;
float Minecraft::guiScaleMultiplier = 1.0f;
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
	field_288 = false;
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
	m_bGrabbedMouse = true;
	m_progressPercent = 0;
	m_bPreparingLevel = false;
	m_pLevelStorageSource = nullptr; // TODO
	field_D9C = 0;
	field_DA0 = 0;
	field_DA4 = 0;
	field_DA8 = 0;
	field_DAC = 0;
	m_bUsingScreen = false;
	m_bHasQueuedScreen = false;
	m_pQueuedScreen = nullptr;
	m_licenseID = -2;
	m_fLastUpdated = 0;
	m_fDeltaTime = 0;

	m_Logger = new Logger();
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
	platform()->setMouseGrabbed(false);
}

void Minecraft::grabMouse()
{
	if (m_bGrabbedMouse)
		return;

	m_bGrabbedMouse = true;
	m_mouseHandler.grab();

	setScreen(nullptr);

	platform()->setMouseGrabbed(!isTouchscreen());
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

	m_pScreen = pScreen;
	if (pScreen)
	{
		releaseMouse();
		pScreen->init(this, int(width * Gui::InvGuiScale), int(height * Gui::InvGuiScale));
	}
	else
	{
		grabMouse();
	}
}

void Minecraft::onGraphicsReset()
{
	m_pTextures->clear();
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

bool Minecraft::isLevelGenerated()
{
	if (m_pLevel)
		return !m_bPreparingLevel;

	return false;
}

bool Minecraft::isOnline()
{
	return m_pNetEventCallback != nullptr;
}

bool Minecraft::isOnlineClient()
{
	if (!m_pLevel)
		return false;

	return m_pLevel->m_bIsMultiplayer;
}

bool Minecraft::isTouchscreen()
{
	return m_bIsTouchscreen;
}

void Minecraft::setGuiScaleMultiplier(float f)
{
	guiScaleMultiplier = f;
}

void Minecraft::handleMouseDown(int type, bool b)
{
	if (!m_pGameMode->field_8 && (type != 1 || this->field_DA4 <= 0))
	{
		if (b && type == 1 && m_hitResult.m_hitType == HitResult::AABB && !m_hitResult.m_bUnk24)
		{
			m_pGameMode->continueDestroyBlock(m_hitResult.m_tileX, m_hitResult.m_tileY, m_hitResult.m_tileZ, m_hitResult.m_hitSide);
			m_pParticleEngine->crack(m_hitResult.m_tileX, m_hitResult.m_tileY, m_hitResult.m_tileZ, m_hitResult.m_hitSide);
		}
		else
		{
			m_pGameMode->stopDestroyBlock();
		}
	}
}

void Minecraft::handleBuildAction(BuildActionIntention* pAction)
{
	if (pAction->isRemove())
	{
		if (field_DA4 > 0)
			return;

		m_pLocalPlayer->swing();
	}

	bool bInteract = true;
	if (!m_hitResult.isHit())
	{
		if (pAction->isRemove() && !m_pGameMode->isCreativeType())
			field_DA4 = 10;
	}
	else if (m_hitResult.m_hitType == HitResult::ENTITY)
	{
		if (pAction->isAttack())
		{
			m_pGameMode->attack(m_pLocalPlayer, m_hitResult.m_pEnt);
		}
		else if (pAction->isInteract())
		{
			if (m_hitResult.m_pEnt->interactPreventDefault())
				bInteract = false;

			m_pGameMode->interact(m_pLocalPlayer, m_hitResult.m_pEnt);
		}
	}
	else if (m_hitResult.m_hitType == HitResult::AABB)
	{
		Tile* pTile = Tile::tiles[m_pLevel->getTile(m_hitResult.m_tileX, m_hitResult.m_tileY, m_hitResult.m_tileZ)];

		if (pAction->isRemove())
		{
			if (!pTile)
				return;

			// @BUG: This is only done on the client side.
			m_pLevel->extinguishFire(m_hitResult.m_tileX, m_hitResult.m_tileY, m_hitResult.m_tileZ, m_hitResult.m_hitSide);

			if (pTile != Tile::unbreakable || (m_pLocalPlayer->field_B94 > 99 && m_hitResult.m_bUnk24 != 1))
			{
				m_pGameMode->startDestroyBlock(m_hitResult.m_tileX, m_hitResult.m_tileY, m_hitResult.m_tileZ, m_hitResult.m_hitSide);
			}
		}
		else
		{
			ItemInstance* pItem = getSelectedItem();
			if (m_pGameMode->useItemOn(
					m_pLocalPlayer,
					m_pLevel,
					pItem->m_itemID <= 0 ? nullptr : pItem,
					m_hitResult.m_tileX,
					m_hitResult.m_tileY,
					m_hitResult.m_tileZ,
					m_hitResult.m_hitSide))
			{
				bInteract = false;

				m_pLocalPlayer->swing();

				if (isOnline())
				{
					if (pItem->m_itemID > C_MAX_TILES || pItem->m_itemID < 0)
						return;

					int dx = m_hitResult.m_tileX, dz = m_hitResult.m_tileZ;
					uint8_t dy = uint8_t(m_hitResult.m_tileY);

					uint8_t hitSide = m_hitResult.m_hitSide;

					if (m_pLevel->getTile(m_hitResult.m_tileX, m_hitResult.m_tileY, m_hitResult.m_tileZ) != Tile::topSnow->m_ID)
					{
						switch (m_hitResult.m_hitSide)
						{
							case HitResult::NOHIT: break;
							case HitResult::MINY: dy--; break;
							case HitResult::MAXY: dy++; break;
							case HitResult::MINZ: dz--; break;
							case HitResult::MAXZ: dz++; break;
							case HitResult::MINX: dx--; break;
							case HitResult::MAXX: dx++; break;
						}
					}
					else
					{
						hitSide = HitResult::MINY;
					}

					m_pRakNetInstance->send(new PlaceBlockPacket(m_pLocalPlayer->m_EntityID, dx, dy, dz, uint8_t(pItem->m_itemID), hitSide));
				}
			}
		}
	}

	if (bInteract && pAction->isInteract())
	{
		ItemInstance* pItem = getSelectedItem();
		if (pItem)
		{
			if (m_pGameMode->useItem(m_pLocalPlayer, m_pLevel, pItem))
				m_pGameRenderer->m_pItemInHandRenderer->itemUsed();
		}
	}
}

void Minecraft::handleMouseClick(int type)
{
	if (!isTouchscreen())
	{
		if (type == 1)
		{
			BuildActionIntention bai(INTENT_HELD);
			handleBuildAction(&bai);
		}
		if (type == 2)
		{
			BuildActionIntention bai(INTENT_CLICKED);
			handleBuildAction(&bai);
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
			m_gui.handleClick(1, Mouse::getX(), Mouse::getY());

		if (!bIsInGUI && getOptions()->field_19)
		{
			if (Mouse::getEventButton() == BUTTON_LEFT && Mouse::getEventButtonState())
			{
				handleMouseClick(1);
				field_DAC = field_DA8;
			}
			if (Mouse::getEventButton() == BUTTON_RIGHT && Mouse::getEventButtonState())
			{
				handleMouseClick(2);
				field_DAC = field_DA8;
			}
#ifdef ENH_ALLOW_SCROLL_WHEEL
			if (Mouse::getEventButton() == BUTTON_SCROLLWHEEL)
			{
				int slot = m_pLocalPlayer->m_pInventory->m_SelectedHotbarSlot;

				int maxItems = m_gui.getNumSlots() - 1;
				if (isTouchscreen())
					maxItems--;

				if (Mouse::getEventButtonState() == 0) // @NOTE: Scroll up
				{
					if (slot-- == 0)
					{
						slot = maxItems;
					}
				}
				else
				{
					if (slot++ == maxItems) // @NOTE: Scroll down
					{
						slot = 0;
					}
				}

				m_pLocalPlayer->m_pInventory->selectSlot(slot);
			}
#endif
		}
	}

	while (Keyboard::next())
	{
		int keyCode = Keyboard::getEventKey();
		bool bPressed = Keyboard::getEventKeyState() == 1;

		m_pLocalPlayer->m_pMoveInput->setKey(keyCode, bPressed);

		if (bPressed)
		{
			m_gui.handleKeyPressed(keyCode);

			for (int i = 0; i < m_gui.getNumSlots(); i++)
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
				pauseGame();
			}
			else if (getOptions()->isKey(KM_DROP, keyCode))
			{
				int itemID = m_pLocalPlayer->m_pInventory->getSelectedItemId();
				if (itemID > 0)
				{
					ItemInstance inst(itemID, 1, 0);
					m_pLocalPlayer->drop(&inst);
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
		#ifdef ENH_ALLOW_AO
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

		if (getTimeMs() - field_2B4 <= 200)
		{
			if (getOptions()->getKey(KM_DESTROY) == keyCode && bPressed)
			{
				BuildActionIntention intention(INTENT_HELD);
				handleBuildAction(&intention);
			}

			if (getOptions()->getKey(KM_PLACE) == keyCode && bPressed)
			{
				BuildActionIntention intention(INTENT_CLICKED);
				handleBuildAction(&intention);
			}
		}
	}

	BuildActionIntention bai;
	bool b = m_pInputHolder->getBuildInput()->tickBuild(m_pLocalPlayer, &bai);

	if (b && !bai.isRemoveContinue())
		handleBuildAction(&bai);
	
	bool flag =
		// If we are mouse operated, the LMB is held down and it's not in the GUI
		((m_options->field_19 && Mouse::isButtonDown(BUTTON_LEFT) && !bIsInGUI) ||
		// We are instead keyboard operated, so check for the KM_DESTROY key being held down
		(!m_options->field_19 && Keyboard::isKeyDown(m_options->m_keyMappings[KM_DESTROY].value)) ||
		// The build action intention is a remove one
		b && bai.isRemove());

	if (flag && !m_pScreen && (field_DA8 - field_DAC) >= (m_timer.m_ticksPerSecond * 0.25f))
	{
		bai = BuildActionIntention(INTENT_HELD);
		handleBuildAction(&bai); // handleMouseClick(BUTTON_LEFT)
		field_DAC = field_DA8;
	}

	handleMouseDown(BUTTON_LEFT, flag);

	field_2B4 = getTimeMs();

	Keyboard::reset();
	Mouse::reset();
}

void Minecraft::tickMouse()
{
	if (!m_bGrabbedMouse)
		return;

	platform()->recenterMouse();
}

void Minecraft::handleCharInput(char chr)
{
	if (m_pScreen)
		m_pScreen->charInput(chr);
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

void Minecraft::resetPlayer(Player* player)
{
	m_pLevel->validateSpawn();
	player->reset();

	Pos pos = m_pLevel->getSharedSpawnPos();
	player->setPos(float(pos.x), float(pos.y), float(pos.z));
	player->resetPos();

	// Of course we have to add him back into the game, if he isn't already.
	EntityVector& vec = m_pLevel->m_entities;
	for (int i = 0; i < int(vec.size()); i++)
	{
		if (vec[i] == player)
			return;
	}

	std::vector<Player*>& vec2 = m_pLevel->m_players;
	for (int i = 0; i < int(vec2.size()); i++)
	{
		// remove the player if he is already in the player list
		if (vec2[i] == player)
		{
			vec2.erase(vec2.begin() + i);
			i--;
		}
	}

	// add him in!!
	m_pLevel->addEntity(player);
}

void Minecraft::respawnPlayer(Player* player)
{
	resetPlayer(player);

	// TODO: send a RespawnPacket
}

std::string Minecraft::getVersionString()
{
	return "v0.1.0 alpha";
}

void Minecraft::_reloadInput()
{
	if (m_pInputHolder)
		delete m_pInputHolder;

	if (isTouchscreen())
	{
		m_pInputHolder = new TouchInputHolder(this, m_options);
	}
	else
	{
		m_pInputHolder = new CustomInputHolder(
			new KeyboardInput(m_options),
		#ifdef ORIGINAL_CODE
			new ControllerTurnInput,
		#else
			new MouseTurnInput(this),
		#endif
			new IBuildInput
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

void Minecraft::tick()
{
	if (field_DA4 > 0)
		field_DA4--;

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

		if (m_pLevel && !field_288)
		{
			m_pGameMode->tick();
			m_pGameRenderer->tick();
			m_pLevelRenderer->tick();
			m_pLevel->tickEntities();
			m_pLevel->tick();

			if (m_pLocalPlayer)
			{
				m_pLevel->animateTick(
					Mth::floor(m_pLocalPlayer->m_pos.x),
					Mth::floor(m_pLocalPlayer->m_pos.y),
					Mth::floor(m_pLocalPlayer->m_pos.z));
			}
		}

		m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

		if (!field_288)
		{
			m_pTextures->tick();
			m_pParticleEngine->tick();

#ifndef ORIGINAL_CODE
			if (m_pMobPersp)
			{
				m_pSoundEngine->m_pSoundSystem->setListenerPos(m_pMobPersp->m_pos.x, m_pMobPersp->m_pos.y, m_pMobPersp->m_pos.z);
				m_pSoundEngine->m_pSoundSystem->setListenerAngle(m_pMobPersp->m_yaw, m_pMobPersp->m_pitch);
			}
#endif

		}

		if (m_pScreen)
			m_pScreen->tick();

		Multitouch::reset();
	}
}

void Minecraft::update()
{
	if (field_288 && m_pLevel)
	{
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
		m_pRakNetInstance->runEvents(m_pNetEventCallback);
	}

	for (int i = 0; i < m_timer.m_ticks; i++)
	{
		tick();
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

	double time = double(getTimeS());
	m_fDeltaTime   = time - m_fLastUpdated;
	m_fLastUpdated = time;

	// Added by iProgramInCpp
	m_pGameMode->render(m_timer.m_renderTicks);
}

void Minecraft::init()
{
	if (platform()->hasFileSystemAccess())
		m_options = new Options(m_externalStorageDir);
	else
		m_options = new Options();

	m_bIsTouchscreen = platform()->isTouchscreen();

	_reloadInput();

	m_pRakNetInstance = new RakNetInstance;

	m_pSoundEngine = new SoundEngine(platform()->getSoundSystem());
	m_pSoundEngine->init(m_options);

	m_pTextures = new Textures(m_options, platform());
	m_pTextures->addDynamicTexture(new WaterTexture);
	m_pTextures->addDynamicTexture(new WaterSideTexture);
	m_pTextures->addDynamicTexture(new LavaTexture);
	m_pTextures->addDynamicTexture(new LavaSideTexture);
	m_pTextures->addDynamicTexture(new FireTexture(0));
	m_pLevelRenderer = new LevelRenderer(this, m_pTextures);
	m_pGameRenderer = new GameRenderer(this);
	m_pParticleEngine = new ParticleEngine(m_pLevel, m_pTextures);
	m_pUser = new User(getOptions()->m_playerName, "");

#ifdef TEST_SURVIVAL_MODE
	m_pGameMode = new SurvivalMode(this);
#else
	m_pGameMode = new CreativeMode(this);
#endif

	m_pFont = new Font(m_options, "font/default.png", m_pTextures);

	// Patch Manager
	GetPatchManager()->LoadPatchData(platform()->getPatchData());

	m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
	GetPatchManager()->PatchTextures(platform(), TYPE_TERRAIN);
	m_pTextures->loadAndBindTexture(C_ITEMS_NAME);
	GetPatchManager()->PatchTextures(platform(), TYPE_ITEMS);

	GetPatchManager()->PatchTiles();
}

Minecraft::~Minecraft()
{
	SAFE_DELETE(m_options);
	SAFE_DELETE(m_pNetEventCallback);
	SAFE_DELETE(m_pRakNetInstance);
	SAFE_DELETE(m_pLevelRenderer);
	SAFE_DELETE(m_pGameRenderer);
	SAFE_DELETE(m_pParticleEngine);
	SAFE_DELETE(m_pSoundEngine);
	SAFE_DELETE(m_pGameMode);
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pTextures);

	if (m_pLevel)
	{
		LevelStorage* pStor = m_pLevel->getLevelStorage();
		if (pStor)
			delete pStor;
		if (m_pLevel)
			delete m_pLevel;
	}

	SAFE_DELETE(m_pUser);
	SAFE_DELETE(m_pLevelStorageSource);
	SAFE_DELETE(m_pInputHolder);
	SAFE_DELETE(m_Logger);

	//@BUG: potentially leaking a CThread instance if this is destroyed early?
}

void Minecraft::prepareLevel(const std::string& unused)
{
	field_DA0 = 1;

	float startTime = getTimeS();
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

			float time1 = getTimeS();

			// generating all the chunks at once
			TileID unused = m_pLevel->getTile(i, (C_MAX_Y + C_MIN_Y) / 2, j);

			if (time1 != -1.0f)
				getTimeS();

			float time2 = getTimeS();
			if (m_pLevel->field_B0C)
			{
				while (m_pLevel->updateLights());
			}

			if (time2 != -1.0f)
				getTimeS();
		}
	}

	if (startTime != -1.0f)
		getTimeS();

	m_pLevel->setUpdateLights(1);

	startTime = getTimeS();

	for (int x = 0; x < C_MAX_CHUNKS_X; x++)
	{
		for (int z = 0; z < C_MAX_CHUNKS_Z; z++)
		{
			LevelChunk* pChunk = m_pLevel->getChunk(x, z);
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

	if (m_pLevel->field_B0C)
	{
		m_pLevel->setInitialSpawn();
		m_pLevel->saveLevelData();
		m_pLevel->getChunkSource()->saveAll();
	}
	else
	{
		m_pLevel->saveLevelData();
	}

	m_progressPercent = -1;
	field_DA0 = 2;

	startTime = getTimeS();

	m_pLevel->prepare();

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
	Gui::InvGuiScale = getBestScaleForThisScreenSize(newWidth, newHeight) / guiScaleMultiplier;

	if (m_pScreen)
		m_pScreen->setSize(int(Minecraft::width * Gui::InvGuiScale), int(Minecraft::height * Gui::InvGuiScale));

	if (m_pInputHolder)
		m_pInputHolder->setScreenSize(newWidth, newHeight);
}

float Minecraft::getBestScaleForThisScreenSize(int width, int height)
{
	if (height > 1800)
		return 1.0f / 4.0f;

	// phones only
#if !defined(_WIN32) && !defined(USE_SDL2)
	if (height > 600)
		return 1.0f / 4.0f;

	if (height > 400)
		return 1.0f / 3.0f;

	if (height > 300)
		return 1.0f / 2.0f;
#else
	if (height > 1000)
		return 1.0f / 3.0f;

	if (height > 400)
		return 1.0f / 2.0f;
#endif

	return 1.0f;
}

void Minecraft::generateLevel(const std::string& unused, Level* pLevel)
{
	float time = getTimeS(); //@UNUSED

	prepareLevel(unused);

	if (time != -1.0f)
		getTimeS(); //@QUIRK: unused return value

#pragma warning(disable : 26444) // C26444: Don't try to declare a local variable with no name.
	std::string("Level generated: "); //@QUIRK: unused string instance

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

	pLevel->validateSpawn();
	pLevel->loadPlayer(m_pLocalPlayer);

	if (m_pLocalPlayer)
	{
		m_pLocalPlayer->resetPos();
	}

	m_pMobPersp = m_pLocalPlayer;
	m_pLevel = pLevel;

	m_bPreparingLevel = false;

	if (m_pRakNetInstance->m_bIsHost)
		m_pRakNetInstance->announceServer(m_pUser->field_0);
}

void* Minecraft::prepareLevel_tspawn(void* ptr)
{
	Minecraft* pMinecraft = (Minecraft*)ptr;

	pMinecraft->generateLevel("Currently not used", pMinecraft->m_pLevel);

	return nullptr;
}

void Minecraft::pauseGame()
{
	if (m_pScreen) return;
	m_pLevel->savePlayerData();
	setScreen(new PauseScreen);
}

void Minecraft::setLevel(Level* pLevel, const std::string& text, LocalPlayer* pLocalPlayer)
{
	m_pMobPersp = nullptr;

	if (pLevel)
	{
		m_pGameMode->initLevel(pLevel);

		if (pLocalPlayer && m_pLocalPlayer == nullptr)
		{
			m_pLocalPlayer = pLocalPlayer;
			pLocalPlayer->resetPos();
		}
		else if (m_pLocalPlayer)
		{
			m_pLocalPlayer->resetPos();
			pLevel->addEntity(m_pLocalPlayer);
		}

		m_pLevel = pLevel;
		m_bPreparingLevel = true;
		m_pPrepThread = new CThread(&Minecraft::prepareLevel_tspawn, this);
	}
	else
	{
		m_pLocalPlayer = nullptr;
	}
}

void Minecraft::selectLevel(const std::string& a, const std::string& b, int c)
{
	LevelStorage* pStor = m_pLevelStorageSource->selectLevel(a, false);
	Dimension* pDim = Dimension::getNew(0);

	m_pLevel = new Level(pStor, b, c, 1, pDim);
	setLevel(m_pLevel, "Generating level", nullptr);

	field_D9C = 1;
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
	ItemInstance* pInst = m_pLocalPlayer->m_pInventory->getSelectedItem();

	if (!pInst)
	{
		m_CurrItemInstance.m_itemID = -1;
		m_CurrItemInstance.m_amount = 999;
		m_CurrItemInstance.m_auxValue = 0;
		return &m_CurrItemInstance;
	}

	if (m_pGameMode->isSurvivalType())
		return pInst;

	m_CurrItemInstance.m_itemID = pInst->m_itemID;
	m_CurrItemInstance.m_amount = 999;
	m_CurrItemInstance.m_auxValue = pInst->m_auxValue;

	return &m_CurrItemInstance;
}

int Minecraft::getFpsIntlCounter()
{
	return 0;
}

void Minecraft::leaveGame(bool bCopyMap)
{
	m_bPreparingLevel = false;
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
	field_288 = true;
	setScreen(new SavingWorldScreen(bCopyMap, m_pLocalPlayer));
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
#ifndef __EMSCRIPTEN__
	m_pRakNetInstance->host(m_pUser->field_0, C_DEFAULT_PORT, C_MAX_CONNECTIONS);
	m_pNetEventCallback = new ServerSideNetworkHandler(this, m_pRakNetInstance);
#endif
}

void Minecraft::joinMultiplayer(const PingedCompatibleServer& serverInfo)
{
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
#ifndef __EMSCRIPTEN__
	field_18 = false;
	m_pRakNetInstance->stopPingForHosts();
	delete m_pNetEventCallback;
	m_pNetEventCallback = nullptr;
#endif
}

void Minecraft::locateMultiplayer()
{
#ifndef __EMSCRIPTEN__
	field_18 = true;
	m_pRakNetInstance->pingForHosts(C_DEFAULT_PORT);
	m_pNetEventCallback = new ClientSideNetworkHandler(this, m_pRakNetInstance);
#endif
}
