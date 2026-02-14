#include "FurnaceTileEntity.hpp"
#include "world/item/crafting/FurnaceRecipes.hpp"
#include "world/tile/FurnaceTile.hpp"

FurnaceTileEntity::FurnaceTileEntity()
    : SimpleContainer(3, "gui.furnace"), m_litTime(0), m_litDuration(0), m_tickCount(0)
{
    m_pType = TileEntityType::furnace;
}

bool FurnaceTileEntity::_canBurn()
{
    if (getItem(0).isEmpty())
        return false;

    const ItemStack& result = FurnaceRecipes::singleton().getItemFor(this);

    // Not a furnace recipe
    if (result.isEmpty())
        return false;

    // Nothing has started burning yet
    if (getItem(2).isEmpty())
        return true;

    // Potential result/current result mismatch
    if (getItem(2) != result)
        return false;

    // Check if there's enough item stack size for the item to continue burning
    if (getItem(2).m_count < getMaxStackSize() && getItem(2).m_count < getItem(2).getMaxStackSize())
        return true;

    // Return if result slot is full
    return getItem(2).m_count < result.getMaxStackSize();
}

void FurnaceTileEntity::_burn()
{
    if (!_canBurn())
        return;

    const ItemStack& result = FurnaceRecipes::singleton().getItemFor(this);

    // Either set the item in the result slot to the item, or add what's already there
    if (getItem(2).isEmpty())
        setItem(2, result);
    else if (getItem(2) == result)
        ++getItem(2).m_count;

    // Decrement burning item
    if (getItem(0).m_count > 0)
        --getItem(0).m_count;

    // No more burning item
    if (getItem(0).m_count <= 0)
        setItem(0, ItemStack::EMPTY);
}

void FurnaceTileEntity::tick()
{
    if (m_pLevel->m_bIsClientSide)
        return;

    bool wasBurning = m_litTime > 0;
    bool changed = false;

    if (m_litTime > 0)
        --m_litTime;

    if (m_litTime == 0 && _canBurn())
    {
        m_litDuration = m_litTime = FurnaceRecipes::singleton().getBurnDuration(getItem(1));
        if (m_litTime > 0)
        {
            changed = true;
            if (!getItem(1).isEmpty())
            {
                --getItem(1).m_count;
                if (!getItem(1).m_count)
                    setItem(1, ItemStack::EMPTY);
            }
        }
    }

    if (isLit() && _canBurn())
    {
        ++m_tickCount;
        if (m_tickCount == 200)
        {
            m_tickCount = 0;
            _burn();
            changed = true;
        }
    }
    else
        m_tickCount = 0;

    bool isBurning = m_litTime > 0;
    if (isBurning != wasBurning)
    {
        changed = true;
        FurnaceTile::SetLit(isBurning, m_pLevel, m_pos);
    }

    if (changed)
        setChanged();
}

bool FurnaceTileEntity::stillValid(Player* player) const
{
    if (m_pLevel->getTileEntity(m_pos) != this)
        return false;

    return player->distanceToSqr(m_pos + 0.5f) <= 64.0;
}

void FurnaceTileEntity::setChanged()
{
    TileEntity::setChanged();
}

void FurnaceTileEntity::load(const CompoundTag& tag)
{
    TileEntity::load(tag);
    SimpleContainer::load(tag);

    m_litTime = tag.getInt16("BurnTime");
    m_tickCount = tag.getInt16("CookTime");
    m_litDuration = FurnaceRecipes::singleton().getBurnDuration(getItem(1));
}

void FurnaceTileEntity::save(CompoundTag& tag) const
{
    TileEntity::save(tag);
    tag.putInt16("BurnTime", m_litTime);
    tag.putInt16("CookTime", m_tickCount);
    SimpleContainer::save(tag);
}

std::string FurnaceTileEntity::getName() const
{
    return "Furnace";
}

int FurnaceTileEntity::getBurnProgress(int height) 
{
    return m_tickCount * height / 200;
}

int FurnaceTileEntity::getLitProgress(int height)
{
    if (m_litDuration == 0)
        m_litDuration = 200;

    return m_litTime * height / m_litDuration;
}

bool FurnaceTileEntity::isLit() 
{
    return m_litTime > 0;
}