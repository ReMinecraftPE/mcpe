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
        float x = -Controller::getX(1);
        float y = Controller::getY(1);
        m_horzInput = x; 
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
//LABEL_3:
    if (player->isInWater() && Controller::isTouched(1))
        m_bJumping = 1;
    m_keys[INPUT_LEFT] = m_keys[INPUT_FORWARD] || m_keys[INPUT_BACKWARD];
    if (m_keys[INPUT_LEFT])
        m_vertInput = 0.0;
}

void ControllerMoveInput::setKey(int eventKey, bool eventKeyState)
{
	KeyboardInput::setKey(eventKey, eventKeyState);
	//this->m_culledEntities = m_pOptions[36] == eventKey && eventKeyState;
	//this->field_21 = m_pOptions[34] == eventKey && eventKeyState;
}

void ControllerMoveInput::releaseAllKeys()
{
	KeyboardInput::releaseAllKeys();
	field_21 = 0;
	field_20 = 0;
	m_keys[INPUT_LEFT] = false;
}