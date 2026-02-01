#pragma once

#include <string>
#include "ResourceLocation.hpp"

class ResourcePack
{
public:
	ResourcePack(const std::string& name, ResourceLocation::FileSystem fileSystem = ResourceLocation::EXTERNAL_DIR);

public:
	std::string m_name;
	ResourceLocation::FileSystem m_fileSystem;
};