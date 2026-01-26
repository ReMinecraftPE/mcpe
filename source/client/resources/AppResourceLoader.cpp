#include "AppResourceLoader.hpp"
#include "client/app/AppPlatform.hpp"

AppResourceLoader::AppResourceLoader(const std::string& path)
	: ResourceLoader(path)
{
}

bool AppResourceLoader::hasResource(const ResourceLocation& location) const
{
	return AppPlatform::singleton()->hasAssetFile(getPath() + location.path);
}

bool AppResourceLoader::load(const ResourceLocation& location, std::string& stream) const
{
	AssetFile file = AppPlatform::singleton()->readAssetFile(getPath() + location.path, false);

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
	return AppPlatform::singleton()->loadTexture(getPath() + location.path);
}
