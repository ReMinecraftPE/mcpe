#pragma once
#include <stdint.h>
#include "Container.hpp"

class ContainerSizeChangeListener
{
public:
	virtual ~ContainerSizeChangeListener() {}

public:
	virtual void containerSizeChanged(Container::Size size) = 0;
};
