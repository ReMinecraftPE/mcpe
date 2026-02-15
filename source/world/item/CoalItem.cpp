#include "CoalItem.hpp"

CoalItem::CoalItem(int id) : Item(id)
{
	m_maxDamage = 0;
	m_bStackedByData = true;
}

std::string CoalItem::getDescriptionId(ItemStack* inst) const
{
	return (inst->getAuxValue() == 1) ? "item.charcoal" : "item.coal";
}