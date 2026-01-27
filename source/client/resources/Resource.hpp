#pragma once

#include <string>
#include <map>

#include "client/renderer/texture/TextureData.hpp"
#include "ResourceLoader.hpp"
#include "ResourceLocation.hpp"

class Resource
{
private:
	typedef std::map<ResourceLocation::FileSystem, ResourceLoader*> LoaderMap;
	static LoaderMap m_loaders;

public:
	static bool hasResource(const ResourceLocation& location);
	static bool hasTexture(const ResourceLocation& location);
	static bool load(const ResourceLocation& location, std::string& stream);
	static TextureData loadTexture(const ResourceLocation& location);
	static void registerLoader(ResourceLoader* loader);
	static void teardownLoaders();

	static ResourceLoader* getLoader(ResourceLocation::FileSystem fileSystem)
	{
		LoaderMap::iterator it = m_loaders.find(fileSystem);
		if (it != m_loaders.end())
			return it->second;
		return nullptr;
	}
};

