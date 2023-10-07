#include "AppPlatform_sdl.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cerrno>
#include "thirdparty/stb_image.h"
#include "thirdparty/stb_image_write.h"
#include "thirdparty/GL/GL.hpp"

#include "common/Utils.hpp"

AppPlatform_sdl::AppPlatform_sdl(std::string storageDir, SDL_Window *window)
    : AppPlatform_sdlbase(storageDir, window)
{
	setIcon(loadTexture("icon.png"));
}

// Ensure Screenshots Folder Exists
void AppPlatform_sdl::ensureDirectoryExists(const char* path)
{
	// Check Screenshots Folder
	struct stat obj;
	if (stat(path, &obj) != 0 || !S_ISDIR(obj.st_mode))
	{
		// Create Screenshots Folder
#ifdef _WIN32
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

void AppPlatform_sdl::saveScreenshot(const std::string& filename, int width, int height)
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

	int npixels = width * height;
	uint32_t* pixels = new uint32_t[npixels];
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_flip_vertically_on_write(true);

	stbi_write_png(file.c_str(), width, height, 4, pixels, width * 4);

	delete[] pixels;
}

int SdlReadWrapper(void* User, char* Data, int Size)
{
	SDL_RWops* io = (SDL_RWops*)User;
	return io->read(io, Data, Size, 1);
}

void SdlSkipWrapper(void* User, int Size)
{
	SDL_RWops* io = (SDL_RWops*)User;

	if (Size > 0)
	{
		io->seek(io, Size, RW_SEEK_CUR);
	}
	else
	{
		int pos = io->seek(io, 0, RW_SEEK_CUR);
		io->seek(io, pos - Size, RW_SEEK_SET);
	}
}

int SdlEofWrapper(void* User)
{
	SDL_RWops* io = (SDL_RWops*)User;

	char data[1];
	int old_pos = io->seek(io, 0, RW_SEEK_CUR);
	return old_pos >= io->size(io);
}

static const stbi_io_callbacks SdlIoCallbacks[] =
{
	SdlReadWrapper,
	SdlSkipWrapper,
	SdlEofWrapper,
};

Texture AppPlatform_sdl::loadTexture(const std::string& path, bool b)
{
	Texture out;
	out.field_C = 1;
	out.field_D = 0;

    std::string realPath = getAssetPath(path);

	SDL_RWops* io = SDL_RWFromFile(realPath.c_str(), "rb");

	if (!io)
	{
		LOG_E("Couldn't find file: %s", path.c_str());
		return out;
	}

	int width = 0, height = 0, channels = 0;

	stbi_uc* img = stbi_load_from_callbacks(&SdlIoCallbacks, io, &width, &height, &channels, STBI_rgb_alpha);
	if (!img)
	{
		LOG_E("File %s couldn't be loaded via stb_image", realPath.c_str());
		goto _error;
	}

	uint32_t* img2 = new uint32_t[width * height];
	memcpy(img2, img, width * height * sizeof(uint32_t));
	stbi_image_free(img);
	img = nullptr;

	io->close(io);
	return Texture(width, height, img2, 1, 0);
}

bool AppPlatform_sdl::hasFileSystemAccess()
{
	return true;
}
