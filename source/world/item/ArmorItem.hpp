#pragma once

#include "Item.hpp"

class ArmorItem : public Item
{
public:
	struct Tier
	{
		int m_level;
		std::string m_mainTexture;
		std::string m_legsTexture;

		Tier(int level, std::string texture1, std::string texture2);

		const std::string& getTexture(EquipmentSlot slot) const;

		static Tier CLOTH, CHAIN, IRON, EMERALD, GOLD;
	};

public:
	ArmorItem(int id, Tier& tier, EquipmentSlot slot);

	EquipmentSlot getEquipmentSlot() const override;
	const std::string& getArmorTexture() const override;
	int getDefense() const override;
private:
	const std::string& m_texture;
	EquipmentSlot m_slot;
	int m_defense;

private:
	static const int defensePerSlot[4];
	static const int healthPerSlot[4];
};