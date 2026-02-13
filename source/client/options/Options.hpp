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

class GuiElement;
class Minecraft;

class OptionEntry
{
public:
	OptionEntry(const std::string& key, const std::string& name) : m_key(key), m_name(name), m_pMinecraft(nullptr) {}

	virtual const std::string& getKey() const { return m_key; }
	virtual const std::string& getName() const;
	virtual std::string getDisplayValue() const;
	virtual void save(std::stringstream&) const = 0;
	virtual std::string getMessage() const;
	virtual void load(const std::string& value) = 0;
	virtual void toggle() = 0;

	virtual void addGuiElement(std::vector<GuiElement*>&, const std::string&);

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

	virtual void toggle() {}
	virtual void apply() {}

	void set(const V& v)
	{
		V oldValue = m_value;
		m_value = v;
		if (oldValue != v)
			apply();
	}

	void reset() { set(m_defaultValue); }
	const V& get() const { return m_value; }
};

class BoolOption : public OptionInstance<bool>
{
public:
	BoolOption(const std::string& key, const std::string& name, bool initial = true) : OptionInstance(key, name, initial) {}

	void load(const std::string& value) override;
	void save(std::stringstream& ss) const override;
	void toggle() override { set(get() ^ 1); }
	std::string getDisplayValue() const override;
	void addGuiElement(std::vector<GuiElement*>&, const std::string&) override;
};

class FloatOption : public OptionInstance<float>
{
public:
	FloatOption(const std::string& key, const std::string& name, float initial = 0.0f) : OptionInstance(key, name, initial) {}

	void load(const std::string& value) override;
	void save(std::stringstream& ss) const override { ss << get(); }
	std::string getDisplayValue() const override;
	void addGuiElement(std::vector<GuiElement*>&, const std::string&) override;
};

class SensitivityOption : public FloatOption
{
public:
	SensitivityOption(const std::string& key, const std::string& name, float initial = 0.0f) : FloatOption(key, name, initial) {}

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

class ValuesOption : public IntOption
{
public:
	ValuesOption(const std::string& key, const std::string& name, int initial, const ValuesBuilder& values) : IntOption(key, name, initial), m_values(values.m_values)
	{
	}

	void toggle() override { set((get() + 1) % m_values.size()); }
	std::string getDisplayValue() const override { return m_values[Mth::Min(get(), int(m_values.size()))]; }
	void addGuiElement(std::vector<GuiElement*>&, const std::string&) override;

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
	void initResourceDependentOptions();

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
	BoolOption m_b2dTitleLogo;
	BoolOption m_menuPanorama;
	GuiScaleOption m_guiScale;
	StringOption m_lang;
	ResourcePackStack m_resourcePacks;
};

