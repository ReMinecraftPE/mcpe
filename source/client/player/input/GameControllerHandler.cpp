#include <cstring>
#include "GameControllerHandler.hpp"

GameControllerHandler::GameControllerHandler()
{
	memset(m_connectionStates,  GameController::STATE_DISCONNECTED, C_MAX_GAME_CONTROLLERS);
	memset(m_nextCheckTime,     0, C_MAX_GAME_CONTROLLERS);
	memset(m_leftTrigger,       0, C_MAX_GAME_CONTROLLERS);
	memset(m_rightTrigger,      0, C_MAX_GAME_CONTROLLERS);
	memset(m_leftStickTouched,  0, C_MAX_GAME_CONTROLLERS);
	memset(m_rightStickTouched, 0, C_MAX_GAME_CONTROLLERS);
}

// @TODO: remove this when we can
// @NOTE: please don't use this in new code, we only have it for specific AppPlatforms that need it
bool GameControllerHandler::hasController() const
{
	return m_connectionStates[0] == GameController::STATE_CONNECTED;

	/*for (unsigned int i = 0; i < C_MAX_GAME_CONTROLLERS; i++)
	{
		if (m_connectionStates[i] == GameController::STATE_CONNECTED)
			return true;
	}

	return false;*/
}