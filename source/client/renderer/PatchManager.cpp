#include "PatchManager.hpp"
#include "AppPlatform.hpp"
#include "client/common/Utils.hpp"
#include "compat/GL.hpp"

#define PM_SEPARATOR ('|')

PatchManager* g_pPatchManager;
PatchManager* GetPatchManager()
{
	if (!g_pPatchManager)
		g_pPatchManager = new PatchManager;

	return g_pPatchManager;
}

PatchManager::PatchManager()
{
	m_bGrassTinted = true;
	m_nMetalSideYOffset = -1;
}

void PatchManager::LoadPatchData(const std::string& patchData)
{
	std::stringstream patchDataStream(patchData);
	std::string currLine;

	while (std::getline(patchDataStream, currLine))
	{
		if (currLine.empty()) continue;
		if (currLine[0] == '#') continue;

		std::string command;
		std::stringstream lineStream(currLine);
		// read command type
		if (!std::getline(lineStream, command, PM_SEPARATOR))
			continue;

		if (command == "terrain" || command == "items")
		{
			bool bIsItems = command == "items";
			std::string xStr, yStr, fileName;

			if (!std::getline(lineStream, xStr, PM_SEPARATOR)) continue;
			if (!std::getline(lineStream, yStr, PM_SEPARATOR)) continue;
			if (!std::getline(lineStream, fileName, PM_SEPARATOR)) continue;

			// turn the xStr and yStr into ints.
			int x, y;
			if (!sscanf(xStr.c_str(), "%d", &x)) continue;
			if (!sscanf(yStr.c_str(), "%d", &y)) continue;

			m_patchData.push_back(PatchData(bIsItems ? TYPE_ITEMS : TYPE_TERRAIN, x, y, fileName));

			continue;
		}

		// features -- TODO un-hardcode this
		if (command == "vegetation_tint")
		{
			ReadBool(lineStream, m_bGrassTinted);
			continue;
		}

		// features -- TODO un-hardcode this
		if (command == "metal_block_sides")
		{
			ReadInt(lineStream, m_nMetalSideYOffset);
			continue;
		}

		LogMsg("Unknown command %s from patch data.", command.c_str());
	}
}

void PatchManager::PatchTextures(AppPlatform* pAppPlatform, ePatchType patchType)
{
	// Use glTexSubImage2D to patch the terrain.png texture on the fly.
	for (int i = 0; i < int(m_patchData.size()); i++)
	{
		PatchData& pd = m_patchData[i];
		if (pd.m_type != patchType)
			continue;

		Texture texture = pAppPlatform->loadTexture("patches/" + pd.m_filename, true);
		if (texture.m_width == 0)
		{
			LogMsg("Image %s has width 0, not found?! Skipping", pd.m_filename.c_str());
			continue;
		}

		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			pd.m_destX,
			pd.m_destY,
			texture.m_width,
			texture.m_height,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			texture.m_pixels
		);

		SAFE_DELETE_ARRAY(texture.m_pixels);
	}
}

bool PatchManager::IsGrassTinted()
{
	return m_bGrassTinted;
}

int PatchManager::GetMetalSideYOffset()
{
	return m_nMetalSideYOffset;
}

void PatchManager::ReadBool(std::istream& is, bool& b)
{
	std::string flag;
	if (!std::getline(is, flag, PM_SEPARATOR))
		return;

	b = (flag == "true" || flag == "1" || flag == "TRUE");
}

void PatchManager::ReadInt(std::istream& is, int& i)
{
	std::string flag;
	if (!std::getline(is, flag, PM_SEPARATOR))
		return;

	int x = -1;
	if (sscanf(flag.c_str(), "%d", &x))
		i = x;
}
