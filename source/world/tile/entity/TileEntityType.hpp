#pragma once

#include <string>
#include <vector>
#include <map>

class Level;
class TileEntity;
class TileEntityType
{
public:
	typedef TileEntity* (*FactoryFunction)();

public:
	TileEntityType(const std::string& name, FactoryFunction func);

public:
	const std::string& getName() const { return m_name; }
	TileEntity* newTileEntity() const;
	bool operator==(const std::string& other) const;
	bool operator!=(const std::string& other) const;
	bool operator==(const TileEntityType& other) const;
	bool operator!=(const TileEntityType& other) const;

public:
	static const TileEntityType* getByName(const std::string& name);
	static void initTileEntities();
	static void teardownTileEntities();

private:
	std::string m_name;
	FactoryFunction m_function;

	static std::map<std::string, const TileEntityType*> entityTypeNameMap;
	static std::vector<const TileEntityType*> entityTypes;

public:
	static TileEntityType* furnace;

	template <typename T>
	static TileEntity* makeMe()
	{
		return new T();
	}

public:
	template <typename T>
	static TileEntityType* RegisterType(const std::string& name)
    {
		TileEntityType* type = new TileEntityType(name, &TileEntityType::makeMe<T>);
		entityTypeNameMap[type->m_name] = type;
		entityTypes.push_back(type);
		return type;
	}
};