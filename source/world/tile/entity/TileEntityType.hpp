#pragma once

#include <string>
#include <vector>
#include <map>

class Level;
class TileEntity;
class TileEntityType;

class TileEntityFactory
{
private:
	static std::map<std::string, TileEntityType*> _types;

public:
	static void initTileEntities();
	static void teardownTileEntities();
	static const TileEntityType* getType(const std::string& name);

public:
	template <typename T>
	static TileEntityType* registerTileEntity(const std::string& name);
};

class TileEntityType
{
public:
	static TileEntityType* furnace;
	static TileEntityType* chest;
	static TileEntityType* noteblock;

public:
	friend class TileEntityFactory;
	typedef TileEntity* (*CreateFunction)();

public:
	TileEntityType(const std::string& name, CreateFunction func);

public:
	const std::string& getName() const;
	TileEntity* newTileEntity() const;

private:
	std::string _name;
	CreateFunction _function;

	template <typename T>
	static TileEntity* CreateType() { return new T(); }
};

template <typename T>
TileEntityType* TileEntityFactory::registerTileEntity(const std::string& name)
{
	TileEntityType* type = new TileEntityType(name, &TileEntityType::CreateType<T>);
	_types[type->_name] = type;
	return type;
}