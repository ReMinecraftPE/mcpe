#pragma once

#include "Button.hpp"
#include "ImageButton.hpp"

class CraftButton : public ImageButton
{
public:
	CraftButton(int id, int x, int y, const std::string& str);
	CraftButton(int id, int x, int y, int width, int height, const std::string& str);
	CraftButton(int id);

	void init(Textures* param_1);

private:
};
