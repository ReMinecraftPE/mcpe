#pragma once

#include "KeyboardInput.hpp"

class ControllerMoveInput : public KeyboardInput
{
private:
	bool field_20;
	bool field_21;

public:
	ControllerMoveInput(Options *options);

	void tick(Player *player) override;
	void setKey(int keyCode, bool state) override;
	void releaseAllKeys() override;
};

