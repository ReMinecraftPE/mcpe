#pragma once

#include <string>

struct ResourceLocation
{
public:
	enum FileSystem
	{
		APP_PACKAGE, // From our built-in assets
		USER_PACKAGE, // From a resource pack, or built-in assets
		DATA_DIR,
		USER_DIR,
		WORLD_DIR,
		SETTINGS_DIR,
		EXTERNAL_DIR, // From the external storage directory
		RAW_PATH
	};

	std::string path;
	FileSystem fileSystem;

private:
	void _init(const std::string& path, FileSystem fileSystem = USER_PACKAGE);
	
public:
	ResourceLocation();
	ResourceLocation(const char* path);
	ResourceLocation(const std::string& path);
	ResourceLocation(FileSystem fileSystem, const std::string& path);

	std::string getFullPath();
};

