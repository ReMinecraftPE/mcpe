#include "PatchManager.hpp"

#include "common/Logger.hpp"
#include "client/app/AppPlatform.hpp"
#include "world/tile/Tile.hpp"
#include "world/item/Item.hpp"
#include "thirdparty/GL/GL.hpp"

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
	m_bGlassSemiTransparent = false;
	m_nMetalSideYOffset = -1;
}

void PatchManager::LoadPatchData(const std::string& patchData)
{
	std::stringstream patchDataStream(patchData);
	std::string currLine;

	while (std::getline(patchDataStream, currLine))
	{
		if (currLine.empty()) continue;
        if (currLine.at(currLine.size() - 1) == '\r')
        {
            // Ignore Windows line-endings when processing file on Unix systems
            // How would we possibly standardize this if the game isn't the thing writing the file?
            currLine = currLine.substr(0, currLine.size() - 1);
        }
		if (currLine.empty()) continue;
		if (currLine[0] == '#') continue;

		std::string command;
		std::stringstream lineStream(currLine);
		// read command type
		if (!std::getline(lineStream, command, PM_SEPARATOR))
			continue;
        
        /*if (command[0] == '\n')
        {
            // We'll end up here if we're on a platform that doesn't use Windows line-endings
            // So let's just ignore them
            command = command.erase(0, 1);
        }*/

		if (command == "stop_now")
		{
			LOG_I("PatchManager: Found stop_now, quitting patch processing earlier, iProgramInCpp probably wanted to test something");
			return;
		}

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

		if (command == "frame")
		{
			std::string xStr, yStr, itemName;

			if (!std::getline(lineStream, itemName, PM_SEPARATOR)) continue;
			if (!std::getline(lineStream, xStr, PM_SEPARATOR)) continue;
			if (!std::getline(lineStream, yStr, PM_SEPARATOR)) continue;

			int itemID = -1, x = 0, y = 0;
			if (!sscanf(xStr.c_str(), "%d", &x)) continue;
			if (!sscanf(yStr.c_str(), "%d", &y)) continue;

			// try to parse the item name as an int first
			if (sscanf(itemName.c_str(), "%d", &itemID))
			{
				if (itemID < 0 || itemID >= C_MAX_ITEMS)
					goto namefailure;

				if (itemID < C_MAX_TILES && !Tile::tiles[itemID])
					goto namefailure;

				if (itemID >= C_MAX_TILES && !Item::items[itemID])
					goto namefailure;
			}
			else
			{
				std::string tileDesc = "tile." + itemName;
				std::string itemDesc = "item." + itemName;
				// look through the entirety of the Tiles and Items array
				for (int i = 0; i < C_MAX_TILES && itemID == -1; i++)
				{
					if (!Tile::tiles[i])
						continue;
					if (Tile::tiles[i]->m_descriptionID != tileDesc)
						continue;

					itemID = i;
				}
				for (int i = 0; i < C_MAX_ITEMS && itemID == -1; i++)
				{
					if (!Item::items[i])
						continue;
					if (Item::items[i]->m_DescriptionID != itemDesc)
						continue;

					itemID = i;
				}
			}

			if (itemID == -1)
			{
			namefailure:
				LOG_W("Unknown item/tile with the name %s", itemName.c_str());
				continue;
			}

			m_patchData.push_back(PatchData(TYPE_FRAME, itemID, 16 * y + x));

			continue;
		}

		// features -- TODO un-hardcode this
		if (command == "vegetation_tint")
		{
			ReadBool(lineStream, m_bGrassTinted);
			continue;
		}
		if (command == "semi_transparent_glass")
		{
			ReadBool(lineStream, m_bGlassSemiTransparent);
			continue;
		}
		if (command == "metal_block_sides")
		{
			ReadInt(lineStream, m_nMetalSideYOffset);
			continue;
		}
		if (command == "grass_sides_tint")
		{
			ReadBool(lineStream, m_bGrassSidesTinted);

			if (m_bGrassSidesTinted)
				// push a magic value so we can determine whether to disable it if the file doesn't exist
				m_patchData.push_back(PatchData(TYPE_TERRAIN, 100, 100, "grass_side_transparent.png"));

			continue;
		}

		LOG_W("Unknown command %s from patch data.", command.c_str());
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

		bool bDisableFancyGrassIfFailed = false;

		// got the magic value, we can determine whether to disable fancy pants grass if the file doesn't exist
		if (pd.m_destX == 1600 && pd.m_destY == 1600 && pd.m_type == TYPE_TERRAIN)
		{
			pd.m_destX = 4 * 16;
			pd.m_destY = 5 * 16;

			bDisableFancyGrassIfFailed = true;
		}

		// N.B. Well, in some cases, you do want things to fail nicely.
		Texture texture = pAppPlatform->loadTexture("patches/" + pd.m_filename, false);
		if (!texture.m_pixels || !texture.m_width || !texture.m_height)
		{
			LOG_W("Image %s was not found?! Skipping", pd.m_filename.c_str());
			if (bDisableFancyGrassIfFailed)
				m_bGrassSidesTinted = false;
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

void PatchManager::PatchTiles()
{
	for (int i = 0; i < int(m_patchData.size()); i++)
	{
		PatchData& pd = m_patchData[i];
		if (pd.m_type != TYPE_FRAME)
			continue;

		if (pd.m_destID < C_MAX_TILES && Tile::tiles[pd.m_destID])
		{
			Tile::tiles[pd.m_destID]->m_TextureFrame = pd.m_frameNo;
			continue;
		}

		if (Item::items[pd.m_destID])
		{
			Item::items[pd.m_destID]->m_icon = pd.m_frameNo;
			continue;
		}

		LOG_W("PatchTiles: unknown item ID %d", pd.m_destID);
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

bool PatchManager::IsGlassSemiTransparent()
{
	return m_bGlassSemiTransparent;
}

bool PatchManager::IsGrassSidesTinted()
{
	return m_bGrassSidesTinted;
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
