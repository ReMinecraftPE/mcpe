#pragma once

#include "Recipe.hpp"

class SingleInputRecipe : public Recipe
{
public:
    SingleInputRecipe(const ItemStack& ingredient, const ItemStack& result);
    virtual ~SingleInputRecipe();

    bool matches(Container* container) override;
    bool matches(const ItemStack& input);
    const ItemStack& assemble(Container* container) override;
    int size() const override;

    ItemStack m_ingredient;

private:
    ItemStack m_result;
};