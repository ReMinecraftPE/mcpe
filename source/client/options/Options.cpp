/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// for SDL 1.2 controller buttons

#include <fstream>

#include "thirdparty/SDL/SDL_gamecontroller.h"

#include "Options.hpp"
#include "common/Logger.hpp"
#include "compat/KeyCodes.hpp"
#include "client/app/Minecraft.hpp"
#include "client/player/input/GameController.hpp"

#include "client/renderer/PatchManager.hpp"
#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"
#include "client/resources/ResourcePackRepository.hpp"
#include "client/locale/Language.hpp"
#include "client/gui/components/SmallButton.hpp"
#include "client/gui/components/SliderButton.hpp"
#include "client/gui/components/SwitchButton.hpp"
#include "client/gui/components/SwitchValuesButton.hpp"
#include "client/gui/components/TickBox.hpp"
#include "client/renderer/LogoRenderer.hpp"

#include "renderer/RenderContextImmediate.hpp"

void Options::_initDefaultValues()
{
	m_flySpeed = 1.0f;
	field_248 = 1.0f;
	m_bFixedCamera = false;
	m_bLimitFramerate = false;
	field_240 = true;
	m_skin = "Default";
	m_bUseMouseForDigging = true;
#ifdef ORIGINAL_CODE
	m_viewDistance.set(2);
	m_thirdPerson.set(TPM_FIRST);
	m_bUseMouseToBreak = false;
#endif

	// Force this on until we get a proper UI
	_tryAddResourcePack(C_DEFAULT_RESOURCE_PACK, m_resourcePacks);

	loadControls();
}

static UITheme GetDefaultUiTheme()
{
#if MC_PLATFORM_XBOX360
	return UI_CONSOLE;
#else
	return AppPlatform::singleton()->isTouchscreen() ? UI_POCKET : UI_JAVA;
#endif
}

Options::Options(Minecraft* mc, const std::string& folderPath) :
	m_pMinecraft(mc)
	, m_musicVolume("audio_music", "options.music", 1.0f)
	, m_masterVolume("audio_master", "options.sound", 1.0f)
	, m_sensitivity("ctrl_sensitivity", "options.sensitivity", 0.5f)
	, m_invertMouse("ctrl_invertmouse", "options.invertMouse", false)
	, m_viewDistance("gfx_viewdistance", "options.renderDistance", 2, ValuesBuilder().add("options.renderDistance.tiny").add("options.renderDistance.short").add("options.renderDistance.normal").add("options.renderDistance.far"), true)
	, m_viewBobbing("gfx_bobview", "options.viewBobbing", true)
	, m_anaglyphs("gfx_3danaglyphs", "options.anaglyph", false)
	, m_fancyGraphics("gfx_fancygraphics", "options.fancyGraphics", true)
	, m_ambientOcclusion("gfx_smoothlighting", "options.ao", Minecraft::useAmbientOcclusion)
	, m_fancySky("gfx_fancysky", "options.fancySky", true)
	, m_difficulty("misc_difficulty", "options.difficulty", 2, ValuesBuilder().add("options.difficulty.peaceful").add("options.difficulty.easy").add("options.difficulty.normal").add("options.difficulty.hard"), true)
	, m_hideGui("gfx_hidegui", "options.hideGui", false)
	, m_thirdPerson("gfx_thirdperson", "options.thirdPerson", 0)
	, m_flightHax("misc_flycheat", "options.flightHax", false)
	, m_swapJumpSneak("ctrl_swapjumpsneak", "options.swapJumpSneak", false)
	, m_dpadSize("ctrl_dpadsize", "options.dpadSize", 1.0f)
	, m_guiScale("gfx_guiscale", "options.guiScale", 0, ValuesBuilder().add("options.guiScale.auto").add("options.guiScale.small").add("options.guiScale.normal").add(("options.guiScale.large")))
	, m_gamma("gfx_gamma", "options.gamma", 0.50f)
	, m_fov("gfx_fov", "options.fov", 70.0f)
	, m_playerName("mp_username", "options.username", "Steve")
	, m_serverVisibleDefault("mp_server_visible_default", "options.serverVisibleDefault", true)
	, m_autoJump("ctrl_autojump", "options.autoJump", AppPlatform::singleton()->isTouchscreen())
	, m_debugText("info_debugtext", "options.debugText", false)
	, m_blockOutlines("gfx_blockoutlines", "options.blockOutlines", !AppPlatform::singleton()->isTouchscreen())
	, m_fancyGrass("gfx_fancygrass", "options.fancyGrass", true)
	, m_biomeColors("gfx_biomecolors", "options.biomeColors", true)
	, m_splitControls("ctrl_split", "options.splitControls", false)
	, m_dynamicHand("gfx_dynamichand", "options.dynamicHand", true)
	, m_menuPanorama("misc_menupano", "options.menuPanorama", true)
	, m_lang("gfx_lang", "options.lang", "en_us")
	, m_uiTheme("gfx_uitheme", "options.uiTheme", GetDefaultUiTheme(), ValuesBuilder().add("options.uiTheme.pocket").add("options.uiTheme.java").add("options.uiTheme.console"))
	, m_logoType("gfx_logotype", "options.logoType", LOGO_AUTO, ValuesBuilder().add("options.logoType.auto").add("options.logoType.pocket").add("options.logoType.java").add("options.logoType.console").add("options.logoType.xbox360").add("options.logoType.logo3d"))
	, m_hudSize("gfx_hudsize", "options.hudSize", HUD_SIZE_2)
	, m_classicCrafting("gfx_classiccrafting", "options.classicCrafting", false)
	//, m_limitFramerate("gfx_fpslimit", "options.framerateLimit", 0, ValuesBuilder().add(performance.max").add("performance.balanced").add("performance.powersaver"))
	//, m_bMipmaps("gfx_mipmaps", "options.mipmaps")
	, m_animatedCharacter("gfx_animatedcharacter", "options.animatedCharacter", true)
	, m_vSync("enableVsync", "options.enableVsync", true)
{
	add(OC_GAMEPLAY, m_difficulty);
	add(OC_GAMEPLAY, m_thirdPerson);
	add(OC_GAMEPLAY, m_serverVisibleDefault);
	add(OC_GAMEPLAY, m_classicCrafting);
	add(OC_GAMEPLAY, m_musicVolume);
	add(OC_GAMEPLAY, m_masterVolume);

	add(OC_CONTROLS, m_sensitivity);
	add(OC_CONTROLS, m_invertMouse);
	add(OC_CONTROLS, m_splitControls);
	add(OC_CONTROLS, m_swapJumpSneak);
	add(OC_CONTROLS, m_dpadSize);
	add(OC_CONTROLS, m_autoJump);
	//add(OC_CONTROLS, m_vibrate);
	add(OC_CONTROLS, m_flightHax);

	//add(OC_VIDEO, m_brightness);
	add(OC_VIDEO, m_viewDistance);
	//add(OC_VIDEO, m_antiAliasing);
	add(OC_VIDEO, m_guiScale);
	add(OC_VIDEO, m_fov);
	add(OC_VIDEO, m_gamma);
	add(OC_VIDEO, m_ambientOcclusion);
	add(OC_VIDEO, m_fancyGraphics);
	add(OC_VIDEO, m_fancySky);
	//add(OC_VIDEO, m_animatedTextures);
	add(OC_VIDEO, m_viewBobbing);
	add(OC_VIDEO, m_anaglyphs);
	add(OC_VIDEO, m_blockOutlines);
	//add(OC_VIDEO, m_limitFramerate);
	add(OC_VIDEO, m_vSync);
	add(OC_VIDEO, m_fancyGrass);
	add(OC_VIDEO, m_biomeColors);
	//add(OC_VIDEO, m_bMipmaps);
	add(OC_VIDEO, m_dynamicHand);
	add(OC_VIDEO, m_uiTheme);
	add(OC_VIDEO, m_logoType);
	add(OC_VIDEO, m_hudSize);
	add(OC_VIDEO, m_animatedCharacter);
	add(OC_VIDEO, m_hideGui);
	add(OC_VIDEO, m_debugText);
	add(OC_VIDEO, m_menuPanorama);

	add(m_playerName);
	add(m_lang);
	
	_initDefaultValues();
	if (folderPath.empty()) return;
	m_filePath = folderPath + "/options.txt";
	_load();
}

void Options::add(OptionEntry& entry)
{
	entry.m_pMinecraft = m_pMinecraft;
	m_options[entry.getKey()] = &entry;
}

void Options::add(OptionsCategory cat, OptionEntry& entry)
{
	add(entry);
	m_categoryOptions[cat].push_back(&entry);
}

void Options::_load()
{
	std::vector<std::string> strings = readPropertiesFromFile(m_filePath);

	bool logo3d = false;

	for (size_t i = 0; i < strings.size(); i += 2)
	{
		std::string key = strings[i], value = strings[i + 1];

		HashMap<std::string, OptionEntry*>::iterator it = m_options.find(key);
		if (it != m_options.end())
			it.value()->load(value);
		else if (key == "misc_oldtitle")
			logo3d = !readBool(value);
		else if (key == "gfx_resourcepacks")
			readPackArray(value, m_resourcePacks);
	}

	if (logo3d)
		m_logoType.set(LOGO_3D);
}

AsyncTask Options::_saveAsync()
{
	return AsyncTask(savePropertiesToFileAsync(m_filePath, getOptionStrings()));
}

const AsyncTask& Options::save()
{
	if (!m_saveTask.isRunning())
		m_saveTask = _saveAsync();

	return m_saveTask;
}

bool Options::_hasResourcePack(const ResourcePack& pack, ResourcePackStack& packs)
{
	for (size_t i = 0; i < packs.size(); i++)
	{
		if (packs[i] == pack)
			return true;
	}

	return false;
}

void Options::_tryAddResourcePack(const std::string& name, ResourcePackStack& packs)
{
	ResourceLocation location(ResourcePackRepository::RESOURCE_PACKS_PATH + "/" + name);

	// Search internally (within assets) first
	location.fileSystem = ResourceLocation::APP_PACKAGE;

	std::string fullPath = location.getFullPath();
	if (!isDirectory(fullPath.c_str()))
	{
		// Search externally (within user-writable external storage dir)
		location.fileSystem = ResourceLocation::EXTERNAL_DIR;

		fullPath = location.getFullPath();
		if (!isDirectory(fullPath.c_str()))
		{
			LOG_W("Failed to find resource pack: %s", fullPath.c_str());
			return;
		}
	}

	ResourcePack resourcePack(name, location.fileSystem);
	if (!_hasResourcePack(resourcePack, packs))
		packs.push_back(resourcePack);
}

bool Options::readBool(const std::string& str)
{
	std::string trimmed = Util::stringTrim(str);
	if (trimmed == "true" || trimmed == "1")
		return true;
	if (trimmed == "false" || trimmed == "0")
		return false;

	return false;
}

int Options::readInt(const std::string& str)
{
	int f;
	
	if (!sscanf(str.c_str(), "%d", &f))
		f = 0;

	return f;
}

float Options::readFloat(const std::string& str)
{
	float f;

	if (!sscanf(str.c_str(), "%f", &f))
		f = 0;

	return f;
}

void Options::readArray(const std::string& str, std::vector<std::string>& array)
{
	std::istringstream ss(str);
	std::string element;

	while (std::getline(ss, element, ','))
		array.push_back(element);
}

void Options::readPackArray(const std::string& str, ResourcePackStack& array)
{
	// We create a new array instead of modifying the existing one
	// because erasing elements from a vector doesn't free the memory.
	std::vector<std::string> fullarray;
	readArray(str, fullarray);
	ResourceLocation location;
	for (size_t i = 0; i < fullarray.size(); ++i)
	{
		_tryAddResourcePack(fullarray[i], array);
	}
}

std::string Options::saveBool(bool b)
{
	return b ? "true" : "false";
}

std::string Options::saveInt(int i)
{
	return Util::toString(i);
}

std::string Options::saveFloat(float f)
{
	return Util::toString(f);
}

std::string Options::saveArray(const std::vector<std::string>& arr)
{
	if (arr.empty())
		return "";
	std::string ret;
	bool done = false;
	size_t i = 0;
	while (!done)
	{
		ret += arr[i++];
		size_t size = arr.size();
		if (i < size)
			ret += ",";
		else
			done = true;
	}
	return ret;
}

std::string Options::savePackArray(const ResourcePackStack& arr)
{
	if (arr.empty())
		return "";

	std::vector<std::string> array;
	for (ResourcePackStack::const_iterator it = arr.begin(); it != arr.end(); it++)
	{
		array.push_back(it->m_name);
	}

	return saveArray(array);
}

std::vector<std::string> Options::readPropertiesFromFile(const std::string& filePath)
{
	std::vector<std::string> o;
	AppPlatform& platform = *AppPlatform::singleton();

	std::string nativePath(filePath);
	platform.makeNativePath(nativePath);

	const char* const path = nativePath.c_str();
	LOG_I("Loading options from %s", path);

	platform.beginProfileDataRead(0);

	std::ifstream ifs(path);
	if (!ifs.is_open())
	{
		LOG_W("%s doesn't exist, resetting to defaults", path);
		platform.endProfileDataRead(0);
		return o;
	}

	std::string str;
	while (true)
	{
		if (!std::getline(ifs, str, '\n'))
			break;

		if (str.empty() || str[0] == '#')
			continue;

		std::stringstream ss;
		ss << str;

		std::string key, value;
		if (std::getline(ss, key, ':') && std::getline(ss, value))
		{
			o.push_back(key);
			o.push_back(value);
		}
	}

	ifs.close();

	platform.endProfileDataRead(0);

	return o;
}

void Options::savePropertiesToFile(const std::string& filePath, const std::vector<std::string>& properties)
{
	assert(properties.size() % 2 == 0);

	AppPlatform& platform = *AppPlatform::singleton();

	std::string nativePath(filePath);
	platform.makeNativePath(nativePath);

	platform.beginProfileDataWrite(0);

	std::ofstream os;
	os.open(nativePath.c_str());
	if (!os.is_open())
	{
		LOG_E("Failed to save to: %s", nativePath.c_str());
		return;
	}

	os << "#Config file for " C_GAME_NAME ".  The # at the start denotes a comment, removing it makes it a command.\n\n";

	for (size_t i = 0; i < properties.size(); i += 2)
		os << properties[i] << ':' << properties[i + 1] << '\n';

	os.close();

	platform.endProfileDataWrite(0);
}

std::vector<std::string> Options::getOptionStrings()
{
	std::vector<std::string> vec;

#define SO(optname, value) do { vec.push_back(optname); vec.push_back(value); } while (0)

	std::string str;
	for (HashMap<std::string, OptionEntry*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
	{
		str.clear();
		it.value()->save(str);
		SO(it.key(), str);
	}
	SO("gfx_resourcepacks", savePackArray(m_resourcePacks));

	return vec;
}

void Options::loadControls()
{
	// Win32 key codes are being used by default
#define BM(idx, name, code) m_inputMappings[idx] = InputMapping(name, code)
	BM(AID_FORWARD,      "key.forward",       'W');
	BM(AID_LEFT,         "key.left",          'A');
	BM(AID_BACKWARD,     "key.back",          'S');
	BM(AID_RIGHT,        "key.right",         'D');
	BM(AID_JUMP,         "key.jump",          ' ');
	BM(AID_CRAFTING,     "key.crafting",      'E');
	BM(AID_INVENTORY,    "key.inventory",     'I');
	BM(AID_DROP,         "key.drop",          'Q');
	BM(AID_CHAT,         "key.chat",          'T');
	BM(AID_FOG,          "key.fog",           'F');
	BM(AID_SNEAK,        "key.sneak",         0x10); // VK_SHIFT. In original, it's 10 (misspelling?)
	BM(AID_DESTROY,      "key.destroy",       'K');  // was 'X'
	BM(AID_PLACE,        "key.place",         'L');  // was 'C'
	BM(AID_MENU_UP,      "key.menu.up",       0x26); // VK_UP
	BM(AID_MENU_DOWN,    "key.menu.down",     0x28); // VK_DOWN
	BM(AID_MENU_LEFT,    "key.menu.left",     0x25); // VK_LEFT
	BM(AID_MENU_RIGHT,   "key.menu.right",    0x27); // VK_RIGHT
	BM(AID_MENU_TAB_LEFT,  "key.menu.tab.left",  0x25);	// VK_LEFT
	BM(AID_MENU_TAB_RIGHT, "key.menu.tab.right", 0x27); // VK_RIGHT
	BM(AID_MENU_OK,      "key.menu.ok",       0x0D); // VK_RETURN
	BM(AID_MENU_CANCEL,  "key.menu.cancel",   0x1B); // VK_ESCAPE, was 0x08 = VK_BACK
	BM(AID_MENU_PAUSE,	 "key.menu.pause",	  0x1B); // VK_ESCAPE
	BM(AID_SLOT_1,       "key.slot.1",        '1');
	BM(AID_SLOT_2,       "key.slot.2",        '2');
	BM(AID_SLOT_3,       "key.slot.3",        '3');
	BM(AID_SLOT_4,       "key.slot.4",        '4');
	BM(AID_SLOT_5,       "key.slot.5",        '5');
	BM(AID_SLOT_6,       "key.slot.6",        '6');
	BM(AID_SLOT_7,       "key.slot.7",        '7');
	BM(AID_SLOT_8,       "key.slot.8",        '8');
	BM(AID_SLOT_9,       "key.slot.9",        '9');
	BM(AID_SLOT_L,       "key.slot.left",     'Y');
	BM(AID_SLOT_R,       "key.slot.right",    'U');
	BM(AID_CONTAINER_QUICKMOVE, "key.container.quickmove", 0x10); // VK_SHIFT
	BM(AID_CONTAINER_SPLIT,     "key.container.split",     0xBF); // VK_OEM_2 (keymap is unused on mouse & keyboard)
	BM(AID_TOGGLEGUI,    "key.fn.gui",        0x70); // VK_F1
	BM(AID_SCREENSHOT,   "key.fn.screenshot", 0x71); // VK_F2
	BM(AID_TOGGLEDEBUG,  "key.fn.debug",      0x72); // VK_F3
	BM(AID_TOGGLEAO,     "key.fn.ao",         0x73); // VK_F4
	BM(AID_TOGGLE3RD,    "key.fn.3rd",        0x74); // VK_F5
	BM(AID_FLY_UP,       "key.fly.up",        'X');
	BM(AID_FLY_DOWN,     "key.fly.down",      'C');
	BM(AID_CHAT_CMD,     "key.chat.cmd",      0xBF); // VK_OEM_2
#undef BM

	// @TODO: These should **really** not be defined in here. How about AppPlatform?

#define BM(idx,code) m_inputMappings[idx].info.keyId = code
#ifdef USE_SDL
	BM(AID_FORWARD,       SDLVK_w);
	BM(AID_LEFT,          SDLVK_a);
	BM(AID_BACKWARD,      SDLVK_s);
	BM(AID_RIGHT,         SDLVK_d);
	BM(AID_JUMP,          SDLVK_SPACE);
	BM(AID_DESTROY,       SDLVK_x);
	BM(AID_PLACE,         SDLVK_c);
	BM(AID_MENU_UP,       SDLVK_UP);
	BM(AID_MENU_DOWN,     SDLVK_DOWN);
	BM(AID_MENU_LEFT,     SDLVK_LEFT);
	BM(AID_MENU_RIGHT,    SDLVK_RIGHT);
	BM(AID_MENU_TAB_LEFT, SDLVK_LEFT);
	BM(AID_MENU_TAB_RIGHT, SDLVK_RIGHT);
	BM(AID_MENU_OK,       SDLVK_RETURN);
	BM(AID_MENU_CANCEL,   SDLVK_ESCAPE);
	BM(AID_MENU_PAUSE,	  SDLVK_ESCAPE);
	BM(AID_DROP,          SDLVK_q);
	BM(AID_CHAT,          SDLVK_t);
	BM(AID_FOG,           SDLVK_f);
	BM(AID_CRAFTING,      SDLVK_e);
	BM(AID_INVENTORY,     SDLVK_i);
	BM(AID_SNEAK,         SDLVK_LSHIFT);
	BM(AID_SLOT_1,        SDLVK_1);
	BM(AID_SLOT_2,        SDLVK_2);
	BM(AID_SLOT_3,        SDLVK_3);
	BM(AID_SLOT_4,        SDLVK_4);
	BM(AID_SLOT_5,        SDLVK_5);
	BM(AID_SLOT_6,        SDLVK_6);
	BM(AID_SLOT_7,        SDLVK_7);
	BM(AID_SLOT_8,        SDLVK_8);
	BM(AID_SLOT_9,        SDLVK_9);
	BM(AID_CONTAINER_QUICKMOVE, SDLVK_LSHIFT);
	BM(AID_TOGGLEGUI,     SDLVK_F1);
	BM(AID_SCREENSHOT,    SDLVK_F2);
	BM(AID_TOGGLEDEBUG,   SDLVK_F3);
	BM(AID_TOGGLEAO,      SDLVK_F4);
	BM(AID_TOGGLE3RD,     SDLVK_F5);
	BM(AID_SLOT_L,        SDLVK_y);
	BM(AID_SLOT_R,        SDLVK_u);
	BM(AID_FLY_UP,        SDLVK_c);
	BM(AID_FLY_DOWN,      SDLVK_x);
	BM(AID_CHAT_CMD,      SDLVK_SLASH);
#elif defined(USE_NATIVE_ANDROID)
	// -- Original xperia play controls
	//BM(AID_FORWARD,       AKEYCODE_DPAD_UP);
	//BM(AID_LEFT,          AKEYCODE_DPAD_LEFT);
	//BM(AID_BACKWARD,      AKEYCODE_DPAD_DOWN);
	//BM(AID_RIGHT,         AKEYCODE_DPAD_RIGHT);
	//BM(AID_JUMP,          AKEYCODE_DPAD_CENTER);
	//BM(AID_DESTROY,       AKEYCODE_BUTTON_L1);
	//BM(AID_PLACE,         AKEYCODE_BUTTON_R1);
	//BM(AID_MENU_UP,       AKEYCODE_DPAD_UP);
	//BM(AID_MENU_DOWN,     AKEYCODE_DPAD_DOWN);
	//BM(AID_MENU_LEFT,     AKEYCODE_DPAD_LEFT);
	//BM(AID_MENU_RIGHT,    AKEYCODE_DPAD_RIGHT);
	//BM(AID_MENU_OK,       AKEYCODE_DPAD_CENTER);
	//BM(AID_MENU_CANCEL,   AKEYCODE_BACK);
	//custom
	//BM(AID_INVENTORY,     AKEYCODE_BUTTON_Y);
	//BM(AID_SLOT_R,        AKEYCODE_BACK);
	//BM(AID_SLOT_L,        AKEYCODE_BUTTON_X);
	//BM(AID_FLY_UP,        AKEYCODE_BUTTON_R1);
	//BM(AID_FLY_DOWN,      AKEYCODE_BUTTON_L1);

	//use controller input on android for now.
	BM(AID_FORWARD,       AKEYCODE_W);
	BM(AID_LEFT,          AKEYCODE_A);
	BM(AID_BACKWARD,      AKEYCODE_S);
	BM(AID_RIGHT,         AKEYCODE_D);
	BM(AID_JUMP,	      AKEYCODE_BUTTON_A);
	BM(AID_DESTROY,       AKEYCODE_X);
	BM(AID_PLACE,         AKEYCODE_C);
	BM(AID_MENU_UP,       AKEYCODE_DPAD_UP);
	BM(AID_MENU_DOWN,     AKEYCODE_DPAD_DOWN);
	BM(AID_MENU_LEFT,     AKEYCODE_DPAD_LEFT);
	BM(AID_MENU_RIGHT,    AKEYCODE_DPAD_RIGHT);
	BM(AID_MENU_TAB_LEFT, AKEYCODE_BUTTON_L1);
	BM(AID_MENU_TAB_RIGHT, AKEYCODE_BUTTON_R1);
	BM(AID_MENU_OK,       AKEYCODE_ENTER);
	BM(AID_MENU_CANCEL,	  AKEYCODE_BUTTON_B);
	BM(AID_MENU_PAUSE,	  AKEYCODE_BUTTON_START);
	// custom
	BM(AID_SLOT_L,		  AKEYCODE_BUTTON_L1);
	BM(AID_SLOT_R,		  AKEYCODE_BUTTON_R1);
	BM(AID_DROP,          AKEYCODE_Q);
	BM(AID_CHAT,          AKEYCODE_T);
	BM(AID_FOG,           AKEYCODE_F);
	BM(AID_CRAFTING,	  AKEYCODE_BUTTON_X);
	BM(AID_INVENTORY,	  AKEYCODE_BUTTON_Y);
	BM(AID_SNEAK,         AKEYCODE_SHIFT_LEFT);
	BM(AID_SLOT_1,        AKEYCODE_1);
	BM(AID_SLOT_2,        AKEYCODE_2);
	BM(AID_SLOT_3,        AKEYCODE_3);
	BM(AID_SLOT_4,        AKEYCODE_4);
	BM(AID_SLOT_5,        AKEYCODE_5);
	BM(AID_SLOT_6,        AKEYCODE_6);
	BM(AID_SLOT_7,        AKEYCODE_7);
	BM(AID_SLOT_8,        AKEYCODE_8);
	BM(AID_SLOT_9,        AKEYCODE_9);
	BM(AID_CONTAINER_QUICKMOVE, AKEYCODE_BUTTON_Y);
	BM(AID_CONTAINER_SPLIT,     AKEYCODE_BUTTON_X);
	BM(AID_TOGGLEGUI,     AKEYCODE_F1);
	BM(AID_SCREENSHOT,    AKEYCODE_F2);
	BM(AID_TOGGLEDEBUG,   AKEYCODE_F3);
	BM(AID_TOGGLEAO,      AKEYCODE_F4);
	BM(AID_TOGGLE3RD,     AKEYCODE_F5);
	BM(AID_FLY_UP,        AKEYCODE_C);
	BM(AID_FLY_DOWN,      AKEYCODE_X);
	BM(AID_CHAT_CMD,      AKEYCODE_SLASH);
#endif
#undef BM

#define BTN(idx,code) m_inputMappings[idx].info.controllerButtonId = code
	BTN(AID_TOGGLEDEBUG,		 GameController::BUTTON_GUIDE);
	BTN(AID_JUMP,				 GameController::BUTTON_A);
	BTN(AID_MENU_UP,			 GameController::BUTTON_DPAD_UP);
	BTN(AID_MENU_DOWN,			 GameController::BUTTON_DPAD_DOWN);
	BTN(AID_MENU_LEFT,			 GameController::BUTTON_DPAD_LEFT);
	BTN(AID_MENU_RIGHT,			 GameController::BUTTON_DPAD_RIGHT);
	BTN(AID_MENU_TAB_LEFT,		 GameController::BUTTON_LEFTSHOULDER);
	BTN(AID_MENU_TAB_RIGHT,		 GameController::BUTTON_RIGHTSHOULDER);
	BTN(AID_MENU_OK,			 GameController::BUTTON_A);
	BTN(AID_MENU_CANCEL,		 GameController::BUTTON_B);
	BTN(AID_MENU_PAUSE,			 GameController::BUTTON_START);
	BTN(AID_DROP,				 GameController::BUTTON_B);
	BTN(AID_CHAT,				 GameController::BUTTON_BACK);
	BTN(AID_CRAFTING,			 GameController::BUTTON_X);
	BTN(AID_INVENTORY,			 GameController::BUTTON_Y);
	BTN(AID_SNEAK,				 GameController::BUTTON_RIGHTSTICK);
	BTN(AID_CONTAINER_QUICKMOVE, GameController::BUTTON_Y);
	BTN(AID_CONTAINER_SPLIT,	 GameController::BUTTON_X);
	BTN(AID_TOGGLE3RD,			 GameController::BUTTON_LEFTSTICK);
	BTN(AID_SLOT_L,				 GameController::BUTTON_LEFTSHOULDER);
	BTN(AID_SLOT_R,				 GameController::BUTTON_RIGHTSHOULDER);
	BTN(AID_FLY_UP,				 GameController::BUTTON_A);
	BTN(AID_FLY_DOWN,			 GameController::BUTTON_RIGHTSTICK);
#undef BTN
}

void Options::reset()
{
	for (HashMap<std::string, OptionEntry*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
	{
		it.value()->reset();
	}
}

void Options::resetCategory(OptionsCategory cat)
{
	CategoryMap::iterator it = m_categoryOptions.find(cat);
	if (it != m_categoryOptions.end())
	{
		std::vector<OptionEntry*>& entries = it->second;
		for (size_t i = 0; i < entries.size(); i++)
			entries[i]->reset();
	}
	save();
}

UITheme Options::getUiTheme() const
{
	return UITheme(m_uiTheme.get());
}

LogoType Options::getLogoType() const
{
	if (m_logoType.get() == LOGO_AUTO)
	{
		switch (m_uiTheme.get())
		{
		case UI_POCKET:
			return LOGO_POCKET;
		case UI_JAVA:
			return LOGO_JAVA;
		case UI_CONSOLE:
#if MC_PLATFORM_XBOX360
			return LOGO_XBOX360;
#else
			return LOGO_CONSOLE;
#endif
		default:
			return (LogoType) m_logoType.get();
		}
	}
	else
		return (LogoType) m_logoType.get();
}

void Options::initResourceDependentOptions()
{
	if (!GrassColor::isAvailable() && !FoliageColor::isAvailable())
		m_biomeColors.set(false);

	if (!Screen::isMenuPanoramaAvailable())
		m_menuPanorama.set(false);

	if (!AppPlatform::singleton()->isVSyncSwitchable())
		m_vSync.set(true);
}

const std::string& OptionEntry::getDisplayName() const
{
	return Language::get(getName());
}

std::string OptionEntry::getDisplayValue() const
{
	std::string str;
	save(str);
	return str;
}

std::string OptionEntry::getMessage() const
{
	return Util::format(Language::get("options.value").c_str(), getDisplayName().c_str(), getDisplayValue().c_str());
}

void OptionEntry::addGuiElement(std::vector<GuiElement*>& elements, UITheme uiTheme)
{
	elements.push_back(new SmallButton(0, 0, this, getMessage()));
}

void FloatOption::load(const std::string& value)
{
	set(Options::readFloat(value));
}

std::string FloatOption::getDisplayValue() const
{
	return get() == 0.0f ? Language::get("options.off") : Util::toString(int(get() * 100)) + "%";
}

void FloatOption::addGuiElement(std::vector<GuiElement*>& elements, UITheme uiTheme)
{
	elements.push_back(new SliderButton(0, 0, 200, uiTheme == UI_CONSOLE ? 32 : 20, this, getMessage(), toFloat()));
}

void IntOption::load(const std::string& value)
{
	set(Options::readInt(value));
}

void BoolOption::load(const std::string& value)
{
	set(Options::readBool(value));
}

void BoolOption::save(std::string& str) const
{
	str = Options::saveBool(get());
}

std::string BoolOption::getDisplayValue() const
{
	return Language::get(get() ? "options.on" : "options.off");
}

void BoolOption::addGuiElement(std::vector<GuiElement*>& elements, UITheme uiTheme)
{
	if (uiTheme == UI_CONSOLE)
		elements.push_back(new TickBox(0, 0, this, getDisplayName()));
	else
		elements.push_back(new SwitchButton(0, 0, this, getDisplayName()));
}

std::string ThirdPersonOption::getDisplayValue() const
{
	switch (get())
	{
		case 0: return Language::get("options.thirdPerson.off");
		case 1: return Language::get("options.thirdPerson.behind");
		case 2: return Language::get("options.thirdPerson.front");
	}
	return "";
}

void ThirdPersonOption::addGuiElement(std::vector<GuiElement*>& elements, UITheme uiTheme)
{
	if (uiTheme == UI_CONSOLE)
		elements.push_back(new SliderButton(0, 0, 200, 32, this, getMessage(), toFloat()));
	else
		elements.push_back(new SwitchValuesButton(0, 0, this, getDisplayName()));
}

std::string ValuesOption::getDisplayValue() const
{
	return Language::get(getValue());
}

void MinMaxOption::addGuiElement(std::vector<GuiElement*>& elements, UITheme uiTheme)
{
	if (uiTheme == UI_CONSOLE)
	{
		elements.push_back(new SliderButton(0, 0, 200, 32, this, getMessage(), toFloat()));
	}
	else
	{
		if (m_bIsSlider)
			elements.push_back(new SliderButton(0, 0, 200, 32, this, getMessage(), toFloat()));
		else
			elements.push_back(new SwitchValuesButton(0, 0, this, getDisplayName()));
	}
}

std::string SensitivityOption::getDisplayValue() const
{
	return get() == 0.0f ? Language::get("options.sensitivity.min") : get() == 1.0f ? Language::get("options.sensitivity.max") : Util::toString(int(get() * 200)) + "%";
}

void AOOption::apply()
{
	Minecraft::useAmbientOcclusion = get();
	if (m_pMinecraft->m_pLevelRenderer)
		m_pMinecraft->m_pLevelRenderer->allChanged();
}

void GuiScaleOption::apply()
{
	m_pMinecraft->sizeUpdate();
}

void GammaOption::apply()
{
	if (!m_pMinecraft->m_pGameRenderer)
		return;
	apply(*m_pMinecraft->m_pGameRenderer);
}

void GammaOption::apply(GameRenderer& gameRenderer)
{
	// Budget rounding since the 360 just doesn't have a round function
	// @TODO: Then again, we don't need this level or precision to begin with
	// I just don't wanna have to rework the SliderButton to support integers
	gameRenderer.setGamma((float)Mth::floor(get() * 100) / 100);
}

std::string GammaOption::getDisplayValue() const
{
	return Util::toString(int(get() * 100)) + "%";
}

void FovOption::apply()
{
	if (!m_pMinecraft || !m_pMinecraft->m_pGameRenderer)
		return;
	m_pMinecraft->m_pGameRenderer->setFovBase(get());
}

std::string FovOption::getDisplayValue() const
{
	float val = get();
	if (val == 70.0f)
		return Language::get("options.fov.normal");
	if (val == 110.0f)
		return Language::get("options.fov.quakePro");
	return Util::toString(int(val));
}

void GraphicsOption::apply()
{
	if (m_pMinecraft->m_pLevelRenderer)
		m_pMinecraft->m_pLevelRenderer->allChanged();
}

void FancyGraphicsOption::apply()
{
	m_pMinecraft->reloadFancy(get());
}

std::string FancyGraphicsOption::getMessage() const
{
	return Util::format(Language::get("options.value").c_str(), Language::get("options.graphics").c_str(), Language::get(get() ? "options.graphics.fancy" : "options.graphics.fast").c_str());
}

void VsyncOption::apply()
{
	AppPlatform::singleton()->setVSyncEnabled(get());
}

void LogoTypeOption::apply()
{
	if (m_pMinecraft->getOptions())
	{
		LogoRenderer::singleton().init(m_pMinecraft);
		LogoRenderer::singleton().build();
	}
}

void SwapJumpSneakOption::apply()
{
	if (!m_pMinecraft || !m_pMinecraft->m_pInputHolder)
		return;
	m_pMinecraft->m_pInputHolder->setScreenSize(Minecraft::GetViewportSize());
}

void DpadSizeOption::apply()
{
	if (!m_pMinecraft || !m_pMinecraft->m_pInputHolder)
		return;
	m_pMinecraft->m_pInputHolder->setScreenSize(Minecraft::GetViewportSize());
}

std::string HUDSizeOption::getDisplayValue() const
{
	return Util::toString(get() - 1);
}

void UIThemeOption::apply()
{
	Minecraft& mc = *m_pMinecraft;

	if (mc.getOptions())
	{
		mc.getOptions()->m_logoType.apply();
	}

	//if (m_pMinecraft->m_pFont)
	//	m_pMinecraft->m_pFont->clearTextObjectCache();
	m_pMinecraft->reloadFont();
}
