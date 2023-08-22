/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Options.hpp"
#include "Util.hpp"
#include "compat/AKeyCodes.hpp"
#include "Minecraft.hpp"

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

void Options::initDefaultValues()
{
	field_238 = 2;
	field_244 = 1.0f;
	m_bDontRenderGui = false;
	field_248 = 1.0f;
	m_bThirdPerson = false;
	field_0   = 1.0f;
	field_23E = 0;
	m_fMasterVolume = 1.0f;
	m_bFlyCheat = false;
	field_241 = 0;
	field_8   = 0.5f;
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
	field_19  = 1;
	field_1C = "Default";
	m_playerName = "Steve";
	m_bServerVisibleDefault = true;
	m_bDebugText = false;

	m_keyMappings[0] =  KeyMapping("key.forward", 'W');
	m_keyMappings[1] =  KeyMapping("key.left", 'A');
	m_keyMappings[2] =  KeyMapping("key.back", 'S');
	m_keyMappings[3] =  KeyMapping("key.right", 'D');
	m_keyMappings[4] =  KeyMapping("key.jump", ' ');
	m_keyMappings[5] =  KeyMapping("key.inventory", 'E');
	m_keyMappings[6] =  KeyMapping("key.drop", 'Q');
	m_keyMappings[7] =  KeyMapping("key.chat", 'T');
	m_keyMappings[8] =  KeyMapping("key.fog", 'F');
	m_keyMappings[9] =  KeyMapping("key.sneak", '\n');
	m_keyMappings[10] = KeyMapping("key.destroy", 'X');
	m_keyMappings[11] = KeyMapping("key.place", 'C');
	m_keyMappings[12] = KeyMapping("key.menu.next", '(');
	m_keyMappings[13] = KeyMapping("key.menu.previous", '&');
	m_keyMappings[14] = KeyMapping("key.menu.ok", '\r');
	m_keyMappings[15] = KeyMapping("key.menu.cancel", '\b');

#ifdef ORIGINAL_CODE
	m_rotY = 2;
	field_23D = 0;
	field_19 = 0;
#endif
	m_bFancyGraphics = 1;
	// keybind now reprograms some the keybinds.
	// For the restoration, we don't actually need them
	m_keyMappings[0].value = AKEYCODE_DPAD_UP;
	m_keyMappings[1].value = AKEYCODE_DPAD_LEFT;
	m_keyMappings[2].value = AKEYCODE_DPAD_DOWN;
	m_keyMappings[3].value = AKEYCODE_DPAD_RIGHT;
	m_keyMappings[4].value = AKEYCODE_DPAD_CENTER; // lmao, how stupid.
	m_keyMappings[10].value = AKEYCODE_BUTTON_L1;
	m_keyMappings[11].value = AKEYCODE_BUTTON_R1;
	m_keyMappings[12].value = AKEYCODE_DPAD_DOWN;
	m_keyMappings[13].value = AKEYCODE_DPAD_UP;
	m_keyMappings[14].value = AKEYCODE_DPAD_CENTER;
	m_keyMappings[15].value = AKEYCODE_BACK;

#ifndef ORIGINAL_CODE
	m_keyMappings[9].value = AKEYCODE_SHIFT_LEFT;
#endif
}

Options::Options()
{
	initDefaultValues();
}

std::string getMessage(const Options::Option& option)
{
	return "Options::getMessage - Not implemented";
}

void Options::load()
{
	// stub
}

void Options::save()
{
	// stub
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

void Options::update(const std::vector<std::string>& strings)
{
	for (int i = 0; i<int(strings.size()); i += 2)
	{
		std::string key = strings[i], value = strings[i + 1];

		if (key == "mp_username")
			m_playerName = value;
		else if (key == "ctrl_invertmouse")
			m_bInvertMouse = readBool(value);
		else if (key == "ctrl_autojump")
			m_bAutoJump = readBool(value);
		else if (key == "gfx_fancygraphics")
			m_bFancyGraphics = readBool(value);
		else if (key == "mp_server_visible_default")
			m_bServerVisibleDefault = readBool(value);
		else if (key == "gfx_smoothlighting")
			Minecraft::useAmbientOcclusion = m_bAmbientOcclusion = readBool(value);
		else if (key == "gfx_viewdistance")
			m_iViewDistance = readInt(value);
	}
}

std::vector<std::string> Options::getOptionStrings()
{
	std::vector<std::string> vec;

#define SO(optname, value) do { vec.push_back(optname); vec.push_back(value); } while (0)

	SO("mp_username", m_playerName);
	SO("ctrl_invertmouse",          saveBool(m_bInvertMouse));
	SO("ctrl_autojump",             saveBool(m_bAutoJump));
	SO("gfx_fancygraphics",         saveBool(m_bFancyGraphics));
	SO("mp_server_visible_default", saveBool(m_bServerVisibleDefault));
	SO("gfx_smoothlighting",        saveBool(m_bAmbientOcclusion));
	SO("gfx_viewdistance",          saveInt (m_iViewDistance));

	return vec;
}
