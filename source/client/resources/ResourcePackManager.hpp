#pragma once

#include <vector>

#include "ResourceLoader.hpp"

typedef std::vector<std::string> ResourcePackStack;

class ResourcePackManager : public ResourceLoader
{
public:
	ResourcePackManager();

public:
	bool hasResource(const ResourceLocation& location) const override;
	bool getResourcePath(const ResourceLocation& location, std::string& path) const override;
	bool hasTexture(const ResourceLocation& location) const override;
	bool load(const ResourceLocation& location, std::string& stream) const override;
	void loadAllVersionsOf(const ResourceLocation& location) const override;
	TextureData loadTexture(const ResourceLocation& location) const override;
	std::string getPath(const std::string& path) const override;

public:
	ResourcePackStack* m_pPacks;
};

