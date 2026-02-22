#include "Recipes.hpp"
#include "world/tile/ClothTile.hpp"
//#include "world/tile/RecordPlayerTile.hpp"
//#include "world/item/MapItem.hpp"
#include "common/Logger.hpp"

Recipes* Recipes::instance = nullptr;

void Recipes::addTools(const ItemStack& material, Item* sword, Item* pickaxe, Item* axe, Item* shovel, Item* hoe)
{
    add(ShapedRecipeBuilder("X",
                            "X",
                            "#", ItemStack(sword))
        .add('X', material)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder("XXX",
                            " # ",
                            " # ", ItemStack(pickaxe))
        .add('X', material)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder("XX",
                            "X#",
                            " #", ItemStack(axe))
        .add('X', material)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder("X",
                            "#",
                            "#", ItemStack(shovel))
        .add('X', material)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder("XX",
                            " #",
                            " #", ItemStack(hoe))
        .add('X', material)
        .add('#', Item::stick));
}

void Recipes::addArmor(const ItemStack& material, Item* helmet, Item* chestplate, Item* leggings, Item* boots)
{
    add(ShapedRecipeBuilder("XXX",
                            "X X", ItemStack(helmet))
        .add('X', material));

    add(ShapedRecipeBuilder("X X",
                            "XXX",
                            "XXX", ItemStack(chestplate))
        .add('X', material));

    add(ShapedRecipeBuilder("XXX",
                            "X X",
                            "X X", ItemStack(leggings))
        .add('X', material));

    add(ShapedRecipeBuilder("X X",
                            "X X", ItemStack(boots))
        .add('X', material));
}

void Recipes::addOre(const ItemStack& material, Tile* block)
{
    add(ShapedRecipeBuilder("X", ItemStack(material.getId(), 9, material.getAuxValue()))
        .add('X', block));

    add(ShapedRecipeBuilder("XXX",
                            "XXX",
                            "XXX", ItemStack(block))
        .add('X', material));
}

class RecipeSorter
{
public:
    bool operator()(const Recipe* a, const Recipe* b)
    {
        if (!b->isShaped() && b->isShaped())
            return false;
        if (a->isShaped() && !b->isShaped())
            return true;
        return a->size() > b->size();
    }
};

Recipes::Recipes()
{
    // WeaponRecipes & ToolRecipes
    addTools(ItemStack(Tile::wood),       Item::sword_wood,    Item::pickAxe_wood,    Item::hatchet_wood,    Item::shovel_wood,    Item::hoe_wood);
    addTools(ItemStack(Tile::stoneBrick), Item::sword_stone,   Item::pickAxe_stone,   Item::hatchet_stone,   Item::shovel_stone,   Item::hoe_stone);
    addTools(ItemStack(Item::ironIngot),  Item::sword_iron,    Item::pickAxe_iron,    Item::hatchet_iron,    Item::shovel_iron,    Item::hoe_iron);
    addTools(ItemStack(Item::goldIngot),  Item::sword_stone,   Item::pickAxe_gold,    Item::hatchet_gold,    Item::shovel_gold,    Item::hoe_gold);
    addTools(ItemStack(Item::emerald),    Item::sword_emerald, Item::pickAxe_emerald, Item::hatchet_emerald, Item::shovel_emerald, Item::hoe_emerald);

    add(ShapedRecipeBuilder(" #X",
                            "# X",
                            " #X", ItemStack(Item::bow))
        .add('X', Item::string)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder("X",
                            "#",
                            "Y", ItemStack(Item::arrow, 4))
        .add('Y', Item::feather)
        .add('X', Item::flint)
        .add('#', Item::stick));

    // ArmorRecipes
    addArmor(ItemStack(Item::leather),   Item::helmet_cloth,   Item::chestplate_cloth,   Item::leggings_cloth,   Item::boots_cloth);
    addArmor(ItemStack(Tile::fire),      Item::helmet_chain,   Item::chestplate_chain,   Item::leggings_chain,   Item::boots_chain);
    addArmor(ItemStack(Item::ironIngot), Item::helmet_iron,    Item::chestplate_iron,    Item::leggings_iron,    Item::boots_iron);
    addArmor(ItemStack(Item::goldIngot), Item::helmet_gold,    Item::chestplate_gold,    Item::leggings_gold,    Item::boots_gold);
    addArmor(ItemStack(Item::emerald),   Item::helmet_diamond, Item::chestplate_diamond, Item::leggings_diamond, Item::boots_diamond);

    // OreRecipes
    addOre(ItemStack(Item::ironIngot), Tile::ironBlock);
    addOre(ItemStack(Item::goldIngot), Tile::goldBlock);
    addOre(ItemStack(Item::emerald), Tile::emeraldBlock);
    addOre(ItemStack(Item::dye_powder, 1, 4), Tile::lapisBlock);

    // StructureRecipes
    add(ShapedRecipeBuilder("###",
                            "# #",
                            "###", ItemStack(Tile::chest))
        .add('#', Tile::wood));
    add(ShapedRecipeBuilder("###",
                            "# #",
                            "###", ItemStack(Tile::furnace))
        .add('#', Tile::stoneBrick));

    add(ShapedRecipeBuilder("##",
                            "##", ItemStack(Tile::craftingTable))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder("##",
                            "##", ItemStack(Tile::sandStone))
        .add('#', Tile::sand));

    add(ShapedRecipeBuilder("##",
                            "##", ItemStack(Tile::glowstone))
        .add('#', Item::yellowDust));

    // FoodRecipes
    add(ShapedRecipeBuilder("YX#", ItemStack(Item::mushroomStew, 1))
        .add('X', Tile::mushroom1)
        .add('Y', Tile::mushroom2)
        .add('#', Item::bowl));

    add(ShapedRecipeBuilder("#X#", ItemStack(Item::cookie, 1))
        .add('X', ItemStack(Item::dye_powder, 1, 3))
        .add('#', Item::wheat));

    add(ShapedRecipeBuilder("###", ItemStack(Item::paper, 3))
        .add('#', Item::reeds));

    add(ShapedRecipeBuilder("#",
                            "#",
                            "#", ItemStack(Item::book, 1))
        .add('#', Item::paper));

    add(ShapedRecipeBuilder("###",
                            "###", ItemStack(Tile::fence, 2))
        .add('#', Item::stick));

    //add(ShapedRecipeBuilder("###",
    //                        "#X#",
    //                        "###", ItemStack(Tile::recordPlayer, 1))
    //    .add('#', Tile::wood)
    //    .add('X', Item::emerald));

    add(ShapedRecipeBuilder("###",
                            "#X#",
                            "###", ItemStack(Tile::musicBlock, 1))
        .add('#', Tile::wood)
        .add('X', Item::redStone));

    add(ShapedRecipeBuilder("###",
                            "XXX",
                            "###", ItemStack(Tile::bookshelf, 1))
        .add('#', Tile::wood)
        .add('X', Item::book));

    add(ShapedRecipeBuilder("##",
                            "##", ItemStack(Tile::snow, 1))
        .add('#', Item::snowBall));

    add(ShapedRecipeBuilder("##",
                            "##", ItemStack(Tile::clay, 1))
        .add('#', Item::clay));

    add(ShapedRecipeBuilder("##",
                            "##", ItemStack(Tile::redBrick, 1))
        .add('#', Item::brick));

    add(ShapedRecipeBuilder("###",
                            "###",
                            "###", ItemStack(Tile::glowstone, 1))
        .add('#', Item::yellowDust));

    add(ShapedRecipeBuilder("###",
                            "###",
                            "###", ItemStack(Tile::cloth, 1))
        .add('#', Item::string));

    add(ShapedRecipeBuilder("X#X",
                            "#X#",
                            "X#X", ItemStack(Tile::tnt, 1))
        .add('X', Item::sulphur)
        .add('#', Tile::sand));

    add(ShapedRecipeBuilder("###", ItemStack(Tile::stoneSlabHalf, 3))
        .add('#', Tile::rock));

    add(ShapedRecipeBuilder("###", ItemStack(Tile::stoneSlabHalf, 3, 1))
        .add('#', Tile::sandStone));

    add(ShapedRecipeBuilder("###", ItemStack(Tile::stoneSlabHalf, 3, 2))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder("###", ItemStack(Tile::stoneSlabHalf, 3, 3))
        .add('#', Tile::stoneBrick));

    //add(ShapedRecipeBuilder("###",
    //                        "###", ItemStack(Tile::trapDoor, 2))
    //    .add('#', Tile::wood));

    add(ShapedRecipeBuilder("# #",
                            "###",
                            "# #", ItemStack(Tile::ladder, 2))
        .add('#', Item::stick));

    add(ShapedRecipeBuilder("##",
                            "##",
                            "##", ItemStack(Item::door_wood, 1))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder("##",
                            "##",
                            "##", ItemStack(Item::door_iron, 1))
        .add('#', Item::ironIngot));

    add(ShapedRecipeBuilder("###",
                            "###",
                            " X ", ItemStack(Item::sign, 1))
        .add('#', Tile::wood)
        .add('X', Item::stick));

    add(ShapedRecipeBuilder("AAA",
                            "BEB",
                            "CCC", ItemStack(Item::cake, 1))
        .add('A', Item::milk)
        .add('B', Item::sugar)
        .add('C', Item::wheat)
        .add('E', Item::egg));

    add(ShapedRecipeBuilder("#", ItemStack(Item::sugar, 1))
        .add('#', Item::reeds));

    add(ShapedRecipeBuilder("#", ItemStack(Tile::wood, 4))
        .add('#', Tile::treeTrunk));

    add(ShapedRecipeBuilder("#",
                            "#", ItemStack(Item::stick, 4))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder("X",
                            "#", ItemStack(Tile::torch, 4))
        .add('X', Item::coal)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder("X",
                            "#", ItemStack(Tile::torch, 4))
        .add('X', ItemStack(Item::coal, 1, 1))
        .add('#', Item::stick));

    add(ShapedRecipeBuilder("# #",
                            " # ", ItemStack(Item::bowl, 4))
        .add('#', Tile::wood));

    //add(ShapedRecipeBuilder("X X",
    //                        "X#X",
    //                        "X X", ItemStack(Tile::rail, 16))
    //    .add('X', Item::ironIngot)
    //    .add('#', Item::stick));

    add(ShapedRecipeBuilder("# #",
                            "###", ItemStack(Item::minecart, 1))
        .add('#', Item::ironIngot));

    add(ShapedRecipeBuilder("A",
                            "B", ItemStack(Tile::pumpkinLantern, 1))
        .add('A', Tile::pumpkin)
        .add('B', Tile::torch));

    //add(ShapedRecipeBuilder("A",
    //                        "B", ItemStack(Item::minecart_chest, 1))
    //    .add('A', Tile::chest)
    //    .add('B', Item::minecart));

    //add(ShapedRecipeBuilder("A",
    //                        "B", ItemStack(Item::minecart_furnace, 1))
    //    .add('A', Tile::furnace)
    //    .add('B', Item::minecart));

    add(ShapedRecipeBuilder("# #",
                            "###", ItemStack(Item::boat, 1))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder("# #",
                            " # ", ItemStack(Item::bucket_empty, 1))
        .add('#', Item::ironIngot));

    add(ShapedRecipeBuilder("A ",
                            " B", ItemStack(Item::flintAndSteel, 1))
        .add('A', Item::ironIngot)
        .add('B', Item::flint));

    add(ShapedRecipeBuilder("A ",
                            " A", ItemStack(Item::shears, 1))
        .add('A', Item::ironIngot));

    add(ShapedRecipeBuilder("###", ItemStack(Item::bread, 1))
        .add('#', Item::wheat));

    add(ShapedRecipeBuilder("#  ",
                            "## ",
                            "###", ItemStack(Tile::stairs_wood, 4))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder("  #",
                            " #X",
                            "# X", ItemStack(Item::fishingRod, 1))
        .add('#', Item::stick)
        .add('X', Item::string));

    add(ShapedRecipeBuilder("#  ",
                            "## ",
                            "###", ItemStack(Tile::stairs_stone, 4))
        .add('#', Tile::stoneBrick));

    add(ShapedRecipeBuilder("###",
                            "#X#",
                            "###", ItemStack(Item::painting, 1))
        .add('#', Item::stick)
        .add('X', Tile::cloth));

    add(ShapedRecipeBuilder("###",
                            "#X#",
                            "###", ItemStack(Item::apple_gold, 1))
        .add('#', Tile::goldBlock)
        .add('X', Item::apple));

    //add(ShapedRecipeBuilder("X",
    //                        "#", ItemStack(Tile::lever, 1))
    //    .add('X', Item::stick)
    //    .add('#', Tile::stoneBrick));

    //add(ShapedRecipeBuilder("X",
    //                        "#", ItemStack(Tile::redstoneTorchLit, 1))
    //    .add('X', Item::redStone)
    //    .add('#', Item::stick));

    //add(ShapedRecipeBuilder("#X#",
    //                        "III", ItemStack(Item::diode, 1))
    //    .add('#', Tile::redstoneTorchLit)
    //    .add('X', Item::redStone)
    //    .add('I', Tile::rock));

    add(ShapedRecipeBuilder(" # ",
                            "#X#",
                            " # ", ItemStack(Item::clock, 1))
        .add('#', Item::goldIngot)
        .add('X', Item::redStone));

    add(ShapedRecipeBuilder(" # ",
                            "#X#",
                            " # ", ItemStack(Item::compass, 1))
        .add('#', Item::ironIngot)
        .add('X', Item::redStone));

    add(ShapedRecipeBuilder("###",
                            "#X#",
                            "###", ItemStack(Item::map, 1))
        .add('#', Item::paper)
        .add('X', Item::compass));

    //add(ShapedRecipeBuilder("#", ItemStack(Tile::button, 1))
    //    .add('#', Tile::rock));

    //add(ShapedRecipeBuilder("##", ItemStack(Tile::stonePressurePlate, 1))
    //    .add('#', Tile::rock));

    //add(ShapedRecipeBuilder("##", ItemStack(Tile::woodPressurePlate, 1))
    //    .add('#', Tile::wood));

    //add(ShapedRecipeBuilder("###",
    //                        "#X#",
    //                        "#R#", ItemStack(Tile::dispenser, 1))
    //    .add('#', Tile::stoneBrick)
    //    .add('X', Item::bow)
    //    .add('R', Item::redStone));

    add(ShapedRecipeBuilder("###",
                            "XXX", ItemStack(Item::bed))
        .add('#', Tile::cloth)
        .add('X', Tile::wood));

    //add(ShapedRecipeBuilder("###",
    //                        "XIX",
    //                        "XRX", ItemStack(Tile::piston))
    //    .add('#', Tile::wood)
    //    .add('X', Tile::stoneBrick)
    //    .add('I', Item::ironIngot)
    //    .add('R', Item::redStone));

    //add(ShapedRecipeBuilder("X",
    //                        "#", ItemStack(Tile::stickyPiston))
    //    .add('X', Item::slimeBall)
    //    .add('#', Tile::piston));

    // ClothDyeRecipes
    for (int i = 0; i < 16; ++i)
    {
        add(ShapelessRecipeBuilder(ItemStack(Tile::cloth, 1, ClothTile::getColorFromData(i)))
            .add(ItemStack(Item::dye_powder, 1, i))
            .add(ItemStack(Tile::cloth, 1, 0)));
    }

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 11))
        .add(Tile::flower));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 1))
        .add(Tile::rose));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 3, 15))
        .add(Item::bone));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 9))
        .add(ItemStack(Item::dye_powder, 1, 1))
        .add(ItemStack(Item::dye_powder, 1, 15)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 14))
        .add(ItemStack(Item::dye_powder, 1, 1))
        .add(ItemStack(Item::dye_powder, 1, 11)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 10))
        .add(ItemStack(Item::dye_powder, 1, 2))
        .add(ItemStack(Item::dye_powder, 1, 15)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 8))
        .add(ItemStack(Item::dye_powder, 1, 0))
        .add(ItemStack(Item::dye_powder, 1, 15)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 7))
        .add(ItemStack(Item::dye_powder, 1, 8))
        .add(ItemStack(Item::dye_powder, 1, 15)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 3, 7))
        .add(ItemStack(Item::dye_powder, 1, 0))
        .add(ItemStack(Item::dye_powder, 1, 15))
        .add(ItemStack(Item::dye_powder, 1, 15)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 12))
        .add(ItemStack(Item::dye_powder, 1, 4))
        .add(ItemStack(Item::dye_powder, 1, 15)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 6))
        .add(ItemStack(Item::dye_powder, 1, 4))
        .add(ItemStack(Item::dye_powder, 1, 2)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 5))
        .add(ItemStack(Item::dye_powder, 1, 4))
        .add(ItemStack(Item::dye_powder, 1, 1)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 2, 13))
        .add(ItemStack(Item::dye_powder, 1, 5))
        .add(ItemStack(Item::dye_powder, 1, 9)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 3, 13))
        .add(ItemStack(Item::dye_powder, 1, 4))
        .add(ItemStack(Item::dye_powder, 1, 1))
        .add(ItemStack(Item::dye_powder, 1, 9)));

    add(ShapelessRecipeBuilder(ItemStack(Item::dye_powder, 4, 13))
        .add(ItemStack(Item::dye_powder, 1, 4))
        .add(ItemStack(Item::dye_powder, 1, 1))
        .add(ItemStack(Item::dye_powder, 1, 1))
        .add(ItemStack(Item::dye_powder, 1, 15)));

    std::sort(m_recipes.begin(), m_recipes.end(), RecipeSorter());

    LOG_I("%d recipes loaded", m_recipes.size());
}

Recipes::~Recipes()
{
    for (size_t i = 0; i < m_recipes.size(); ++i)
    {
        delete m_recipes[i];
    }
}
