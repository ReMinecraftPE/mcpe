#include "AppPlatform_sdl2_emscripten.hpp"

#include "common/Utils.hpp"

AppPlatform_sdl2_emscripten::AppPlatform_sdl2_emscripten(std::string storageDir, SDL_Window *window)
    : AppPlatform_sdl2(storageDir, window)
{
}

void AppPlatform_sdl2_emscripten::loadImage(ImageData& data, const std::string& path)
{
    std::string realPath = getAssetPath(path);

	char *rawData = emscripten_get_preloaded_image_data(("/" + realPath).c_str(), &data.m_width, &data.m_height);
	if (rawData == nullptr)
	{
		LOG_E("Couldn't find file: %s", realPath.c_str());
		return;
	}
	
	data.m_data = (uint8_t*)rawData;
	data.m_colorSpace = COLOR_SPACE_RGBA;
}

bool AppPlatform_sdl2_emscripten::doesTextureExist(const std::string& path) const
{
	// Get Full Path
	std::string realPath = getAssetPath(path);

	// Open File
	int width = 0, height = 0;
	char *data = emscripten_get_preloaded_image_data(("/" + realPath).c_str(), &width, &height);
	if (!data)
	{
		// Does Not Exist
		return false;
	}
	else
	{
		// File Exists
		free(data);
		return true;
	}
}
