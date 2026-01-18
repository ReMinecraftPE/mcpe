/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Screen.hpp"
#include "client/player/input/GameControllerManager.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/hal/interface/RasterizerState.hpp"

#define C_SOUND_BTN_PRESS   "random.click"
#define C_SOUND_BTN_RELEASE "random.click"

#define C_POINTER_FAST_MOVE_SPEED 0.09f
#define C_POINTER_MINIMUM_SPEED 0.04f
#define C_POINTER_DIAGONAL_SPEED 0.4f
#define C_ANGLE8 (45.0f * MTH_DEG_TO_RAD)
#define C_ANGLE16 (22.5f * MTH_DEG_TO_RAD)

Screen::Materials::Materials()
{
	MATERIAL_PTR(common, ui_cubemap);
	MATERIAL_PTR(common, ui_background);
}

bool Screen::_isPanoramaAvailable = false;

Screen::Screen()
{
	m_width = 1;
	m_height = 1;
	field_10 = false;
	m_pMinecraft = nullptr;
	m_elementListIndex = 0;
	m_elementIndex = 0;
	m_bTabWrap = true;
	m_pFont = nullptr;
	m_pClickedButton = 0;
	m_yOffset = -1;
	m_bLastPointerPressedState = false;
	m_bRenderPointer = false;
	m_lastTimeMoved = 0;
	m_cursorTick = 0;

	_addElementList();
}

Screen::~Screen()
{
	m_pClickedButton = nullptr;
	m_elements.clear();
}

bool Screen::_nextElement()
{
	if (!doElementTabbing())
		return false;

	if (!m_bTabWrap && (m_elementIndex + 1) == _getElementList().size())
		return false;

	_deselectCurrentElement();

	m_elementIndex++;
	
	if (m_bTabWrap && m_elementIndex == _getElementList().size())
	{
		m_elementIndex = 0;
	}

	_selectCurrentElement();

	return true;
}

bool Screen::_prevElement()
{
	if (!doElementTabbing())
		return false;

	if (!m_bTabWrap && m_elementIndex == 0)
		return false;

	_deselectCurrentElement();

	m_elementIndex--;

	if (m_bTabWrap && m_elementIndex == -1)
	{
		m_elementIndex = _getElementList().size() - 1;
	}

	_selectCurrentElement();

	return true;
}

void Screen::_addElement(Button& element, bool isTabbable)
{
	_addElementToList(m_elementListIndex, element, isTabbable);
}

void Screen::_addElementToList(unsigned int index, Button& element, bool isTabbable)
{
	m_elements.push_back(&element);

	if (isTabbable)
		_getElementList(index).push_back(&element);
}

bool Screen::_nextElementList()
{
	if (!m_bTabWrap && (m_elementListIndex + 1) == m_elementTabLists.size())
		return false;

	_deselectCurrentElement();

	m_elementListIndex++;

	if (m_bTabWrap && m_elementListIndex == m_elementTabLists.size())
	{
		m_elementListIndex = 0;
	}

	m_elementIndex = 0;

	_selectCurrentElement();

	return true;
}

bool Screen::_prevElementList()
{
	if (!m_bTabWrap && m_elementListIndex == 0)
		return false;

	_deselectCurrentElement();

	m_elementListIndex--;

	if (m_bTabWrap && m_elementListIndex == -1)
	{
		m_elementListIndex = m_elementTabLists.size() - 1;
	}

	m_elementIndex = 0;

	_selectCurrentElement();

	return true;
}

void Screen::_addElementList()
{
	m_elementTabLists.push_back(GuiElementList());
}

void Screen::_selectCurrentElement()
{
	GuiElement* element = _getSelectedElement();
	if (element)
		element->setSelected(true);
}

void Screen::_deselectCurrentElement()
{
	GuiElement* element = _getSelectedElement();
	if (element)
		element->setSelected(false);
}

void Screen::_renderPointer()
{
	GameRenderer& gameRenderer = *m_pMinecraft->m_pGameRenderer;
	gameRenderer.renderPointer(m_menuPointer);
}

GuiElement* Screen::_getInternalElement(unsigned int index)
{
	GuiElementList& list = m_elements;
	if (index >= list.size())
		return nullptr;

	return list[index];
}

GuiElement* Screen::_getElement(unsigned int index)
{
	GuiElementList& list = _getElementList();
	if (index >= list.size())
		return nullptr;

	return list[index];
}

GuiElement* Screen::_getSelectedElement()
{
	return _getElement(m_elementIndex);
}

GuiElementList& Screen::_getElementList(unsigned int index)
{
	return m_elementTabLists[index];
}

GuiElementList& Screen::_getElementList()
{
	return _getElementList(m_elementListIndex);
}

bool Screen::_useController() const
{
	return m_pMinecraft->useController();
}

void Screen::init(Minecraft* pMinecraft, int a3, int a4)
{
	m_width  = a3;
	m_height = a4;
	m_pMinecraft = pMinecraft;
	m_pFont = pMinecraft->m_pFont;

	// Center the menu pointer
	m_menuPointer.x = m_width / 2;
	m_menuPointer.y = m_height / 2;

	init();
	_updateTabButtonSelection();
}

void Screen::keyPressed(int key)
{
	if (m_pMinecraft->getOptions()->isKey(KM_MENU_CANCEL, key))
	{
		m_pMinecraft->handleBack(false);
	}

	if (doElementTabbing())
	{
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_DOWN, key))
		{
			_nextElement();
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_UP, key))
		{
			_prevElement();
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_LEFT, key))
		{
			_prevTab();
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_RIGHT, key))
		{
			_nextTab();
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_OK, key))
		{
			GuiElement* element = _getSelectedElement();
			if (element && element->isEnabled())
			{
				if (element->getType() == GuiElement::TYPE_BUTTON)
				{
					Button* button = (Button*)element;
					m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_BTN_PRESS);
					_buttonClicked(button);
				}
			}
		}

		// Calling this every time we either go left or right was insanely dumb
		//_updateTabButtonSelection();
	}
	
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->keyPressed(key);
	}
}

void Screen::keyboardNewChar(char chr)
{
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->charPressed(chr);
	}
}

void Screen::keyboardTextPaste(const std::string& text)
{
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->pasteText(text);
	}
}

static const char* g_panoramaList[] =
{
	"gui/background/panorama_0.png",
	"gui/background/panorama_1.png",
	"gui/background/panorama_2.png",
	"gui/background/panorama_3.png",
	"gui/background/panorama_4.png",
	"gui/background/panorama_5.png",
};

static float g_panoramaAngle = 0.0f;

void Screen::renderMenuBackground(float f)
{
	if (!m_pMinecraft->getOptions()->m_bMenuPanorama)
	{
		renderDirtBackground(0);
		return;
	}

	Textures* pTextures = m_pMinecraft->m_pTextures;

	g_panoramaAngle += float(30.0 * m_pMinecraft->m_fDeltaTime);

	float aspectRatio = 1.0f;
	//aspectRatio = float(m_width) / float(m_height);

	// @HAL: this should be using ui_cubemap, but for whatever reason we need to disable culling
	mce::MaterialPtr& materialPtr = m_screenMaterials.ui_background;

	{
		MatrixStack::Ref projMtx = MatrixStack::Projection.pushIdentity();
		projMtx->setPerspective(120.0f, aspectRatio, 0.05f, 10.0f);

		MatrixStack::Ref viewMtx = MatrixStack::View.pushIdentity();
		MatrixStack::Ref worldMtx = MatrixStack::World.push();
		currentShaderColor = Color::WHITE;
		worldMtx->rotate(180.0f, Vec3::UNIT_X);
		worldMtx->rotate(Mth::sin((f + g_panoramaAngle) / 400.0f) * 25.0f + 20.0f, Vec3::UNIT_X);
		worldMtx->rotate(-0.1f * (f + g_panoramaAngle), Vec3::UNIT_Y);

		for (int i = 0; i < 6; i++)
		{
			MatrixStack::Ref mtx = MatrixStack::World.push();

			float ang = 0.0f;
			Vec2 axis;
			switch (i)
			{
			case 1:
				ang = 90.0f;
				axis = Vec2::UNIT_Y;
				break;
			case 2:
				ang = 180.0f;
				axis = Vec2::UNIT_Y;
				break;
			case 3:
				ang = -90.0f;
				axis = Vec2::UNIT_Y;
				break;
			case 4:
				ang = 90.0f;
				axis = Vec2::UNIT_X;
				break;
			case 5:
				ang = -90.0f;
				axis = Vec2::UNIT_X;
				break;
			default:
				goto skip_rotate;
			}

			mtx->rotate(ang, Vec3(axis.x, axis.y, 0.0f));

		skip_rotate:
			pTextures->setSmoothing(true);
			pTextures->setClampToEdge(true);
			pTextures->loadAndBindTexture(std::string(g_panoramaList[i]));
			pTextures->setSmoothing(false);
			pTextures->setClampToEdge(false);

			Tesselator& t = Tesselator::instance;
			t.begin(4);
			t.vertexUV(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f);
			t.vertexUV(+1.0f, -1.0f, 1.0f, 1.0f, 0.0f);
			t.vertexUV(+1.0f, +1.0f, 1.0f, 1.0f, 1.0f);
			t.vertexUV(-1.0f, +1.0f, 1.0f, 0.0f, 1.0f);
			t.draw(materialPtr);
		}
	}

	fillGradient(0, 0, m_width, m_height, Color(0, 0, 0, 137), Color(255, 255, 255, 137));
}

void Screen::pointerPressed(int xPos, int yPos, MouseButtonType btn) // d = clicked?
{
	if (btn == MOUSE_BUTTON_NONE) return;
	
	for (unsigned int i = 0; i < unsigned int(m_elements.size()); i++)
	{
		GuiElement* element = _getInternalElement(i);
		if (element->getType() != GuiElement::TYPE_BUTTON)
			continue;

		Button* button = (Button*)element;
		if (button->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pClickedButton = button;

			if (!m_pMinecraft->isTouchscreen())
			{
				m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_BTN_PRESS);
				_buttonClicked(button);
			}
		}
	}

#ifndef ORIGINAL_CODE
	// Iterate over focused text inputs first. This is because if changing
	// focus, the previous focus must hide the OSK, before the new focus
	// shows it.
	for (int phase = 0; phase < 2; phase++)
	{
		bool handleFocused = phase == 0;
		for (int i = 0; i < int(m_textInputs.size()); i++)
		{
			TextInputBox* textInput = m_textInputs[i];
			if (textInput->hasFocus() == handleFocused)
			{
				textInput->onClick(xPos, yPos);
			}
		}
	}

#ifdef USE_NATIVE_ANDROID
	// if the keyboard is shown:
	if (m_pMinecraft->platform()->getKeyboardUpOffset())
	{
		// if there are none focused at the moment:
		bool areAnyFocused = false;
		for (int i = 0; i < int(m_textInputs.size()); i++)
		{
			TextInputBox* textInput = m_textInputs[i];
			if (textInput->m_bFocused)
			{
				areAnyFocused = true;
				break;
			}
		}

		if (!areAnyFocused)
			m_pMinecraft->platform()->hideKeyboard();
	}
#endif
#endif
}

void Screen::pointerReleased(int xPos, int yPos, MouseButtonType btn)
{
	if (btn == MOUSE_BUTTON_NONE) return;

	if (m_pClickedButton)
	{
		if (m_pMinecraft->isTouchscreen() && m_pClickedButton->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_BTN_RELEASE);
			_buttonClicked(m_pClickedButton);
		}
		m_pClickedButton->released(xPos, yPos);
		m_pClickedButton = nullptr;
	}
}

void Screen::render(float a)
{
	for (int i = 0; i < int(m_elements.size()); i++)
	{
		GuiElement* element = _getInternalElement(i);
		if (element->getType() != GuiElement::TYPE_BUTTON)
			continue;

		Button* button = (Button*)element;
		button->render(m_pMinecraft, m_menuPointer.x, m_menuPointer.y);
	}

#ifndef ORIGINAL_CODE
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->tick();
		textInput->render();
	}
#endif
}

void Screen::tick()
{
	g_panoramaAngle++;
}

void Screen::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	// not original code. Will need to re-init again
	m_elements.clear();
	m_elementTabLists.clear();
	_addElementList();
	m_textInputs.clear();
	init();
}

void Screen::onRender(float f)
{
	m_yOffset = getYOffset();

	MatrixStack::Ref matrix;

	if (m_yOffset != 0)
	{
		// push the entire screen up
		matrix = MatrixStack::World.push();
		matrix->translate(Vec3(0.0f, -m_yOffset, 0.0f));
	}

	render(f);
	if (m_bRenderPointer && _useController())
		_renderPointer();
}

int Screen::getYOffset() const
{
#ifdef USE_NATIVE_ANDROID
	int keybOffset = m_pMinecraft->platform()->getKeyboardUpOffset();
	if (!keybOffset)
		return 0;

	int offset = 0;

	// look through every text box, see if one's open
	// and determine its offset from there
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		const TextInputBox* pBox = m_textInputs[i];

		if (!pBox->m_bFocused)
			continue;
		
		int heightLeft = m_height - int(float(keybOffset) * Gui::InvGuiScale);

		// we want to keep the center of the text box in the center of the screen
		int textCenterY = pBox->m_yPos + pBox->m_height / 2;
		int scrnCenterY = heightLeft / 2;

		int diff = textCenterY - scrnCenterY;
		
		// Prevent the difference from revealing the outside of the screen.
		if (diff > m_height - heightLeft)
			diff = m_height - heightLeft;
		if (diff < 0)
			diff = 0;

		offset = diff;
		break;
	}

	return offset;
#else
	return 0;
#endif
}

bool Screen::doElementTabbing() const
{
	return !m_bRenderPointer && _useController();
}

void Screen::_processControllerDirection(GameController::StickID stickId)
{
	unsigned int time = getTimeMs();
	GameController::StickState stickState = GameControllerManager::getDirection(stickId);
	float stickPos;
	switch (stickState)
	{
	case GameController::STICK_STATE_UP:
	case GameController::STICK_STATE_DOWN:
		stickPos = GameControllerManager::getY(stickId);
		break;
	case GameController::STICK_STATE_LEFT:
	case GameController::STICK_STATE_RIGHT:
		stickPos = GameControllerManager::getX(stickId);
		break;
	default:
		stickPos = 0.0f;
		break;
	}

	if (m_lastStickState[stickId] != stickState)
	{
		_controllerDirectionChanged(stickId, stickState);
		m_lastTimeMoved = time;
	}
	else if (stickState != GameController::STICK_STATE_NONE)
	{
		if (fabsf(stickPos) >= 0.8f
			&& time - m_lastTimeMoved >= 250
			&& time - m_cursorTick > getCursorMoveThrottle())
		{
			_controllerDirectionHeld(stickId, stickState);
			m_cursorTick = time;
		}
	}

	m_lastStickState[stickId] = stickState;
}

void Screen::_controllerDirectionChanged(GameController::StickID stickId, GameController::StickState stickState)
{
	_controllerDirectionHeld(stickId, stickState);
}

void Screen::_controllerDirectionHeld(GameController::StickID stickId, GameController::StickState stickState)
{
	if (stickId == 1)
	{
		switch (stickState)
		{
		case GameController::STICK_STATE_UP:
			_prevElement();
			break;
		case GameController::STICK_STATE_DOWN:
			_nextElement();
			break;
		}
		// Calling this every time we either go left or right was insanely dumb
		//_updateTabButtonSelection();
	}
}

void Screen::_buttonClicked(Button* pButton)
{
}

void Screen::_guiElementClicked(GuiElement& element)
{
}

void Screen::_updateTabButtonSelection()
{
	if (!_useController() || m_bRenderPointer)
		return;

	for (size_t i = 0; i < _getElementList().size(); i++)
	{
		_getElement(i)->setSelected(m_elementIndex == i);
	}
}

void Screen::_nextTab()
{
}

void Screen::_prevTab()
{
}

void Screen::updateEvents()
{
	if (field_10) return;

	while (Mouse::next())
		mouseEvent();

	while (Keyboard::next())
		keyboardEvent();

	if (_useController())
	{
		checkForPointerEvent();
		controllerEvent();
	}
}

void Screen::mouseEvent()
{
	// We maintain this so we can ensure all hardware mouse inputs are properly processed, and not discarded
	MouseAction* pAction = Mouse::getEvent();
	if (pAction->isButton())
	{
		handlePointerLocation(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset());
		handlePointerPressed(Mouse::getEventButtonState());

		checkForPointerEvent();
	}
	if (pAction->_buttonType == MOUSE_BUTTON_SCROLLWHEEL)
		handleScrollWheel(Mouse::getEventButtonState() ? -1.0f : 1.0f);
}

void Screen::keyboardEvent()
{
	// Ugly hack
	if (!doElementTabbing())
	{
		if (Keyboard::getEventKeyState() && m_pMinecraft->getOptions()->isKey(KM_MENU_OK, Keyboard::getEventKey()))
		{
			m_menuPointer.isPressed = true;
		}
		else
		{
			m_menuPointer.isPressed = false;
		}
	}

	if (Keyboard::getEventKeyState())
		keyPressed(Keyboard::getEventKey());
}

void Screen::controllerEvent()
{
	_processControllerDirection(1);
	_processControllerDirection(2);

	// @TODO: this probably shouldn't be here
	GameController::StickEvent event;
	event.id = 1;
	event.state = GameControllerManager::getDirection(1);
	if (event.state != GameController::STICK_STATE_NONE)
	{
		event.x = GameControllerManager::getX(1);
		event.y = GameControllerManager::getY(1);
		handleControllerStickEvent(event);
	}
}

void Screen::checkForPointerEvent()
{
	if (m_menuPointer.isPressed == m_bLastPointerPressedState)
		return;

	handlePointerAction(m_menuPointer);
	m_bLastPointerPressedState = m_menuPointer.isPressed;
}

void Screen::handlePointerLocation(MenuPointer::Unit x, MenuPointer::Unit y)
{
	m_menuPointer.x = Mth::clamp(x, 0.0f, m_width);
	m_menuPointer.y = Mth::clamp(y, 0.0f, m_height);
}

void Screen::handlePointerPressed(bool isPressed)
{
	m_menuPointer.isPressed = isPressed;
}

void Screen::handlePointerAction(const MenuPointer& pointer)
{
	if (pointer.isPressed)
	{
		// pointerPressed(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset(), Mouse::getEventButton());
		pointerPressed(pointer.x, pointer.y + getYOffset(), MOUSE_BUTTON_LEFT);
	}
	else
	{
		// pointerReleased(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset(), Mouse::getEventButton());
		pointerReleased(pointer.x, pointer.y + getYOffset(), MOUSE_BUTTON_LEFT);
	}
}

void Screen::handleScrollWheel(float force)
{
}

void Screen::handleControllerStickEvent(const GameController::StickEvent& stick)
{
	/*if (stick.state == GameController::STICK_STATE_DOWN)
	{
		handleScrollWheel(stick.y);
	}
	else if (stick.state <= GameController::STICK_STATE_UP)
	{
		MenuGamePad::setX(directionId + 1, x);
		MenuGamePad::setY(directionId + 1, y);
	}*/

	if (m_bRenderPointer && stick.id == 1)
	{
		// Behold WilyIcaro's magic numbers
		float baseSensitivity = m_pMinecraft->getOptions()->m_fSensitivity;
		float sensitivity = baseSensitivity * 2.0f;
		float moveSensitivity = baseSensitivity / 2.0f; // Legacy4J's was * 0.5f
		//float affectY = Mth::clamp((sensitivity - 0.4f) * 1.67f, 0, 1);

		Vec2 stickPos(stick.x, stick.y);
		Vec2 stickAbs(Mth::abs(stickPos.x), Mth::abs(stickPos.y));
		float deltaLength = stickPos.length() * sensitivity;

		float angle = Mth::atan2(stickPos.y, stickPos.x);
		float snapAngle = deltaLength > C_POINTER_FAST_MOVE_SPEED ?
			Mth::floor(angle / C_ANGLE16) * C_ANGLE16 :
			Mth::floor(angle / C_ANGLE8) * C_ANGLE8;
		Vec2 snap(Mth::cos(snapAngle) * stickAbs.x, Mth::sin(snapAngle) * stickAbs.y);
		float speed = Mth::Lerp(Mth::Max(Mth::abs(snap.x) + Mth::abs(snap.y) - 1, 0.0f) * 1.41f, 1, C_POINTER_DIAGONAL_SPEED);

		//float speedY = Mth::Lerp(affectY * stickAbs.y, 0.5f, 1);

		Vec2 move(snap * speed * moveSensitivity);
		move.x = stickAbs.x < C_POINTER_MINIMUM_SPEED ? C_POINTER_MINIMUM_SPEED * Mth::signum(move.x) : move.x;
		move.y = stickAbs.y < C_POINTER_MINIMUM_SPEED ? C_POINTER_MINIMUM_SPEED * Mth::signum(move.y) : move.y; // * speedY;
		move *= m_height / Gui::InvGuiScale;

		float scale = Gui::InvGuiScale;
		handlePointerLocation(m_menuPointer.x + (move.x * scale), m_menuPointer.y - (move.y * scale));
	}
}

void Screen::renderBackground(int vo)
{
	if (m_pMinecraft->isLevelGenerated())
	{
		// draw the background offset by the Y offset so that the smaller virtual
		// keyboards don't reveal undrawn areas
		fillGradient(0, m_yOffset, m_width, m_height, Color(16, 16, 16, 192), Color(16, 16, 16, 208)); // 0xC0101010, 0xD0101010
	}
	else
	{
		renderDirtBackground(vo);
	}
}

void Screen::renderBackground()
{
	renderBackground(0);
}

void Screen::renderDirtBackground(int vo)
{
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");
	currentShaderColor = Color::WHITE;

	Tesselator& t = Tesselator::instance;
	t.begin(4);
	//t.setOffset(0, m_yOffset, 0);
	t.color(0x404040);
	t.vertexUV(0.0f,           float(m_height), 0, 0,                      float(vo) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), float(m_height), 0, float(m_width) / 32.0f, float(vo) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), 0,               0, float(m_width) / 32.0f, float(vo) + 0.0f);
	t.vertexUV(0.0f,           0,               0, 0,                      float(vo) + 0.0f);
	//t.setOffset(0, 0, 0);
	t.draw(m_materials.ui_texture_and_color);
}
