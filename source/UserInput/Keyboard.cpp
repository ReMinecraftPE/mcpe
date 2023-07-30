#include "Keyboard.hpp"

std::vector<Keyboard::Input> Keyboard::_inputs;
int Keyboard::_index = -1;
int Keyboard::_states[256];

void Keyboard::feed(int down, int key)
{
	Input i;
	i.field_0 = down;
	i.field_4 = uint8_t(key);
	_inputs.push_back(i);

	_states[key] = down;
}
