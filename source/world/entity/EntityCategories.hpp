#pragma once

class EntityCategories
{
public:
	enum CategoriesMask
	{
		ENTITY           = 0,
		MOB              = 1<<0,
		PATHFINDER_MOB   = 1<<1  | MOB,
		UNKNOWN          = 1<<2  | PATHFINDER_MOB,
		MONSTER          = 1<<3  | PATHFINDER_MOB,
		ANIMAL           = 1<<4  | PATHFINDER_MOB,
		WATER_ANIMAL     = 1<<5  | PATHFINDER_MOB,
		TAMABLE_ANIMAL   = 1<<6  | ANIMAL,
		AMBIENT          = 1<<7  | MOB,
		UNDEAD_MOB       = 1<<8  | MONSTER,
		ZOMBIE_MONSTER   = 1<<9  | UNDEAD_MOB,
		ARTHROPOD        = 1<<10 | MONSTER,
		MINECART         = 1<<11,
		SKELETON_MONSTER = 1<<12 | UNDEAD_MOB,
		EQUINE_ANIMAL    = 1<<13 | TAMABLE_ANIMAL,
		PROJECTILE       = 1<<14,
		ABSTRACT_ARROW   = 1<<15 | PROJECTILE,
		VILLAGER_BASE    = 1<<16 | PATHFINDER_MOB
	};

public:
	EntityCategories(CategoriesMask = ENTITY);

public:
	CategoriesMask getCategoryMask() const { return m_categoriesMask; }
	bool contains(CategoriesMask categories) const { return (categories & m_categoriesMask) == categories; }

	bool operator==(const EntityCategories& other) const { return this->getCategoryMask() == other.getCategoryMask(); }
	bool operator!=(const EntityCategories& other) const { return this->getCategoryMask() != other.getCategoryMask(); }
	void operator+=(const EntityCategories& other) { *this += other.getCategoryMask(); }
	bool operator==(CategoriesMask categories) const { return this->getCategoryMask() == categories; }
	bool operator!=(CategoriesMask categories) const { return this->getCategoryMask() != categories; }
	void operator+=(CategoriesMask categories) { m_categoriesMask = (CategoriesMask)(m_categoriesMask | categories); }

private:
	CategoriesMask m_categoriesMask;
};