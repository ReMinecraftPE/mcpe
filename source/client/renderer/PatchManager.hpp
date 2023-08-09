#pragma once

#include <string>
#include <vector>
#include <sstream>

class AppPlatform;

enum ePatchType
{
	TYPE_NONE,
	TYPE_TERRAIN,
	TYPE_ITEMS,
	TYPE_FEATURE,
};

enum ePatchOption
{
	PO_NONE,
	PO_GRASS_TINT,
};

struct PatchData
{
	ePatchType m_type;
	ePatchOption m_option;
	int m_destX, m_destY;
	std::string m_filename;
	bool m_bEnable;

	PatchData(ePatchType type, int x, int y, const std::string& fn)
	{
		_init();
		m_type = type;
		m_destX = x * 16;
		m_destY = y * 16;
		m_filename = fn;
	}

	PatchData(ePatchType type, ePatchOption opt, bool enable)
	{
		_init();
		m_type = type;
		m_option = opt;
		m_bEnable = enable;
	}

	void _init()
	{
		m_type = TYPE_NONE;
		m_option = PO_NONE;
		m_destX = m_destY = 0;
		m_bEnable = false;
	}
};

class PatchManager
{
public:
	PatchManager();

	void LoadPatchData(const std::string& patchData);

	void PatchTextures(AppPlatform*, ePatchType);

	// Features
	bool IsGrassTinted();
	int  GetMetalSideYOffset();

private:
	void ReadBool(std::istream& is, bool& b);
	void ReadInt(std::istream& is, int& b);

private:
	bool m_bGrassTinted;
	int m_nMetalSideYOffset;
	std::vector<PatchData> m_patchData;
};

PatchManager* GetPatchManager();
