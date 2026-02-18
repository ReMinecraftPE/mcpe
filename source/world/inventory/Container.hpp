#pragma once

#include <set>

#include "world/item/ItemStack.hpp"

#define C_MAX_CONTAINER_STACK_SIZE (64)

class ContainerContentChangeListener;
class ContainerSizeChangeListener;

class Container
{
protected:
	typedef std::set<ContainerContentChangeListener*> ContentChangeListeners;
	typedef std::set<ContainerSizeChangeListener*> SizeChangeListeners;

public:
	typedef uint16_t Size;

public:
	enum Type
	{
		CONTAINER,
		CRAFTING,
		FURNACE,
		DISPENSER
	};

public:
	virtual Size getContainerSize() const = 0;
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
	// Was called setChanged in Java
	virtual void setContainerChanged(SlotID slot) = 0;
	virtual bool stillValid(Player* player) const = 0;

	virtual void addContentChangeListener(ContainerContentChangeListener* listener) {}
	virtual void addSizeChangeListener(ContainerSizeChangeListener* listener) {}
	virtual void removeContentChangeListener(ContainerContentChangeListener* listener) {}
	virtual void removeSizeChangeListener(ContainerSizeChangeListener* listener) {}
};