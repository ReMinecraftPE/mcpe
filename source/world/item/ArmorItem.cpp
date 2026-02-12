#include "ArmorItem.hpp"

ArmorItem::Tier
                                // level    // main tex             // legs tex
	ArmorItem::Tier::CLOTH      (0,         "armor/cloth_1.png",    "armor/cloth_2.png"),
	ArmorItem::Tier::CHAIN      (1,         "armor/chain_1.png",    "armor/chain_2.png"),
	ArmorItem::Tier::IRON       (2,         "armor/iron_1.png",     "armor/iron_2.png"),
	ArmorItem::Tier::EMERALD    (3,         "armor/diamond_1.png",  "armor/diamond_2.png"),
	ArmorItem::Tier::GOLD       (1,         "armor/gold_1.png",     "armor/gold_2.png");

const int ArmorItem::defensePerSlot[4] = { 3, 8, 6, 3 };
const int ArmorItem::healthPerSlot[4]  = { 11, 16, 15, 13 };

ArmorItem::Tier::Tier(int level, std::string mainTexture, std::string legsTexture) :
    m_level(level), m_mainTexture(mainTexture), m_legsTexture(legsTexture)
{
}

const std::string& ArmorItem::Tier::getTexture(EquipmentSlot slot) const
{
    return (slot == Item::SLOT_LEGS) ? m_legsTexture : m_mainTexture;
}

ArmorItem::ArmorItem(int id, Tier& tier, EquipmentSlot slot) : Item(id), m_texture(tier.getTexture(slot)), m_slot(slot), m_defense(defensePerSlot[3 - slot])
{
	m_maxDamage = healthPerSlot[3 - slot] * 3 << tier.m_level;
	m_maxStackSize = 1;
}

Item::EquipmentSlot ArmorItem::getEquipmentSlot() const
{
	return m_slot;
}

const std::string& ArmorItem::getArmorTexture() const
{
	return m_texture;
}

int ArmorItem::getDefense() const
{
	return m_defense;
}
