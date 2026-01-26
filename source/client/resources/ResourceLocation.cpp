#include "ResourceLocation.hpp"
#include "common/Util.hpp"
#include "Resource.hpp"

ResourceLocation::ResourceLocation()
	: ResourceLocation(Util::EMPTY_STRING)
{
}

ResourceLocation::ResourceLocation(const char* path)
	: path(path)
	, fileSystem(USER_PACKAGE)
{
}

ResourceLocation::ResourceLocation(const std::string& path)
	: path(path)
	, fileSystem(USER_PACKAGE)
{
}

std::string ResourceLocation::getFullPath()
{
	ResourceLoader* pLoader = Resource::getLoader(fileSystem);
	if (!pLoader)
		return path;

	return pLoader->getPath() + path;
}
