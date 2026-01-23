#include "ResultSlot.hpp"
#include "world/item/Item.hpp"
#include "world/entity/Player.hpp"
//#include "stats/Achievement.hpp"

ResultSlot::ResultSlot(Player* player, Container* craftSlots, Container* resultSlots, int slotIndex, int x, int y) : Slot(resultSlots, slotIndex, x, y),
	m_pPlayer(player), m_pCraftSlots(craftSlots)
{
}

bool ResultSlot::mayPlace(const ItemInstance& item) const
{
	return false;
}

bool ResultSlot::canSync()
{
    return false;
}

void ResultSlot::onTake(ItemInstance& item)
{
	item.onCraftedBy(m_pPlayer, m_pPlayer->m_pLevel);

	//if (item.getId() == Tile::craftingTable->m_ID)
	//	m_pPlayer->awardStat(Achievements::buildWorkBench);
	//else if (item.getId() == Item::woodPickaxe->getId())
	//	m_pPlayer->awardStat(Achievements::buildPickaxe);
	//else if (item.getId() == Tile::furnace->m_ID)
	//	m_pPlayer->awardStat(Achievements::buildFurnace);
	//else if (item.getId() == Item::woodHoe->getId())
	//	m_pPlayer->awardStat(Achievements::buildHoe);
	//else if (item.getId() == Item::bread->getId())
	//	m_pPlayer->awardStat(Achievements::makeBread);
	//else if (item.getId() == Item::cake->getId())
	//	m_pPlayer->awardStat(Achievements::bakeCake);
	//else if (item.getId() == Item::stonePickaxe->getId())
	//	m_pPlayer->awardStat(Achievements::buildBetterPickaxe);
	//else if (item.getId() == Item::woodSword->getId())
	//	m_pPlayer->awardStat(Achievements::buildSword);

    for (int i = 0; i < m_pCraftSlots->getContainerSize(); ++i)
	{
		ItemInstance& instance = m_pCraftSlots->getItem(i);
        if (!instance.isEmpty())
		{
            m_pCraftSlots->removeItem(i, 1);
            Item* item = instance.getItem();
            if (item->hasCraftingRemainingItem())
                m_pCraftSlots->setItem(i, ItemInstance(item->getCraftingRemainingItem()));
        }
    }
}
