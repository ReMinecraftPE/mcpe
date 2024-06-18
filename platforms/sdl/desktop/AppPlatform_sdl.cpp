#include "AppPlatform_sdl.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cerrno>

#include "stb_image.h"
#include "stb_image_write.h"

#include "thirdparty/GL/GL.hpp"

#include "common/Utils.hpp"

AppPlatform_sdl::AppPlatform_sdl(std::string storageDir, SDL_Window *window)
	: AppPlatform_sdl_base(storageDir, window)
{
	setIcon(loadTexture("icon.png", false));
}

// Take Screenshot
static int save_png(const char *filename, unsigned char *pixels, int line_size, int width, int height)
{
	// Setup
	stbi_flip_vertically_on_write(true);

	// Write Image
	return stbi_write_png(filename, width, height, 4, pixels, line_size);
}

// Ensure Screenshots Folder Exists
void AppPlatform_sdl::ensureDirectoryExists(const char* path)
{
	// Check Screenshots Folder
	struct stat obj;
	if (stat(path, &obj) != 0 || !S_ISDIR(obj.st_mode))
	{
		// Create Screenshots Folder
#if defined(_WIN32) && !defined(__MINGW32__)
		int ret = XPL_MKDIR(path);
#else
		int ret = XPL_MKDIR(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
		if (ret != 0)
		{
			// Unable To Create Folder
			LOG_E("Error Creating Directory: %s: %s", path, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}

void AppPlatform_sdl::saveScreenshot(const std::string& filename, int glWidth, int glHeight)
{
	// Get Directory
	std::string screenshots = _storageDir + "/screenshots";

	// Get Timestamp
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char time[256];
	strftime(time, sizeof (time), "%Y-%m-%d_%H.%M.%S", timeinfo);

	// Ensure Screenshots Folder Exists
	ensureDirectoryExists(screenshots.c_str());

	// Prevent Overwriting Screenshots
	int num = 1;
	const std::string path = screenshots + "/";
	std::string file = path + time + ".png";
	while (XPL_ACCESS(file.c_str(), F_OK) != -1)
	{
		file = path + SSTR(time << "-" << num << ".png");
		num++;
	}

	// Get Image Size
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int x = viewport[0];
	int y = viewport[1];
	int width = viewport[2];
	int height = viewport[3];

	// Get Line Size
	int line_size = width * 4;
	{
		// Handle Alignment
		int alignment;
		glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
		// Round
		int diff = line_size % alignment;
		if (diff > 0)
		{
			line_size = line_size + (alignment - diff);
		}
	}
	int size = height * line_size;

	// Read Pixels
	bool fail = false;
	unsigned char *pixels = new unsigned char[size];
	if (!pixels)
	{
		fail = true;
	}
	else
	{
		glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	// Save Image
	if (fail || !save_png(file.c_str(), pixels, line_size, width, height))
	{
		LOG_E("Screenshot Failed: %s", file.c_str());
	}
	else
	{
		LOG_I("Screenshot Saved: %s", file.c_str());
	}

	// Free
	if (!pixels)
	{
		delete[] pixels;
	}
}

Texture AppPlatform_sdl::loadTexture(const std::string& path, bool bIsRequired)
{
	Texture out;
	out.field_C = 1;
	out.field_D = 0;

	// Get Full Path
    std::string realPath = getAssetPath(path);

	// Read File
	SDL_RWops *io = SDL_RWFromFile(realPath.c_str(), "rb");
	if (!io)
	{
		LOG_E("Couldn't find file: %s", path.c_str());
		return out;
	}
	Sint64 size = SDL_RWsize(io);
	unsigned char *file = new unsigned char[size];
	SDL_RWread(io, file, size, 1);
	SDL_RWclose(io);

	// Parse Image
	int width = 0, height = 0, channels = 0;
	stbi_uc *img = stbi_load_from_memory(file, size, &width, &height, &channels, STBI_rgb_alpha);
	delete[] file;
	if (!img)
	{
		// Failed To Parse Image
		LOG_E("The image could not be loaded properly: %s", path.c_str());
		return out;
	}

	// Copy Image
	uint32_t *img2 = new uint32_t[width * height];
	memcpy(img2, img, width * height * sizeof (uint32_t));
	stbi_image_free(img);

	// Create Texture
	out.m_width = width;
	out.m_height = height;
	out.m_pixels = img2;

	// Return
	return out;
}
bool AppPlatform_sdl::doesTextureExist(const std::string& path)
{
	// Get Full Path
	std::string realPath = getAssetPath(path);

	// Open File
	SDL_RWops *io = SDL_RWFromFile(realPath.c_str(), "rb");
	if (!io)
	{
		// Does Not Exist
		return false;
	}
	else
	{
		// File Exists
		SDL_RWclose(io);
		return true;
	}
}

bool AppPlatform_sdl::hasFileSystemAccess()
{
	return true;
}
