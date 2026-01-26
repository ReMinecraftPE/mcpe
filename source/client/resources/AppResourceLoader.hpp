#pragma once

#include "ResourceLoader.hpp"

class AppResourceLoader : public ResourceLoader
{
public:
	AppResourceLoader(const std::string& path);

public:
	bool hasResource(const ResourceLocation& location) const override;
	bool load(const ResourceLocation& location, std::string& stream) const override;
	void loadAllVersionsOf(const ResourceLocation& location) const override;
	TextureData loadTexture(const ResourceLocation& location) const override;
};

