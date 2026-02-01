#include "ResourcePackManager.hpp"
#include "client/app/AppPlatform.hpp"
#include "Resource.hpp"
#include "ResourcePackRepository.hpp"

ResourcePackManager::ResourcePackManager()
	: ResourceLoader(ResourceLocation::USER_PACKAGE)
{
	m_pPacks = nullptr;
}

ResourceLocation ResourcePackManager::_getLocationInPack(const ResourceLocation& location, const ResourcePack& pack) const
{
	return ResourceLocation(pack.m_fileSystem, ResourcePackRepository::RESOURCE_PACKS_PATH + "/" + pack.m_name + "/" + location.path);
}

bool ResourcePackManager::hasResource(const ResourceLocation& location) const
{
	bool hasAssetFile = false;

	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			hasAssetFile = Resource::hasResource(_getLocationInPack(location, *it));
			if (hasAssetFile)
				break;
		}
	}

	if (!hasAssetFile)
		hasAssetFile = Resource::hasResource(ResourceLocation(ResourceLocation::APP_PACKAGE, location.path));

	return hasAssetFile;
}

bool ResourcePackManager::getResourcePath(const ResourceLocation& location, std::string& path) const
{
	bool success = false;

	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			success = Resource::getResourcePath(_getLocationInPack(location, *it), path);
			if (success)
				break;
		}
	}

	if (!success)
		success = Resource::getResourcePath(ResourceLocation(ResourceLocation::APP_PACKAGE, location.path), path);

	return success;
}

bool ResourcePackManager::hasTexture(const ResourceLocation& location) const
{
	bool hasAssetFile = false;

	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			hasAssetFile = Resource::hasTexture(_getLocationInPack(location, *it));
			if (hasAssetFile)
				break;
		}
	}

	if (!hasAssetFile)
		hasAssetFile = Resource::hasTexture(ResourceLocation(ResourceLocation::APP_PACKAGE, location.path));

	return hasAssetFile;
}

bool ResourcePackManager::load(const ResourceLocation& location, std::string& stream) const
{
	bool success = false;

	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			success = Resource::load(_getLocationInPack(location, *it), stream);
			if (success)
				break;
		}
	}

	if (!success)
		success = Resource::load(ResourceLocation(ResourceLocation::APP_PACKAGE, location.path), stream);

	return success;
}

void ResourcePackManager::loadAllVersionsOf(const ResourceLocation& location) const
{
	// I don't even know, fuck this function
}

TextureData ResourcePackManager::loadTexture(const ResourceLocation& location) const
{
	TextureData data;
	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			data = Resource::loadTexture(_getLocationInPack(location, *it));
			if (!data.isEmpty())
				break;
		}
	}

	if (data.isEmpty())
		data = Resource::loadTexture(ResourceLocation(ResourceLocation::APP_PACKAGE, location.path));

	return data;
}

std::string ResourcePackManager::getPath(const std::string& path) const
{
	ResourceLocation location(ResourceLocation::EXTERNAL_DIR, ResourcePackRepository::RESOURCE_PACKS_PATH + "/" + path);
	return location.getFullPath();
}
