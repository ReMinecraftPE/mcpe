/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <sstream>
#include <stdint.h>

#include <string>
#include <vector>
#include <map>

#include "common/threading/AsyncTask.hpp"
#include "client/resources/ResourcePackManager.hpp"

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
	KM_MENU_UP,
	KM_MENU_DOWN,
	KM_MENU_LEFT,
	KM_MENU_RIGHT,
	KM_MENU_TAB_LEFT,
	KM_MENU_TAB_RIGHT,
	KM_MENU_OK,
	KM_MENU_CANCEL, KM_BACK = KM_MENU_CANCEL,
	KM_MENU_PAUSE,
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
	KM_CONTAINER_QUICKMOVE,
	KM_CONTAINER_SPLIT,
	KM_TOGGLEGUI,
	KM_SCREENSHOT,
	KM_TOGGLEDEBUG,
	KM_TOGGLEAO,
	KM_TOGGLE3RD,
	KM_FLY_UP,
	KM_FLY_DOWN,
	KM_CHAT_CMD, // called "Open Chat" in Release 1.8
	KM_COUNT
};

struct KeyMapping
{
	std::string key;
	int value;

	KeyMapping() : value(-1) {} // key is automatically clear when constructed
	KeyMapping(const char* keyName, int keyCode) : key(keyName), value(keyCode) {}
};

enum UITheme
{
	UI_POCKET,
	UI_JAVA,
	UI_CONSOLE,

	UI_UNIVERSAL
};

enum LogoType
{
	LOGO_AUTO,
	LOGO_POCKET,
	LOGO_JAVA,
	LOGO_CONSOLE,
	LOGO_XBOX360,
	LOGO_3D
};

//@NOTE: Used only for the UI_CONSOLE UITheme for now
enum HUDSize
{
	HUD_SIZE_1 = 2,
	HUD_SIZE_2 = 3,
	HUD_SIZE_3 = 4
};

class Minecraft;
class GuiElement;
class Minecraft;

class OptionEntry
{
public:
	OptionEntry(const std::string& key, const std::string& name) : m_key(key), m_name(name), m_pMinecraft(nullptr) {}

	virtual const std::string& getKey() const { return m_key; }
	virtual const std::string& getName() const { return m_name; }
	virtual const std::string& getDisplayName() const;
	virtual std::string getDisplayValue() const;
	virtual void save(std::stringstream&) const = 0;
	virtual std::string getMessage() const;
	virtual void load(const std::string& value) = 0;
	virtual void toggle() {}
	virtual void addUnit(int mul) {}
	virtual void fromFloat(float) {}
	virtual float toFloat() const { return 0.0f; }
	virtual void reset() {};

	virtual void addGuiElement(std::vector<GuiElement*>&, UITheme uiTheme);

private:
	std::string m_key;
	std::string m_name;

public:
	Minecraft* m_pMinecraft;
};

template <typename V>
class OptionInstance : public OptionEntry
{
private:
	V m_value;
	V m_defaultValue;

public:
	OptionInstance(const std::string& key, const std::string& name, V initial) : OptionEntry(key, name), m_value(initial), m_defaultValue(initial) {}

	virtual void apply() {}

	void set(const V& v)
	{
		V oldValue = m_value;
		m_value = v;
		if (oldValue != v)
			apply();
	}

	void setDefault(const V& v) { m_defaultValue = v; }
	void reset() override { set(m_defaultValue); }
	const V& get() const { return m_value; }
	const V& getDefault() const { return m_defaultValue; }
};

class BoolOption : public OptionInstance<bool>
{
public:
	BoolOption(const std::string& key, const std::string& name, bool initial = true) : OptionInstance(key, name, initial) {}

	void load(const std::string& value) override;
	void save(std::stringstream& ss) const override;
	void toggle() override { set(get() ^ 1); }
	std::string getDisplayValue() const override;
	void addGuiElement(std::vector<GuiElement*>&, UITheme uiTheme) override;
};

class FloatOption : public OptionInstance<float>
{
public:
	FloatOption(const std::string& key, const std::string& name, float initial = 0.0f, float unit = 0.01f) :
		OptionInstance(key, name, initial),
		m_unit(unit)
	{
	}

	void load(const std::string& value) override;
	void save(std::stringstream& ss) const override { ss << get(); }
	void addUnit(int mul) override { set(Mth::clamp(get() + mul * m_unit, 0.0f, 1.0f)); }
	void fromFloat(float v) override { set(v); }
	float toFloat() const override { return get(); }
	std::string getDisplayValue() const override;
	void addGuiElement(std::vector<GuiElement*>&, UITheme uiTheme) override;

public:
	float m_unit;
};

class SensitivityOption : public FloatOption
{
public:
	SensitivityOption(const std::string& key, const std::string& name, float initial = 0.0f) : FloatOption(key, name, initial, 0.005f) {}

	std::string getDisplayValue() const override;
};

class AOOption : public BoolOption
{
public:
	AOOption(const std::string& key, const std::string& name, bool initial = true) : BoolOption(key, name, initial) {}

	void apply() override;
};

class GraphicsOption : public BoolOption
{
public:
	GraphicsOption(const std::string& key, const std::string& name, bool initial = true) : BoolOption(key, name, initial) {}

	void apply() override;
};

class FancyGraphicsOption : public GraphicsOption
{
public:
	FancyGraphicsOption(const std::string& key, const std::string& name, bool initial = true) : GraphicsOption(key, name, initial) {}

	std::string getMessage() const override;
};

class IntOption : public OptionInstance<int>
{
public:
	IntOption(const std::string& key, const std::string& name, int initial = 0) : OptionInstance(key, name, initial) {}

	void load(const std::string& value) override;
	void save(std::stringstream& ss) const override { ss << get(); }
};

class StringOption : public OptionInstance<std::string>
{
public:
	StringOption(const std::string& key, const std::string& name, std::string initial = "") : OptionInstance(key, name, initial) {}

	void load(const std::string& value) override { set(value); }
	void save(std::stringstream& ss) const override { ss.str(get()); }
};

class ValuesBuilder
{
public:
	ValuesBuilder& add(const std::string& value)
	{
		m_values.push_back(value);
		return *this;
	}

public:
	std::vector<std::string> m_values;
};

class MinMaxOption : public IntOption
{
public:
	MinMaxOption(const std::string& key, const std::string& name, int initial, int min, int max) : IntOption(key, name, initial), m_min(min), m_max(max)
	{
	}

	void toggle() override { set(Mth::Max(m_min, (get() + 1) % m_max)); }
	void addUnit(int mul) override { set(Mth::clamp(get() + mul, m_min, m_max - 1)); }
	void fromFloat(float v) override { set(Mth::round(m_min + (m_max - 1 - m_min) * v)); }
	float toFloat() const override { return (get() - m_min) / float(m_max - 1 - m_min); }
	void addGuiElement(std::vector<GuiElement*>&, UITheme uiTheme) override;

public:
	int m_min, m_max;
};


class ValuesOption : public MinMaxOption
{
public:
	ValuesOption(const std::string& key, const std::string& name, int initial, const ValuesBuilder& values) : MinMaxOption(key, name, initial, 0, values.m_values.size()), m_values(values.m_values)
	{
	}

	const std::string& getValue() const { return m_values[Mth::clamp(get(), m_min, m_max - 1)]; }
	std::string getDisplayValue() const override;

public:
	std::vector<std::string> m_values;
};

class GuiScaleOption : public ValuesOption
{
public:
	GuiScaleOption(const std::string& key, const std::string& name, int initial, const ValuesBuilder& values) : ValuesOption(key, name, initial, values)
	{
	}

	void apply() override;
};

class LogoTypeOption : public ValuesOption
{
public:
	LogoTypeOption(const std::string& key, const std::string& name, int initial, const ValuesBuilder& values) : ValuesOption(key, name, initial, values)
	{
	}

	void apply() override;
};

class UIThemeOption : public ValuesOption
{
public:
	UIThemeOption(const std::string& key, const std::string& name, int initial, const ValuesBuilder& values) : ValuesOption(key, name, initial, values)
	{
	}

	void apply() override;
};

class HUDSizeOption : public MinMaxOption
{
public:
	HUDSizeOption(const std::string& key, const std::string& name, int initial) : MinMaxOption(key, name, initial, HUD_SIZE_1, HUD_SIZE_3 + 1)
	{
	}

	std::string getDisplayValue() const override;
};

class Options
{
public:
	struct KeyBind;
private:
	static bool _hasResourcePack(const ResourcePack& pack, ResourcePackStack& packs);
	static void _tryAddResourcePack(const std::string& name, ResourcePackStack& packs);

    // @TODO: Move these to a "PlainTextSerializer" class, since we probably want to use these
    // in PatchManager as well.
	static bool readBool(const std::string& str);
	static int readInt(const std::string& str);
	static float readFloat(const std::string& str);
	static void readArray(const std::string& str, std::vector<std::string>& array);
	static void readPackArray(const std::string& str, ResourcePackStack& array);
	static std::string saveBool(bool b);
	static std::string saveInt(int i);
	static std::string saveFloat(float f);
	static std::string saveArray(const std::vector<std::string>& arr);
	static std::string savePackArray(const ResourcePackStack& arr);
	static std::vector<std::string> readPropertiesFromFile(const std::string& filePath);
	static void savePropertiesToFile(const std::string& filePath, const std::vector<std::string>& properties);

private: // async
	MC_ASYNC_FUNC_VOID_2(savePropertiesToFile,
		const std::string, filePath,
		const std::vector<std::string>, properties
	);

private:
	void _initDefaultValues();
	void _load();
	AsyncTask _saveAsync();
public:
	Options(Minecraft*, const std::string& folderPath = "");

	void add(OptionEntry&);
	const AsyncTask& save();
	std::vector<std::string> getOptionStrings();
	
	int getKey(eKeyMappingIndex idx) const { return m_keyMappings[idx].value; }
	bool isKey(eKeyMappingIndex idx, int keyCode) const { return getKey(idx) == keyCode; }

	void loadControls();
	void reset();
	void initResourceDependentOptions();

	UITheme getUiTheme() const;
	LogoType getLogoType() const;

private:
	Minecraft* m_pMinecraft;
	std::map<std::string, OptionEntry*> m_options;
	AsyncTask m_saveTask;
	std::string m_filePath;
	KeyMapping m_keyMappings[KM_COUNT];

public:
	friend class BoolOption;
	friend class FloatOption;
	friend class SensitivityOption;
	friend class IntOption;
	friend class HUDSizeOption;

	FloatOption m_musicVolume;
	FloatOption m_masterVolume;
	SensitivityOption m_sensitivity;
	BoolOption m_invertMouse;
	ValuesOption m_viewDistance;
	BoolOption m_viewBobbing;
	BoolOption m_anaglyphs;
	uint8_t field_16;
	FancyGraphicsOption m_fancyGraphics;
	AOOption m_ambientOcclusion;
	uint8_t field_19; // use Mouse as input for breaking
	std::string field_1C;
	ValuesOption m_difficulty;
	BoolOption m_hideGui;
	BoolOption m_thirdPerson;
	uint8_t field_23E;
	BoolOption m_flightHax;
	uint8_t field_240;
	bool field_241;
	float field_244;
	float field_248;
	int field_24C;
	StringOption m_playerName;
	BoolOption m_serverVisibleDefault;
	BoolOption m_autoJump;
	BoolOption m_debugText;
	BoolOption m_blockOutlines;
	GraphicsOption m_fancyGrass;
	GraphicsOption m_biomeColors;
	BoolOption m_splitControls;
	BoolOption m_bUseController;
	BoolOption m_dynamicHand;
	BoolOption m_menuPanorama;
	GuiScaleOption m_guiScale;
	StringOption m_lang;
	UIThemeOption m_uiTheme;
	LogoTypeOption m_logoType;
	HUDSizeOption m_hudSize;
	BoolOption m_classicCrafting;
	ResourcePackStack m_resourcePacks;
};


#define OPTIONS_LIST_GAMEPLAY_GAME         \
	HEADER("Game");                        \
	OPTION(m_difficulty);                  \
	OPTION(m_thirdPerson);                 \
	OPTION(m_serverVisibleDefault);        \

#define OPTIONS_LIST_GAMEPLAY_AUDIO        \
	HEADER("Audio");                       \
	OPTION(m_musicVolume);                 \
	OPTION(m_masterVolume);                \

#define OPTIONS_LIST_CONTROLS_CONTROLS     \
	HEADER("Controls");                    \
	OPTION(m_sensitivity);                 \
	OPTION(m_invertMouse);                 \
	OPTION(m_splitControls); idxSplit = currentIndex; \
	/*OPTION(m_swapJumpSneak);*/           \
	/*OPTION(m_buttonSize);*/              \
	OPTION(m_autoJump);                    \
	OPTION(m_bUseController); idxController = currentIndex; \

#define OPTIONS_LIST_CONTROLS_FEEDBACK     \
	/*HEADER("Feedback");*/                \
	/*OPTION(m_vibrate);*/                 \

#define OPTIONS_LIST_CONTROLS_EXPERIMENTAL \
	HEADER("Experimental");                \
	OPTION(m_flightHax);                   \

#define OPTIONS_LIST_VIDEO_GRAPHICS        \
	HEADER("Graphics");                    \
	/*OPTION(m_brightness);*/              \
	OPTION(m_viewDistance);                \
	/*OPTION(m_antiAliasing);*/            \
	/*OPTION(m_guiScale);*/                \
	/*OPTION(m_fov);*/                     \
	OPTION(m_ambientOcclusion);            \
	OPTION(m_fancyGraphics);               \
	OPTION(m_viewBobbing);                 \
	OPTION(m_anaglyphs);                   \
	OPTION(m_blockOutlines);               \
	OPTION(m_fancyGrass);                  \
	OPTION(m_biomeColors);                 \
	OPTION(m_dynamicHand);                 \
	OPTION(m_uiTheme);                     \
	OPTION(m_logoType);                    \

#define OPTIONS_LIST_VIDEO_EXPERIMENTAL    \
	HEADER("Experimental");                \
	OPTION(m_hideGui);                     \
	OPTION(m_debugText);                   \
	OPTION(m_menuPanorama); idxPano = currentIndex; \

