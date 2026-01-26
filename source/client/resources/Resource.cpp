#include "Resource.hpp"

Resource::LoaderMap Resource::m_loaders;

bool Resource::hasResource(const ResourceLocation& location)
{
	ResourceLoader* pLoader = getLoader(location.fileSystem);
	if (!pLoader)
		return false;

	return pLoader->hasResource(location);
}

bool Resource::load(const ResourceLocation& location, std::string& stream)
{
	ResourceLoader* pLoader = getLoader(location.fileSystem);
	if (!pLoader)
	{
		LOG_E("Attempting to read from resource file system that isn't initialized: %d", location.fileSystem);
		throw std::bad_cast();
	}

	return pLoader->load(location, stream);
}

TextureData Resource::loadTexture(const ResourceLocation& location)
{
	ResourceLoader* pLoader = getLoader(location.fileSystem);
	if (!pLoader)
		return TextureData();

	return pLoader->loadTexture(location);
}

void Resource::registerLoader(ResourceLocation::FileSystem fileSystem, ResourceLoader* loader)
{
	if (m_loaders.find(fileSystem) != m_loaders.end())
	{
		LOG_W("ResourceLoader \"%d\" is already registered. Re-registering...", fileSystem);
	}

	m_loaders[fileSystem] = loader;
}

void Resource::teardownLoaders()
{
	for (LoaderMap::iterator it = m_loaders.begin(); it != m_loaders.end(); it++)
	{
		delete it->second;
	}
	m_loaders.clear();
}