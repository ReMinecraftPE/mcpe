/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Minecraft.hpp"
#include "client/gui/screens/PauseScreen.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"
#include "client/gui/screens/RenameMPLevelScreen.hpp"
#include "client/gui/screens/SavingWorldScreen.hpp"
#include "client/network/ServerSideNetworkHandler.hpp"
#include "client/network/ClientSideNetworkHandler.hpp"

#include "world/gamemode/SurvivalMode.hpp"
#include "world/gamemode/CreativeMode.hpp"

#ifndef ORIGINAL_CODE
#include "client/player/input/MouseTurnInput.hpp"
#else
#include "client/player/input/ControllerTurnInput.hpp"
#endif

// note: Nothing changes these, so it'll think we're always running at 854x480 even if not
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

Minecraft::Minecraft() : m_gui(this)
{
#ifndef ORIGINAL_CODE
	m_pTurnInput = new MouseTurnInput(this);
#else
	m_pTurnInput = new ControllerTurnInput;
#endif

	m_pRakNetInstance = new RakNetInstance;

	m_pSoundEngine = new SoundEngine;
	m_pSoundEngine->init(&m_options);
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
		m_pLocalPlayer->m_pKeyboardInput->releaseAllKeys();

	m_bGrabbedMouse = false;

	platform()->setMouseGrabbed(false);
}

void Minecraft::grabMouse()
{
	if (m_bGrabbedMouse)
		return;

	m_bGrabbedMouse = true;
	field_D20 = 0.0f;
	field_D24 = 0.0f;
	setScreen(nullptr);

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

void Minecraft::reloadOptions()
{
	m_options.update(platform()->getOptionStrings());

	// update the user's name.
	m_pUser->field_0 = m_options.m_playerName;
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

	return m_pLevel->field_11;
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

void Minecraft::handleMouseClick(int type)
{
	int a;

	HitResult& hr = m_hitResult;

	// @TODO: fix goto hell
	if (type == 1)
	{
		if (field_DA4 > 0)
			return;

		m_pLocalPlayer->swing();

		if (m_hitResult.m_hitType != HitResult::NONE)
			goto label_3;

	label_9:
		if (type != 1)
			goto label_5;

		if (!m_pGameMode->isCreativeType())
			field_DA4 = 10;

		return;
	}

	if (m_hitResult.m_hitType == HitResult::NONE)
		goto label_9;

label_3:
	if (m_hitResult.m_hitType != HitResult::ENTITY)
	{
		if (m_hitResult.m_hitType != HitResult::AABB)
			goto label_5;

		// @NOTE: extra scope to avoid error
		{
			Tile* pTile = Tile::tiles[m_pLevel->getTile(hr.m_tileX, hr.m_tileY, hr.m_tileZ)];

			if (type == 1)
			{
				if (pTile)
				{
					// @BUG: This is only done on the client side.
					m_pLevel->extinguishFire(hr.m_tileX, hr.m_tileY, hr.m_tileZ, hr.m_hitSide);

					if (pTile != Tile::unbreakable || m_pLocalPlayer->field_B94 > 99 && !hr.m_bUnk24)
					{
						m_pGameMode->startDestroyBlock(hr.m_tileX, hr.m_tileY, hr.m_tileZ, hr.m_hitSide);
					}
				}
				return;
			}

			ItemInstance* pItem = getSelectedItem();

			if (m_pGameMode->useItemOn(m_pLocalPlayer, m_pLevel, pItem->m_itemID < 0 ? nullptr : pItem, hr.m_tileX, hr.m_tileY, hr.m_tileZ, hr.m_hitSide))
			{
				m_pLocalPlayer->swing();
				if (!isOnline())
					return;

				if (pItem->m_itemID > C_MAX_TILES || pItem->m_itemID < 0)
					return;

				int dx = hr.m_tileX, dz = hr.m_tileZ;
				uint8_t dy = uint8_t(hr.m_tileY);

				if (m_pLevel->getTile(hr.m_tileX, hr.m_tileY, hr.m_tileZ) != Tile::topSnow->m_ID)
				{
					switch (hr.m_hitSide)
					{
					case DIR_YNEG: dy--; break;
					case DIR_YPOS: dy++; break;
					case DIR_ZNEG: dz--; break;
					case DIR_ZPOS: dz++; break;
					case DIR_XNEG: dx--; break;
					case DIR_XPOS: dx++; break;
					}
				}

				m_pRakNetInstance->send(new PlaceBlockPacket(m_pLocalPlayer->m_EntityID, dx, dy, dz, uint8_t(pItem->m_itemID), uint8_t(hr.m_hitSide)));
				return;
			}
		}

	label_5:
		if (type != 2)
			return;
		goto label_15;
	}

	if (type == 1)
	{
		m_pGameMode->attack(m_pLocalPlayer, hr.m_pEnt);
		return;
	}

	if (type == 2)
	{
		a = hr.m_pEnt->interactPreventDefault();
		m_pGameMode->interact(m_pLocalPlayer, hr.m_pEnt);
		if (!a)
		{
		label_15:
			int id = m_pLocalPlayer->m_pInventory->getSelectedItemId();
			if (id >= 0)
			{
				ItemInstance* pItem = getSelectedItem();

				if (m_pGameMode->useItem(m_pLocalPlayer, m_pLevel, pItem))
					m_pGameRenderer->m_pItemInHandRenderer->itemUsed();
			}
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

	bool bIsInGUI = m_gui.isInside(Mouse::_x, Mouse::_y);

	while (Mouse::_index + 1 < Mouse::_inputs.size())
	{
		Mouse::_index++;

		if (getTimeMs() - field_2B4 > 200)
			continue;

		if (Mouse::_buttonStates[1])
			m_gui.handleClick(1, Mouse::_x, Mouse::_y);

		if (!bIsInGUI && m_options.field_19)
		{
			MouseInput& input = Mouse::_inputs[Mouse::_index];

			if (input.field_0 == 1 && input.field_4 == 1)
			{
				handleMouseClick(1);
				field_DAC = field_DA8;
			}
			if (input.field_0 == 2 && input.field_4 == 1)
			{
				handleMouseClick(2);
				field_DAC = field_DA8;
			}
#ifdef ENH_ALLOW_SCROLL_WHEEL
			if (input.field_0 == 3)
			{
				int slot = m_pLocalPlayer->m_pInventory->m_SelectedHotbarSlot;

#ifdef ENH_ENABLE_9TH_SLOT
#define MAX_ITEMS (C_MAX_HOTBAR_ITEMS - 1)
#else
#define MAX_ITEMS (C_MAX_HOTBAR_ITEMS - 2)
#endif

				if (input.field_4 > 0) // @NOTE: Scroll up
				{
					if (slot-- == 0)
					{
						slot = MAX_ITEMS;
					}
				}
				else
				{
					if (slot++ == MAX_ITEMS) // @NOTE: Scroll down
					{
						slot = 0;
					}
				}

				m_pLocalPlayer->m_pInventory->selectSlot(slot);
			}
#endif
		}

	}

	while (Keyboard::_index + 1 < Keyboard::_inputs.size())
	{
		Keyboard::_index++;
		Keyboard::Input& input = Keyboard::_inputs[Keyboard::_index];

		int keyCode = input.field_4;
		bool bPressed = input.field_0 == 1;

		m_pLocalPlayer->m_pKeyboardInput->setKey(keyCode, bPressed);

		if (bPressed)
		{
			m_gui.handleKeyPressed(keyCode);

			int index = keyCode - AKEYCODE_1;
			if (index <= 8 && index >= 0)
			{
				m_pLocalPlayer->m_pInventory->selectSlot(index);
			}
			else if (keyCode == AKEYCODE_SEARCH)
			{
				m_options.field_23D ^= 1;
			}
			else if (keyCode == AKEYCODE_MENU)
			{
				pauseGame();
			}
			else if (keyCode == AKEYCODE_Q)
			{
				int itemID = m_pLocalPlayer->m_pInventory->getSelectedItemId();
				if (itemID > 0)
				{
					ItemInstance inst(itemID, 1, 0);
					m_pLocalPlayer->drop(&inst);
				}
			}
		#ifdef ENH_ALLOW_AO
			else if (keyCode == AKEYCODE_F4)
			{
				// Toggle ambient occlusion.
				m_options.field_18 ^= 1;
				Minecraft::useAmbientOcclusion = m_options.field_18;
				m_pLevelRenderer->allChanged();
			}
		#endif
		}

		if (m_options.field_19)
			continue;

		if (getTimeMs() - field_2B4 <= 200)
		{
			if (m_options.m_keyBinds[Options::DESTROY].value == keyCode && bPressed)
				handleMouseClick(1);
			if (m_options.m_keyBinds[Options::PLACE].value == keyCode && bPressed)
				handleMouseClick(2);
		}
	}

	// @TODO: fix gotos
	bool v12 = false;

	if (m_options.field_19)
	{
		if (!Mouse::_buttonStates[1] || bIsInGUI)
			goto label_12;
	}
	else if (Keyboard::_states[m_options.m_keyBinds[Options::DESTROY].value] != 1)
	{
		goto label_12;
	}
	
	if (!m_pScreen && (field_DA8 - field_DAC) >= (m_timer.field_10 * 0.25f))
	{
		handleMouseClick(1);
		field_DAC = field_DA8;
	}

	if (m_bGrabbedMouse)
	{
		v12 = true;
	}
	else
	{
	label_12:
		v12 = false;
	}

	handleMouseDown(1, v12);

	field_2B4 = getTimeMs();

	Keyboard::_inputs.clear();
	Keyboard::_index = -1;
	Mouse::_inputs.clear();
	Mouse::_index = -1;
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

void Minecraft::_levelGenerated()
{
	if (m_pNetEventCallback)
		m_pNetEventCallback->levelGenerated(m_pLevel);
}

void Minecraft::tick()
{
	if (field_DA4 > 0)
		field_DA4--;

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
#ifdef USE_SDL
				m_pSoundEngine->m_soundSystem.update(m_pMobPersp->m_pos.x, m_pMobPersp->m_pos.y, m_pMobPersp->m_pos.z, m_pMobPersp->m_yaw);
#else
				m_pSoundEngine->m_soundSystem.setListenerPos(m_pMobPersp->m_pos.x, m_pMobPersp->m_pos.y, m_pMobPersp->m_pos.z);
				m_pSoundEngine->m_soundSystem.setListenerAngle(m_pMobPersp->m_yaw, m_pMobPersp->m_pitch);
#endif
			}
#endif

		}

		if (m_pScreen)
			m_pScreen->tick();
	}
}

void Minecraft::update()
{
	if (field_288 && m_pLevel)
	{
		float x = m_timer.field_18;
		m_timer.advanceTime();
		m_timer.field_18 = x;
	}
	else
	{
		m_timer.advanceTime();
	}

	if (m_pRakNetInstance)
	{
		m_pRakNetInstance->runEvents(m_pNetEventCallback);
	}

	if (m_timer.field_14 > 0)
	{
		for (int i = 0; i < m_timer.field_14; i++)
		{
			tick();
			field_DA8++;
		}
	}

	if (m_pLevel && !m_bPreparingLevel)
	{
		m_pLevel->updateLights();
	}

#ifndef ORIGINAL_CODE
	tickMouse();
#endif

	m_pGameRenderer->render(m_timer.field_18);
}

void Minecraft::init()
{
	m_pTextures = new Textures(&m_options, platform());
	m_pTextures->addDynamicTexture(new WaterTexture);
	m_pTextures->addDynamicTexture(new WaterSideTexture);
	m_pLevelRenderer = new LevelRenderer(this, m_pTextures);
	m_pGameRenderer = new GameRenderer(this);
	m_pParticleEngine = new ParticleEngine(m_pLevel, m_pTextures);
	m_pUser = new User("TestUser", "");

#ifdef TEST_SURVIVAL_MODE
	m_pGameMode = new SurvivalMode(this);
#else
	m_pGameMode = new CreativeMode(this);
#endif

	reloadOptions();

	m_pFont = new Font(&m_options, "font/default.png", m_pTextures);
}

Minecraft::~Minecraft()
{
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
	SAFE_DELETE(m_pTurnInput);

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
	{
		pLocalPlayer->m_pKeyboardInput = new KeyboardInput(&m_options);
	}

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
	setScreen(new SavingWorldScreen(bCopyMap));
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
