#include <assert.h>
#include "EntityTypeDescriptor.hpp"
#include "compat/LegacyCPP.hpp"

std::map<EntityType::ID, const EntityTypeDescriptor*> EntityTypeDescriptor::entityTypeIdMap = std::map<EntityType::ID, const EntityTypeDescriptor*>();
std::map<std::string, const EntityTypeDescriptor*> EntityTypeDescriptor::entityTypeNameMap = std::map<std::string, const EntityTypeDescriptor*>();

EntityTypeDescriptor::EntityTypeDescriptor(const EntityType& type, const EntityCategories& categories)
{
	m_type = type;
	m_categories = categories;
}

void EntityTypeDescriptor::initDescriptors()
{
	for (int i = 0; i < allCount; i++)
	{
		const EntityTypeDescriptor* desc = all[i];
		const EntityType& entityType = desc->getEntityType();

		entityTypeIdMap[entityType.getId()] = desc;
		// Multiple EntityTypes have the same Name
		assert(entityTypeNameMap[entityType.getName()] == nullptr);
		entityTypeNameMap[entityType.getName()] = desc;
	}
}

void EntityTypeDescriptor::teardownDescriptors()
{

}

const EntityTypeDescriptor* EntityTypeDescriptor::GetByEntityTypeID(EntityType::ID id)
{
	const EntityTypeDescriptor* result = nullptr;
	std::map<EntityType::ID, const EntityTypeDescriptor*>::iterator it = entityTypeIdMap.find(id);
	if (it != entityTypeIdMap.end())
		result = it->second;
	else
		assert(!"Failed to find EntityTypeDescriptor for specified EntityType::ID");

	return result;
}

const EntityTypeDescriptor* EntityTypeDescriptor::GetByEntityTypeName(EntityType::Name name)
{
	const EntityTypeDescriptor* result = nullptr;
	std::map<std::string, const EntityTypeDescriptor*>::iterator it = entityTypeNameMap.find(name);
	if (it != entityTypeNameMap.end())
		result = it->second;

	return result;
}

const EntityTypeDescriptor* EntityTypeDescriptor::all[] = {
		  &EntityTypeDescriptor::unknown
		, &EntityTypeDescriptor::chicken
		, &EntityTypeDescriptor::cow
		, &EntityTypeDescriptor::pig
		, &EntityTypeDescriptor::sheep
		, &EntityTypeDescriptor::wolf
		, &EntityTypeDescriptor::villager
		, &EntityTypeDescriptor::mushroomCow
		, &EntityTypeDescriptor::squid
		, &EntityTypeDescriptor::rabbit
		, &EntityTypeDescriptor::bat
		, &EntityTypeDescriptor::ironGolem
		, &EntityTypeDescriptor::snowGolem
		, &EntityTypeDescriptor::ocelot
		, &EntityTypeDescriptor::horse
		, &EntityTypeDescriptor::donkey
		, &EntityTypeDescriptor::mule
		, &EntityTypeDescriptor::skeletonHorse
		, &EntityTypeDescriptor::zombieHorse
		, &EntityTypeDescriptor::polarBear
		, &EntityTypeDescriptor::llama
		, &EntityTypeDescriptor::parrot
		, &EntityTypeDescriptor::dolphin
		, &EntityTypeDescriptor::zombie
		, &EntityTypeDescriptor::creeper
		, &EntityTypeDescriptor::skeleton
		, &EntityTypeDescriptor::spider
		, &EntityTypeDescriptor::pigZombie
		, &EntityTypeDescriptor::slime
		, &EntityTypeDescriptor::enderMan
		, &EntityTypeDescriptor::silverfish
		, &EntityTypeDescriptor::caveSpider
		, &EntityTypeDescriptor::ghast
		, &EntityTypeDescriptor::lavaSlime
		, &EntityTypeDescriptor::blaze
		, &EntityTypeDescriptor::zombieVillager
		, &EntityTypeDescriptor::witch
		, &EntityTypeDescriptor::stray
		, &EntityTypeDescriptor::husk
		, &EntityTypeDescriptor::witherSkeleton
		, &EntityTypeDescriptor::guardian
		, &EntityTypeDescriptor::elderGuardian
		, &EntityTypeDescriptor::npc
		, &EntityTypeDescriptor::wither
		, &EntityTypeDescriptor::enderDragon
		, &EntityTypeDescriptor::shulker
		, &EntityTypeDescriptor::endermite
		, &EntityTypeDescriptor::agent
		, &EntityTypeDescriptor::vindicator
		, &EntityTypeDescriptor::phantom
		, &EntityTypeDescriptor::ravager
		, &EntityTypeDescriptor::armorStand
		, &EntityTypeDescriptor::tripodCamera
		, &EntityTypeDescriptor::player
		, &EntityTypeDescriptor::item
		, &EntityTypeDescriptor::primedTnt
		, &EntityTypeDescriptor::fallingTile
		, &EntityTypeDescriptor::movingTile
		, &EntityTypeDescriptor::experiencePotion
		, &EntityTypeDescriptor::experienceOrb
		, &EntityTypeDescriptor::eyeOfEnder
		, &EntityTypeDescriptor::enderCrystal
		, &EntityTypeDescriptor::fireworksRocket
		, &EntityTypeDescriptor::thrownTrident
		, &EntityTypeDescriptor::turtle
		, &EntityTypeDescriptor::cat
		, &EntityTypeDescriptor::shulkerBullet
		, &EntityTypeDescriptor::fishingHook
		, &EntityTypeDescriptor::chalkboard
		, &EntityTypeDescriptor::dragonFireball
		, &EntityTypeDescriptor::arrow
		, &EntityTypeDescriptor::snowball
		, &EntityTypeDescriptor::thrownEgg
		, &EntityTypeDescriptor::painting
		, &EntityTypeDescriptor::minecartRideable
		, &EntityTypeDescriptor::largeFireball
		, &EntityTypeDescriptor::splashPotion
		, &EntityTypeDescriptor::enderPearl
		, &EntityTypeDescriptor::leashKnot
		, &EntityTypeDescriptor::witherSkull
		, &EntityTypeDescriptor::boat
		, &EntityTypeDescriptor::witherSkullDangerous
		, &EntityTypeDescriptor::itemFrame
		, &EntityTypeDescriptor::lightningBolt
		, &EntityTypeDescriptor::smallFireball
		, &EntityTypeDescriptor::areaEffectCloud
		, &EntityTypeDescriptor::minecartHopper
		, &EntityTypeDescriptor::minecartTnt
		, &EntityTypeDescriptor::minecartChest
		, &EntityTypeDescriptor::minecartCommandBlock
		, &EntityTypeDescriptor::lingeringPotion
		, &EntityTypeDescriptor::llamaSpit
		, &EntityTypeDescriptor::evocationFang
		, &EntityTypeDescriptor::evocationIllager
		, &EntityTypeDescriptor::vex
		, &EntityTypeDescriptor::iceBomb
		, &EntityTypeDescriptor::balloon
		, &EntityTypeDescriptor::pufferfish
		, &EntityTypeDescriptor::salmon
		, &EntityTypeDescriptor::drowned
		, &EntityTypeDescriptor::tropicalFish
		, &EntityTypeDescriptor::cod
		, &EntityTypeDescriptor::panda
		, &EntityTypeDescriptor::pillager
		, &EntityTypeDescriptor::villagerV2
		, &EntityTypeDescriptor::zombieVillagerV2
		, &EntityTypeDescriptor::shield
		, &EntityTypeDescriptor::wanderingTrader
		, &EntityTypeDescriptor::elderGuardianGhost
		, &EntityTypeDescriptor::fox
		, &EntityTypeDescriptor::bee
		, &EntityTypeDescriptor::piglin
		, &EntityTypeDescriptor::hoglin
		, &EntityTypeDescriptor::strider
		, &EntityTypeDescriptor::zoglin
		, &EntityTypeDescriptor::piglinBrute
		, &EntityTypeDescriptor::goat
		, &EntityTypeDescriptor::glowSquid
		, &EntityTypeDescriptor::axolotl
		, &EntityTypeDescriptor::warden
		, &EntityTypeDescriptor::frog
		, &EntityTypeDescriptor::tadpole
		, &EntityTypeDescriptor::allay
		, &EntityTypeDescriptor::firefly
		, &EntityTypeDescriptor::camel
		, &EntityTypeDescriptor::sniffer
		, &EntityTypeDescriptor::breeze
		, &EntityTypeDescriptor::breezeWindChargeProjectile
		, &EntityTypeDescriptor::armadillo
		, &EntityTypeDescriptor::windChargeProjectile
		, &EntityTypeDescriptor::bogged
		, &EntityTypeDescriptor::ominousItemSpawner
		, &EntityTypeDescriptor::creaking
		, &EntityTypeDescriptor::traderLlama
		, &EntityTypeDescriptor::chestBoat
};
const int EntityTypeDescriptor::allCount = sizeof(EntityTypeDescriptor::all) / sizeof(const EntityTypeDescriptor*);

// The Monolith //
const EntityTypeDescriptor
  EntityTypeDescriptor::unknown                    = EntityTypeDescriptor(EntityType(EntityType::UNKNOWN, "unknown"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::chicken                    = EntityTypeDescriptor(EntityType(EntityType::CHICKEN, "chicken"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::cow                        = EntityTypeDescriptor(EntityType(EntityType::COW, "cow"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::pig                        = EntityTypeDescriptor(EntityType(EntityType::PIG, "pig"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::sheep                      = EntityTypeDescriptor(EntityType(EntityType::SHEEP, "sheep"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::wolf                       = EntityTypeDescriptor(EntityType(EntityType::WOLF, "wolf"), EntityCategories(EntityCategories::TAMABLE_ANIMAL))
, EntityTypeDescriptor::villager                   = EntityTypeDescriptor(EntityType(EntityType::VILLAGER, "villager"), EntityCategories(EntityCategories::VILLAGER_BASE))
, EntityTypeDescriptor::mushroomCow                = EntityTypeDescriptor(EntityType(EntityType::MUSHROOM_COW, "mooshroom"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::squid                      = EntityTypeDescriptor(EntityType(EntityType::SQUID, "squid"), EntityCategories(EntityCategories::WATER_ANIMAL))
, EntityTypeDescriptor::rabbit                     = EntityTypeDescriptor(EntityType(EntityType::RABBIT, "rabbit"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::bat                        = EntityTypeDescriptor(EntityType(EntityType::BAT, "bat"), EntityCategories(EntityCategories::AMBIENT))
, EntityTypeDescriptor::ironGolem                  = EntityTypeDescriptor(EntityType(EntityType::IRON_GOLEM, "iron_golem"), EntityCategories(EntityCategories::PATHFINDER_MOB))
, EntityTypeDescriptor::snowGolem                  = EntityTypeDescriptor(EntityType(EntityType::SNOW_GOLEM, "snow_golem"), EntityCategories(EntityCategories::PATHFINDER_MOB))
, EntityTypeDescriptor::ocelot                     = EntityTypeDescriptor(EntityType(EntityType::OCELOT, "ocelot"), EntityCategories(EntityCategories::TAMABLE_ANIMAL))
, EntityTypeDescriptor::horse                      = EntityTypeDescriptor(EntityType(EntityType::HORSE, "horse"), EntityCategories(EntityCategories::EQUINE_ANIMAL))
, EntityTypeDescriptor::donkey                     = EntityTypeDescriptor(EntityType(EntityType::DONKEY, "donkey"), EntityCategories(EntityCategories::EQUINE_ANIMAL))
, EntityTypeDescriptor::mule                       = EntityTypeDescriptor(EntityType(EntityType::MULE, "mule"), EntityCategories(EntityCategories::EQUINE_ANIMAL))
, EntityTypeDescriptor::skeletonHorse              = EntityTypeDescriptor(EntityType(EntityType::SKELETON_HORSE, "skeleton_horse"), EntityCategories((EntityCategories::CategoriesMask)(EntityCategories::EQUINE_ANIMAL | EntityCategories::UNDEAD_MOB)))
, EntityTypeDescriptor::zombieHorse                = EntityTypeDescriptor(EntityType(EntityType::ZOMBIE_HORSE, "zombie_horse"), EntityCategories((EntityCategories::CategoriesMask)(EntityCategories::EQUINE_ANIMAL | EntityCategories::UNDEAD_MOB)))
, EntityTypeDescriptor::polarBear                  = EntityTypeDescriptor(EntityType(EntityType::POLAR_BEAR, "polar_bear"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::llama                      = EntityTypeDescriptor(EntityType(EntityType::LLAMA, "llama"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::parrot                     = EntityTypeDescriptor(EntityType(EntityType::PARROT, "parrot"), EntityCategories(EntityCategories::TAMABLE_ANIMAL))
, EntityTypeDescriptor::dolphin                    = EntityTypeDescriptor(EntityType(EntityType::DOLPHIN, "dolphin"), EntityCategories(EntityCategories::WATER_ANIMAL))
, EntityTypeDescriptor::zombie                     = EntityTypeDescriptor(EntityType(EntityType::ZOMBIE, "zombie"), EntityCategories(EntityCategories::ZOMBIE_MONSTER))
, EntityTypeDescriptor::creeper                    = EntityTypeDescriptor(EntityType(EntityType::CREEPER, "creeper"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::skeleton                   = EntityTypeDescriptor(EntityType(EntityType::SKELETON, "skeleton"), EntityCategories(EntityCategories::SKELETON_MONSTER))
, EntityTypeDescriptor::spider                     = EntityTypeDescriptor(EntityType(EntityType::SPIDER, "spider"), EntityCategories(EntityCategories::ARTHROPOD))
, EntityTypeDescriptor::pigZombie                  = EntityTypeDescriptor(EntityType(EntityType::PIG_ZOMBIE, "zombie_pigman"), EntityCategories(EntityCategories::UNDEAD_MOB))
, EntityTypeDescriptor::slime                      = EntityTypeDescriptor(EntityType(EntityType::SLIME, "slime"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::enderMan                   = EntityTypeDescriptor(EntityType(EntityType::ENDER_MAN, "ender_man"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::silverfish                 = EntityTypeDescriptor(EntityType(EntityType::SILVERFISH, "silverfish"), EntityCategories(EntityCategories::ARTHROPOD))
, EntityTypeDescriptor::caveSpider                 = EntityTypeDescriptor(EntityType(EntityType::CAVE_SPIDER, "cave_spider"), EntityCategories(EntityCategories::ARTHROPOD))
, EntityTypeDescriptor::ghast                      = EntityTypeDescriptor(EntityType(EntityType::GHAST, "ghast"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::lavaSlime                  = EntityTypeDescriptor(EntityType(EntityType::LAVA_SLIME, "magma_cube"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::blaze                      = EntityTypeDescriptor(EntityType(EntityType::BLAZE, "blaze"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::zombieVillager             = EntityTypeDescriptor(EntityType(EntityType::ZOMBIE_VILLAGER, "zombie_villager"), EntityCategories(EntityCategories::ZOMBIE_MONSTER))
, EntityTypeDescriptor::witch                      = EntityTypeDescriptor(EntityType(EntityType::WITCH, "witch"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::stray                      = EntityTypeDescriptor(EntityType(EntityType::STRAY, "stray"), EntityCategories(EntityCategories::SKELETON_MONSTER))
, EntityTypeDescriptor::husk                       = EntityTypeDescriptor(EntityType(EntityType::HUSK, "husk"), EntityCategories(EntityCategories::ZOMBIE_MONSTER))
, EntityTypeDescriptor::witherSkeleton             = EntityTypeDescriptor(EntityType(EntityType::WITHER_SKELETON, "wither_skeleton"), EntityCategories(EntityCategories::SKELETON_MONSTER))
, EntityTypeDescriptor::guardian                   = EntityTypeDescriptor(EntityType(EntityType::GUARDIAN, "guardian"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::elderGuardian              = EntityTypeDescriptor(EntityType(EntityType::ELDER_GUARDIAN, "elder_guardian"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::npc                        = EntityTypeDescriptor(EntityType(EntityType::NPC, "npc"), EntityCategories(EntityCategories::MOB))
, EntityTypeDescriptor::wither                     = EntityTypeDescriptor(EntityType(EntityType::WITHER, "wither"), EntityCategories(EntityCategories::UNDEAD_MOB))
, EntityTypeDescriptor::enderDragon                = EntityTypeDescriptor(EntityType(EntityType::ENDER_DRAGON, "ender_dragon"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::shulker                    = EntityTypeDescriptor(EntityType(EntityType::SHULKER, "shulker"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::endermite                  = EntityTypeDescriptor(EntityType(EntityType::ENDERMITE, "endermite"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::agent                      = EntityTypeDescriptor(EntityType(EntityType::AGENT, "agent"), EntityCategories(EntityCategories::MOB))
, EntityTypeDescriptor::vindicator                 = EntityTypeDescriptor(EntityType(EntityType::VINDICATOR, "vindicator"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::phantom                    = EntityTypeDescriptor(EntityType(EntityType::PHANTOM, "phantom"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::ravager                    = EntityTypeDescriptor(EntityType(EntityType::RAVAGER, "ravager"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::armorStand                 = EntityTypeDescriptor(EntityType(EntityType::ARMOR_STAND, "armor_stand"), EntityCategories(EntityCategories::MOB))
, EntityTypeDescriptor::tripodCamera               = EntityTypeDescriptor(EntityType(EntityType::TRIPOD_CAMERA, "tripod_camera"), EntityCategories(EntityCategories::MOB))
, EntityTypeDescriptor::player                     = EntityTypeDescriptor(EntityType(EntityType::PLAYER, "player"), EntityCategories(EntityCategories::MOB))
, EntityTypeDescriptor::item                       = EntityTypeDescriptor(EntityType(EntityType::ITEM, "item"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::primedTnt                  = EntityTypeDescriptor(EntityType(EntityType::PRIMED_TNT, "primed_tnt"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::fallingTile                = EntityTypeDescriptor(EntityType(EntityType::FALLING_TILE, "falling_block"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::movingTile                 = EntityTypeDescriptor(EntityType(EntityType::MOVING_TILE, "moving_block"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::experiencePotion           = EntityTypeDescriptor(EntityType(EntityType::EXPERIENCE_POTION, "xp_bottle"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::experienceOrb              = EntityTypeDescriptor(EntityType(EntityType::EXPERIENCE_ORB, "xp_orb"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::eyeOfEnder                 = EntityTypeDescriptor(EntityType(EntityType::EYE_OF_ENDER, "eye_of_ender_signal"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::enderCrystal               = EntityTypeDescriptor(EntityType(EntityType::ENDER_CRYSTAL, "ender_crystal"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::fireworksRocket            = EntityTypeDescriptor(EntityType(EntityType::FIREWORKS_ROCKET, "fireworks_rocket"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::thrownTrident              = EntityTypeDescriptor(EntityType(EntityType::THROWN_TRIDENT, "thrown_trident"), EntityCategories(EntityCategories::ABSTRACT_ARROW))
, EntityTypeDescriptor::turtle                     = EntityTypeDescriptor(EntityType(EntityType::TURTLE, "turtle"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::cat                        = EntityTypeDescriptor(EntityType(EntityType::CAT, "cat"), EntityCategories(EntityCategories::TAMABLE_ANIMAL))
, EntityTypeDescriptor::shulkerBullet              = EntityTypeDescriptor(EntityType(EntityType::SHULKER_BULLET, "shulker_bullet"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::fishingHook                = EntityTypeDescriptor(EntityType(EntityType::FISHING_HOOK, "fishing_hook"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::chalkboard                 = EntityTypeDescriptor(EntityType(EntityType::CHALKBOARD, "chalkboard"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::dragonFireball             = EntityTypeDescriptor(EntityType(EntityType::DRAGON_FIREBALL, "dragon_fireball"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::arrow                      = EntityTypeDescriptor(EntityType(EntityType::ARROW, "arrow"), EntityCategories(EntityCategories::ABSTRACT_ARROW))
, EntityTypeDescriptor::snowball                   = EntityTypeDescriptor(EntityType(EntityType::SNOWBALL, "snowball"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::thrownEgg                  = EntityTypeDescriptor(EntityType(EntityType::THROWN_EGG, "egg"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::painting                   = EntityTypeDescriptor(EntityType(EntityType::PAINTING, "painting"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::minecartRideable           = EntityTypeDescriptor(EntityType(EntityType::MINECART_RIDEABLE, "minecart"), EntityCategories(EntityCategories::MINECART))
, EntityTypeDescriptor::largeFireball              = EntityTypeDescriptor(EntityType(EntityType::LARGE_FIREBALL, "fireball"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::splashPotion               = EntityTypeDescriptor(EntityType(EntityType::SPLASH_POTION, "splash_potion"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::enderPearl                 = EntityTypeDescriptor(EntityType(EntityType::ENDER_PEARL, "ender_pearl"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::leashKnot                  = EntityTypeDescriptor(EntityType(EntityType::LEASH_KNOT, "leash_knot"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::witherSkull                = EntityTypeDescriptor(EntityType(EntityType::WITHER_SKULL, "wither_skull"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::boat                       = EntityTypeDescriptor(EntityType(EntityType::BOAT, "boat"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::witherSkullDangerous       = EntityTypeDescriptor(EntityType(EntityType::WITHER_SKULL_DANGEROUS, "wither_skull_dangerous"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::itemFrame                  = EntityTypeDescriptor(EntityType(EntityType::ITEM_FRAME, "item_frame"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::lightningBolt              = EntityTypeDescriptor(EntityType(EntityType::LIGHTNING_BOLT, "lightning_bolt"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::smallFireball              = EntityTypeDescriptor(EntityType(EntityType::SMALL_FIREBALL, "small_fireball"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::areaEffectCloud            = EntityTypeDescriptor(EntityType(EntityType::AREA_EFFECT_CLOUD, "area_effect_cloud"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::minecartHopper             = EntityTypeDescriptor(EntityType(EntityType::MINECART_HOPPER, "hopper_minecart"), EntityCategories(EntityCategories::MINECART))
, EntityTypeDescriptor::minecartTnt                = EntityTypeDescriptor(EntityType(EntityType::MINECART_TNT, "tnt_minecart"), EntityCategories(EntityCategories::MINECART))
, EntityTypeDescriptor::minecartChest              = EntityTypeDescriptor(EntityType(EntityType::MINECART_CHEST, "chest_minecart"), EntityCategories(EntityCategories::MINECART))
, EntityTypeDescriptor::minecartCommandBlock       = EntityTypeDescriptor(EntityType(EntityType::MINECART_COMMAND_BLOCK, "command_block_minecart"), EntityCategories(EntityCategories::MINECART))
, EntityTypeDescriptor::lingeringPotion            = EntityTypeDescriptor(EntityType(EntityType::LINGERING_POTION, "lingering_potion"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::llamaSpit                  = EntityTypeDescriptor(EntityType(EntityType::LLAMA_SPIT, "llama_spit"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::evocationFang              = EntityTypeDescriptor(EntityType(EntityType::EVOCATION_FANG, "evocation_fang"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::evocationIllager           = EntityTypeDescriptor(EntityType(EntityType::EVOCATION_ILLAGER, "evocation_illager"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::vex                        = EntityTypeDescriptor(EntityType(EntityType::VEX, "vex"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::iceBomb                    = EntityTypeDescriptor(EntityType(EntityType::ICE_BOMB, "ice_bomb"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::balloon                    = EntityTypeDescriptor(EntityType(EntityType::BALLOON, "balloon"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::pufferfish                 = EntityTypeDescriptor(EntityType(EntityType::PUFFERFISH, "pufferfish"), EntityCategories(EntityCategories::WATER_ANIMAL))
, EntityTypeDescriptor::salmon                     = EntityTypeDescriptor(EntityType(EntityType::SALMON, "salmon"), EntityCategories(EntityCategories::WATER_ANIMAL))
, EntityTypeDescriptor::drowned                    = EntityTypeDescriptor(EntityType(EntityType::DROWNED, "drowned"), EntityCategories(EntityCategories::ZOMBIE_MONSTER))
, EntityTypeDescriptor::tropicalFish               = EntityTypeDescriptor(EntityType(EntityType::TROPICAL_FISH, "tropicalfish"), EntityCategories(EntityCategories::WATER_ANIMAL))
, EntityTypeDescriptor::cod                        = EntityTypeDescriptor(EntityType(EntityType::COD, "cod"), EntityCategories(EntityCategories::WATER_ANIMAL))
, EntityTypeDescriptor::panda                      = EntityTypeDescriptor(EntityType(EntityType::PANDA, "panda"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::pillager                   = EntityTypeDescriptor(EntityType(EntityType::PILLAGER, "pillager"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::villagerV2                 = EntityTypeDescriptor(EntityType(EntityType::VILLAGER_V2, "villager_v2"), EntityCategories(EntityCategories::VILLAGER_BASE))
, EntityTypeDescriptor::zombieVillagerV2           = EntityTypeDescriptor(EntityType(EntityType::ZOMBIE_VILLAGER_V2, "zombie_villager_v2"), EntityCategories(EntityCategories::ZOMBIE_MONSTER))
, EntityTypeDescriptor::shield                     = EntityTypeDescriptor(EntityType(EntityType::SHIELD, "shield"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::wanderingTrader            = EntityTypeDescriptor(EntityType(EntityType::WANDERING_TRADER, "wandering_trader"), EntityCategories(EntityCategories::PATHFINDER_MOB))
, EntityTypeDescriptor::elderGuardianGhost         = EntityTypeDescriptor(EntityType(EntityType::ELDER_GUARDIAN_GHOST, "elder_guardian_ghost"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::fox                        = EntityTypeDescriptor(EntityType(EntityType::FOX, "fox"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::bee                        = EntityTypeDescriptor(EntityType(EntityType::BEE, "bee"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::piglin                     = EntityTypeDescriptor(EntityType(EntityType::PIGLIN, "piglin"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::hoglin                     = EntityTypeDescriptor(EntityType(EntityType::HOGLIN, "hoglin"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::strider                    = EntityTypeDescriptor(EntityType(EntityType::STRIDER, "strider"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::zoglin                     = EntityTypeDescriptor(EntityType(EntityType::ZOGLIN, "zoglin"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::piglinBrute                = EntityTypeDescriptor(EntityType(EntityType::PIGLIN_BRUTE, "piglin_brute"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::goat                       = EntityTypeDescriptor(EntityType(EntityType::GOAT, "goat"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::glowSquid                  = EntityTypeDescriptor(EntityType(EntityType::GLOW_SQUID, "glow_squid"), EntityCategories(EntityCategories::WATER_ANIMAL))
, EntityTypeDescriptor::axolotl                    = EntityTypeDescriptor(EntityType(EntityType::AXOLOTL, "axolotl"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::warden                     = EntityTypeDescriptor(EntityType(EntityType::WARDEN, "warden"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::frog                       = EntityTypeDescriptor(EntityType(EntityType::FROG, "frog"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::tadpole                    = EntityTypeDescriptor(EntityType(EntityType::TADPOLE, "tadpole"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::allay                      = EntityTypeDescriptor(EntityType(EntityType::ALLAY, "allay"), EntityCategories(EntityCategories::MOB))
, EntityTypeDescriptor::firefly                    = EntityTypeDescriptor(EntityType(EntityType::FIREFLY, "firefly"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::camel                      = EntityTypeDescriptor(EntityType(EntityType::CAMEL, "camel"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::sniffer                    = EntityTypeDescriptor(EntityType(EntityType::SNIFFER, "sniffer"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::breeze                     = EntityTypeDescriptor(EntityType(EntityType::BREEZE, "breeze"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::breezeWindChargeProjectile = EntityTypeDescriptor(EntityType(EntityType::BREEZE_WIND_CHARGE_PROJECTILE, "breeze_wind_charge_projectile"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::armadillo                  = EntityTypeDescriptor(EntityType(EntityType::ARMADILLO, "armadillo"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::windChargeProjectile       = EntityTypeDescriptor(EntityType(EntityType::WIND_CHARGE_PROJECTILE, "wind_charge_projectile"), EntityCategories(EntityCategories::PROJECTILE))
, EntityTypeDescriptor::bogged                     = EntityTypeDescriptor(EntityType(EntityType::BOGGED, "bogged"), EntityCategories(EntityCategories::SKELETON_MONSTER))
, EntityTypeDescriptor::ominousItemSpawner         = EntityTypeDescriptor(EntityType(EntityType::OMINOUS_ITEM_SPAWNER, "ominous_item_spawner"), EntityCategories(EntityCategories::ENTITY))
, EntityTypeDescriptor::creaking                   = EntityTypeDescriptor(EntityType(EntityType::CREAKING, "creaking"), EntityCategories(EntityCategories::MONSTER))
, EntityTypeDescriptor::traderLlama                = EntityTypeDescriptor(EntityType(EntityType::TRADER_LLAMA, "trader_llama"), EntityCategories(EntityCategories::ANIMAL))
, EntityTypeDescriptor::chestBoat                  = EntityTypeDescriptor(EntityType(EntityType::CHEST_BOAT, "chest_boat"), EntityCategories(EntityCategories::ENTITY))
;