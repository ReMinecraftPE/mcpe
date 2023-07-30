#include "Tile.hpp"
#include "Level.hpp"

ClothTile::ClothTile(int id, int type) : Tile(id, TEXTURE_CLOTH_64, Material::cloth)
{
	field_6C = type;

	m_TextureFrame = getTexture(0, type);
}

int ClothTile::getTexture(int dir)
{
	return getTexture(dir, field_6C);
}

int ClothTile::getTexture(int dir, int data)
{
	//@HUH: what?
	return ((~(this->field_6C & 0xFu) >> 3) & 1) + 16 * (~(this->field_6C & 0xF) & 7) + 113;
}

int ClothTile::getSpawnResourcesAuxValue(int val)
{
	return val;
}
