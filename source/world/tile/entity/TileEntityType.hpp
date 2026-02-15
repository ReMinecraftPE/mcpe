#pragma once

#include <string>
#include <vector>
#include <map>

class Level;
class TileEntity;

class TileEntityType
{
public:
	typedef TileEntity* (*CreateFunction)();

public:
	TileEntityType(const std::string& name, CreateFunction func);

public:
	const std::string& getName() const;
	TileEntity* newTileEntity() const;

public:
	static void InitTileEntities();
	static const TileEntityType* GetType(const std::string& name);
	static void TeardownTileEntities();

private:
	static std::map<std::string, TileEntityType*> _types;

private:
	std::string _name;
	CreateFunction _function;

public:
	static TileEntityType* furnace;
	static TileEntityType* chest;
	static TileEntityType* noteblock;

	template <typename T>
	static TileEntity* CreateType() { return new T(); }

public:
	template <typename T>
	static TileEntityType* RegisterTileEntity(const std::string& name)
    {
		TileEntityType* type = new TileEntityType(name, &TileEntityType::CreateType<T>);
		_types[type->_name] = type;
		return type;
	}
};