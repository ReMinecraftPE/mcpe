#pragma once

#include "world/item/ItemStack.hpp"

#define C_MAX_CONTAINER_STACK_SIZE (64)

class Container
{
public:

	virtual uint16_t getContainerSize() const = 0;

	virtual ItemStack& getItem(int index) = 0;

	virtual ItemStack* tryGetItem(int index)
	{
		if (index >= 0 && index < getContainerSize())
			return &getItem(index);
		else
			return nullptr;
	}

	virtual ItemStack removeItem(int index, int count) = 0;

	virtual void setItem(int index, const ItemStack& item) = 0;

	virtual std::string getName() const = 0;

	virtual int getMaxStackSize()
	{
		return C_MAX_CONTAINER_STACK_SIZE;
	}

	virtual void setChanged() = 0;

	virtual bool stillValid(Player* player) const = 0;
};