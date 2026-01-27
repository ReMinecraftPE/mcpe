#pragma once

#include <string>
#include <vector>

#include "client/renderer/texture/TextureData.hpp"
#include "ResourceLocation.hpp"

class ResourceLoader
{
public:
	ResourceLoader(ResourceLocation::FileSystem fileSystem);
	virtual ~ResourceLoader();

protected:
	static std::string GetPath(ResourceLocation::FileSystem fileSystem, const std::string& path);

public:
	virtual bool hasResource(const ResourceLocation& location) const = 0;
	virtual bool hasTexture(const ResourceLocation& location) const = 0;
	virtual bool load(const ResourceLocation& location, std::string& stream) const = 0;
	virtual void loadAllVersionsOf(const ResourceLocation& location) const = 0;
	virtual TextureData loadTexture(const ResourceLocation& location) const = 0;
	virtual std::string getPath(const std::string& path) const;

	const ResourceLocation::FileSystem getFileSystem() const { return m_fileSystem; }

private:
	ResourceLocation::FileSystem m_fileSystem;
};

