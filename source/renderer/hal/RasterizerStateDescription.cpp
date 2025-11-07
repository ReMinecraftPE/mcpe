#include "RasterizerStateDescription.hpp"

using namespace mce;

RasterizerStateDescription::RasterizerStateDescription()
{
    depthBias = 0.0f;
    cullMode = CULL_BACK;
    enableScissorTest = 0;
}

bool RasterizerStateDescription::operator==(const RasterizerStateDescription& other) const
{
	return this->depthBias == other.depthBias &&
		   this->cullMode == other.cullMode;
}

bool RasterizerStateDescription::operator!=(const RasterizerStateDescription& other) const
{
	return !(*this == other);
}
