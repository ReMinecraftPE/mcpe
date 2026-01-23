#pragma once

#include "world/item/ItemInstance.hpp"

#define C_MAX_CONTAINER_STACK_SIZE (64)

class Container
{
public:

	virtual int getContainerSize() const = 0;

	virtual ItemInstance& getItem(int index) const = 0;

	virtual ItemInstance removeItem(int index, int count) = 0;

	virtual void setItem(int index, const ItemInstance& item) = 0;

	virtual std::string getName() const = 0;

	virtual int getMaxStackSize()
	{
		return C_MAX_CONTAINER_STACK_SIZE;
	}

	virtual void setChanged() = 0;

	virtual bool stillValid(Player* player) = 0;
};