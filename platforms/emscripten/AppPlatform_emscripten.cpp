#include "AppPlatform_emscripten.hpp"

#include <emscripten.h>

#include "client/common/Utils.hpp"

AppPlatform_emscripten::AppPlatform_emscripten(std::string storageDir, SDL_Window *window)
    : AppPlatform_sdlbase(storageDir, window)
{
}

Texture AppPlatform_emscripten::loadTexture(const std::string& path, bool b)
{
	Texture out;
	out.field_C = 1;
	out.field_D = 0;

    std::string realPath = getAssetPath(path);

	char *data = emscripten_get_preloaded_image_data(("/" + realPath).c_str(), &out.m_width, &out.m_height);
	if (data != NULL)
	{
		size_t data_size = out.m_width * out.m_height * 4;
		out.m_pixels = (uint32_t *) new unsigned char[data_size];
		memcpy(out.m_pixels, data, data_size);
		free(data);
		return out;
	}

    // I don't think this logic makes any sense
	LogMsg("Couldn't find file: %s", realPath.c_str());
	return out;
}
