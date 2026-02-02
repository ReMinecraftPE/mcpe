#pragma once

#include <vector>

#include "Recipe.hpp"

class ShapelessRecipe : public Recipe
{
public:
    ShapelessRecipe(const ItemStack& result, const std::vector<ItemStack>& ingredients);
    virtual ~ShapelessRecipe();

    virtual bool matches(Container* container) override;
    virtual const ItemStack& assemble(Container* container) override;
    virtual int size() const override;

private:
    ItemStack m_result;
    std::vector<ItemStack> m_ingredients;
};