#include "SingleInputRecipe.hpp"

SingleInputRecipe::SingleInputRecipe(const ItemStack& ingredient, const ItemStack& result) :
    m_ingredient(ingredient),
    m_result(result)
{
}

SingleInputRecipe::~SingleInputRecipe()
{
}

bool SingleInputRecipe::matches(Container* container)
{
    return matches(container->getItem(0));
}

bool SingleInputRecipe::matches(const ItemStack& input)
{
    return input.getId() == m_ingredient.getId() && (m_ingredient.getAuxValue() == -1 || input.getAuxValue() == m_ingredient.getAuxValue());
}

const ItemStack& SingleInputRecipe::assemble(Container* container)
{
    return m_result;
}

int SingleInputRecipe::size() const
{
	return 1;
}
