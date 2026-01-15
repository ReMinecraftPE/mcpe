#include "GameControllerHandler.hpp"

GameControllerHandler::GameControllerHandler()
{
	for (int i = 0; i < C_MAX_GAME_CONTROLLERS; i++)
	{
		m_connectionStates[i] = GameController::STATE_DISCONNECTED;
		m_nextCheckTime[i] = 0.0;
		m_leftTrigger[i] = 0.0f;
		m_rightTrigger[i] = 0.0f;
		m_leftStickTouched[i] = false;
		m_rightStickTouched[i] = false;
	}
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