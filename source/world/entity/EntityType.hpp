#pragma once
#include <string>

class EntityType
{
public:
	enum ID
	{
		UNKNOWN = 0, // Particle, CameraEntity

		// Animals
		CHICKEN = 10,
		COW,
		PIG,
		SHEEP,
		WOLF,
		VILLAGER,
		MUSHROOM_COW,
		SQUID,
		RABBIT,
		BAT,
		IRON_GOLEM,
		SNOW_GOLEM,
		OCELOT,
		HORSE,
		DONKEY,
		MULE,
		SKELETON_HORSE,
		ZOMBIE_HORSE,
		POLAR_BEAR,
		LLAMA,
		PARROT,
		DOLPHIN,

		// Monsters
		ZOMBIE = 32,
		CREEPER,
		SKELETON,
		SPIDER,
		PIG_ZOMBIE,
		SLIME,
		ENDER_MAN,
		SILVERFISH,
		CAVE_SPIDER,
		GHAST,
		LAVA_SLIME,
		BLAZE,
		ZOMBIE_VILLAGER,
		WITCH,
		STRAY,
		HUSK,
		WITHER_SKELETON,
		GUARDIAN,
		ELDER_GUARDIAN,
		NPC,
		WITHER,
		ENDER_DRAGON,
		SHULKER,
		ENDERMITE,
		AGENT,
		VINDICATOR,
		PHANTOM,
		RAVAGER,

		ARMOR_STAND = 61,
		TRIPOD_CAMERA,
		PLAYER,
		// Non-living
		ITEM = 64,
		PRIMED_TNT,
		FALLING_TILE,
		MOVING_TILE,
		EXPERIENCE_POTION,
		EXPERIENCE_ORB,
		EYE_OF_ENDER,
		ENDER_CRYSTAL,
		FIREWORKS_ROCKET,
		THROWN_TRIDENT,
		// They ran out of Animal IDs at this point
		TURTLE,
		CAT,
		SHULKER_BULLET,
		FISHING_HOOK,
		CHALKBOARD,
		DRAGON_FIREBALL,

		ARROW = 80,
		SNOWBALL,
		THROWN_EGG,
		PAINTING,
		MINECART_RIDEABLE,
		LARGE_FIREBALL,
		SPLASH_POTION,
		ENDER_PEARL,
		LEASH_KNOT,
		WITHER_SKULL,
		BOAT,
		WITHER_SKULL_DANGEROUS,
		ITEM_FRAME, // Entity in JE, TileEntity in BE
		LIGHTNING_BOLT,
		SMALL_FIREBALL,
		AREA_EFFECT_CLOUD,
		MINECART_HOPPER,
		MINECART_TNT,
		MINECART_CHEST,

		MINECART_COMMAND_BLOCK = 100,
		LINGERING_POTION,
		LLAMA_SPIT,
		EVOCATION_FANG,
		EVOCATION_ILLAGER,
		VEX,
		ICE_BOMB,
		BALLOON,
		PUFFERFISH,
		SALMON,
		DROWNED,
		TROPICAL_FISH,
		COD,
		PANDA,
		PILLAGER,
		VILLAGER_V2,
		ZOMBIE_VILLAGER_V2,
		SHIELD,
		WANDERING_TRADER,

		ELDER_GUARDIAN_GHOST = 120,
		FOX,
		BEE,
		PIGLIN,
		HOGLIN,
		STRIDER,
		ZOGLIN,
		PIGLIN_BRUTE,
		GOAT,
		GLOW_SQUID,
		AXOLOTL,
		WARDEN,
		FROG,
		TADPOLE,
		ALLAY,
		FIREFLY,

		CAMEL = 138,
		SNIFFER,
		BREEZE,
		BREEZE_WIND_CHARGE_PROJECTILE,
		ARMADILLO,
		WIND_CHARGE_PROJECTILE,
		BOGGED,
		OMINOUS_ITEM_SPAWNER,
		CREAKING,

		TRADER_LLAMA = 157,

		CHEST_BOAT = 218
	};
	typedef const std::string& Name;

public:
	EntityType(ID type = UNKNOWN, Name name = "unknown");

public:
	ID getId() const { return m_id; }
	Name getName() const { return m_name; }

	bool operator==(EntityType::ID other) const { return this->getId() == other; }
	bool operator!=(EntityType::ID other) const { return this->getId() != other; }
	bool operator==(const EntityType& other) const { return this->getId() == other.getId(); }
	bool operator!=(const EntityType& other) const { return this->getId() != other.getId(); }

private:
	ID m_id;
	std::string m_name;
};