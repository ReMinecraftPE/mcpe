#include "ViewportOrigin.hpp"

using namespace mce;

const ViewportOrigin ViewportOrigin::ZERO = ViewportOrigin();

ViewportOrigin::ViewportOrigin()
{
	leftX = 0;
	bottomLeftY = 0;
	topLeftY = 0;
}