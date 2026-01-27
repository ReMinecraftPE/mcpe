#include "ResourceLocation.hpp"
#include "common/Util.hpp"
#include "Resource.hpp"

void ResourceLocation::_init(const std::string& path, FileSystem fileSystem)
{
	this->path = path;
	this->fileSystem = fileSystem;
}

ResourceLocation::ResourceLocation()
{
	_init("");
}

ResourceLocation::ResourceLocation(const char* path)
{
	_init(path);
}

ResourceLocation::ResourceLocation(const std::string& path)
{
	_init(path);
}

ResourceLocation::ResourceLocation(FileSystem fileSystem, const std::string& path)
{
	_init(path, fileSystem);
}

std::string ResourceLocation::getFullPath()
{
	ResourceLoader* pLoader = Resource::getLoader(fileSystem);
	if (!pLoader)
		return path;

	return pLoader->getPath(path);
}
