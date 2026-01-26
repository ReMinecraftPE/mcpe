#pragma once

#include <string>

struct ResourceLocation
{
	enum FileSystem
	{
		APP_PACKAGE, // From our built-in assets
		USER_PACKAGE, // From a resource pack, or built-in assets
		DATA_DIR,
		USER_DIR,
		WORLD_DIR,
		SETTINGS_DIR,
		EXTERNAL_DIR,
		RAW_PATH
	};

	std::string path;
	FileSystem fileSystem;

	ResourceLocation();
	ResourceLocation(const char* path);
	ResourceLocation(const std::string& path);

	std::string getFullPath();
};

