/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>

class Options
{
public:
	struct Option;
	struct KeyBind;
public:
	Options();
	void initDefaultValues();
	void load();
	void save();
	std::string getMessage(const Options::Option&);
	void update(const std::vector<std::string>& string);

public:
	static bool readBool(const std::string& str);
#ifndef ORIGINAL_CODE
	static int readInt(const std::string& str);
#endif

public:
	enum KeyBindIndex
	{
		FORWARD,
		LEFT,
		BACK,
		RIGHT,
		JUMP,
		INVENTORY,
		DROP,
		CHAT,
		FOG,
		SNEAK,
		DESTROY,
		PLACE,
		MENU_NEXT,
		MENU_PREVIOUS,
		MENU_OK,
		MENU_CANCEL,
	};

	struct KeyBind {
		std::string key;
		int value;
	};

	struct Option
	{
		bool field_0;
		bool field_1;
		std::string str;
		int field_1C;
		
		Option(int i, const std::string& str, bool b1, bool b2) : field_0(b1), field_1(b2), str(str), field_1C(i) {}

		static Option MUSIC;
		static Option SOUND;
		static Option INVERT_MOUSE;
		static Option SENSITIVITY;
		static Option RENDER_DISTANCE;
		static Option VIEW_BOBBING;
		static Option ANAGLYPH;
		static Option LIMIT_FRAMERATE;
		static Option DIFFICULTY;
		static Option GRAPHICS;
		static Option AMBIENT_OCCLUSION;
		static Option GUI_SCALE;
	};

public:
	float field_0;
	float field_4;
	float field_8;
	bool m_bInvertMouse;
	int field_10;          // @NOTE: Render distance?
	bool field_14;
	bool m_bAnaglyphs;
	uint8_t field_16;
	bool m_bFancyGraphics;
	uint8_t field_18;      // @NOTE: Ambient occlusion?
	uint8_t field_19;
	std::string field_1C;
	KeyBind m_keyBinds[16];

	int field_238;
	uint8_t field_23C;
	uint8_t field_23D;     // @NOTE: Third person mode?
	uint8_t field_23E;
	bool m_bFlyCheat;
	uint8_t field_240;
	uint8_t field_241;
	float field_244;
	float field_248;
	int field_24C;
	std::string m_playerName;
	bool m_bServerVisibleDefault;
};

