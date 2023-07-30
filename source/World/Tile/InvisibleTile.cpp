#include "Tile.hpp"
#include "Level.hpp"

InvisibleTile::InvisibleTile(int ID, int texture, Material* pMtl) :
	Tile(ID, texture, pMtl)
{

}

int InvisibleTile::getRenderShape()
{
	return SHAPE_NONE;
}

