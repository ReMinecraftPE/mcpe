/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <fstream>

// for SDL 1.2 controller buttons
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


void Options::_initDefaultValues()
{
	field_244 = 1.0f;
	field_248 = 1.0f;
	field_23E = 0;
	field_241 = false;
	field_24C = 0;
	field_16  = 0;
	field_240 = 1;
	field_1C = "Default";
	field_19 = 1;

#ifdef ORIGINAL_CODE
	m_viewDistance.set(2);
	m_thirdPerson.set(0);
	field_19 = 0;
#endif

	loadControls();
}

Options::Options(Minecraft* mc, const std::string& folderPath) :
	m_pMinecraft(mc)
	, m_musicVolume("audio_music", "options.music", 1.0f)
	, m_masterVolume("audio_master", "options.sound", 1.0f)
	, m_sensitivity("ctrl_sensitivity", "options.sensitivity", 0.5f)
	, m_invertMouse("ctrl_invertmouse", "options.invertMouse", false)
	, m_viewDistance("gfx_viewdistance", "options.renderDistance", 1, ValuesBuilder().add("options.renderDistance.far").add("options.renderDistance.normal").add("options.renderDistance.short").add("options.renderDistance.tiny"))
	, m_viewBobbing("gfx_bobview", "options.viewBobbing", false)
	, m_anaglyphs("gfx_3danaglyphs", "options.anaglyph", false)
	, m_fancyGraphics("gfx_fancygraphics", "options.fancyGraphics", true)
	, m_ambientOcclusion("gfx_smoothlighting", "options.ao", Minecraft::useAmbientOcclusion)
	, m_difficulty("misc_difficulty", "options.difficulty", 2, ValuesBuilder().add("options.difficulty.peaceful").add("options.difficulty.easy").add("options.difficulty.normal").add("options.difficulty.hard"))
	, m_hideGui("gfx_hidegui", "options.hideGui", false)
	, m_thirdPerson("gfx_thirdperson", "options.thirdPerson", false)
	, m_flightHax("misc_flycheat", "options.flightHax", false)
	, m_playerName("mp_username", "options.username", "Steve")
	, m_serverVisibleDefault("mp_server_visible_default", "options.serverVisibleDefault", true)
	, m_autoJump("ctrl_autojump", "options.autoJump", false)
	, m_debugText("info_debugtext", "options.debugText", false)
	, m_blockOutlines("gfx_blockoutlines", "options.blockOutlines", false)
	, m_fancyGrass("gfx_fancygrass", "options.fancyGrass", true)
	, m_biomeColors("gfx_biomecolors", "options.biomeColors", true)
	, m_splitControls("ctrl_split", "options.splitControls", false)
	, m_bUseController("ctrl_usecontroller", "options.useController", false)
	, m_dynamicHand("gfx_dynamichand", "options.dynamicHand", false)
	, m_b2dTitleLogo("misc_oldtitle", "options.2dTitleLogo", false)
	, m_menuPanorama("misc_menupano", "options.menuPanorama", true)
	, m_guiScale("gfx_guiscale", "options.guiScale", 0, ValuesBuilder().add("options.guiScale.auto").add("options.guiScale.small").add("options.guiScale.normal").add(("options.guiScale.large")))
	, m_lang("gfx_lang", "options.lang", "en_us")
	//, m_limitFramerate("gfx_fpslimit", "options.framerateLimit", 0, ValuesBuilder().add(performance.max").add("performance.balanced").add("performance.powersaver"))
	//, m_bMipmaps("gfx_mipmaps", "options.mipmaps")
	//, m_moreWorldOptions("misc_moreworldoptions", "options.moreWorldOptions", true)
	//, m_vSync("enableVsync", "options.enableVsync")
{
	add(m_musicVolume);
	add(m_masterVolume);
	add(m_invertMouse);
	add(m_difficulty);
	add(m_splitControls);
	add(m_sensitivity);
	add(m_viewDistance);
	add(m_viewBobbing);
	add(m_anaglyphs);
	add(m_fancyGraphics);
	add(m_fancyGrass);
	add(m_biomeColors);
	add(m_ambientOcclusion);
	add(m_guiScale);
	//add(m_limitFramerate);
	add(m_autoJump);
	//add(m_bMipmaps);
	//add(m_moreWorldOptions);
	add(m_blockOutlines);
	add(m_dynamicHand);
	add(m_menuPanorama);
	add(m_b2dTitleLogo);
	add(m_thirdPerson);
	add(m_hideGui);
	add(m_playerName);
	add(m_debugText);
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

void Options::_load()
{
	std::vector<std::string> strings = readPropertiesFromFile(m_filePath);

	for (size_t i = 0; i < strings.size(); i += 2)
	{
		std::string key = strings[i], value = strings[i + 1];

		std::map<std::string, OptionEntry*>::iterator opt = m_options.find(key);
		if (opt != m_options.end())
			opt->second->load(value);
		else if (key == "gfx_resourcepacks")
			readPackArray(value, m_resourcePacks);
	}
}

void Options::save()
{
	savePropertiesToFile(m_filePath, getOptionStrings());
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
		// Search internally (within assets) first
		location.fileSystem = ResourceLocation::APP_PACKAGE;
		location.path = ResourcePackRepository::RESOURCE_PACKS_PATH + "/" + fullarray[i];
		std::string fullPath = location.getFullPath();
		if (!isDirectory(fullPath.c_str()))
		{
			// Search externally (within user-writable external storage dir)
			location.fileSystem = ResourceLocation::EXTERNAL_DIR;
			fullPath = location.getFullPath();
			if (!isDirectory(fullPath.c_str()))
			{
				LOG_W("Failed to find resource pack: %s", fullPath.c_str());
				continue;
			}
		}
		array.push_back(ResourcePack(fullarray[i], location.fileSystem));
	}
}

std::string Options::saveBool(bool b)
{
	return b ? "true" : "false";
}

std::string Options::saveInt(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

std::string Options::saveFloat(float f)
{
	std::stringstream ss;
	ss << f;
	return ss.str();
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

	os << "#Config file for Minecraft PE.  The # at the start denotes a comment, removing it makes it a command.\n\n";

	for (size_t i = 0; i < properties.size(); i += 2)
		os << properties[i] << ':' << properties[i + 1] << '\n';

	os.close();

	platform.endProfileDataWrite(0);
}

std::vector<std::string> Options::getOptionStrings()
{
	std::vector<std::string> vec;

#define SO(optname, value) do { vec.push_back(optname); vec.push_back(value); } while (0)

	for (std::map<std::string, OptionEntry*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
	{
		SO(it->first, it->second->save());
	}
	SO("gfx_resourcepacks", savePackArray(m_resourcePacks));

	return vec;
}

void Options::loadControls()
{
	// Win32 key codes are being used by default
#define KM(idx, name, code) m_keyMappings[idx] = KeyMapping(name, code)
	KM(KM_FORWARD,      "key.forward",       'W');
	KM(KM_LEFT,         "key.left",          'A');
	KM(KM_BACKWARD,     "key.back",          'S');
	KM(KM_RIGHT,        "key.right",         'D');
	KM(KM_JUMP,         "key.jump",          ' ');
	KM(KM_INVENTORY,    "key.inventory",     'E');
	KM(KM_DROP,         "key.drop",          'Q');
	KM(KM_CHAT,         "key.chat",          'T');
	KM(KM_FOG,          "key.fog",           'F');
	KM(KM_SNEAK,        "key.sneak",         0x10); // VK_SHIFT. In original, it's 10 (misspelling?)
	KM(KM_DESTROY,      "key.destroy",       'K');  // was 'X'
	KM(KM_PLACE,        "key.place",         'L');  // was 'C'
	KM(KM_MENU_UP,      "key.menu.up",       0x26); // VK_UP
	KM(KM_MENU_DOWN,    "key.menu.down",     0x28); // VK_DOWN
	KM(KM_MENU_LEFT,    "key.menu.left",     0x25); // VK_LEFT
	KM(KM_MENU_RIGHT,   "key.menu.right",    0x27); // VK_RIGHT
	KM(KM_MENU_OK,      "key.menu.ok",       0x0D); // VK_RETURN
	KM(KM_MENU_CANCEL,  "key.menu.cancel",   0x1B); // VK_ESCAPE, was 0x08 = VK_BACK
	KM(KM_SLOT_1,       "key.slot.1",        '1');
	KM(KM_SLOT_2,       "key.slot.2",        '2');
	KM(KM_SLOT_3,       "key.slot.3",        '3');
	KM(KM_SLOT_4,       "key.slot.4",        '4');
	KM(KM_SLOT_5,       "key.slot.5",        '5');
	KM(KM_SLOT_6,       "key.slot.6",        '6');
	KM(KM_SLOT_7,       "key.slot.7",        '7');
	KM(KM_SLOT_8,       "key.slot.8",        '8');
	KM(KM_SLOT_9,       "key.slot.9",        '9');
	KM(KM_SLOT_L,       "key.slot.left",     'Y');
	KM(KM_SLOT_R,       "key.slot.right",    'U');
	KM(KM_CONTAINER_QUICKMOVE, "key.container.quickmove", 0x10); // VK_SHIFT
	KM(KM_CONTAINER_SPLIT,     "key.container.split",     0xBF); // VK_OEM_2 (keymap is unused on mouse & keyboard)
	KM(KM_TOGGLEGUI,    "key.fn.gui",        0x70); // VK_F1
	KM(KM_SCREENSHOT,   "key.fn.screenshot", 0x71); // VK_F2
	KM(KM_TOGGLEDEBUG,  "key.fn.debug",      0x72); // VK_F3
	KM(KM_TOGGLEAO,     "key.fn.ao",         0x73); // VK_F4
	KM(KM_TOGGLE3RD,    "key.fn.3rd",        0x74); // VK_F5
	KM(KM_FLY_UP,       "key.fly.up",        'X');
	KM(KM_FLY_DOWN,     "key.fly.down",      'C');
	KM(KM_CHAT_CMD,     "key.chat.cmd",      0xBF); // VK_OEM_2
#undef KM

	// @TODO: These should **really** not be defined in here. How about AppPlatform?

#define KM(idx,code) m_keyMappings[idx].value = code
#ifdef USE_SDL
	KM(KM_FORWARD,       SDLVK_w);
	KM(KM_LEFT,          SDLVK_a);
	KM(KM_BACKWARD,      SDLVK_s);
	KM(KM_RIGHT,         SDLVK_d);
	KM(KM_JUMP,          SDLVK_SPACE);
	KM(KM_DESTROY,       SDLVK_x);
	KM(KM_PLACE,         SDLVK_c);
	KM(KM_MENU_UP,       SDLVK_UP);
	KM(KM_MENU_DOWN,     SDLVK_DOWN);
	KM(KM_MENU_LEFT,     SDLVK_LEFT);
	KM(KM_MENU_RIGHT,    SDLVK_RIGHT);
	KM(KM_MENU_OK,       SDLVK_RETURN);
	KM(KM_MENU_CANCEL,   SDLVK_ESCAPE);
	KM(KM_DROP,          SDLVK_q);
	KM(KM_CHAT,          SDLVK_t);
	KM(KM_FOG,           SDLVK_f);
	KM(KM_INVENTORY,     SDLVK_e);
	KM(KM_SNEAK,         SDLVK_LSHIFT);
	KM(KM_SLOT_1,        SDLVK_1);
	KM(KM_SLOT_2,        SDLVK_2);
	KM(KM_SLOT_3,        SDLVK_3);
	KM(KM_SLOT_4,        SDLVK_4);
	KM(KM_SLOT_5,        SDLVK_5);
	KM(KM_SLOT_6,        SDLVK_6);
	KM(KM_SLOT_7,        SDLVK_7);
	KM(KM_SLOT_8,        SDLVK_8);
	KM(KM_SLOT_9,        SDLVK_9);
	KM(KM_CONTAINER_QUICKMOVE, SDLVK_LSHIFT);
	KM(KM_TOGGLEGUI,     SDLVK_F1);
	KM(KM_SCREENSHOT,    SDLVK_F2);
	KM(KM_TOGGLEDEBUG,   SDLVK_F3);
	KM(KM_TOGGLEAO,      SDLVK_F4);
	KM(KM_TOGGLE3RD,     SDLVK_F5);
	KM(KM_SLOT_L,        SDLVK_y);
	KM(KM_SLOT_R,        SDLVK_u);
	KM(KM_FLY_UP,        SDLVK_c);
	KM(KM_FLY_DOWN,      SDLVK_x);
	KM(KM_CHAT_CMD,      SDLVK_SLASH);
#elif defined(USE_NATIVE_ANDROID)
	// -- Original xperia play controls
	//KM(KM_FORWARD,       AKEYCODE_DPAD_UP);
	//KM(KM_LEFT,          AKEYCODE_DPAD_LEFT);
	//KM(KM_BACKWARD,      AKEYCODE_DPAD_DOWN);
	//KM(KM_RIGHT,         AKEYCODE_DPAD_RIGHT);
	//KM(KM_JUMP,          AKEYCODE_DPAD_CENTER);
	//KM(KM_DESTROY,       AKEYCODE_BUTTON_L1);
	//KM(KM_PLACE,         AKEYCODE_BUTTON_R1);
	//KM(KM_MENU_UP,       AKEYCODE_DPAD_UP);
	//KM(KM_MENU_DOWN,     AKEYCODE_DPAD_DOWN);
	//KM(KM_MENU_LEFT,     AKEYCODE_DPAD_LEFT);
	//KM(KM_MENU_RIGHT,    AKEYCODE_DPAD_RIGHT);
	//KM(KM_MENU_OK,       AKEYCODE_DPAD_CENTER);
	//KM(KM_MENU_CANCEL,   AKEYCODE_BACK);
	//custom
	//KM(KM_INVENTORY,     AKEYCODE_BUTTON_Y);
	//KM(KM_SLOT_R,        AKEYCODE_BACK);
	//KM(KM_SLOT_L,        AKEYCODE_BUTTON_X);
	//KM(KM_FLY_UP,        AKEYCODE_BUTTON_R1);
	//KM(KM_FLY_DOWN,      AKEYCODE_BUTTON_L1);

	//use controller input on android for now.
	KM(KM_FORWARD,       AKEYCODE_W);
	KM(KM_LEFT,          AKEYCODE_A);
	KM(KM_BACKWARD,      AKEYCODE_S);
	KM(KM_RIGHT,         AKEYCODE_D);
	KM(KM_JUMP,			 AKEYCODE_BUTTON_A);
	KM(KM_DESTROY,       AKEYCODE_X);
	KM(KM_PLACE,         AKEYCODE_C);
	KM(KM_MENU_UP,       AKEYCODE_DPAD_UP);
	KM(KM_MENU_DOWN,     AKEYCODE_DPAD_DOWN);
	KM(KM_MENU_LEFT,     AKEYCODE_BUTTON_L1);
	KM(KM_MENU_RIGHT,    AKEYCODE_BUTTON_R1);
	KM(KM_MENU_OK,       AKEYCODE_ENTER);
	KM(KM_MENU_CANCEL,	 AKEYCODE_BUTTON_START);
	// custom
	KM(KM_SLOT_L,		 AKEYCODE_BUTTON_L1);
	KM(KM_SLOT_R,		 AKEYCODE_BUTTON_R1);
	KM(KM_DROP,          AKEYCODE_Q);
	KM(KM_CHAT,          AKEYCODE_T);
	KM(KM_FOG,           AKEYCODE_F);
	KM(KM_INVENTORY,	 AKEYCODE_BUTTON_Y);
	KM(KM_SNEAK,         AKEYCODE_SHIFT_LEFT);
	KM(KM_SLOT_1,        AKEYCODE_1);
	KM(KM_SLOT_2,        AKEYCODE_2);
	KM(KM_SLOT_3,        AKEYCODE_3);
	KM(KM_SLOT_4,        AKEYCODE_4);
	KM(KM_SLOT_5,        AKEYCODE_5);
	KM(KM_SLOT_6,        AKEYCODE_6);
	KM(KM_SLOT_7,        AKEYCODE_7);
	KM(KM_SLOT_8,        AKEYCODE_8);
	KM(KM_SLOT_9,        AKEYCODE_9);
	KM(KM_CONTAINER_QUICKMOVE, AKEYCODE_BUTTON_Y);
	KM(KM_CONTAINER_SPLIT,     AKEYCODE_BUTTON_X);
	KM(KM_TOGGLEGUI,     AKEYCODE_F1);
	KM(KM_SCREENSHOT,    AKEYCODE_F2);
	KM(KM_TOGGLEDEBUG,   AKEYCODE_F3);
	KM(KM_TOGGLEAO,      AKEYCODE_F4);
	KM(KM_TOGGLE3RD,     AKEYCODE_F5);
	KM(KM_FLY_UP,        AKEYCODE_C);
	KM(KM_FLY_DOWN,      AKEYCODE_X);
	KM(KM_CHAT_CMD,      AKEYCODE_SLASH);
#endif
#undef KM

	if (m_bUseController.get())
	{
#define KM(idx,code) m_keyMappings[idx].value = code
#ifdef USE_SDL
		KM(KM_TOGGLEDEBUG,   SDL_CONTROLLER_BUTTON_GUIDE);
		KM(KM_JUMP,          SDL_CONTROLLER_BUTTON_A);
		KM(KM_MENU_UP,       SDL_CONTROLLER_BUTTON_DPAD_UP);
		KM(KM_MENU_DOWN,     SDL_CONTROLLER_BUTTON_DPAD_DOWN);
		KM(KM_MENU_LEFT,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
		KM(KM_MENU_RIGHT,    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
		KM(KM_MENU_OK,       SDL_CONTROLLER_BUTTON_A);
		KM(KM_MENU_CANCEL,   SDL_CONTROLLER_BUTTON_B);
		KM(KM_DROP,          SDL_CONTROLLER_BUTTON_B);
		KM(KM_CHAT,          SDL_CONTROLLER_BUTTON_BACK);
		KM(KM_INVENTORY,     SDL_CONTROLLER_BUTTON_Y);
		KM(KM_SNEAK,         SDL_CONTROLLER_BUTTON_RIGHTSTICK);
		KM(KM_CONTAINER_QUICKMOVE, SDL_CONTROLLER_BUTTON_Y);
		KM(KM_CONTAINER_SPLIT,     SDL_CONTROLLER_BUTTON_X);
		KM(KM_TOGGLE3RD,     SDL_CONTROLLER_BUTTON_LEFTSTICK);
		KM(KM_SLOT_L,        SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
		KM(KM_SLOT_R,        SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
		KM(KM_FLY_UP,        SDL_CONTROLLER_BUTTON_A);
		KM(KM_FLY_DOWN,      SDL_CONTROLLER_BUTTON_RIGHTSTICK);
#else
		KM(KM_TOGGLEDEBUG,   GameController::BUTTON_GUIDE);
		KM(KM_JUMP,          GameController::BUTTON_A);
		KM(KM_MENU_UP,       GameController::BUTTON_DPAD_UP);
		KM(KM_MENU_DOWN,     GameController::BUTTON_DPAD_DOWN);
		KM(KM_MENU_LEFT,     GameController::BUTTON_LEFTSHOULDER);
		KM(KM_MENU_RIGHT,    GameController::BUTTON_RIGHTSHOULDER);
		KM(KM_MENU_OK,       GameController::BUTTON_A);
		KM(KM_MENU_CANCEL,   GameController::BUTTON_B);
		KM(KM_DROP,          GameController::BUTTON_B);
		KM(KM_CHAT,          GameController::BUTTON_BACK);
		KM(KM_INVENTORY,     GameController::BUTTON_Y);
		KM(KM_SNEAK,         GameController::BUTTON_RIGHTSTICK);
		KM(KM_CONTAINER_QUICKMOVE, GameController::BUTTON_Y);
		KM(KM_CONTAINER_SPLIT,     GameController::BUTTON_X);
		KM(KM_TOGGLE3RD,     GameController::BUTTON_LEFTSTICK);
		KM(KM_SLOT_L,        GameController::BUTTON_LEFTSHOULDER);
		KM(KM_SLOT_R,        GameController::BUTTON_RIGHTSHOULDER);
		KM(KM_FLY_UP,        GameController::BUTTON_A);
		KM(KM_FLY_DOWN,      GameController::BUTTON_RIGHTSTICK);
#endif
#undef KM
	}
}

void Options::initResourceDependentOptions()
{
	if (!GrassColor::isAvailable() && !FoliageColor::isAvailable())
		m_biomeColors.set(false);

	if (!Screen::isMenuPanoramaAvailable())
		m_menuPanorama.set(false);
}

const std::string& Options::OptionEntry::getName() const
{
	return Language::get(m_name);
}

std::string Options::OptionEntry::getMessage() const
{
	return Util::format(Language::get("options.value").c_str(), getName().c_str(), getDisplayValue().c_str());
}

void Options::OptionEntry::addGuiElement(std::vector<GuiElement*>& elements, const std::string& text)
{
	elements.push_back(new SmallButton(0, 0, 0, this, text));
}

void Options::AOOption::apply()
{
	Minecraft::useAmbientOcclusion = get();
	if (m_pMinecraft->m_pLevelRenderer)
		m_pMinecraft->m_pLevelRenderer->allChanged();
}

void Options::GuiScaleOption::apply()
{
	m_pMinecraft->sizeUpdate(Minecraft::width, Minecraft::height);
}

std::string Options::FloatOption::getDisplayValue() const
{
	return get() == 0.0f ? Language::get("options.off") : saveInt(get() * 100) + "%";
}

void Options::FloatOption::addGuiElement(std::vector<GuiElement*>& elements, const std::string& text)
{
	elements.push_back(new SliderButton(0, 0, 0, this, getMessage(), get()));
}

std::string Options::BoolOption::getDisplayValue() const
{
	return Language::get(get() ? "options.on" : "options.off");
}

void Options::BoolOption::addGuiElement(std::vector<GuiElement*>& elements, const std::string& text)
{
	elements.push_back(new SwitchButton(0, 0, 0, this, text));
}

void Options::ValuesOption::addGuiElement(std::vector<GuiElement*>& elements, const std::string& text)
{
	elements.push_back(new SwitchValuesButton(0, 0, 0, this, text));
}

void Options::GraphicsOption::apply()
{
	if (m_pMinecraft->m_pLevelRenderer)
		m_pMinecraft->m_pLevelRenderer->allChanged();
}

std::string Options::FancyGraphicsOption::getMessage() const
{
	return Util::format(Language::get("options.value").c_str(), Language::get("options.graphics").c_str(), Language::get(get() ? "options.graphics.fancy" : "options.graphics.fast").c_str());
}

std::string Options::SensitivityOption::getDisplayValue() const
{
	return get() == 0.0f ? Language::get("options.sensitivity.min") : get() == 1.0f ? Language::get("options.sensitivity.max") : saveInt(get() * 200) + "%";
}