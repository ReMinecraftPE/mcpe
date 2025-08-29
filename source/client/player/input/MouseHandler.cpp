#include "MouseHandler.hpp"

#include "compat/LegacyCPP.hpp"

MouseHandler::MouseHandler() :
	m_pTurnInput(nullptr)
{
}

void MouseHandler::setTurnInput(ITurnInput* pTurnInput)
{
	m_pTurnInput = pTurnInput;
}

void MouseHandler::grab()
{
	m_delta = TurnDelta();
}

void MouseHandler::poll()
{
	if (m_pTurnInput)
		m_delta = m_pTurnInput->getTurnDelta();
}

void MouseHandler::release()
{
	//platform()->recenterMouse();
}

bool MouseHandler::smoothTurning()
{
	if (!m_pTurnInput)
		return false;

	return m_pTurnInput->smoothTurning();
}
