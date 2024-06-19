#include "ControllerMoveInput.hpp"
#include "Controller.hpp"
#include "world/entity/Player.hpp"

ControllerMoveInput::ControllerMoveInput(Options *options)
	: KeyboardInput(options)
{
	field_20 = false;
	field_21 = false;
}

void ControllerMoveInput::tick(Player* player)
{
    KeyboardInput::tick(player);
    if (Controller::isTouched(1))
    {
        float x = Controller::getX(1)/* ^ 0x80000000*/;
        if (fabsf(x) <= 0.1f)
            x = 0.0f;
        float y = Controller::getY(1);
        if (fabsf(y) <= 0.1f)
            y = 0.0f;
        m_horzInput = -x; // Not sure if it's an SDL2 thing, but I have to invert this to correct the movement
        m_vertInput = -y;
        /*if (*(player + 3169) && this->m_keys[7])
        {
            this->m_keys[INPUT_FORWARD] = m_vertInput > 0.0f;
            this->m_keys[INPUT_BACKWARD] = m_vertInput < 0.0f;
            goto LABEL_3;
        }*/
        goto LABEL_8;
    }
    /*if (*(player + 3169))
    {*/
    LABEL_8:
        m_keys[INPUT_FORWARD] = field_20;
        m_keys[INPUT_BACKWARD] = field_21;
    //}
LABEL_3:
    if (player->isInWater() && Controller::isTouched(1))
        m_bJumpButton = 1;
    m_keys[INPUT_LEFT] = m_keys[INPUT_FORWARD] || m_keys[INPUT_BACKWARD];
    if (m_keys[INPUT_LEFT])
        m_vertInput = 0.0;
}

void ControllerMoveInput::setKey(int keyCode, bool state)
{
	KeyboardInput::setKey(keyCode, state);
	//this->field_20 = m_pOptions[36] == keyCode && state;
	//this->field_21 = m_pOptions[34] == keyCode && state;
}

void ControllerMoveInput::releaseAllKeys()
{
	KeyboardInput::releaseAllKeys();
	field_21 = 0;
	field_20 = 0;
	m_keys[INPUT_LEFT] = false;
}