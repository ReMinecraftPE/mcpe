#include "AppResourceLoader.hpp"
#include "client/app/AppPlatform.hpp"

AppResourceLoader::AppResourceLoader(ResourceLocation::FileSystem fileSystem)
	: ResourceLoader(fileSystem)
{
}

bool AppResourceLoader::hasResource(const ResourceLocation& location) const
{
	return AppPlatform::singleton()->hasAssetFile(getPath(location.path));
}

bool AppResourceLoader::getResourcePath(const ResourceLocation& location, std::string& path) const
{
	path = getPath(location.path);
	return AppPlatform::singleton()->hasAssetFile(path);
}

bool AppResourceLoader::hasTexture(const ResourceLocation& location) const
{
	return AppPlatform::singleton()->doesTextureExist(getPath(location.path));
}

bool AppResourceLoader::load(const ResourceLocation& location, std::string& stream) const
{
	AssetFile file = AppPlatform::singleton()->readAssetFile(getPath(location.path), true);

	if (file.data)
	{
		stream.assign((char*)file.data, file.size);
		delete[] file.data;
	}

	return !stream.empty();
}

void AppResourceLoader::loadAllVersionsOf(const ResourceLocation& location) const
{
	// I don't even know, fuck this function
}

TextureData AppResourceLoader::loadTexture(const ResourceLocation& location) const
{
	return AppPlatform::singleton()->loadTexture(getPath(location.path));
}
