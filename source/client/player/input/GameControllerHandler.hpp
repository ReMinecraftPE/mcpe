#pragma once

#include <map>

#include "client/app/AppPlatform.hpp"
#include "world/phys/Vec2.hpp"
#include "GameController.hpp"

#define C_MAX_GAME_CONTROLLERS C_MAX_LOCAL_PLAYERS

class GameControllerHandler
{
public:
	typedef std::map<GameController::NativeButtonID, GameController::EngineButtonID> ButtonIDMap;
	typedef std::map<GameController::NativeButtonID, GameController::ButtonState> ButtonStateLookup;

public:
	GameControllerHandler();

public:
	virtual void refresh() = 0;
	virtual float normalizeAxis(float raw, float deadzone) const = 0;
	virtual void normalizeAxes(Vec2& io, float deadzone) const = 0;

	bool hasController() const;

protected:
	ButtonIDMap m_buttonIdMap;
	ButtonStateLookup m_buttonStates[C_MAX_GAME_CONTROLLERS];
	GameController::ConnectionState m_connectionStates[C_MAX_GAME_CONTROLLERS];
	double m_nextCheckTime[C_MAX_GAME_CONTROLLERS];
	float m_leftTrigger[C_MAX_GAME_CONTROLLERS];
	float m_rightTrigger[C_MAX_GAME_CONTROLLERS];
	bool m_leftStickTouched[C_MAX_GAME_CONTROLLERS];
	bool m_rightStickTouched[C_MAX_GAME_CONTROLLERS];
};
