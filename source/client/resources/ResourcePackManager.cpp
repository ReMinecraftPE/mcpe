#include "ResourcePackManager.hpp"
#include "client/app/AppPlatform.hpp"
#include "Resource.hpp"

ResourcePackManager::ResourcePackManager()
	: ResourceLoader(ResourceLocation::USER_PACKAGE)
{
	m_pPacks = nullptr;
}

bool ResourcePackManager::hasResource(const ResourceLocation& location) const
{
	bool hasAssetFile = false;
	std::string slashpath = "/" + location.path;
	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			hasAssetFile = AppPlatform::singleton()->hasAssetFile(getPath(*it + slashpath));
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
	bool hasAssetFile = false;
	std::string slashpath = "/" + location.path;
	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			path = getPath(*it + slashpath);
			hasAssetFile = AppPlatform::singleton()->hasAssetFile(path);
			if (hasAssetFile)
				break;
		}
	}

	if (!hasAssetFile)
		hasAssetFile = Resource::getResourcePath(ResourceLocation(ResourceLocation::APP_PACKAGE, location.path), path);

	return hasAssetFile;
}

bool ResourcePackManager::hasTexture(const ResourceLocation& location) const
{
	bool hasAssetFile = false;
	std::string slashpath = "/" + location.path;
	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			hasAssetFile = AppPlatform::singleton()->doesTextureExist(getPath(*it + slashpath));
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
	AssetFile file;
	std::string slashpath = "/" + location.path;
	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			file = AppPlatform::singleton()->readAssetFile(getPath(*it + slashpath), true);
			if (file.data)
				break;
		}
	}

	if (file.data)
	{
		stream.assign((char*)file.data, file.size);
		delete[] file.data;
	}
	else
	{
		Resource::load(ResourceLocation(ResourceLocation::APP_PACKAGE, location.path), stream);
	}

	return !stream.empty();
}

void ResourcePackManager::loadAllVersionsOf(const ResourceLocation& location) const
{
	// I don't even know, fuck this function
}

TextureData ResourcePackManager::loadTexture(const ResourceLocation& location) const
{
	TextureData data;
	std::string slashpath = "/" + location.path;
	if (m_pPacks)
	{
		for (ResourcePackStack::const_iterator it = m_pPacks->begin(); it != m_pPacks->end(); it++)
		{
			data = AppPlatform::singleton()->loadTexture(getPath(*it + slashpath));
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
	ResourceLocation location(ResourceLocation::EXTERNAL_DIR, "resource_packs/" + path);
	return location.getFullPath();
}
