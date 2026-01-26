#pragma once

#include <string>
#include <vector>

#include "client/renderer/texture/TextureData.hpp"
#include "ResourceLocation.hpp"

class ResourceLoader
{
public:
	ResourceLoader(const std::string& path);

public:
	virtual bool hasResource(const ResourceLocation& location) const = 0;
	virtual bool load(const ResourceLocation& location, std::string& stream) const = 0;
	virtual void loadAllVersionsOf(const ResourceLocation& location) const = 0;
	virtual TextureData loadTexture(const ResourceLocation& location) const = 0;

	const std::string& getPath() const { return m_path; }

private:
	std::string m_path;
};

