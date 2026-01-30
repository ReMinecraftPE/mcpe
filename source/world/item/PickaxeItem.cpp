#include "PickaxeItem.hpp"
#include "world/tile/Tile.hpp"

PickaxeItem::PickaxeItem(int id, Tier& tier) : ToolItem(id, 1, tier)
{
   static const Tile* pickaxeTiles[] =
   {
      Tile::stoneBrick,
      Tile::stoneSlab,
      Tile::stoneSlabHalf,
      Tile::stairs_stone,
      Tile::rock,
      Tile::mossStone,
      Tile::ironOre,
      Tile::ironBlock,
      Tile::coalOre,
      Tile::goldBlock,
      Tile::goldOre,
      Tile::emeraldOre,
      Tile::emeraldBlock,
      Tile::ice,
      Tile::netherrack,
      Tile::lapisOre,
      Tile::lapisBlock
   };

   static const int PICKAXE_TILE_COUNT = sizeof(pickaxeTiles) / sizeof(Tile*);

   initializeTiles(pickaxeTiles, PICKAXE_TILE_COUNT);
}

bool PickaxeItem::canDestroySpecial(const Tile* tile) const
{
   // Try to keep the level checks in order.
   int level = m_tier.m_level;

   if (tile == Tile::obsidian)
      return level >= 3;

   if (tile == Tile::emeraldBlock || tile == Tile::emeraldOre)
      return level >= 2;

   if (tile == Tile::goldBlock || tile == Tile::goldOre)
      return level >= 2;

   if (tile == Tile::redStoneOre || tile == Tile::redStoneOre_lit)
      return level >= 2;

   if (tile == Tile::ironBlock || tile == Tile::ironOre)
      return level >= 1;

   if (tile == Tile::lapisBlock || tile == Tile::lapisOre)
      return level >= 1;

   if (tile->m_pMaterial == Material::stone)
      return true;

   return tile->m_pMaterial == Material::metal;
}