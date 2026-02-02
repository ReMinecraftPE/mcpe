#pragma once

#include <vector>

#include "Recipe.hpp"
#include "world/inventory/CraftingContainer.hpp"

class ShapedRecipe : public Recipe
{
public:
    ShapedRecipe(int width, int height, const ItemStack& result, const std::vector<ItemStack>& ingredients);

    virtual bool matches(Container* container) override;
    virtual const ItemStack& assemble(Container* container) override;
    virtual int size() const override;

private:
    bool matches(CraftingContainer* container, int i, int j, bool lenient);

    bool isShaped() const override;

private:
    int m_width, m_height;
    ItemStack m_result;
    std::vector<ItemStack> m_ingredients;
};