#pragma once

#ifdef _XBOX
#include <xtl.h>
#else
#include <Xinput.h>
#endif

#include "client/player/input/GameControllerHandler.hpp"

#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT C_MAX_GAME_CONTROLLERS
#endif

class GameControllerHandler_xinput : public GameControllerHandler
{
public:
	class InputState_xinput
	{
	public:
		XINPUT_STATE m_inputState[XUSER_MAX_COUNT];

	public:
		InputState_xinput()
		{
			memset(m_inputState, 0, XUSER_MAX_COUNT);
		}
	};


public:
	GameControllerHandler_xinput();

protected:
	void _initButtonMap();
	void _processButton(GameController::ID controllerId, const XINPUT_STATE& state, GameController::NativeButtonID nativeBtn, GameController::EngineButtonID engineBtn, bool& joinGameAlreadyFired);
	void _processMotion(GameController::ID controllerId, const XINPUT_STATE& state);

public:
	void refresh() override;
	float normalizeAxis(float raw, float deadzone) const override;
	void normalizeAxes(Vec2& io, float deadzone) const override;

protected:
	InputState_xinput m_inputStates;
};