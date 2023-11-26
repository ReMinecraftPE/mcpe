#include "AppPlatform_sdl.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cerrno>

#include "thirdparty/LibPNG/png.h"

#include "thirdparty/GL/GL.hpp"

#include "common/Utils.hpp"

AppPlatform_sdl::AppPlatform_sdl(std::string storageDir, SDL_Window *window)
	: AppPlatform_sdl_base(storageDir, window)
{
	setIcon(loadTexture("icon.png"));
}

// Take Screenshot
static int save_png(const char *filename, unsigned char *pixels, int line_size, int width, int height)
{
	// Return value
	int ret = 0;

	// Variables
	png_structp png = NULL;
	png_infop info = NULL;
	FILE *file = NULL;
	png_colorp palette = NULL;
	png_bytep *rows = new png_bytep[height];
	for (int i = 0; i < height; ++i)
	{
		rows[height - i - 1] = (png_bytep)(&pixels[i * line_size]);
	}

	// Init
	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
	{
		ret = 1;
		goto ret;
	}
	info = png_create_info_struct(png);
	if (!info)
	{
		ret = 1;
		goto ret;
	}

	// Open File
	file = fopen(filename, "wb");
	if (!file)
	{
		ret = 1;
		goto ret;
	}

	// Prepare To Write
	png_init_io(png, file);
	png_set_IHDR(png, info, width, height, 8 /* Depth */, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	palette = (png_colorp) png_malloc(png, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
	if (!palette)
	{
		ret = 1;
		goto ret;
	}
	png_set_PLTE(png, info, palette, PNG_MAX_PALETTE_LENGTH);
	png_write_info(png, info);
	png_set_packing(png);

	// Write
	png_write_image(png, rows);
	png_write_end(png, info);

ret:
	// Free
	if (palette != NULL)
	{
		png_free(png, palette);
	}
	if (rows != NULL)
	{
		delete[] rows;
	}
	if (file != NULL)
	{
		fclose(file);
	}
	if (png != NULL)
	{
		png_destroy_write_struct(&png, &info);
	}

	// Return
	return ret;
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
	unsigned char *pixels = (unsigned char *) malloc(size);
	if (pixels == NULL)
	{
		fail = true;
	}
	else
	{
		glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	// Save Image
	if (fail || save_png(file.c_str(), pixels, line_size, width, height))
	{
		LOG_E("Screenshot Failed: %s", file.c_str());
	}
	else
	{
		LOG_I("Screenshot Saved: %s", file.c_str());
	}

	// Free
	if (pixels != NULL)
	{
		free(pixels);
	}
}

static void png_read_sdl(png_structp png_ptr, png_bytep data, png_size_t length)
{
	SDL_RWread((SDL_RWops *) png_get_io_ptr(png_ptr), (char *) data, length, 1);
}

static void nop_png_warning(png_structp png_ptr, png_const_charp warning_message)
{
	// Do Nothing
}

Texture AppPlatform_sdl::loadTexture(const std::string& path, bool b)
{
	Texture out;
	out.field_C = 1;
	out.field_D = 0;

    std::string realPath = getAssetPath(path);

	SDL_RWops *io = SDL_RWFromFile(realPath.c_str(), "rb");

	if (!io)
	{
		LOG_E("Couldn't find file: %s", path.c_str());
		return out;
	}

	png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, nop_png_warning);
	if (!pngPtr)
	{
		return out;
	}

	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (!infoPtr)
	{
		png_destroy_read_struct(&pngPtr, NULL, NULL);
		return out;
	}

	png_set_read_fn(pngPtr, (png_voidp) io, png_read_sdl);

	png_read_info(pngPtr, infoPtr);

	png_set_expand(pngPtr);
	png_set_strip_16(pngPtr);
	png_set_gray_to_rgb(pngPtr);
	png_read_update_info(pngPtr, infoPtr);

	out.m_width = png_get_image_width(pngPtr, infoPtr);
	out.m_height = png_get_image_height(pngPtr, infoPtr);

	int pixelSize = 4;

	png_bytep *rowPtrs = new png_bytep[out.m_height];
	unsigned char *pixels = new unsigned char[pixelSize * out.m_width * out.m_height];

	int rowStrideBytes = pixelSize * out.m_width;
	for (int i = 0; i < out.m_height; i++)
	{
		rowPtrs[i] = (png_bytep) &pixels[i * rowStrideBytes];
	}
	png_read_image(pngPtr, rowPtrs);

	// Convert RGB Images To RGBA
	bool opaque = png_get_color_type(pngPtr, infoPtr) != PNG_COLOR_TYPE_RGBA;
	if (opaque)
	{
		for (int y = 0; y < out.m_height; y++)
		{
			unsigned char *row = &pixels[y * rowStrideBytes];
			for (int x = out.m_width - 1; x >= 0; x--)
			{
				// Find Indexes
				int rgb = x * 3;
				int rgba = x * 4;

				// Read RGB Pixel
				unsigned char a = row[rgb];
				unsigned char b = row[rgb + 1];
				unsigned char c = row[rgb + 2];

				// Store RGBA Pixel
				row[rgba] = a;
				row[rgba + 1] = b;
				row[rgba + 2] = c;
				row[rgba + 3] = 255;
			}
		}
	}

	out.m_pixels = (uint32_t *) pixels;

	png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp) 0);
	delete[](png_bytep) rowPtrs;
	SDL_RWclose(io);

	return out;
}

bool AppPlatform_sdl::hasFileSystemAccess()
{
	return true;
}

std::string AppPlatform_sdl::getPatchData()
{
    std::string path = getAssetPath("patches/patch_data.txt");
    SDL_RWops *io = SDL_RWFromFile(path.c_str(), "rb");
    
	if (!io)
	{
		LOG_W("Couldn't find patch data file!");
		return "";
	}
    size_t size = io->size(io);
    if (size == -1)
    {
        LOG_E("Error determining the size of the patch data file!");
    }
    
	char *buf = new char[size];
    SDL_RWread(io, buf, size, 1);
    
    SDL_RWclose(io);
    
    return std::string(buf);
}
