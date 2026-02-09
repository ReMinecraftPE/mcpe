#include "FoodItem.hpp"
#include "world/entity/Player.hpp"

FoodItem::FoodItem(int id, int nutrition) : Item(id), m_nutrition(nutrition)
{
	m_maxStackSize = 1;
}

ItemStack* FoodItem::use(ItemStack* inst, Level* level, Mob* mob) const
{
    if (mob->m_health < 20)
    {
        --inst->m_count;
        mob->heal(m_nutrition);
    }
    return inst;
}