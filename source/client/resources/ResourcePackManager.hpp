#pragma once

#include <vector>

#include "ResourceLoader.hpp"

typedef std::vector<std::string> ResourcePackStack;

class ResourcePackManager : public ResourceLoader
{
public:
	ResourcePackManager(const std::string& path);

public:
	bool hasResource(const ResourceLocation& location) const override;
	bool load(const ResourceLocation& location, std::string& stream) const override;
	void loadAllVersionsOf(const ResourceLocation& location) const override;
	TextureData loadTexture(const ResourceLocation& location) const override;

public:
	ResourcePackStack* m_pPacks;
};

