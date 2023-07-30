#include "MouseTurnInput.hpp"
#include "Mouse.hpp"

constexpr float C_SENSITIVITY = 0.9f;

MouseTurnInput::MouseTurnInput(Minecraft* pMC)
{
	m_pMinecraft = pMC;
}

ITurnInput::~ITurnInput()
{
}

ITurnInput::Delta MouseTurnInput::getTurnDelta()
{
	int deltaX = 0, deltaY = 0;
	m_pMinecraft->platform()->getMouseDiff(deltaX, deltaY);
	m_pMinecraft->platform()->clearDiff();

	Delta d;
	d.x = C_SENSITIVITY * deltaX;
	d.y = C_SENSITIVITY * deltaY;

	return d;
}
