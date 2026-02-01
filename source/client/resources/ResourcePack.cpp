#include "ResourcePack.hpp"

ResourcePack::ResourcePack(const std::string& name, ResourceLocation::FileSystem fileSystem)
	: m_name(name)
	, m_fileSystem(fileSystem)
{
}