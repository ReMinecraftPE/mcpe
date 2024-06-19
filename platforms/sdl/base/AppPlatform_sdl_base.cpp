#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>

#include "AppPlatform_sdl_base.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include "thirdparty/GL/GL.hpp"
#endif

#include "common/Utils.hpp"

#include "SoundSystemAL.hpp"

#include "client/player/input/Controller.hpp"

void AppPlatform_sdl_base::_init(std::string storageDir, SDL_Window *window)
{
	_storageDir = storageDir;
	_window = window;

	_iconTexture = nullptr;
	_icon = nullptr;

	m_bShiftPressed[0] = false;
	m_bShiftPressed[1] = false;

	_mousegrabbed = false;
	m_bWasUnfocused = false;

	ensureDirectoryExists(_storageDir.c_str());

	m_pLogger = new Logger;
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
}

void AppPlatform_sdl_base::initSoundSystem()
{
	if (!m_pSoundSystem)
	{
		LOG_I("Initializing OpenAL SoundSystem...");
		m_pSoundSystem = new SoundSystemAL();
	}
	else
	{
		LOG_E("Trying to initialize SoundSystem more than once!");
	}
}

std::string AppPlatform_sdl_base::getDateString(int time)
{
	time_t tt = time;
	struct tm t;
#ifdef _WIN32
	gmtime_s(&t, &tt);
#else
	gmtime_r(&tt, &t);
#endif

	// Format String
	char buf[2048];
	strftime(buf, sizeof buf, "%b %d %Y %H:%M:%S", &t);

	// Return
	return std::string(buf);
}

void AppPlatform_sdl_base::setIcon(const Texture& icon)
{
	if (!icon.m_pixels)
		return;

	SAFE_DELETE(_iconTexture);
	if (_icon) SDL_FreeSurface(_icon);

	_iconTexture = new Texture(icon);
	_icon = getSurfaceForTexture(_iconTexture);

	if (_icon)
		SDL_SetWindowIcon(_window, _icon);
}

AppPlatform_sdl_base::~AppPlatform_sdl_base()
{
	if (_icon) SDL_FreeSurface(_icon);
	SAFE_DELETE(_iconTexture);

	SAFE_DELETE(m_pSoundSystem);

	// DELETE THIS LAST
	SAFE_DELETE(m_pLogger);
}

SDL_GameController* AppPlatform_sdl_base::findGameController()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			return SDL_GameControllerOpen(i);
		}
	}

	return nullptr;
}

SDL_Surface* AppPlatform_sdl_base::getSurfaceForTexture(const Texture* const texture)
{
	if (!texture) return nullptr;

	void * const pixels = texture->m_pixels;
	const int width = texture->m_width;
	const int height = texture->m_height;
	const int depth = 32; // Color depth (32-bit by default)
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

int AppPlatform_sdl_base::checkLicense()
{
	// we own the game!!
	return 1;
}

const char* const AppPlatform_sdl_base::getWindowTitle() const
{
	return SDL_GetWindowTitle(_window);
}

int AppPlatform_sdl_base::getScreenWidth() const
{
	int width;
	SDL_GL_GetDrawableSize(_window, &width, nullptr);
	return width;
}

int AppPlatform_sdl_base::getScreenHeight() const
{
	int height;
	SDL_GL_GetDrawableSize(_window, nullptr, &height);
	return height;
}

void AppPlatform_sdl_base::recenterMouse()
{
	// only recenter the mouse if it's grabbed
	if (!_mousegrabbed)
		return;

	// If we aren't the foreground window, return
	if (!(SDL_GetWindowFlags(_window) & SDL_WINDOW_INPUT_FOCUS))
	{
		m_bWasUnfocused = true;
		return;
	}

	// If we were unfocused last frame, ignore the diff data we have.
	if (!m_bWasUnfocused)
	{
		// Note. The only reason we do it this way instead of
		// using the Mouse class is because, after SDL_WarpMouseInWindow,
		// we'll get an event on our window telling us "hey, the
		// user has moved their cursor back to the center! Move
		// the camera back as well", causing a camera that just
		// refuses to move
		int w = 0, h = 0;
		SDL_GetWindowSize(_window, &w, &h);
		SDL_WarpMouseInWindow(_window, w / 2, h / 2);
		//Mouse::feed(BUTTON_NONE, false, w / 2, h / 2);
	}

	m_bWasUnfocused = false;
}

void AppPlatform_sdl_base::setMouseGrabbed(bool b)
{
	_mousegrabbed = b;
	SDL_SetWindowGrab(_window, b ? SDL_TRUE : SDL_FALSE);
	/**
	 * @NOTE: There is a bug with older versions of SDL2 (ex: 2.0.4) where disabling RelativeMouseMode will cause a mouse event to be fired
	 * that just moves the cursor to where it would've been if the mode had never been enabled in the first place, effectively uncentering it.
	 * https://github.com/libsdl-org/SDL/issues/6002 (I'm not sure if this is the right issue, I just updated SDL after seeing this and it fixed the above problem.)
	 **/
	SDL_SetRelativeMouseMode(b ? SDL_TRUE : SDL_FALSE);
}

void AppPlatform_sdl_base::setMouseDiff(int x, int y)
{
	xrel = x;
	yrel = y;
}

void AppPlatform_sdl_base::getMouseDiff(int& x, int& y)
{
	x = xrel;
	y = yrel;
}

void AppPlatform_sdl_base::clearDiff()
{
	xrel = 0;
	yrel = 0;
}

bool AppPlatform_sdl_base::shiftPressed()
{
	return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_sdl_base::setShiftPressed(bool b, bool isLeft)
{
	m_bShiftPressed[isLeft ? 0 : 1] = b;
}

int AppPlatform_sdl_base::getUserInputStatus()
{
	return -1;
}

MouseButtonType AppPlatform_sdl_base::GetMouseButtonType(SDL_MouseButtonEvent event)
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

bool AppPlatform_sdl_base::GetMouseButtonState(SDL_Event event)
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

Keyboard::KeyState AppPlatform_sdl_base::GetKeyState(uint8_t state)
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

void AppPlatform_sdl_base::showKeyboard(int x, int y, int w, int h)
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

void AppPlatform_sdl_base::hideKeyboard()
{
	if (SDL_IsTextInputActive())
	{
		SDL_StopTextInput();
	}
}

bool AppPlatform_sdl_base::isTouchscreen() const
{
    return m_bIsTouchscreen;
}

bool AppPlatform_sdl_base::hasGamepad() const
{
	return _controller != nullptr;
}

void AppPlatform_sdl_base::gameControllerAdded(int32_t index)
{
	if (!getPrimaryGameController())
	{
		setPrimaryGameController(SDL_GameControllerOpen(index));
		Controller::reset();
	}
}

void AppPlatform_sdl_base::gameControllerRemoved(int32_t index)
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

void AppPlatform_sdl_base::handleKeyEvent(int key, uint8_t state)
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
		// Android Back Button
		// @TODO: handleBack function in AppPlatform that calls back to App via function pointer
		//g_pApp->handleBack(event.key.state == SDL_PRESSED);
		return;
	case SDLVK_BACKSPACE:
		// Text Editing
		/*if (state == SDL_PRESSED)
			g_pApp->handleCharInput('\b');*/
		break;
	case SDLVK_LSHIFT:
	case SDLVK_RSHIFT:
		setShiftPressed(state == SDL_PRESSED, key == SDLVK_LSHIFT);
		break;
	}

	// Normal Key Press
	Keyboard::feed(AppPlatform_sdl_base::GetKeyState(state), key);
}

void AppPlatform_sdl_base::handleButtonEvent(SDL_JoystickID controllerIndex, uint8_t button, uint8_t state)
{
	// Normal Key Press
	Keyboard::feed(AppPlatform_sdl_base::GetKeyState(state), button);
}

void AppPlatform_sdl_base::handleControllerAxisEvent(SDL_JoystickID controllerIndex, uint8_t axis, int16_t value)
{
	float val = value / 30000.0f; // -32768 to 32767

	switch (axis)
	{
	case SDL_CONTROLLER_AXIS_LEFTX:
		Controller::feedStickX(1, 1, val);
		break;
	case SDL_CONTROLLER_AXIS_LEFTY:
		Controller::feedStickY(1, 1, val);
		break;
	case SDL_CONTROLLER_AXIS_RIGHTX:
		Controller::feedStickX(2, 1, val);
		break;
	case SDL_CONTROLLER_AXIS_RIGHTY:
		Controller::feedStickY(2, 1, val);
		break;
	case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
		Controller::feedTrigger(1, val);
		break;
	case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
		Controller::feedTrigger(2, val);
		break;
	}
}