/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>

#include <string>
#include <vector>

enum eKeyMappingIndex
{
	KM_FORWARD,
	KM_LEFT,
	KM_BACKWARD,
	KM_RIGHT,
	KM_JUMP,
	KM_INVENTORY,
	KM_DROP,
	KM_CHAT,
	KM_FOG,
	KM_SNEAK,
	KM_DESTROY,
	KM_PLACE,
	KM_MENU_NEXT,
	KM_MENU_PREVIOUS,
	KM_MENU_OK,
	KM_MENU_CANCEL, KM_BACK = KM_MENU_CANCEL,
	KM_SLOT_1,
	KM_SLOT_2,
	KM_SLOT_3,
	KM_SLOT_4,
	KM_SLOT_5,
	KM_SLOT_6,
	KM_SLOT_7,
	KM_SLOT_8,
	KM_SLOT_9,
	KM_SLOT_L,
	KM_SLOT_R,
	KM_TOGGLEGUI,
	KM_SCREENSHOT,
	KM_TOGGLEDEBUG,
	KM_TOGGLEAO,
	KM_TOGGLE3RD,
	KM_FLY_UP,
	KM_FLY_DOWN,
	KM_CHAT_CMD, // called "Open Chat" in Release 1.8
	KM_COUNT,
};

struct KeyMapping
{
	std::string key;
	int value;

	KeyMapping() : value(-1) {} // key is automatically clear when constructed
	KeyMapping(const char* keyName, int keyCode) : key(keyName), value(keyCode) {}
};

class Options
{
public:
	struct Option;
	struct KeyBind;
private:
	static bool readBool(const std::string& str);
	static int readInt(const std::string& str);
	static std::string saveBool(bool b);
	static std::string saveInt(int i);
	static std::vector<std::string> readPropertiesFromFile(const std::string& filePath);
	static void savePropertiesToFile(const std::string& filePath, std::vector<std::string> properties);

private:
	void _initDefaultValues();
	void _load();
public:
	Options();
	Options(const std::string& folderPath);
	void save();
	std::string getMessage(const Options::Option&);
	std::vector<std::string> getOptionStrings();
	
	int getKey(eKeyMappingIndex idx) const { return m_keyMappings[idx].value; }
	bool isKey(eKeyMappingIndex idx, int keyCode) const { return getKey(idx) == keyCode; }

	void loadControls();

private:
	std::string m_filePath;
	KeyMapping m_keyMappings[KM_COUNT];

public:
	float m_fMusicVolume;
	float m_fMasterVolume;
	float m_fSensitivity;
	bool m_bInvertMouse;
	int m_iViewDistance;
	bool m_bViewBobbing;
	bool m_bAnaglyphs;
	uint8_t field_16;
	bool m_bFancyGraphics;
	bool m_bAmbientOcclusion;
	uint8_t field_19; // use Mouse as input for breaking
	std::string field_1C;
	int m_difficulty;
	bool m_bDontRenderGui;
	bool m_bThirdPerson;
	uint8_t field_23E;
	bool m_bFlyCheat;
	uint8_t field_240;
	bool field_241;
	float field_244;
	float field_248;
	int field_24C;
	std::string m_playerName;
	bool m_bServerVisibleDefault;
	bool m_bAutoJump;
	bool m_bDebugText;
	bool m_bBlockOutlines;
	bool m_bFancyGrass;
	bool m_bBiomeColors;
	bool m_bSplitControls;
	bool m_bUseController;
	bool m_bDynamicHand;
	bool m_bOldTitleLogo;
	bool m_bMenuPanorama;

public:
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
};

