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

public:
	bool operator==(const ResourcePack& other) const
	{
		return this->m_name == other.m_name &&
			   this->m_fileSystem == other.m_fileSystem;
	}
};