/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Screen.hpp"
#include "client/app/Minecraft.hpp"
#include "client/gui/components/Button.hpp"
#include "client/player/input/GameControllerManager.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/hal/interface/RasterizerState.hpp"

#define C_SOUND_BTN_CLICK   "random.click"
#define C_SOUND_BTN_RELEASE "random.click"

#define C_POINTER_FAST_MOVE_SPEED 0.09f
#define C_POINTER_MINIMUM_SPEED 0.04f
#define C_POINTER_DIAGONAL_SPEED 0.4f
#define C_POINTER_FRICTION 1500.0f
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
	m_bLastPointerPressedState = false;
	m_width = 1;
	m_height = 1;
	m_bPassEvents = false;
	m_bDeletePrevious = true;
	m_pMinecraft = nullptr;
	m_pSelectedElement = nullptr;
	m_pFont = nullptr;
	m_pClickedElement = 0;
	m_yOffset = -1;
	m_bRenderPointer = false;
	m_lastTimeMoved = 0;
	m_cursorTick = 0;
	m_uiTheme = UI_POCKET;
}

Screen::~Screen()
{
	m_pClickedElement = nullptr;
	m_elements.clear();
}

void Screen::controllerEvent(GameController::StickID stickID, double deltaTime)
{
	// @TODO: this probably shouldn't be here
	GameController::StickEvent event;
	event.id = stickID;
	event.state = GameControllerManager::getDirection(stickID);
	if (event.state != GameController::STICK_STATE_NONE)
	{
		event.x = GameControllerManager::getX(stickID);
		event.y = GameControllerManager::getY(stickID);
		handleControllerStickEvent(event, deltaTime);
	}
}

void Screen::_addElement(GuiElement& element, bool navigable)
{
	element.setId(m_elements.size());
	element.setNavigable(navigable);
	m_elements.push_back(&element);

	element.m_uiTheme = m_uiTheme;
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
	{
		element->setSelected(false);
		element->setFocused(false);
	}
}

void Screen::_playSelectSound()
{
	float pitch = 1.3f; // pitched based on how TU0 sounds
	// Randomization from Legacy4J
	pitch += (Mth::random() - 0.5f) / 10;
	m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_UI_FOCUS, 1.0f, pitch);
}

void Screen::initMenuPointer()
{
	handlePointerLocation(m_width / 2, m_height / 2);
}

void Screen::_renderPointer()
{
	GameRenderer& gameRenderer = *m_pMinecraft->m_pGameRenderer;
	gameRenderer.renderPointer(m_menuPointer);
}

GuiElement* Screen::_getElement(GuiElement::ID id)
{
	GuiElementList& list = m_elements;
	if (id >= list.size())
		return nullptr;

	return list[id];
}

GuiElement* Screen::_getSelectedElement()
{
	return m_pSelectedElement;
}

bool Screen::_useController() const
{
	return m_pMinecraft->useController();
}

void Screen::init(Minecraft* pMinecraft, int width, int height)
{
	m_pMinecraft = pMinecraft;
	m_pFont = pMinecraft->m_pFont;

	setSize(width, height);
	initMenuPointer();
	_updateTabButtonSelection();
}

void Screen::keyPressed(int key)
{
	Options& options = *m_pMinecraft->getOptions();
	GuiElement* element = _getSelectedElement();

	if (options.isKey(KM_MENU_CANCEL, key))
	{
		m_pMinecraft->handleBack(false);
	}

	if (m_pMinecraft->getOptions()->isKey(KM_MENU_TAB_LEFT, key))
	{
		prevTab();
	}
	if (m_pMinecraft->getOptions()->isKey(KM_MENU_TAB_RIGHT, key))
	{
		nextTab();
	}

	if (doElementTabbing())
	{
		if (options.isKey(KM_MENU_DOWN, key))
		{
			_areaNavigation(AreaNavigation::DOWN);
		}
		else if (options.isKey(KM_MENU_UP, key))
		{
			_areaNavigation(AreaNavigation::UP);
		}
		else if (options.isKey(KM_MENU_RIGHT, key))
		{
			_areaNavigation(AreaNavigation::RIGHT);
		}
		else if (options.isKey(KM_MENU_LEFT, key))
		{
			_areaNavigation(AreaNavigation::LEFT);
		}
		else if (options.isKey(KM_MENU_OK, key))
		{
			if (element && element->isEnabled())
			{
				m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_UI_PRESS);
				element->pressed(m_pMinecraft);
				if (element->getType() == GuiElement::TYPE_BUTTON)
					_buttonClicked((Button*)element);
			}
		}

		// Calling this every time we either go left or right was insanely dumb
		//_updateTabButtonSelection();
	}
	
	if (element && element->isEnabled())
	{
		element->handleButtonPress(m_pMinecraft, key);
	}
}

void Screen::handleTextChar(char chr)
{
	GuiElement* element = _getSelectedElement();
	if (element && element->isEnabled())
	{
		element->handleTextChar(m_pMinecraft, chr);
	}
}

void Screen::keyboardTextPaste(const std::string& text)
{
	GuiElement* element = _getSelectedElement();
	if (element && element->isEnabled())
	{
		element->handleClipboardPaste(text);
	}
}

float Screen::getScale(int width, int height)
{
	return m_uiTheme == UI_CONSOLE ? getConsoleScale(height) : 0.0f;
}

float Screen::getConsoleScale(int height)
{
	return 1 / float(Mth::round((Mth::round(height / 180.0f) * 180) / 360.0f) / 2.0f);
void Screen::setTextboxText(const std::string& text)
{
	GuiElement* element = _getSelectedElement();
	if (element && element->isEnabled())
	{
		element->setTextboxText(text);
		element->setFocused(false);
	}
}

void Screen::handleKeyboardClosed()
{
	GuiElement* element = _getSelectedElement();
	if (element && element->isEnabled())
	{
		element->setFocused(false);
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
	if (m_uiTheme == UI_CONSOLE)
	{
		renderConsolePanorama();
		return;
	}

	if (!m_pMinecraft->getOptions()->m_menuPanorama.get())
	{
		renderDirtBackground(0);
		return;
	}

	Textures* pTextures = m_pMinecraft->m_pTextures;

	g_panoramaAngle += float(30.0 * m_pMinecraft->m_fDeltaTime);

	float aspectRatio = 1.0f;
	//aspectRatio = float(width) / float(height);

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

void Screen::renderConsolePanorama(bool isNight)
{
	m_pMinecraft->m_pTextures->setSmoothing(true);
	blitTexture(*m_pMinecraft->m_pTextures, isNight ? "consolegui/Panorama_Background_N.png" : "consolegui/Panorama_Background_S.png", 0, 0, getTimeS() * 1000 * m_height / 360 / 66.32f, 1, m_width, m_height + 2, m_height * 820 / 144, m_height + 2);
	m_pMinecraft->m_pTextures->setSmoothing(false);
}

void Screen::renderConsolePanorama()
{
	renderConsolePanorama(m_pMinecraft->m_pLevel && !m_pMinecraft->m_pLevel->isDay());
}

void Screen::pointerPressed(const MenuPointer& pointer, MouseButtonType btn) // d = clicked?
{
	if (btn == MOUSE_BUTTON_NONE) return;
	
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		GuiElement* element = _getElement((unsigned int)i);
		element->pointerPressed(m_pMinecraft, pointer);

		if (element->isHovered(m_pMinecraft, pointer))
		{
			m_pClickedElement = element;

			if (!m_pMinecraft->isTouchscreen())
			{
				if (_useController())
					m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_UI_PRESS);
				else
					m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_BTN_CLICK);
				element->pressed(m_pMinecraft, pointer);
				if (m_pClickedElement->getType() == GuiElement::TYPE_BUTTON)
					_buttonClicked((Button*)m_pClickedElement);
			}
		}
	}

#ifndef ORIGINAL_CODE
	// @TODO: old code? why is this only doing this for Android? how does this work on iOS?
#ifdef USE_NATIVE_ANDROID
	// if the keyboard is shown:
	if (m_pMinecraft->platform()->getKeyboardUpOffset())
	{
		// if there are none focused at the moment:
		bool areAnyFocused = false;
		
		GuiElement* element = _getSelectedElement();
		if (element && element->getType() == GuiElement::TYPE_TEXTBOX)
			areAnyFocused = true;

		if (!areAnyFocused)
			m_pMinecraft->platform()->hideKeyboard(0);
	}
#endif
#endif
}

void Screen::pointerReleased(const MenuPointer& pointer, MouseButtonType btn)
{
	if (btn == MOUSE_BUTTON_NONE) return;
	
	// @TODO: call GuiElement::pointerReleased(m_pMinecraft, pointer);

	if (m_pClickedElement)
	{
		if (m_pMinecraft->isTouchscreen() && m_pClickedElement->isHovered(m_pMinecraft, pointer))
		{
			m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_BTN_RELEASE);
			m_pClickedElement->pressed(m_pMinecraft, pointer);
			if (m_pClickedElement->getType() == GuiElement::TYPE_BUTTON)
				_buttonClicked((Button*)m_pClickedElement);
		}
		m_pClickedElement->released(pointer);
		m_pClickedElement = nullptr;
	}
}

void Screen::render(float a)
{
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		GuiElement* element = _getElement(i);
		if (!element) continue;

		element->tick(m_pMinecraft);
		element->render(m_pMinecraft, m_menuPointer);
	}
}

void Screen::tick()
{
	g_panoramaAngle++;
}

void Screen::setSize(int width, int height)
{
	if (width != m_width || height != m_height)
		handlePointerLocation(m_menuPointer.x * width / m_width, m_menuPointer.y * height / m_height);

	m_width = width;
	m_height = height;

	// not original code. Will need to re-init again
	m_elements.clear();
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

bool Screen::onBack(bool b)
{
	bool result = handleBackEvent(b);
	// Play the sound regardless, since NinecraftApp will set the current screen to null anyways
	m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_UI_BACK);
	return result;
}

bool Screen::selectElement(AreaNavigation::ID id)
{
	GuiElement* element = _getElement(id);

	if (element)
	{
		selectElement(element);
		return true;
	}

	return false;
}

void Screen::selectElement(GuiElement* element)
{
	_deselectCurrentElement();
	m_pSelectedElement = element;
	_selectCurrentElement();
}

bool Screen::_areaNavigation(AreaNavigation::Direction dir)
{
	if (!m_pSelectedElement) return false;

	if (m_pSelectedElement->areaNavigation(m_pMinecraft, dir)) return true;

	if (selectElement(Navigation(this).navigateCyclic(dir, m_pSelectedElement->m_xPos + m_pSelectedElement->m_width / 2, m_pSelectedElement->m_yPos + m_pSelectedElement->m_height / 2)))
	{
		_playSelectSound();
		return true;
	}

	return false;
}

void Screen::onClose()
{
	m_pMinecraft->setScreen(nullptr);
}

bool Screen::nextTab()
{
	bool result = _nextTab();
	if (result)
		_playSelectSound();
	return result;
}

bool Screen::prevTab()
{
	bool result = _prevTab();
	if (result)
		_playSelectSound();
	return result;
}

int Screen::getYOffset()
{
#ifdef USE_NATIVE_ANDROID
	int keybOffset = m_pMinecraft->platform()->getKeyboardUpOffset();
	if (!keybOffset)
		return 0;

	int offset = 0;

	GuiElement* element = _getSelectedElement();
	if (element && element->getType() == GuiElement::TYPE_TEXTBOX)
	{
		int heightLeft = m_height - int(float(keybOffset) * Gui::InvGuiScale);

		// we want to keep the center of the text box in the center of the screen
		int textCenterY = element->m_yPos + element->m_height / 2;
		int scrnCenterY = heightLeft / 2;

		int diff = textCenterY - scrnCenterY;

		// Prevent the difference from revealing the outside of the screen.
		if (diff > m_height - heightLeft)
			diff = m_height - heightLeft;
		if (diff < 0)
			diff = 0;

		offset = diff;
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
			_areaNavigation(AreaNavigation::UP);
			break;
		case GameController::STICK_STATE_DOWN:
			_areaNavigation(AreaNavigation::DOWN);
			break;
		case GameController::STICK_STATE_LEFT:
			_areaNavigation(AreaNavigation::LEFT);
			break;
		case GameController::STICK_STATE_RIGHT:
			_areaNavigation(AreaNavigation::RIGHT);
			break;
		default:
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

	if (!m_pSelectedElement)
	{
		selectElement(Navigation(this).navigate(AreaNavigation::DOWN, m_width / 2, 0));
	}

	for (size_t i = 0; i < m_elements.size(); i++)
	{
		GuiElement* element = m_elements[i];
		element->setFocused(element == m_pSelectedElement);
		element->setSelected(element == m_pSelectedElement);
	}
}

bool Screen::_nextTab()
{
	return false;
}

bool Screen::_prevTab()
{
	return false;
}

void Screen::updateEvents()
{
	if (m_bPassEvents) return;

	while (Mouse::next())
		mouseEvent();

	while (Keyboard::next())
		keyboardEvent();

	if (_useController())
	{
		checkForPointerEvent(MOUSE_BUTTON_LEFT);
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

		checkForPointerEvent(Mouse::getEventButton());
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

	controllerEvent(2);
}

void Screen::checkForPointerEvent(MouseButtonType button)
{
	if (m_menuPointer.isPressed == m_bLastPointerPressedState)
		return;

	handlePointerAction(m_menuPointer, button);
	m_bLastPointerPressedState = m_menuPointer.isPressed;
}

bool Screen::handleBackEvent(bool b)
{
	return false;
}

void Screen::handlePointerLocation(MenuPointer::Unit x, MenuPointer::Unit y)
{
	m_menuPointer.x = Mth::clamp(x, 0.0f, float(m_width));
	m_menuPointer.y = Mth::clamp(y, 0.0f, float(m_height));
}

void Screen::handlePointerPressed(bool isPressed)
{
	m_menuPointer.isPressed = isPressed;
}

void Screen::handlePointerAction(const MenuPointer& pointer, MouseButtonType button)
{
	if (pointer.isPressed)
	{
		// pointerPressed(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset(), Mouse::getEventButton());
		pointerPressed(MenuPointer(pointer.x, pointer.y + getYOffset()), button);
	}
	else
	{
		// pointerReleased(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset(), Mouse::getEventButton());
		pointerReleased(MenuPointer(pointer.x, pointer.y + getYOffset()), button);
	}
}

void Screen::handleScrollWheel(float force)
{
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		m_elements[i]->handleScroll(force);
	}
}

void Screen::handleControllerStickEvent(const GameController::StickEvent& stick, double deltaTime)
{
	if (m_bRenderPointer && stick.id == 1)
	{
		// Behold pizzart's magic numbers
		float baseSensitivity = m_pMinecraft->getOptions()->m_sensitivity.get();
		float sensitivity = baseSensitivity * 2.0f;
		float moveSensitivity = baseSensitivity * 2.0f;

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

		// Set up our movement vector based on our joystick input
		Vec2 targetVelocity(snap * speed * moveSensitivity);
		targetVelocity.x = stickAbs.x < C_POINTER_MINIMUM_SPEED ? C_POINTER_MINIMUM_SPEED * Mth::signum(targetVelocity.x) : targetVelocity.x;
		targetVelocity.y = stickAbs.y < C_POINTER_MINIMUM_SPEED ? C_POINTER_MINIMUM_SPEED * Mth::signum(targetVelocity.y) : targetVelocity.y; // * speedY;
		targetVelocity *= Gui::InvGuiScale;

		// Multiply by delta for smooth movement
		Vec2 move = targetVelocity * C_POINTER_FRICTION * deltaTime;

		handlePointerLocation(m_menuPointer.x + move.x, m_menuPointer.y - move.y);
	}
	else if (stick.id == 2)
	{
		if (stick.state == GameController::STICK_STATE_DOWN || stick.state == GameController::STICK_STATE_UP)
		{
			handleScrollWheel(stick.y);
		}
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
		if (m_uiTheme == UI_CONSOLE) renderConsolePanorama();
		else renderDirtBackground(vo);
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

Screen::Navigation::Navigation(Screen* screen) : m_pScreen(screen)
{
	m_area.w = screen->m_width;
	m_area.h = screen->m_height;
}

bool Screen::Navigation::next(int& x, int& y, bool cycle)
{
	while (++m_index < int(m_pScreen->m_elements.size()))
	{
		GuiElement* element = m_pScreen->m_elements[m_index];

		if (!element->isVisible() || !element->isNavigable() || !(cycle || isValid(m_index))) continue;

		x = element->m_xPos + element->m_width / 2;
		y = element->m_yPos + element->m_height / 2;

		return true;
	}

	return false;
}

bool Screen::Navigation::isValid(ID id)
{
	return !m_pScreen->m_pSelectedElement || m_pScreen->m_pSelectedElement->getId() != id;
}
