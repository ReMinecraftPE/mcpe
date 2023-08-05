#include "AppPlatform_sdl.hpp"

#include <sstream>
#include <fstream>
#include <sys/stat.h>

#ifndef __EMSCRIPTEN__
#include <png.h>

#include "compat/GL.hpp"
#else
#include <emscripten.h>
#endif

#include "client/common/Utils.hpp"

AppPlatform_sdl::AppPlatform_sdl(std::string storageDir, SDL_Window *window)
{
	_storageDir = storageDir;
	_window = window;
}

int AppPlatform_sdl::checkLicense()
{
	// we own the game!!
	return 1;
}

// Ensure Screenshots Folder Exists
void ensure_screenshots_folder(const char *screenshots)
{
	// Check Screenshots Folder
	struct stat obj;
	if (stat(screenshots, &obj) != 0 || !S_ISDIR(obj.st_mode))
	{
		// Create Screenshots Folder
#ifdef _WIN32
		int ret = mkdir(screenshots);
#else
		int ret = mkdir(screenshots, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
		if (ret != 0)
		{
			// Unable To Create Folder
			LogMsg("Error Creating Directory: %s: %s", screenshots, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}

#ifndef __EMSCRIPTEN__
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
	png_bytep rows[height];
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
	ensure_screenshots_folder(screenshots.c_str());

	// Prevent Overwriting Screenshots
	int num = 1;
	std::string file = screenshots + '/' + time + ".png";
	while (access(file.c_str(), F_OK) != -1)
	{
		file = screenshots + '/' + time + '-' + std::to_string(num) + ".png";
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
		LogMsg("Screenshot Failed: %s", file.c_str());
	}
	else
	{
		LogMsg("Screenshot Saved: %s", file.c_str());
	}

	// Free
	if (pixels != NULL)
	{
		free(pixels);
	}
}
#endif

int AppPlatform_sdl::getScreenWidth() const
{
	int width;
	SDL_GL_GetDrawableSize(_window, &width, nullptr);
	return width;
}

int AppPlatform_sdl::getScreenHeight() const
{
	int height;
	SDL_GL_GetDrawableSize(_window, nullptr, &height);
	return height;
}

#ifndef __EMSCRIPTEN__
static void png_read_sdl(png_structp png_ptr, png_bytep data, png_size_t length)
{
	SDL_RWread((SDL_RWops *) png_get_io_ptr(png_ptr), (char *) data, length, 1);
}
static void nop_png_warning(png_structp png_ptr, png_const_charp warning_message)
{
	// Do Nothing
}
#endif
Texture AppPlatform_sdl::loadTexture(const std::string& str, bool b)
{
	Texture out;
	out.field_C = 1;
	out.field_D = 0;

	std::string realPath = str;
	if (realPath.size() && realPath[0] == '/')
	{
		// trim it off
		realPath = realPath.substr(1);
	}
	realPath = "assets/" + realPath;

#ifndef __EMSCRIPTEN__
	SDL_RWops *io = SDL_RWFromFile(realPath.c_str(), "rb");

	if (io != NULL)
	{
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
	}
#else
	char *data = emscripten_get_preloaded_image_data(("/" + realPath).c_str(), &out.m_width, &out.m_height);
	if (data != NULL)
	{
		size_t data_size = out.m_width * out.m_height * 4;
		out.m_pixels = (uint32_t *) new unsigned char[data_size];
		memcpy(out.m_pixels, data, data_size);
		free(data);
		return out;
	}
#endif

	LogMsg("Couldn't find file: %s", str.c_str());
	return out;
}

void AppPlatform_sdl::setMouseGrabbed(bool b)
{
	SDL_SetWindowGrab(_window, b ? SDL_TRUE : SDL_FALSE);
	SDL_SetRelativeMouseMode(b ? SDL_TRUE : SDL_FALSE);
}

void AppPlatform_sdl::setMouseDiff(int x, int y)
{
	xrel = x;
	yrel = y;
}

void AppPlatform_sdl::getMouseDiff(int& x, int& y)
{
	x = xrel;
	y = yrel;
}

void AppPlatform_sdl::clearDiff()
{
	xrel = 0;
	yrel = 0;
}

bool AppPlatform_sdl::shiftPressed()
{
	return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_sdl::setShiftPressed(bool b, bool isLeft)
{
	m_bShiftPressed[isLeft ? 0 : 1] = b;
}

int AppPlatform_sdl::getUserInputStatus()
{
	return -1;
}
