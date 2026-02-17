#include "TileEntityType.hpp"
#include "FurnaceTileEntity.hpp"
#include "ChestTileEntity.hpp"
#include "MusicTileEntity.hpp"
// #include "MobSpawnerTileEntity.hpp"
// #include "DispenserTileEntity.hpp"
// #include "SignTileEntity.hpp"
// #include "RecordPlayerTileEntity.hpp"
// #include "PistonMovingTileEntity.hpp"

TileEntityType* TileEntityType::furnace;
TileEntityType* TileEntityType::chest;
TileEntityType* TileEntityType::noteblock;

std::map<std::string, TileEntityType*> TileEntityFactory::_types;

void TileEntityFactory::InitTileEntities()
{
	TileEntityType::furnace =       RegisterTileEntity<FurnaceTileEntity>("Furnace");
	TileEntityType::chest =         RegisterTileEntity<ChestTileEntity>("Chest");
    TileEntityType::noteblock =     RegisterTileEntity<MusicTileEntity>("Music");
}

void TileEntityFactory::TeardownTileEntities()
{
    // delete all heap allocated tile entity types (furnace, chest, etc.)
    for (std::map<std::string, TileEntityType*>::const_iterator it = _types.begin(); it != _types.end(); ++it)
    {
        SAFE_DELETE(it->second);
    }
	_types.clear();
}

const TileEntityType* TileEntityFactory::GetType(const std::string& name)
{
    return _types[name];
}

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