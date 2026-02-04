#include <map>

#include "SingleInputRecipe.hpp"
#include "world/tile/Tile.hpp"

class FurnaceRecipes
{
public:
    typedef std::multimap<int, SingleInputRecipe*> FurnaceRecipesMap;
    typedef std::map<int, int> BurnTimeMap;
    typedef std::map<Material*, int> MaterialBurnTimeMap;

public:
    FurnaceRecipes();
    ~FurnaceRecipes();

    static FurnaceRecipes& singleton()
    {
        if (!instance)
        {
            instance = new FurnaceRecipes;
        }
        return *instance;
    }

    void addFurnaceRecipe(Tile* tile, const ItemStack& result)
    {
        addFurnaceRecipe(ItemStack(tile, 1, -1), result);
    }

    void addFurnaceRecipe(Item* item, const ItemStack& result)
    {
        addFurnaceRecipe(ItemStack(item, 1, -1), result);
    }

    void addFurnaceRecipe(const ItemStack& ingredient, const ItemStack& result)
    {
        m_furnaceRecipes.insert(std::make_pair(ingredient.getId(), new SingleInputRecipe(ingredient, result)));
    }

    void add(SingleInputRecipe* recipe)
    {
        m_furnaceRecipes.insert(std::make_pair(recipe->m_ingredient.getId(), recipe));
    }

    const ItemStack& getItemFor(Container* container)
    {
        ItemStack& testItem = container->getItem(0);
        std::pair<FurnaceRecipesMap::iterator,
            FurnaceRecipesMap::iterator> matches =
            m_furnaceRecipes.equal_range(testItem.getId());

        for (FurnaceRecipesMap::iterator it = matches.first;
            it != matches.second; ++it)
        {
            if (it->second->matches(container))
                return it->second->assemble(container);
        }
        return ItemStack::EMPTY;
    }

    bool isFurnaceItem(const ItemStack& item)
    {
        if (item.isEmpty()) return false;

        std::pair<FurnaceRecipesMap::iterator,
            FurnaceRecipesMap::iterator> matches =
            m_furnaceRecipes.equal_range(item.getId());

        for (FurnaceRecipesMap::iterator it = matches.first;
            it != matches.second; ++it)
        {
            if (it->second->matches(item))
                return true;
        }
        return false;
    }

    int getBurnDuration(const ItemStack& item)
    {
        if (item.isEmpty()) return 0;

        BurnTimeMap::const_iterator it = m_burnDuration.find(item.getId());
        if (it != m_burnDuration.end())
            return it->second;

        if (item.getId() < 256)
        {
            MaterialBurnTimeMap::const_iterator it2 =
                m_materialBurnDuration.find(Tile::tiles[item.getId()]->m_pMaterial);
            if (it2 != m_materialBurnDuration.end())
                return it2->second;
        }
        return 0;
    }

    void addFurnaceFuel(Tile* tile, int burnDuration)
    {
        m_burnDuration[tile->m_ID] = burnDuration;
    }

    void addFurnaceFuel(Item* item, int burnDuration)
    {
        m_burnDuration[item->m_itemID] = burnDuration;
    }

    void addFurnaceFuel(Material* material, int burnDuration)
    {
        m_materialBurnDuration[material] = burnDuration;
    }

private:
    static FurnaceRecipes* instance;

    BurnTimeMap m_burnDuration;
    MaterialBurnTimeMap m_materialBurnDuration;

public:
    FurnaceRecipesMap m_furnaceRecipes;
};