#pragma once

#include <vector>

#include "ResourceLoader.hpp"
#include "ResourcePack.hpp"

typedef std::vector<ResourcePack> ResourcePackStack;

class ResourcePackManager : public ResourceLoader
{
public:
	ResourcePackManager();

private:
	ResourceLocation _getLocationInPack(const ResourceLocation& location, const ResourcePack& pack) const;

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

