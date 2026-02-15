#include "TileEntityType.hpp"
#include "FurnaceTileEntity.hpp"
#include "ChestTileEntity.hpp"
#include "MusicTileEntity.hpp"
// #include "MobSpawnerTileEntity.hpp"
// #include "DispenserTileEntity.hpp"
// #include "SignTileEntity.hpp"
// #include "RecordPlayerTileEntity.hpp"
// #include "PistonMovingTileEntity.hpp"

std::map<std::string, TileEntityType*> TileEntityType::_types;

TileEntityType* TileEntityType::furnace;
TileEntityType* TileEntityType::chest;
TileEntityType* TileEntityType::noteblock;

TileEntityType::TileEntityType(const std::string& name, CreateFunction func) : _name(name), _function(func)
{
}

const std::string& TileEntityType::getName() const
{
    return _name;
}

TileEntity* TileEntityType::newTileEntity() const
{
    return _function();
}

void TileEntityType::InitTileEntities()
{
	furnace = RegisterTileEntity<FurnaceTileEntity>("Furnace");
	chest = RegisterTileEntity<ChestTileEntity>("Chest");
    noteblock = RegisterTileEntity<MusicTileEntity>("Music");
}

const TileEntityType* TileEntityType::GetType(const std::string& name)
{
    return _types[name];
}

void TileEntityType::TeardownTileEntities()
{
    // delete all heap allocated tile entity types (furnace, chest, etc.)
    for (std::map<std::string, TileEntityType*>::const_iterator it = _types.begin(); it != _types.end(); ++it)
    {
        SAFE_DELETE(it->second);
    }
	_types.clear();
}