#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>

#include "AppPlatform_sdl2.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include "thirdparty/GL/GL.hpp"
#endif

#include "common/Utils.hpp"

#include "CustomSoundSystem.hpp"
// Macros are cursed
#define _STR(x) #x
#define STR(x) _STR(x)

#include "client/player/input/Controller.hpp"

void AppPlatform_sdl2::_init(std::string storageDir, SDL_Window *window)
{
	_storageDir = storageDir;
	_window = window;

	_iconTexture = nullptr;
	_icon = nullptr;

	m_bShiftPressed[0] = false;
	m_bShiftPressed[1] = false;

	ensureDirectoryExists(_storageDir.c_str());

	m_pSoundSystem = nullptr;

	// Default Touchscreen Mode
#ifdef ANDROID
	m_bIsTouchscreen = true;
#else
	m_bIsTouchscreen = false;
#endif
	// Custom Touchscreen Mode
	const char *mode = getenv("MCPE_INPUT_MODE");
	if (mode)
	{
		if (strcmp(mode, "touch") == 0)
		{
			m_bIsTouchscreen = true;
		}
		else if (strcmp(mode, "mouse") == 0)
		{
			m_bIsTouchscreen = false;
		}
	}

	// Look for a pre-existing controller
	_controller = findGameController();

	clearDiff();
}

void AppPlatform_sdl2::initSoundSystem()
{
	if (!m_pSoundSystem)
	{
		LOG_I("Initializing " STR(SOUND_SYSTEM) "...");
		m_pSoundSystem = new SOUND_SYSTEM();
	}
	else
	{
		LOG_E("Trying to initialize SoundSystem more than once!");
	}
}

void AppPlatform_sdl2::setIcon(const Texture& icon)
{
	if (!icon.m_pixels)
		return;

	SAFE_DELETE(_iconTexture);
	if (_icon) SDL_FreeSurface(_icon);

	_iconTexture = new Texture(icon);
	_icon = GetSurfaceForTexture(*_iconTexture);

	if (_icon)
		SDL_SetWindowIcon(_window, _icon);
}

AppPlatform_sdl2::~AppPlatform_sdl2()
{
	if (_icon) SDL_FreeSurface(_icon);
	SAFE_DELETE(_iconTexture);

	SAFE_DELETE(m_pSoundSystem);
}

SDL_GameController* AppPlatform_sdl2::findGameController()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			return SDL_GameControllerOpen(i);
		}
	}

	return nullptr;
}

SDL_Surface* AppPlatform_sdl2::GetSurfaceForTexture(const Texture& texture)
{
	void* pixels = texture.m_pixels;
	int width = texture.m_width;
	int height = texture.m_height;
	int depth = 32; // Color depth (32-bit by default)

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
			pixels, width, height, depth,
			width * 4, // Pitch
			0x000000FF, 0x0000FF00, 0x00FF0000,
			0xFF000000
	);
	if (!surface)
		LOG_E("Failed loading SDL_Surface from Texture: %s", SDL_GetError());

	return surface;
}

int AppPlatform_sdl2::checkLicense()
{
	// we own the game!!
	return 1;
}

const char* const AppPlatform_sdl2::getWindowTitle() const
{
	return SDL_GetWindowTitle(_window);
}

int AppPlatform_sdl2::getScreenWidth() const
{
	int width;
	SDL_GL_GetDrawableSize(_window, &width, nullptr);
	return width;
}

int AppPlatform_sdl2::getScreenHeight() const
{
	int height;
	SDL_GL_GetDrawableSize(_window, nullptr, &height);
	return height;
}

void AppPlatform_sdl2::setMouseGrabbed(bool b)
{
	SDL_SetWindowGrab(_window, b ? SDL_TRUE : SDL_FALSE);
	/**
	 * @NOTE: There is a bug with older versions of SDL2 (ex: 2.0.4) where disabling RelativeMouseMode will cause a mouse event to be fired
	 * that just moves the cursor to where it would've been if the mode had never been enabled in the first place, effectively uncentering it.
	 * https://github.com/libsdl-org/SDL/issues/6002 (I'm not sure if this is the right issue, I just updated SDL after seeing this and it fixed the above problem.)
	 **/
	SDL_SetRelativeMouseMode(b ? SDL_TRUE : SDL_FALSE);
	clearDiff();
}

void AppPlatform_sdl2::setMouseDiff(int x, int y)
{
	xrel += x;
	yrel += y;
}

void AppPlatform_sdl2::getMouseDiff(int& x, int& y)
{
	x = xrel;
	y = yrel;
}

void AppPlatform_sdl2::clearDiff()
{
	xrel = 0;
	yrel = 0;
}

bool AppPlatform_sdl2::shiftPressed()
{
	return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_sdl2::setShiftPressed(bool b, bool isLeft)
{
	m_bShiftPressed[isLeft ? 0 : 1] = b;
}

int AppPlatform_sdl2::getUserInputStatus()
{
	return -1;
}

MouseButtonType AppPlatform_sdl2::GetMouseButtonType(SDL_MouseButtonEvent event)
{
	switch (event.button)
	{
		case SDL_BUTTON_LEFT:
			return BUTTON_LEFT;
		case SDL_BUTTON_RIGHT:
			return BUTTON_RIGHT;
		case SDL_BUTTON_MIDDLE:
			return BUTTON_MIDDLE;
		default:
			return BUTTON_NONE;
	}
}

bool AppPlatform_sdl2::GetMouseButtonState(SDL_Event event)
{
	bool result;

	switch (event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
			result = true;
			break;
		case SDL_MOUSEBUTTONUP:
			result = false;
			break;
		case SDL_MOUSEWHEEL:
		{
			short wheelDelta = event.wheel.y;
			if (wheelDelta > 0)
			{
				// "A positive value indicates that the wheel was rotated forward, away from the user."
				result = false;
			}
			else
			{
				// "A negative value indicates that the wheel was rotated backward, toward the user."
				result = true;
			}
			break;
		}
		default:
			result = false;
			break;
	}

	return result;
}

Keyboard::KeyState AppPlatform_sdl2::GetKeyState(uint8_t state)
{
	switch (state)
	{
		case SDL_RELEASED:
			return Keyboard::UP;
		case SDL_PRESSED:
		default:
			return Keyboard::DOWN;
	}
}

void AppPlatform_sdl2::showKeyboard(int x, int y, int w, int h)
{
	if (SDL_IsTextInputActive())
	{
		hideKeyboard();
	}
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_SetTextInputRect(&rect);
	SDL_StartTextInput();
}

void AppPlatform_sdl2::hideKeyboard()
{
	if (SDL_IsTextInputActive())
	{
		SDL_StopTextInput();
	}
}

bool AppPlatform_sdl2::isTouchscreen() const
{
    return m_bIsTouchscreen;
}

bool AppPlatform_sdl2::hasGamepad() const
{
	return _controller != nullptr;
}

void AppPlatform_sdl2::gameControllerAdded(int32_t index)
{
	if (!getPrimaryGameController())
	{
		setPrimaryGameController(SDL_GameControllerOpen(index));
		Controller::reset();
	}
}

void AppPlatform_sdl2::gameControllerRemoved(int32_t index)
{
	SDL_GameController* controller = getPrimaryGameController();
	SDL_JoystickID joystickId = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
	// Check if current controller has been removed
	if (controller && index == joystickId)
	{
		SDL_GameControllerClose(controller);
		// Hunt for a new primary controller
		setPrimaryGameController(findGameController());
	}
}

void AppPlatform_sdl2::handleKeyEvent(int key, uint8_t state)
{
	// This really should be handled somewhere else.
	// Unforunately, there is no global keyboard handler.
	// Keyboard events are either handled in Screen::keyboardEvent
	// when a Screen is visible, or in Minecraft::tickInput
	// when LocalPlayer exists.
	switch (key)
	{
	case SDLVK_F2:
		if (state == SDL_PRESSED)
			saveScreenshot("", -1, -1);
		return;
	case SDLVK_AC_BACK:
		// Android Back Button (This is currently handled in handle_events() in platforms/sdl/main.cpp)
		// @TODO: handleBack function in AppPlatform that calls back to App via function pointer
		//g_pApp->handleBack(event.key.state == SDL_PRESSED);
		return;
	case SDLVK_BACKSPACE:
		// Text Editing (This is currently handled in handle_events() in platforms/sdl/main.cpp)
		/*if (state == SDL_PRESSED)
			g_pApp->handleCharInput('\b');*/
		break;
	case SDLVK_LSHIFT:
	case SDLVK_RSHIFT:
		setShiftPressed(state == SDL_PRESSED, key == SDLVK_LSHIFT);
		break;
	}

	// Normal Key Press
	Keyboard::feed(AppPlatform_sdl2::GetKeyState(state), key);
}

void AppPlatform_sdl2::handleButtonEvent(SDL_JoystickID controllerIndex, uint8_t button, uint8_t state)
{
	// Normal Key Press
	Keyboard::feed(AppPlatform_sdl2::GetKeyState(state), button);
}

void AppPlatform_sdl2::handleControllerAxisEvent(SDL_JoystickID controllerIndex, uint8_t axis, int16_t value)
{
	float val = value / 32767.0f; // -32768 to 32767

	switch (axis)
	{
	case SDL_CONTROLLER_AXIS_LEFTX:
		Controller::feedStickX(1, true, val);
		break;
	case SDL_CONTROLLER_AXIS_LEFTY:
		Controller::feedStickY(1, true, val);
		break;
	case SDL_CONTROLLER_AXIS_RIGHTX:
		Controller::feedStickX(2, true, val);
		break;
	case SDL_CONTROLLER_AXIS_RIGHTY:
		Controller::feedStickY(2, true, val);
		break;
	case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
		Controller::feedTrigger(1, val);
		break;
	case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
		Controller::feedTrigger(2, val);
		break;
	}
}

AssetFile AppPlatform_sdl2::readAssetFile(const std::string& str, bool quiet) const
{
	std::string path = getAssetPath(str);
	SDL_RWops *io = SDL_RWFromFile(path.c_str(), "rb");
	// Open File
	if (!io)
	{
		if (!quiet) LOG_W("Couldn't find asset file: %s", path.c_str());
		return AssetFile();
	}
	// Get File Size
	int64_t size = SDL_RWsize(io);
	if (size < 0)
	{
		if (!quiet) LOG_E("Error determining the size of the asset file!");
		SDL_RWclose(io);
		return AssetFile();
	}
	// Read Data
	unsigned char *buf = new unsigned char[size];
	SDL_RWread(io, buf, size, 1);
	// Close File
	SDL_RWclose(io);
	// Return
	return AssetFile(size, buf);
}