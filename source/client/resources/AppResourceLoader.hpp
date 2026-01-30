#pragma once

#include "ResourceLoader.hpp"

class AppResourceLoader : public ResourceLoader
{
public:
	AppResourceLoader(ResourceLocation::FileSystem fileSystem);

public:
	bool hasResource(const ResourceLocation& location) const override;
	bool getResourcePath(const ResourceLocation& location, std::string& path) const override;
	bool hasTexture(const ResourceLocation& location) const override;
	bool load(const ResourceLocation& location, std::string& stream) const override;
	void loadAllVersionsOf(const ResourceLocation& location) const override;
	TextureData loadTexture(const ResourceLocation& location) const override;
};

