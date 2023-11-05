#include "AppPlatform_sdl.hpp"

#include "common/Utils.hpp"

AppPlatform_sdl::AppPlatform_sdl(std::string storageDir, SDL_Window *window)
    : AppPlatform_sdl_base(storageDir, window)
{
}

Texture AppPlatform_sdl::loadTexture(const std::string& path, bool b)
{
	Texture out;
	out.field_C = 1;
	out.field_D = 0;

    std::string realPath = getAssetPath(path);

	char *data = emscripten_get_preloaded_image_data(("/" + realPath).c_str(), &out.m_width, &out.m_height);
	if (data != NULL)
	{
		size_t data_size = out.m_width * out.m_height;
		out.m_pixels = new uint32_t[data_size];
		memcpy(out.m_pixels, data, data_size * sizeof(uint32_t));
		free(data);
		return out;
	}

	LOG_E("Couldn't find file: %s", realPath.c_str());
	return out;
}
bool AppPlatform_sdl::doesTextureExist(const std::string& path)
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
