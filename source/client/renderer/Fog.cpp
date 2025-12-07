#include "Fog.hpp"
#include "renderer/RenderContextImmediate.hpp"

mce::FogState Fog::_currentState;
mce::FogStateDescription Fog::nextState;

void Fog::_updateState(const mce::FogStateDescription desc)
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	_currentState.createFogState(renderContext, desc);
	_currentState.bindFogState(renderContext);
}

void Fog::_setEnabled(bool enabled)
{
	nextState.enableFog = enabled;
	updateState();
}

void Fog::updateState()
{
	_updateState(nextState);
}

void Fog::updateRange(float startZ, float endZ)
{
	nextState.fogStartZ = startZ;
	nextState.fogEndZ = endZ;
	updateState();
}

void Fog::enable()
{
	_setEnabled(true);
}

void Fog::disable()
{
	_setEnabled(false);
}