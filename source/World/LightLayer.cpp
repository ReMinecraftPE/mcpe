#include "LightLayer.hpp"

LightLayer LightLayer::Sky(15), LightLayer::Block(0);

LightLayer::LightLayer(int x)
{
	m_x = x;
}
