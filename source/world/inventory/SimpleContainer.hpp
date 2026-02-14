#pragma once

#include "world/Container.hpp"
#include "nbt/CompoundTag.hpp"
#include <vector>

class SimpleContainer : public Container
{
public:
	SimpleContainer(int size, const std::string& name);

public:
	uint16_t getContainerSize() const override;
	ItemStack& getItem(int index) override;
	ItemStack removeItem(int index, int count) override;
	void setItem(int index, const ItemStack& item) override;
	std::string getName() const override;
	void setChanged() override;
	bool stillValid(Player* player) const override;

public:
	virtual void clear();
	virtual void load(const CompoundTag& tag);
	virtual void save(CompoundTag& tag) const;

private:
	std::vector<ItemStack> m_items;
	std::string m_name;
};