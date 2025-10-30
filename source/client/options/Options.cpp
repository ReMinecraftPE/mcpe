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

#include "client/renderer/PatchManager.hpp"
#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"

Options::Option
	Options::Option::MUSIC            (0,  "options.music",          true,  false),
	Options::Option::SOUND            (1,  "options.sound",          true,  false),
	Options::Option::INVERT_MOUSE     (2,  "options.invertMouse",    false, true),
	Options::Option::SENSITIVITY      (3,  "options.sensitivity",    true,  false),
	Options::Option::RENDER_DISTANCE  (4,  "options.renderDistance", false, false),
	Options::Option::VIEW_BOBBING     (5,  "options.viewBobbing",    false, true),
	Options::Option::ANAGLYPH         (6,  "options.anaglyph",       false, true),
	Options::Option::LIMIT_FRAMERATE  (7,  "options.limitFramerate", false, true),
	Options::Option::DIFFICULTY       (8,  "options.difficulty",     false, false),
	Options::Option::GRAPHICS         (9,  "options.graphics",       false, false),
	Options::Option::AMBIENT_OCCLUSION(10, "options.ao",             false, true),
	Options::Option::GUI_SCALE        (11, "options.guiScale",       false, false);

void Options::_initDefaultValues()
{
	m_difficulty = 2;
	field_244 = 1.0f;
	m_bDontRenderGui = false;
	field_248 = 1.0f;
	m_bThirdPerson = false;
	m_fMusicVolume = 1.0f;
	field_23E = 0;
	m_fMasterVolume = 1.0f;
	m_bFlyCheat = false;
	field_241 = false;
	m_fSensitivity   = 0.5f;
	field_24C = 0;
	m_bInvertMouse = false;
	m_bAnaglyphs = false;
	field_16  = 0;
	m_bAmbientOcclusion = Minecraft::useAmbientOcclusion;
	field_240 = 1;
	m_iViewDistance = 2;
	m_bViewBobbing  = 1;
	m_bAutoJump = true;
	m_bFancyGraphics = true;
	field_1C = "Default";
	m_playerName = "Steve";
	m_bServerVisibleDefault = true;
	m_bDebugText = false;
	m_bBlockOutlines = false;
	m_bFancyGrass = false;
	m_bBiomeColors = false;
	m_bSplitControls = false;
	m_bUseController = false;
	m_bDynamicHand = false;
	m_bOldTitleLogo = false;
	m_bMenuPanorama = false;
	field_19 = 1;

#ifdef ORIGINAL_CODE
	m_iViewDistance = 2;
	m_bThirdPerson = 0;
	field_19 = 0;
#endif

	loadControls();
}

Options::Options()
{
	_initDefaultValues();
}

Options::Options(const std::string& folderPath)
{
	m_filePath = folderPath + "/options.txt";
	_initDefaultValues();
	_load();
}

std::string getMessage(const Options::Option& option)
{
	return "Options::getMessage - Not implemented";
}

void Options::_load()
{
	std::vector<std::string> strings = readPropertiesFromFile(m_filePath);

	for (int i = 0; i < strings.size(); i += 2)
	{
		std::string key = strings[i], value = strings[i + 1];

		if (key == "mp_username")
			m_playerName = value;
		else if (key == "ctrl_invertmouse")
			m_bInvertMouse = readBool(value);
		else if (key == "ctrl_autojump")
			m_bAutoJump = readBool(value);
		else if (key == "ctrl_split")
			m_bSplitControls = readBool(value);
		else if (key == "gfx_fancygraphics")
			m_bFancyGraphics = readBool(value);
		else if (key == "mp_server_visible_default")
			m_bServerVisibleDefault = readBool(value);
		else if (key == "gfx_smoothlighting")
			Minecraft::useAmbientOcclusion = m_bAmbientOcclusion = readBool(value);
		else if (key == "gfx_viewdistance")
			m_iViewDistance = readInt(value);
		else if (key == "gfx_blockoutlines")
			m_bBlockOutlines = readBool(value);
		else if (key == "gfx_fancygrass")
		{
			if (!(GetPatchManager()->IsGrassSidesTinted()))
				m_bFancyGrass = false;
			else
				m_bFancyGrass = readBool(value);
		}
		else if (key == "gfx_biomecolors")
		{
			if (!GrassColor::isAvailable() && !FoliageColor::isAvailable())
				m_bBiomeColors = false;
			else
				m_bBiomeColors = readBool(value);
		}
		else if (key == "gfx_hidegui")
			m_bDontRenderGui = readBool(value);
		else if (key == "gfx_thirdperson")
			m_bThirdPerson = readBool(value);
		else if (key == "gfx_3danaglyphs")
			m_bAnaglyphs = readBool(value);
		else if (key == "gfx_dynamichand")
			m_bDynamicHand = readBool(value);
		else if (key == "misc_oldtitle")
			m_bOldTitleLogo = readBool(value);
		else if (key == "info_debugtext")
			m_bDebugText = readBool(value);
		else if (key == "misc_menupano")
		{
			m_bMenuPanorama = !Screen::isMenuPanoramaAvailable() ? false : readBool(value);
		}
	}
}

void Options::save()
{
	savePropertiesToFile(m_filePath, getOptionStrings());
}

std::string Options::getMessage(const Options::Option& option)
{
	return "Options::getMessage - Not implemented";
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

std::vector<std::string> Options::readPropertiesFromFile(const std::string& filePath)
{
	std::vector<std::string> o;

	const char* const path = filePath.c_str();
	LOG_I("Loading options from %s", path);

	std::ifstream ifs(path);
	if (!ifs.is_open())
	{
		LOG_W("%s doesn't exist, resetting to defaults", path);
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

	return o;
}

void Options::savePropertiesToFile(const std::string& filePath, std::vector<std::string> properties)
{
	assert(properties.size() % 2 == 0);

	std::ofstream os;
	os.open(filePath.c_str());
	if (!os.is_open())
	{
		LOG_E("Failed to read %s", filePath.c_str());
		return;
	}

	os << "#Config file for Minecraft PE.  The # at the start denotes a comment, removing it makes it a command.\n\n";

	for (int i = 0; i < properties.size(); i += 2)
		os << properties[i] << ':' << properties[i + 1] << '\n';
}

std::vector<std::string> Options::getOptionStrings()
{
	std::vector<std::string> vec;

#define SO(optname, value) do { vec.push_back(optname); vec.push_back(value); } while (0)

	SO("mp_username", m_playerName);
	SO("ctrl_invertmouse",          saveBool(m_bInvertMouse));
	SO("ctrl_autojump",             saveBool(m_bAutoJump));
	SO("ctrl_split",                saveBool(m_bSplitControls));
	SO("mp_server_visible_default", saveBool(m_bServerVisibleDefault));
	SO("gfx_fancygraphics",         saveBool(m_bFancyGraphics));
	SO("gfx_smoothlighting",        saveBool(m_bAmbientOcclusion));
	SO("gfx_hidegui",               saveBool(m_bDontRenderGui));
	SO("gfx_thirdperson",           saveBool(m_bThirdPerson));
	SO("gfx_3danaglyphs",           saveBool(m_bAnaglyphs));
	SO("gfx_viewdistance",          saveInt (m_iViewDistance));
	SO("gfx_blockoutlines",         saveBool(m_bBlockOutlines));
	SO("gfx_fancygrass", 			saveBool(m_bFancyGrass));
	SO("gfx_biomecolors",           saveBool(m_bBiomeColors));
	SO("gfx_dynamichand",           saveBool(m_bDynamicHand));
	SO("misc_oldtitle",             saveBool(m_bOldTitleLogo));
	SO("info_debugtext",            saveBool(m_bDebugText));
	SO("misc_menupano",			    saveBool(m_bMenuPanorama));

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
	KM(KM_DESTROY,      "key.destroy",       'K'); // was 'X'
	KM(KM_PLACE,        "key.place",         'L'); // was 'C'
	KM(KM_MENU_NEXT,    "key.menu.next",     0x28); // VK_DOWN
	KM(KM_MENU_PREVIOUS,"key.menu.previous", 0x26); // VK_UP
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
	KM(KM_MENU_NEXT,     SDLVK_DOWN);
	KM(KM_MENU_PREVIOUS, SDLVK_UP);
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
	//KM(KM_MENU_NEXT,     AKEYCODE_DPAD_DOWN);
	//KM(KM_MENU_PREVIOUS, AKEYCODE_DPAD_UP);
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
	KM(KM_MENU_NEXT,     AKEYCODE_DPAD_DOWN);
	KM(KM_MENU_PREVIOUS, AKEYCODE_DPAD_UP);
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

	if (m_bUseController)
	{
#define KM(idx,code) m_keyMappings[idx].value = code
#ifdef USE_SDL
		KM(KM_TOGGLEDEBUG,   SDL_CONTROLLER_BUTTON_GUIDE);
		KM(KM_JUMP,          SDL_CONTROLLER_BUTTON_A);
		KM(KM_MENU_NEXT,     SDL_CONTROLLER_BUTTON_DPAD_DOWN);
		KM(KM_MENU_PREVIOUS, SDL_CONTROLLER_BUTTON_DPAD_UP);
		KM(KM_MENU_OK,       SDL_CONTROLLER_BUTTON_A);
		KM(KM_MENU_CANCEL,   SDL_CONTROLLER_BUTTON_B);
		KM(KM_DROP,          SDL_CONTROLLER_BUTTON_B);
		KM(KM_CHAT,          SDL_CONTROLLER_BUTTON_BACK);
		KM(KM_INVENTORY,     SDL_CONTROLLER_BUTTON_Y);
		KM(KM_SNEAK,         SDL_CONTROLLER_BUTTON_RIGHTSTICK);
		KM(KM_TOGGLE3RD,     SDL_CONTROLLER_BUTTON_LEFTSTICK);
		KM(KM_SLOT_L,        SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
		KM(KM_SLOT_R,        SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
		KM(KM_FLY_UP,        SDL_CONTROLLER_BUTTON_A);
		KM(KM_FLY_DOWN,      SDL_CONTROLLER_BUTTON_RIGHTSTICK);
#endif
#undef KM
	}
}
