#include "EntityCategories.hpp"

const EntityCategories::CategoriesMask EntityCategories::maskEnums[] =
{
	ENTITY,          
	MOB,             
	PATHFINDER_MOB,  
	UNKNOWN,         
	MONSTER,         
	ANIMAL,          
	WATER_ANIMAL,    
	TAMABLE_ANIMAL,  
	AMBIENT,         
	UNDEAD_MOB,      
	ZOMBIE_MONSTER,  
	ARTHROPOD,       
	MINECART,        
	SKELETON_MONSTER,
	EQUINE_ANIMAL,   
	PROJECTILE,      
	ABSTRACT_ARROW,  
	VILLAGER_BASE
};
const int EntityCategories::maskEnumCount = sizeof(EntityCategories::maskEnums) / sizeof(const EntityCategories::CategoriesMask);

EntityCategories::EntityCategories(CategoriesMask categoriesMask)
{
	m_categoriesMask = categoriesMask;
}