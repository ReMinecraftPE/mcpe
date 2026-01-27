#include "ResourceLoader.hpp"
#include "client/app/AppPlatform.hpp"

ResourceLoader::ResourceLoader(ResourceLocation::FileSystem fileSystem)
	: m_fileSystem(fileSystem)
{
}

ResourceLoader::~ResourceLoader()
{
}

std::string ResourceLoader::GetPath(ResourceLocation::FileSystem fileSystem, const std::string& path)
{
	AppPlatform& platform = *AppPlatform::singleton();
	switch (fileSystem)
	{
	case ResourceLocation::APP_PACKAGE:  return platform.getAssetPath(path);
	case ResourceLocation::EXTERNAL_DIR: return platform.getExternalStoragePath(path); // platform()->getExternalStoragePath("/resource_packs/")
	case ResourceLocation::RAW_PATH:     return path;
	default:
		LOG_E("Unsupported Resource FileSystem: %d", fileSystem);
		throw std::bad_cast();
	}
}

std::string ResourceLoader::getPath(const std::string& path) const
{
	return GetPath(getFileSystem(), path);
}