#include "CraftButton.hpp"
#include "ImageButton.hpp"
/*
CraftButton::CraftButton(int id, int x, int y, const std::string& str) :
	ImageButton(id, x, y, str)
{

}
CraftButton::CraftButton(int id, int x, int y, int width, int height, const std::string& str) :
	ImageButton(id, x, y, width, height, str)
{

}*/

CraftButton::CraftButton(int id) :
	ImageButton(id, "")
{

}

void CraftButton::init(Textures* param_1)
{
	
}