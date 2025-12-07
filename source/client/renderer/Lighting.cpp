#include "Lighting.hpp"
#include "renderer/RenderContextImmediate.hpp"

void Lighting::turnOff(bool teardown)
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.disableFixedLighting(teardown);
}

void Lighting::turnOn(bool init)
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.enableFixedLighting(init);
	renderContext.setShadeMode(mce::SHADE_MODE_FLAT);
}

void Lighting::turnOn(const Matrix& matrix)
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.loadMatrix(MATRIX_VIEW, matrix);
	turnOn();
}
