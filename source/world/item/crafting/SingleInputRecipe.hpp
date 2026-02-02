#pragma once

#include "Recipe.hpp"

class SingleInputRecipe : public Recipe
{
public:
    SingleInputRecipe(const ItemStack& ingredient, const ItemStack& result);

    virtual bool matches(Container* container) override;
    virtual bool matches(const ItemStack& input);
    virtual const ItemStack& assemble(Container* container) override;
    virtual int size() const override;

    ItemStack m_ingredient;

private:
    ItemStack m_result;
};