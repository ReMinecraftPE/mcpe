#include "ShapelessRecipe.hpp"
#include "world/inventory/CraftingContainer.hpp"

ShapelessRecipe::ShapelessRecipe(const ItemStack& result, const std::vector<ItemStack>& ingredients) :
    m_result(result),
    m_ingredients(ingredients)
{
}

bool ShapelessRecipe::matches(Container* container)
{
    CraftingContainer* craftingContainer = (CraftingContainer*)container;
    if (!craftingContainer) return false;

    std::vector<ItemStack> ings = m_ingredients;

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            const ItemStack& item = craftingContainer->getItem(x, y);
            if (!item.isEmpty())
            {
                bool matches = false;

                for (std::vector<ItemStack>::iterator it = ings.begin(); it != ings.end(); ++it)
                {
                    ItemStack& secondItem = *it;
                    if (item.getId() == secondItem.getId() && (secondItem.getAuxValue() == -1 || item.getAuxValue() == secondItem.getAuxValue()))
                    {
                        matches = true;
                        ings.erase(it);
                        break;
                    }
                }

                if (!matches)
                    return false;
            }
        }
    }

    return ings.empty();
}

const ItemStack& ShapelessRecipe::assemble(Container* container)
{
    return m_result;
}

int ShapelessRecipe::size() const
{
	return int(m_ingredients.size());
}
