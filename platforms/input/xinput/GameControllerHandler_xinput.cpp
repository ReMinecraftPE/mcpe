#ifdef _XBOX
#include <xtl.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "GameControllerHandler_xinput.hpp"
#include "client/player/input/Keyboard.hpp"
#include "client/player/input/GameControllerManager.hpp"

// @HACK: this is an XInput macro, remove this once we support more than one local player
#undef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 1

GameControllerHandler_xinput::GameControllerHandler_xinput()
	: GameControllerHandler()
{
    _initButtonMap();

    // need to have the connection states ready for AppPlatform->hasGamepad()
    refresh();
}

void GameControllerHandler_xinput::_initButtonMap()
{
    ButtonIDMap& m = m_buttonIdMap;

    m[XINPUT_GAMEPAD_A]              = GameController::BUTTON_A;
    m[XINPUT_GAMEPAD_B]              = GameController::BUTTON_B;
    m[XINPUT_GAMEPAD_X]              = GameController::BUTTON_X;
    m[XINPUT_GAMEPAD_Y]              = GameController::BUTTON_Y;
    m[XINPUT_GAMEPAD_DPAD_UP]        = GameController::BUTTON_DPAD_UP;
    m[XINPUT_GAMEPAD_DPAD_DOWN]      = GameController::BUTTON_DPAD_DOWN;
    m[XINPUT_GAMEPAD_DPAD_LEFT]      = GameController::BUTTON_DPAD_LEFT;
    m[XINPUT_GAMEPAD_DPAD_RIGHT]     = GameController::BUTTON_DPAD_RIGHT;
    m[XINPUT_GAMEPAD_LEFT_THUMB]     = GameController::BUTTON_LEFTSTICK;
    m[XINPUT_GAMEPAD_RIGHT_THUMB]    = GameController::BUTTON_RIGHTSTICK;
    m[XINPUT_GAMEPAD_LEFT_SHOULDER]  = GameController::BUTTON_LEFTSHOULDER;
    m[XINPUT_GAMEPAD_RIGHT_SHOULDER] = GameController::BUTTON_RIGHTSHOULDER;
    m[XINPUT_GAMEPAD_BACK]           = GameController::BUTTON_BACK;
    m[XINPUT_GAMEPAD_START]          = GameController::BUTTON_START;
}

Keyboard::KeyState _getKeyState(bool value)
{
    return value ? Keyboard::DOWN : Keyboard::UP;
}

void GameControllerHandler_xinput::_processButton(GameController::ID controllerId, const XINPUT_STATE& state, GameController::NativeButtonID nativeBtn, GameController::EngineButtonID engineBtn, bool& joinGameAlreadyFired)
{
    bool bButtonPressed = (state.Gamepad.wButtons & nativeBtn) != 0;
    GameController::ButtonState btnState = bButtonPressed ? GameController::BTN_STATE_DOWN : GameController::BTN_STATE_UP;
    GameController::ButtonState& lastBtnState = m_buttonStates[controllerId][nativeBtn];

    // don't do anything if the button state hasn't changed
    if (btnState == lastBtnState)
        return;

    if (bButtonPressed && engineBtn == GameController::BUTTON_START)
    {
        //GameController::addEvent(GameController::EVENT_JOIN_GAME);
        joinGameAlreadyFired = true;
    }

    // @TODO: should call GameControllerManager::feedButton() instead
    Keyboard::feed(_getKeyState(bButtonPressed), engineBtn);

    lastBtnState = btnState;
}

void GameControllerHandler_xinput::_processMotion(GameController::ID controllerId, const XINPUT_STATE& state)
{
    Vec2 vec(state.Gamepad.sThumbLX, -state.Gamepad.sThumbLY);
    normalizeAxes(vec, 0.0f);
    GameControllerManager::feedStick(1, true, vec);
    m_leftStickTouched[controllerId] = true;

    vec = Vec2(state.Gamepad.sThumbRX, -state.Gamepad.sThumbRY);
    normalizeAxes(vec, 0.0f);
    GameControllerManager::feedStick(2, true, vec);
    m_rightStickTouched[controllerId] = true;
}

void GameControllerHandler_xinput::refresh()
{
    // Ingest our input "queue"
    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
    {
        DWORD result = XInputGetState(i, &m_inputStates.m_inputState[i]);
        m_connectionStates[i] = result == ERROR_SUCCESS ? GameController::STATE_CONNECTED : GameController::STATE_DISCONNECTED;
    }

    for (GameController::ID id = 0; id < XUSER_MAX_COUNT; id++)
    {
        XINPUT_STATE& inputState = m_inputStates.m_inputState[id];
        bool joinGameAlreadyFired = false;
        for (ButtonIDMap::const_iterator it = m_buttonIdMap.begin(); it != m_buttonIdMap.end(); it++)
        {
            _processButton(id, inputState, it->first, it->second, joinGameAlreadyFired);
        }

        GameControllerManager::feedTrigger(1, (float)inputState.Gamepad.bLeftTrigger / 255.0f);
        GameControllerManager::feedTrigger(2, (float)inputState.Gamepad.bRightTrigger / 255.0f);

        _processMotion(id, inputState);
    }
}

float GameControllerHandler_xinput::normalizeAxis(float raw, float deadzone) const
{
    return Mth::Max(-1.0f, raw / 32767.0f); // -32768 to 32767

    // Deadzone is currently handled in GameControllerManager
    /*float v3 = Mth::Max(-1.0f, raw / 32767.0f);
    float v4 = fabs(v3);
    float v5;
    if (v4 >= deadzone)
        v5 = v3 * ((v4 - deadzone) / v4);
    else
        v5 = 0.0f;
    return v5 / (1.0f - deadzone);*/
}

void GameControllerHandler_xinput::normalizeAxes(Vec2& io, float deadzone) const
{
    io.x = Mth::Max(-1.0f, io.x / 32767.0f);
    io.y = Mth::Max(-1.0f, io.y / 32767.0f);

    // Deadzone is currently handled in GameControllerManager
    /*float length = io.length();
    if (length > deadzone)
    {
        float v10 = 1.0f / length;
        io.x -= ((v10 * io.x) * deadzone);
        io.y -= ((v10 * io.y) * deadzone);
        io *= (1.0f / (1.0f - deadzone))
    }
    else
    {
        io = Vec2::ZERO;
    }*/
}