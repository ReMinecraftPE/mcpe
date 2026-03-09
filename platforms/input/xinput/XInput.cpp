#include "XInput.hpp"

DWORD (WINAPI *XInput::XInputGetState)(DWORD, XINPUT_STATE *) = nullptr;
