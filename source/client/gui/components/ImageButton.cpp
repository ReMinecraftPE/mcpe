#include "ImageButton.hpp"


ImageButton::ImageButton(int id, int x, int y, const std::string& str) :
	Button(id, x, y, str)
{
}
ImageButton::ImageButton(int id, int x, int y, int width, int height, const std::string& str) :
	Button(id, x, y, width, height, str)
{
}
ImageButton::ImageButton(int id, const std::string& str) :
	Button(id, str)
{
}
/*
ImageButton::ImageButton(int id, int param_1, const std::string& str, ImageDef *param_3) :
	Button(id, str)
{
	ImageDef::ImageDef(param_3);
}*/

/*
void ImageButton::setImageDef(ImagDef *param_1, bool param_2)
{

}*/