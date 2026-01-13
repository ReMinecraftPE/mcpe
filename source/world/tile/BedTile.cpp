#include "BedTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/item/Item.hpp"

const int BedTile::headBlockToFootBlockMap[4][2] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
const int BedTile::hiddenFace[4] = {3, 4, 2, 5};
const int BedTile::hiddenFaceIndex[4] = {2, 3, 0, 1};
const int BedTile::bedDirection[4][6] = {{1, 0, 3, 2, 5, 4}, {1, 0, 5, 4, 2, 3}, {1, 0, 2, 3, 4, 5}, {1, 0, 4, 5, 3, 2}};

BedTile::BedTile(TileID id, int texture) : Tile(id, texture, Material::cloth)
{
	m_renderLayer = RENDER_LAYER_ALPHATEST;
	updateDefaultShape();
}

int BedTile::getTexture(Facing::Name face, TileData data) const
{
	if (face != Facing::DOWN && face != Facing::UP) {
		int var3 = getDirectionIndex(data);
		int var4 = bedDirection[var3][face];
		return isHead(data) ? (var4 == 2 ? m_TextureFrame + 2 + 16 : (var4 != 5 && var4 != 4 ? m_TextureFrame + 1 : m_TextureFrame + 1 + 16)) : (var4 == 3 ? m_TextureFrame - 1 + 16 : (var4 != 5 && var4 != 4 ? m_TextureFrame : m_TextureFrame + 16));
	}
	else {
		return isHead(data) ? m_TextureFrame + 1 : m_TextureFrame;
	}
}

void BedTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	updateDefaultShape();
}

void BedTile::updateDefaultShape()
{
	setShape(0, 0, 0, 1, 9.0f / 16.0f, 1);
}

bool BedTile::isSolidRender() const
{
	return false;
}

bool BedTile::isCubeShaped() const
{
	return false;
}

eRenderShape BedTile::getRenderShape() const
{
	return SHAPE_BED;
}

void BedTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	TileData data = level->getData(pos);
	int dir = getDirectionIndex(data);
	
	if (isHead(data)) {
		TilePos footPos(pos.x - headBlockToFootBlockMap[dir][0], pos.y, pos.z - headBlockToFootBlockMap[dir][1]);
		if (level->getTile(footPos) != m_ID) {
			level->setTile(pos, TILE_AIR);
		}
	}
	else {
		TilePos headPos(pos.x + headBlockToFootBlockMap[dir][0], pos.y, pos.z + headBlockToFootBlockMap[dir][1]);
		if (level->getTile(headPos) != m_ID) {
			level->setTile(pos, TILE_AIR);
			if (!level->m_bIsClientSide) {
				Tile::spawnResources(level, pos, data);
			}
		}
	}
}

bool BedTile::use(Level* level, const TilePos& pos, Player* player)
{
	if (level->m_bIsClientSide) {
		return true;
	}

	TileData data = level->getData(pos);
	TilePos tp(pos);
	
	if (!isHead(data)) {
		int dir = getDirectionIndex(data);
		tp.x += headBlockToFootBlockMap[dir][0];
		tp.z += headBlockToFootBlockMap[dir][1];
		if (level->getTile(tp) != m_ID) {
			return true;
		}
		data = level->getData(tp);
	}

	if (!level->m_pDimension->mayRespawn()) {
		level->setTile(tp, TILE_AIR);
		int dir = getDirectionIndex(data);
		TilePos footPos(tp);
		footPos.x -= headBlockToFootBlockMap[dir][0];
		footPos.z -= headBlockToFootBlockMap[dir][1];
		if (level->getTile(footPos) == m_ID) {
			level->setTile(footPos, TILE_AIR);
		}

		Vec3 explodePos(tp.x + 0.5f, tp.y + 0.5f, tp.z + 0.5f);
		level->explode(NULL, explodePos, 5.0f, true);
		return true;
	}
	else {
		if (isBedOccupied(data)) {
			// Check if there's a player sleeping in this bed
			for (size_t i = 0; i < level->m_players.size(); i++) {
				Player* p = level->m_players[i];
				if (p->isSleeping() && p->m_bHasBedSleepPos) {
					if (p->m_bedSleepPos == tp) {
						// Bed is occupied by another player
						return true;
					}
				}
			}
			setBedOccupied(level, tp, false);
		}

		Player::BedSleepingProblem result = player->startSleepInBed(tp);
		if (result == Player::BED_SLEEPING_OK) {
			setBedOccupied(level, tp, true);
			return true;
		}
		else {
			if (result == Player::BED_SLEEPING_NOT_POSSIBLE_NOW) {
				// Localization is to be implemented
				// player->displayClientMessage("tile.bed.noSleep");
			}

			return true;
		}
	}
}

int BedTile::getResource(TileData data, Random* random) const
{
	return TILE_AIR;  // Bed should not drop any items
}

void BedTile::spawnResources(Level* level, const TilePos& pos, TileData data, float chance)
{
	// Bed should not drop any items
}

void BedTile::setBedOccupied(Level* level, const TilePos& pos, bool occupied)
{
	TileData data = level->getData(pos);
	if (occupied) {
		data |= 4;
	}
	else {
		data &= ~4;
	}
	level->setData(pos, data);
}

TilePos BedTile::getRespawnTilePos(const Level* level, const TilePos& pos, int steps)
{
	TileData data = level->getData(pos);
	int dir = getDirectionIndex(data);

	for (int i = 0; i <= 1; ++i) {
		int startX = pos.x - headBlockToFootBlockMap[dir][0] * i - 1;
		int startZ = pos.z - headBlockToFootBlockMap[dir][1] * i - 1;

		TilePos tp(pos);
		for (tp.x = startX; tp.x <= startX + 2; ++tp.x) {
			for (tp.z = startZ; tp.z <= startZ + 2; ++tp.z) {
				TilePos belowPos = tp.below();
				Material* mat = level->getMaterial(belowPos);
				if (mat && mat->isSolid() && level->isEmptyTile(tp) && level->isEmptyTile(tp.above())) {
					if (steps <= 0)
						return tp;
					--steps;
				}
			}
		}
	}

	return pos;
}
