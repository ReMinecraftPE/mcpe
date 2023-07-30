#pragma once

#include <string>

class User
{
public:
	User(const std::string& a, const std::string& b)
	{
		field_0 = a;
		field_18 = b;
	}

public:
	std::string field_0;
	std::string field_18;
};
