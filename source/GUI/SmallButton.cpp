#include "SmallButton.hpp"

// @NOTE: Used in the ConfirmScreen.
// I reckon this was used in the OptionsScreen as well, since the button sizes are the same.

SmallButton::SmallButton(int id, int x, int y, const std::string& str) :
	Button(id, x, y, 150, 20, str)
{
}

SmallButton::SmallButton(int id, int x, int y, int width, int height, const std::string& str) :
	Button(id, x, y, width, height, str)
{
}

SmallButton::SmallButton(int id, int x, int y, Options::Option* pOption, const std::string& str) :
	Button(id, x, y, 150, 20, str),
	m_pOption(pOption)
{
}

Options::Option* SmallButton::getOption()
{
	return m_pOption;
}
