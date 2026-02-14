#pragma once

#include "world/Container.hpp"
#include "nbt/CompoundTag.hpp"
#include <vector>

class SimpleContainer : public Container
{
public:
	SimpleContainer(int size, const std::string& name);

	virtual uint16_t getContainerSize() const override;

	virtual ItemStack& getItem(int index) override;

	virtual ItemStack removeItem(int index, int count) override;

	virtual void setItem(int index, const ItemStack& item) override;

	virtual std::string getName() const override;

	virtual void setChanged() override;

	virtual bool stillValid(Player* player) const override;

	virtual void clear();

	virtual void load(const CompoundTag& tag);
	virtual void save(CompoundTag& tag);

private:
	std::vector<ItemStack> m_items;
	std::string m_name;
};


