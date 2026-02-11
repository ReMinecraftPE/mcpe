#pragma once

#include <string>
#include "client/gui/IntRectangle.hpp"

struct VirtualKeyboard
{
	enum Type
	{
		DEFAULT,
		EMAIL,
		PLATFORM_USERNAME,
		PHONE_NUMBER,
		IP_ADDRESS,
		NUMERIC,
		ALPHABET,
		PASSWORD
	};

	VirtualKeyboard()
	{
		type = DEFAULT;
		maxLength = 0;
	}

	Type type;
	unsigned int maxLength;
	IntRectangle rect;
	std::string defaultText;
	std::string titleText;
	std::string descriptionText;
};
