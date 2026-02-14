#include "TileEntityType.hpp"

#include "TileEntityType.hpp"
#include "FurnaceTileEntity.hpp"
// #include "ChestTileEntity.hpp"
// #include "MobSpawnerTileEntity.hpp"
// #include "DispenserTileEntity.hpp"
// #include "SignTileEntity.hpp"
// #include "MusicTileEntity.hpp"
// #include "RecordPlayerTileEntity.hpp"
// #include "PistonMovingTileEntity.hpp"

std::map<std::string, const TileEntityType*> TileEntityType::entityTypeNameMap;
std::vector<const TileEntityType*> TileEntityType::entityTypes;

TileEntityType* TileEntityType::furnace;

TileEntityType::TileEntityType(const std::string& name, FactoryFunction func) : m_name(name), m_function(func)
{
}

TileEntity* TileEntityType::newTileEntity() const
{
    return m_function();
}

bool TileEntityType::operator==(const std::string& other) const
{
    return this->getName() == other;
}

bool TileEntityType::operator!=(const std::string& other) const
{
    return this->getName() != other;
}

bool TileEntityType::operator==(const TileEntityType& other) const
{
    return this->getName() == other.getName();
}

bool TileEntityType::operator!=(const TileEntityType& other) const
{
    return this->getName() != other.getName();
}

const TileEntityType* TileEntityType::getByName(const std::string& name)
{
    return entityTypeNameMap[name];
}

void TileEntityType::initTileEntities()
{
	furnace = RegisterType<FurnaceTileEntity>("Furnace");
	// chest = RegisterType<ChestTileEntity>("Chest");
}

void TileEntityType::teardownTileEntities()
{
    // @TODO: just clearing but not freeing memory
	entityTypeNameMap.clear();
	entityTypes.clear();
}