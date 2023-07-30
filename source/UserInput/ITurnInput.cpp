#include "ITurnInput.hpp"
#include "Utils.hpp"

float ITurnInput::getDeltaTime()
{
	if (m_prevTime == -1.0f)
		m_prevTime = getTimeS();

	float newTime = getTimeS();
	float delta = newTime - m_prevTime;
	m_prevTime = newTime;

	return delta;
}
