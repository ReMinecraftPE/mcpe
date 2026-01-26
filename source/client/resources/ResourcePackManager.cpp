#include "ResourcePackManager.hpp"
#include "client/app/AppPlatform.hpp"

ResourcePackManager::ResourcePackManager(const std::string& path)
	: ResourceLoader(path)
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
			hasAssetFile = AppPlatform::singleton()->hasAssetFile(getPath() + *it + slashpath);
			if (hasAssetFile)
				break;
		}
	}

	if (!hasAssetFile)
		hasAssetFile = AppPlatform::singleton()->hasAssetFile("." + slashpath);

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
			file = AppPlatform::singleton()->readAssetFile(getPath() + *it + slashpath, true);
			if (file.data)
				break;
		}
	}

	if (!file.data)
		file = AppPlatform::singleton()->readAssetFile("." + slashpath, false);

	if (file.data)
	{
		stream.assign((char*)file.data, file.size);
		delete[] file.data;
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
			data = AppPlatform::singleton()->loadTexture(getPath() + *it + slashpath);
			if (!data.isEmpty())
				break;
		}
	}

	if (data.isEmpty())
		data = AppPlatform::singleton()->loadTexture("." + slashpath);

	return data;
}
