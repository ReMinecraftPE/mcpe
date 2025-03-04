#pragma once

#include <vector>
#include <map>

#include "EntityType.hpp"
#include "EntityCategories.hpp"

class EntityTypeDescriptor
{
public:
	EntityTypeDescriptor(const EntityType&, const EntityCategories&);

public:
	const EntityType& getEntityType() const { return m_type; }
	bool isType(const EntityType& type) const { return type == getEntityType(); }
	const EntityCategories& getCategories() const { return m_categories; }
	bool hasCategory(EntityCategories::CategoriesMask categoriesMask) const { return m_categories.contains(categoriesMask); }

	static void initDescriptors();
	static void teardownDescriptors();

	static const EntityTypeDescriptor* GetByEntityTypeID(EntityType::ID);
	static const EntityTypeDescriptor* GetByEntityTypeName(EntityType::Name);

private:
	EntityType m_type;
	EntityCategories m_categories;

private:
	static std::map<EntityType::ID, const EntityTypeDescriptor*> entityTypeIdMap;
	static std::map<std::string, const EntityTypeDescriptor*> entityTypeNameMap;

public:
	static const EntityTypeDescriptor* all[];
	static const int allCount;
	static const EntityTypeDescriptor
		  unknown
		, chicken
		, cow
		, pig
		, sheep
		, wolf
		, villager
		, mushroomCow
		, squid
		, rabbit
		, bat
		, ironGolem
		, snowGolem
		, ocelot
		, horse
		, donkey
		, mule
		, skeletonHorse
		, zombieHorse
		, polarBear
		, llama
		, parrot
		, dolphin
		, zombie
		, creeper
		, skeleton
		, spider
		, pigZombie
		, slime
		, enderMan
		, silverfish
		, caveSpider
		, ghast
		, lavaSlime
		, blaze
		, zombieVillager
		, witch
		, stray
		, husk
		, witherSkeleton
		, guardian
		, elderGuardian
		, npc
		, wither
		, enderDragon
		, shulker
		, endermite
		, agent
		, vindicator
		, phantom
		, ravager
		, armorStand
		, tripodCamera
		, player
		, item
		, primedTnt
		, fallingTile
		, movingTile
		, experiencePotion
		, experienceOrb
		, eyeOfEnder
		, enderCrystal
		, fireworksRocket
		, thrownTrident
		, turtle
		, cat
		, shulkerBullet
		, fishingHook
		, chalkboard
		, dragonFireball
		, arrow
		, snowball
		, thrownEgg
		, painting
		, minecartRideable
		, largeFireball
		, splashPotion
		, enderPearl
		, leashKnot
		, witherSkull
		, boat
		, witherSkullDangerous
		, itemFrame
		, lightningBolt
		, smallFireball
		, areaEffectCloud
		, minecartHopper
		, minecartTnt
		, minecartChest
		, minecartCommandBlock
		, lingeringPotion
		, llamaSpit
		, evocationFang
		, evocationIllager
		, vex
		, iceBomb
		, balloon
		, pufferfish
		, salmon
		, drowned
		, tropicalFish
		, cod
		, panda
		, pillager
		, villagerV2
		, zombieVillagerV2
		, shield
		, wanderingTrader
		, elderGuardianGhost
		, fox
		, bee
		, piglin
		, hoglin
		, strider
		, zoglin
		, piglinBrute
		, goat
		, glowSquid
		, axolotl
		, warden
		, frog
		, tadpole
		, allay
		, firefly
		, camel
		, sniffer
		, breeze
		, breezeWindChargeProjectile
		, armadillo
		, windChargeProjectile
		, bogged
		, ominousItemSpawner
		, creaking
		, traderLlama
		, chestBoat
		;
};