#include "TallGrass.hpp"
#include "world/level/Level.hpp"
#include "client/renderer/PatchManager.hpp"
#include <client/renderer/GrassColor.hpp>
#include <client/renderer/FoliageColor.hpp>

TallGrass::TallGrass(int id, int texture) : Bush(id, texture)
{
	setShape(0.1F, 0.0F, 0.1F, 0.9F, 0.8F, 0.9F);
}

bool TallGrass::isValidGrowTile(const TileID tile) const
{
	return tile == Tile::grass->m_ID;
}

int TallGrass::getResource(int x, Random* random) const
{
	return random->nextInt(8) == 0 ? Item::seeds->m_itemID : 0;
}

int TallGrass::getColor(const LevelSource* levelSource, const TilePos& pos) const
{
	if (GetPatchManager()->IsGrassTinted())
	{
		return 0x339933;
	}
	return 0xFFFFFF;
}

int TallGrass::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	auto data = level->getData(pos);
	return data == 1 ? m_TextureFrame : (data == 2 ? m_TextureFrame + 16 + 1 : (data == 0 ? m_TextureFrame + 16 : m_TextureFrame));
}

int TallGrass::getRenderShape() const
{
	return SHAPE_RANDOM_CROSS;
}
