#include "XInput.hpp"

DWORD (WINAPI *XInput::GetState)(DWORD, XINPUT_STATE *) = nullptr;
