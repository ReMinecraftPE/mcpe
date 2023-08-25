/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <fstream>
#include <sstream>
#include <shlobj.h>

#include "AppPlatform_windows.hpp"

#include "thirdparty/stb_image.h"
#include "thirdparty/stb_image_write.h"

AppPlatform_windows::AppPlatform_windows()
{
	m_WindowTitle = "ReMinecraftPE";
	// just assume an 854x480 window for now:
	m_ScreenWidth = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;
	m_UserInputStatus = -1;

	m_bIsFocused = false;
	m_bGrabbedMouse = false;
	m_bActuallyGrabbedMouse = false;
	m_bWasUnfocused = false;
	m_bShiftPressed = false;

	m_MouseDiffX = 0, m_MouseDiffY = 0;
}

int AppPlatform_windows::checkLicense()
{
	// we own the game!!
	return 1;
}

void AppPlatform_windows::buyGame()
{
	MessageBox(GetHWND(), TEXT("Buying the game!"), getWindowTitle(), MB_OK | MB_ICONINFORMATION);
}

void AppPlatform_windows::saveScreenshot(const std::string& fileName, int width, int height)
{
	int npixels = width * height;
	uint32_t* pixels = new uint32_t[npixels];
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_flip_vertically_on_write(true);

	// Verify if the folder exists for saving screenshots and
	// create it if it doesn't 
	// Kinda inefficient but I didn't want to be too intrusive 
	// -Vruk
	// https://stackoverflow.com/a/22182041

	// https://stackoverflow.com/a/8901001
	CHAR mypicturespath[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, mypicturespath);

	static char str[MAX_PATH];

	if (result == S_OK)
		sprintf(str, "%s\\%s", mypicturespath, "MCPE");
	else
		sprintf(str, "%s\\%s", ".", "Screenshots");

	// https://stackoverflow.com/a/8233867
	DWORD ftyp = GetFileAttributesA(str);

	DWORD error = GetLastError();
	if (error == ERROR_PATH_NOT_FOUND || error == ERROR_FILE_NOT_FOUND || error == ERROR_INVALID_NAME)
	{
		// https://stackoverflow.com/a/22182041
		CreateDirectory(str, NULL);
	}
	
	char fullpath[MAX_PATH];
	sprintf(fullpath, "%s\\%s", str, fileName.c_str());

	stbi_write_png(fullpath, width, height, 4, pixels, width * 4);

	delete[] pixels;
}

void AppPlatform_windows::createUserInput()
{
	m_UserInput.clear();
	m_UserInputStatus = -1;

	switch (m_DialogType)
	{
		case DLG_CREATE_WORLD:
		{
			// some placeholder for now
			m_UserInput.push_back("New World");
			m_UserInput.push_back("123456");
			m_UserInputStatus = 1;
			break;
		}
	}
}

void AppPlatform_windows::showDialog(eDialogType type)
{
	m_DialogType = type;
}

std::string AppPlatform_windows::getDateString(int time)
{
	time_t tt = time;
	struct tm t;
	// using the _s variant. For a different platform there's gmtime_r. This is not directly portable however.
	gmtime_s(&t, &tt);

	//format it with strftime
	char buf[2048];
	strftime(buf, sizeof buf, "%b %d %Y %H:%M:%S", &t);
	//strftime(buf, sizeof buf, "%a %b %d %H:%M:%S %Z %Y", &t);

	return std::string(buf);
}

Texture AppPlatform_windows::loadTexture(const std::string& str, bool b)
{
	std::string realPath = str;
	if (realPath.size() && realPath[0] == '/')
		// trim it off
		realPath = realPath.substr(1);

	realPath = "assets/" + realPath;

	FILE* f = fopen(realPath.c_str(), "rb");
	if (!f)
	{
		LogMsg("File %s couldn't be opened", realPath.c_str());

	_error:
		const std::string msg = "Error loading " + realPath + ". Did you unzip the Minecraft assets?";
		MessageBoxA(GetHWND(), msg.c_str(), getWindowTitle(), MB_OK);

		if (f)
			fclose(f);
		::exit(1);
	}

	int width = 0, height = 0, channels = 0;

	stbi_uc* img = stbi_load_from_file(f, &width, &height, &channels, STBI_rgb_alpha);
	if (!img)
	{
		LogMsg("File %s couldn't be loaded via stb_image", realPath.c_str());
		goto _error;
	}

	uint32_t* img2 = new uint32_t[width * height];
	memcpy(img2, img, width * height * sizeof(uint32_t));
	stbi_image_free(img);
	img = nullptr;

	fclose(f);
	return Texture(width, height, img2, 1, 0);
}

std::vector<std::string> AppPlatform_windows::getOptionStrings()
{
	std::vector<std::string> o;

	std::ifstream ifs("options.txt");
	if (!ifs.is_open())
		return o;

	std::string str;
	while (true)
	{
		if (!std::getline(ifs, str, '\n'))
			break;

		if (str.empty() || str[0] == '#')
			continue;

		std::stringstream ss;
		ss << str;

		std::string key, value;
		if (std::getline(ss, key, ':') && std::getline(ss, value))
		{
			o.push_back(key);
			o.push_back(value);
		}
	}

	return o;
}

std::string AppPlatform_windows::convertKeycodeToString(int keyCode)
{
	if (keyCode >= 'A' && keyCode <= 'Z')
		return std::string(1, char(keyCode));

	if (keyCode >= '0' && keyCode <= '9')
		return std::string(1, char(keyCode));

	if (keyCode >= VK_F1 && keyCode <= VK_F24)
	{
		std::stringstream ss;
		ss << "F" << (keyCode - VK_F1 + 1);
		return ss.str();
	}
	if (keyCode >= VK_NUMPAD0 && keyCode <= VK_NUMPAD9)
	{
		std::stringstream ss;
		ss << "NUMPAD " << (keyCode - VK_NUMPAD0);
		return ss.str();
	}

	switch (keyCode)
	{
		case VK_BACK:     return "BACKSPACE";
		case VK_TAB:      return "TAB";
		case VK_RETURN:   return "ENTER";
		case VK_CONTROL:  return "CTRL";
		case VK_MENU:     return "ALT";
		case VK_PAUSE:    return "PAUSE";
		case VK_CAPITAL:  return "CAPSLOCK";
		case VK_ESCAPE:   return "ESC";
		case VK_SPACE:    return "SPACE";
		case VK_PRIOR:    return "PAGE UP";
		case VK_NEXT:     return "PAGE DOWN";
		case VK_HOME:     return "HOME";
		case VK_END:      return "END";
		case VK_LEFT:     return "LEFT";
		case VK_RIGHT:    return "RIGHT";
		case VK_UP:       return "UP";
		case VK_DOWN:     return "DOWN";
		case VK_SNAPSHOT: return "SYSRQ";
		case VK_INSERT:   return "INSERT";
		case VK_DELETE:   return "DELETE";
		case VK_LWIN:     return "LEFT WIN";
		case VK_RWIN:     return "RIGHT WIN";
		case VK_MULTIPLY: return "MULTIPLY";
		case VK_SUBTRACT: return "SUBTRACT";
		case VK_ADD:      return "ADD";
		case VK_SEPARATOR:return "SEPARATOR";
		case VK_DECIMAL:  return "DECIMAL";
		case VK_DIVIDE:   return "DIVIDE";
		case VK_NUMLOCK:  return "NUMLOCK";
		case VK_SCROLL:   return "SCROLLLOCK";
		case VK_OEM_1:    return "SEMICOLON";
		case VK_OEM_2:    return "SLASH";
		case VK_OEM_3:    return "GRAVE";
		case VK_OEM_4:    return "L BRACKET";
		case VK_OEM_5:    return "BACKSLASH";
		case VK_OEM_6:    return "R BRACKET";
		case VK_OEM_7:    return "APOSTROPHE";
		case VK_SHIFT:    return "SHIFT";
	}

	std::stringstream ss;
	ss << "UNK_" << keyCode;
	return ss.str();

	switch (keyCode)
	{
		case ' ':
			return "Space";
		case 16:
			return "Left Shift";
		case 27:
			return "ESC";
		case 112:
			return "F1";
		case 113:
			return "F2";
		case 114:
			return "F3";
		case 115:
			return "F4";
		case 116:
			return "F5";
		case 117:
			return "F6";
		case 118:
			return "F7";
		case 119:
			return "F8";
		case 120:
			return "F9";
		case 121:
			return "F10";
		case 122:
			return "F11";
		case 123:
			return "F12";
		case 191:
			return "/";
	}

	return std::string(1, keyCode);
}

void AppPlatform_windows::setOptionStrings(const std::vector<std::string>& str)
{
	assert(str.size() % 2 == 0);

	std::ofstream os("options.txt");

	os << "#Config file for Minecraft PE.  The # at the start denotes a comment, removing it makes it a command.\n\n";
	
	for (int i = 0; i < int(str.size()); i += 2)
		os << str[i] << ':' << str[i + 1] << '\n';
}

std::string AppPlatform_windows::getPatchData()
{
	std::ifstream ifs("assets/patches/patch_data.txt");
	if (!ifs.is_open())
		return "";

	std::stringstream ss;
	ss << ifs.rdbuf();

	return ss.str();
}

void AppPlatform_windows::SetDefaultBindings(KeyMapping keymap[])
{
	keymap[KM_FORWARD] =		KeyMapping("key.forward", 'W');
	keymap[KM_LEFT] =			KeyMapping("key.left", 'A');
	keymap[KM_BACKWARD] =		KeyMapping("key.back", 'S');
	keymap[KM_RIGHT] =			KeyMapping("key.right", 'D');
	keymap[KM_JUMP] =			KeyMapping("key.jump", ' ');
	keymap[KM_INVENTORY] =		KeyMapping("key.inventory", 'E');
	keymap[KM_DROP] =			KeyMapping("key.drop", 'Q');
	keymap[KM_CHAT] =			KeyMapping("key.chat", 'T');
	keymap[KM_FOG] =			KeyMapping("key.fog", 'F');
	keymap[KM_SNEAK] =			KeyMapping("key.sneak", VK_SHIFT);
	keymap[KM_DESTROY] =		KeyMapping("key.destroy", 'K');
	keymap[KM_PLACE] =			KeyMapping("key.place", 'L');
	keymap[KM_MENU_NEXT] =		KeyMapping("key.menu.next", VK_DOWN);
	keymap[KM_MENU_PREVIOUS] =	KeyMapping("key.menu.previous", VK_UP);
	keymap[KM_MENU_OK] =		KeyMapping("key.menu.ok", VK_RETURN);
	keymap[KM_MENU_CANCEL] =	KeyMapping("key.menu.cancel", VK_ESCAPE);
	keymap[KM_SLOT_1] =			KeyMapping("key.slot.1", '1');
	keymap[KM_SLOT_2] =			KeyMapping("key.slot.2", '2');
	keymap[KM_SLOT_3] =			KeyMapping("key.slot.3", '3');
	keymap[KM_SLOT_4] =			KeyMapping("key.slot.4", '4');
	keymap[KM_SLOT_5] =			KeyMapping("key.slot.5", '5');
	keymap[KM_SLOT_6] =			KeyMapping("key.slot.6", '6');
	keymap[KM_SLOT_7] =			KeyMapping("key.slot.7", '7');
	keymap[KM_SLOT_8] =			KeyMapping("key.slot.8", '8');
	keymap[KM_SLOT_9] =			KeyMapping("key.slot.9", '9');
	keymap[KM_SLOT_L] =			KeyMapping("key.slot.left", 'Y');
	keymap[KM_SLOT_R] =			KeyMapping("key.slot.right", 'U');
	keymap[KM_TOGGLEGUI] =		KeyMapping("key.fn.gui", VK_F1);
	keymap[KM_SCREENSHOT] =		KeyMapping("key.fn.screenshot", VK_F2);
	keymap[KM_TOGGLEDEBUG] =	KeyMapping("key.fn.debug", VK_F3);
	keymap[KM_TOGGLEAO] =		KeyMapping("key.fn.ao", VK_F4);
	keymap[KM_TOGGLE3RD] =		KeyMapping("key.fn.3rd", VK_F5);
	keymap[KM_FLY_UP] =			KeyMapping("key.fly.up", 'X');
	keymap[KM_FLY_DOWN] =		KeyMapping("key.fly.down", 'C');
	keymap[KM_CHAT_CMD] =		KeyMapping("key.chat.cmd", VK_OEM_2);
}

void AppPlatform_windows::setScreenSize(int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;
}

void AppPlatform_windows::recenterMouse()
{
	// only recenter the mouse if it's grabbed
	if (!m_bGrabbedMouse)
		return;

	// If we aren't the foreground window, return
	if (GetForegroundWindow() != GetHWND())
	{
		m_bWasUnfocused = true;
		return;
	}

	POINT oldPos = { 0, 0 };
	GetCursorPos(&oldPos);

	RECT rect;
	GetClientRect(GetHWND(), &rect);

	POINT offs = { m_ScreenWidth / 2, m_ScreenHeight / 2 };
	ClientToScreen(GetHWND(), &offs);

	SetCursorPos(offs.x, offs.y);

	// Note. The only reason we do it this way instead of
	// using the Mouse class is because, after SetCursorPos,
	// we'll get an event on our window telling us "hey, the
	// user has moved their cursor back to the center! Move
	// the camera back as well", causing a camera that just
	// refuses to move

	// If we were unfocused last frame, ignore the diff data we have.
	if (!m_bWasUnfocused)
	{
		m_MouseDiffX -= offs.x - oldPos.x;
		m_MouseDiffY -= offs.y - oldPos.y;
	}

	m_bWasUnfocused = false;
}

void AppPlatform_windows::setMouseGrabbed(bool b)
{
	m_bGrabbedMouse = b;

	if (m_bActuallyGrabbedMouse == (b && m_bIsFocused))
		return;

	if (!b || !m_bIsFocused)
	{
		m_bActuallyGrabbedMouse = false;

		//show the cursor
		ShowCursor(TRUE);

		//unconfine it
		ClipCursor(NULL);

		clearDiff();
	}
	else
	{
		m_bActuallyGrabbedMouse = true;

		//hide the cursor
		ShowCursor(FALSE);

		//confine it to our client area
		RECT rect;
		GetClientRect(GetHWND(), &rect);

		POINT offs = { 0, 0 };
		ClientToScreen(GetHWND(), &offs);
		rect.left   += offs.x;
		rect.top    += offs.y;
		rect.right  += offs.x;
		rect.bottom += offs.y;

		ClipCursor(&rect);

		// set the cursor pos to the middle of the screen
		recenterMouse();

		clearDiff();
	}
}

void AppPlatform_windows::getMouseDiff(int& x, int& y)
{
	x = m_MouseDiffX;
	y = m_MouseDiffY;
}

void AppPlatform_windows::clearDiff()
{
	m_MouseDiffX = m_MouseDiffY = 0;
}

void AppPlatform_windows::updateFocused(bool focused)
{
	m_bIsFocused = focused;
	setMouseGrabbed(m_bGrabbedMouse);
}

Mouse::ButtonType AppPlatform_windows::GetMouseButtonType(UINT iMsg)
{
	switch (iMsg)
	{
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
		return Mouse::ButtonType::LEFT;
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
		return Mouse::ButtonType::RIGHT;
	case WM_MBUTTONUP:
	case WM_MBUTTONDOWN:
		return Mouse::ButtonType::MIDDLE;
	case WM_MOUSEWHEEL:
		return Mouse::ButtonType::SCROLLWHEEL;
	default:
		return Mouse::ButtonType::NONE;
	}
}

Mouse::ButtonState AppPlatform_windows::GetMouseButtonState(UINT iMsg, WPARAM wParam)
{
	Mouse::ButtonState result;

	switch (iMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		result = Mouse::ButtonState::DOWN;
		break;
	case WM_MOUSEWHEEL:
	{
		short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (wheelDelta > 0)
		{
			// "A positive value indicates that the wheel was rotated forward, away from the user."
			result = Mouse::ButtonState::UP;
		}
		else
		{
			// "A negative value indicates that the wheel was rotated backward, toward the user."
			result = Mouse::ButtonState::DOWN;
		}
		break;
	}
	default:
		result = Mouse::ButtonState::UP;
		break;
	}

	return result;
}

Keyboard::KeyState AppPlatform_windows::GetKeyState(UINT iMsg)
{
	switch (iMsg)
	{
	case WM_KEYUP:
		return Keyboard::KeyState::UP;
	case WM_KEYDOWN:
	default:
		return Keyboard::KeyState::DOWN;
	}
}
