#pragma once

#include <string>
#include <vector>

class ResourcePackRepository
{
public:
	static const std::string RESOURCE_PACKS_PATH;
	static const std::string BEHAVIOR_PACK_PATH;
	static const std::string RESOURCE_PACK_DOWNLOAD_CACHE_PATH;
	static const std::string RESOURCE_PACK_ICON_PATH;
	static std::vector<std::string> EXPECTED_EXTENSIONS;
};

