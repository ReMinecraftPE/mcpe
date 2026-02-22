#pragma once
#include <stdint.h>
#include "Container.hpp"

class ContainerContentChangeListener
{
public:
	virtual ~ContainerContentChangeListener() {}

public:
	virtual void containerContentChanged(Container* container, SlotID slot) = 0;
};
