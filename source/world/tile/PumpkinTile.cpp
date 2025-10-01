#include "PumpkinTile.hpp"
#include "world/level/Level.hpp"

PumpkinTile::PumpkinTile(TileID id, bool lantern) : Tile(id, TEXTURE_PUMPKIN_TOP, Material::vegetable), m_bLantern(lantern)
{
}

int PumpkinTile::getTexture(Facing::Name face, TileData data) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame;
	default: 
		return face == 2 && data == 2 || face == 5 && data == 3 || face == 3 && data == 0 || face == 4 && data == 1 ? m_TextureFrame + (m_bLantern ? 18 : 17) : m_TextureFrame + 16;
	}
}

int PumpkinTile::getTexture(Facing::Name face) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame;
	case Facing::SOUTH: return m_TextureFrame + 17;
	default: return m_TextureFrame + 16;
	}
}

void PumpkinTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob)
{
	int rot = Mth::floor(0.5f + (mob->m_rot.x * 4.0f / 360.0f)) & 3;

	TileData data = 0;

	switch (rot)
	{
		case 0: data = 2; break;
		case 1: data = 3; break;
		case 2: data = 0; break;
		case 3: data = 1; break;
	}

	level->setData(pos, data);
}
