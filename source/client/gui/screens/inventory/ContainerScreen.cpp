#include "client/gui/screens/inventory/ContainerScreen.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/Lighting.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"
#include "renderer/ShaderConstants.hpp"
#include <cfloat>

ContainerScreen::ContainerScreen(ContainerMenu* menu) :
    m_pMenu(menu),
    m_imageWidth(176),
    m_imageHeight(166),
    m_leftPos(0),
    m_topPos(0),
    m_timeSlotDragged(0)
{
    m_bRenderPointer = true;
}

void ContainerScreen::_renderSlot(Slot& slot)
{
    const SlotDisplay& display = getSlotDisplay(slot);

    if (!display.bVisible) return;

    if (display.bIconHolder)
    {
        MatrixStack::Ref matrix = MatrixStack::World.push();
        float off = 3 * display.size / 50.0f;
        matrix->translate(Vec3(-off, -off, 0.0f));
        blitSprite(*m_pMinecraft->m_pTextures, "gui/console/Graphics/IconHolder.png", display.x, display.y, display.size, display.size);
    }
    MatrixStack::Ref matrix = MatrixStack::World.push();
    matrix->translate(Vec3(display.x, display.y, 0));
    matrix->scale(display.size / 18.0f);
    ItemStack& item = slot.getItem();
    if (item.isEmpty())
    {
        const std::string& noItemSprite = display.noItemSprite;

        if (!noItemSprite.empty())
        {
            blitSprite(*m_pMinecraft->m_pTextures, noItemSprite, 0, 0, 16, 16);
            return;
        }

        int icon = display.noItemIcon;
        if (icon >= 0)
        {
            m_pMinecraft->m_pTextures->loadAndBindTexture(C_ITEMS_NAME);
            blit(0, 0, (icon % 16) * 16, (icon / 16) * 16, 16, 16, 0, 0);
        }

        return;
    }
    ItemRenderer::singleton().renderGuiItem(m_pFont, m_pMinecraft->m_pTextures, item, 0, 0, true);
    ItemRenderer::singleton().renderGuiItemOverlay(m_pFont, m_pMinecraft->m_pTextures, item, 0, 0);
}

Slot* ContainerScreen::_findSlot()
{
    return _findSlot(m_menuPointer.x, m_menuPointer.y);
}

Slot* ContainerScreen::_findSlot(int mouseX, int mouseY)
{
    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        Slot* slot = *it;
        if (_isHovering(*slot)) return slot;
    }
    return nullptr;
}

bool ContainerScreen::_isHovering(const Slot& slot) const
{
    return _isHovering(slot, m_menuPointer.x, m_menuPointer.y);
}

bool ContainerScreen::_isHovering(const Slot& slot, int mouseX, int mouseY) const
{
    const SlotDisplay& display = getSlotDisplay(slot);
    if (!display.bVisible) return false;
    mouseX -= m_leftPos;
    mouseY -= m_topPos;
    float off = 3 * display.size / 50.0f;
    return mouseX >= display.x - off && mouseX < display.x + display.size - off && mouseY >= display.y - off && mouseY < display.y + display.size - off;
}

void ContainerScreen::_playInteractSound()
{
    m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_UI_PRESS);
}

void ContainerScreen::_tryPlayInteractSound()
{
    if (_useController())
        _playInteractSound();
}

void ContainerScreen::_selectSlot(Slot* slot)
{
    if (!slot) return;
    const SlotDisplay& display = getSlotDisplay(*slot);
    if (!display.bVisible) return;
    int off = 3 * display.size / 50;
    handlePointerLocation(m_leftPos + display.x - off + display.size / 2, m_topPos + display.y - off + display.size / 2);
}

bool ContainerScreen::_selectSlotInDirection(AreaNavigation::Direction dir)
{
    AreaNavigation::ID found = SlotNavigation(this).navigateCyclic(dir, m_menuPointer.x, m_menuPointer.y);

    if (found >= 0)
    {
        _selectSlot(m_pMenu->getSlot(found));
        return true;
    }

    return false;
}

void ContainerScreen::_controllerDirectionChanged(GameController::StickID stickId, GameController::StickState stickState)
{
    Screen::_controllerDirectionChanged(stickId, stickState);
    if (stickId == 1 && stickState == GameController::STICK_STATE_NONE)
    {
        _selectSlot(_findSlot());
    }
}

void ContainerScreen::initMenuPointer()
{
    //@NOTE: Calling this as a fallback, if for some reason, there isn't a slot available
    Screen::initMenuPointer();

    if (!_useController()) return;

    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        Slot* slot = *it;
        //@NOTE: Selects the first hotbar slot
        if (slot->m_slot == 0 && m_pMinecraft->m_pLocalPlayer && slot->m_pContainer == m_pMinecraft->m_pLocalPlayer->m_pInventory)
        {
            _selectSlot(slot);
            break;
        }
    }
}

void ContainerScreen::init()
{
    Screen::init();
    m_pMinecraft->m_pLocalPlayer->m_pContainerMenu = m_pMenu;
    m_leftPos = (m_width - m_imageWidth) / 2;
    m_topPos = (m_height - m_imageHeight) / 2;

    m_slotDisplays.clear();
    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        m_slotDisplays.push_back(_createSlotDisplay(*(*it)));
    }
}

void ContainerScreen::render(float partialTicks)
{
    renderBackground();
    _renderBg(partialTicks);

    MatrixStack::Ref matrix = MatrixStack::World.push();
    matrix->translate(Vec3(m_leftPos, m_topPos, 0.0f));
    
    _renderLabels();

    currentShaderColor = Color::WHITE;

    Slot* hoveredSlot = nullptr;

    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        Slot* slot = *it;
        _renderSlot(*slot);
        if (_isHovering(*slot))
        {
            const SlotDisplay& display = getSlotDisplay(*slot);
            hoveredSlot = slot;
            //@NOTE: fillGradient is being used instead of fill, so the shader color won't be changed, I think the same happened on the original
            MatrixStack::Ref highlightMatrix = MatrixStack::World.push();
            highlightMatrix->translate(Vec3(display.x, display.y, 0));
            highlightMatrix->scale(display.size / 18.0f);
            fillGradient(0, 0, 16, 16, 0x80FFFFFF, 0x80FFFFFF);
        }
    }

    Inventory* inv = m_pMinecraft->m_pLocalPlayer->m_pInventory;
    if (!inv->getCarried().isEmpty())
    {
        matrix->translate(Vec3(0.0f, 0.0f, 200.0f));
        MatrixStack::Ref carriedMatrix = MatrixStack::World.push();
        carriedMatrix->translate(Vec3(m_menuPointer.x - m_leftPos - 8, m_menuPointer.y - m_topPos - 8, 0.0f));
        if (m_uiTheme == UI_CONSOLE)
            carriedMatrix->scale(3.0f); // 54 / 18.0f
        ItemRenderer::singleton().renderGuiItem(m_pFont, m_pMinecraft->m_pTextures, inv->getCarried(), 0, 0, true);
        ItemRenderer::singleton().renderGuiItemOverlay(m_pFont, m_pMinecraft->m_pTextures, inv->getCarried(), 0, 0);
    }

    Lighting::turnOff();

    if (!inv->getCarried() && hoveredSlot && hoveredSlot->hasItem())
    {
        std::string name = Language::get(hoveredSlot->getItem().getDescriptionId() + ".name");
        if (!name.empty())
        {
            int w = m_pFont->width(name);
            int tx = m_menuPointer.x - m_leftPos + 12;
            int ty = m_menuPointer.y - m_topPos - 12;
            if (m_uiTheme == UI_CONSOLE)
            {
                blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::POINTER_TEXT_PANEL_SLICES, tx - 6, ty - 6, w * 2 + 12, 28, 8);
                MatrixStack::Ref tooltipMatrix = MatrixStack::World.push();
                m_pFont->drawScalable(name, tx, ty, -1);
            }
            else
            {
                fillGradient(tx - 3, ty - 3, tx + w + 3, ty + 8 + 3, 0xC0000000, 0xC0000000);
                m_pFont->drawShadow(name, tx, ty, -1);
            }
        }
    }

    matrix.release();
    Screen::render(partialTicks);
}

void ContainerScreen::pointerPressed(const MenuPointer& pointer, MouseButtonType button)
{
    Screen::pointerPressed(pointer, button);
    if (m_pMinecraft->useTouchscreen()) return;
    slotClicked(pointer, button);
}

void ContainerScreen::pointerReleased(const MenuPointer& pointer, MouseButtonType button)
{
    Screen::pointerReleased(pointer, button);
    if (m_pMinecraft->useTouchscreen() && m_timeSlotDragged < 5)
        slotClicked(pointer, button);
    m_timeSlotDragged = 0;
}

void ContainerScreen::handlePointerPressed(bool isPressed)
{
    Screen::handlePointerPressed(isPressed);
    if (isPressed && _findSlot())
        m_timeSlotDragged++;
    else m_timeSlotDragged = 0;

    if (m_pMinecraft->useTouchscreen() && m_timeSlotDragged % 5 == 0)
    {
        slotClicked(m_menuPointer, MOUSE_BUTTON_RIGHT);
    }
}

void ContainerScreen::slotClicked(const MenuPointer& pointer, MouseButtonType button, bool quick)
{
    if (button == MOUSE_BUTTON_LEFT || button == MOUSE_BUTTON_RIGHT)
    {
        Slot* slot = _findSlot(pointer.x, pointer.y);
        bool outside = pointer.x < m_leftPos || pointer.y < m_topPos || pointer.x >= m_leftPos + m_imageWidth || pointer.y >= m_topPos + m_imageHeight;
        int index = -1;
        if (slot) index = slot->m_index;
        if (outside) index = -999;
        if (index != -1)
            slotClicked(slot, index, button, index != -999 && quick);
    }
}

void ContainerScreen::slotClicked(Slot* slot, int index, MouseButtonType button, bool quick)
{
    _tryPlayInteractSound();
    m_pMinecraft->m_pGameMode->handleInventoryMouseClick(m_pMenu->m_containerId, index, button, quick, m_pMinecraft->m_pLocalPlayer);
}

void ContainerScreen::slotClicked(const MenuPointer& pointer, MouseButtonType button)
{
    slotClicked(pointer, button, m_pMinecraft->m_pPlatform->shiftPressed());
}

void ContainerScreen::keyPressed(int keyCode)
{
    if (!_useController() && m_pMinecraft->getOptions()->isKey(KM_INVENTORY, keyCode))
    {
        m_pMinecraft->handleBack(false);
    }
    else if (m_pMinecraft->getOptions()->isKey(KM_CONTAINER_QUICKMOVE, keyCode) && _useController())
    {
        slotClicked(m_menuPointer, MOUSE_BUTTON_LEFT, true);
    }
    else if (m_pMinecraft->getOptions()->isKey(KM_CONTAINER_SPLIT, keyCode) && _useController())
    {
        slotClicked(m_menuPointer, MOUSE_BUTTON_RIGHT, false);
    }
    else
    {
        if (_useController() &&
                ((m_pMinecraft->getOptions()->isKey(KM_MENU_UP, keyCode) && _selectSlotInDirection(AreaNavigation::UP)) ||
                (m_pMinecraft->getOptions()->isKey(KM_MENU_DOWN, keyCode) && _selectSlotInDirection(AreaNavigation::DOWN)) ||
                (m_pMinecraft->getOptions()->isKey(KM_MENU_RIGHT, keyCode) && _selectSlotInDirection(AreaNavigation::RIGHT)) ||
                (m_pMinecraft->getOptions()->isKey(KM_MENU_LEFT, keyCode) && _selectSlotInDirection(AreaNavigation::LEFT))))
        {
            _playSelectSound();
            return;
        }

        Screen::keyPressed(keyCode);
    }
}

const SlotDisplay& ContainerScreen::getSlotDisplay(const Slot& slot) const
{
    return m_slotDisplays[slot.m_index];
}

void ContainerScreen::onClose()
{
    if (m_pMinecraft->m_pLocalPlayer)
        m_pMinecraft->m_pLocalPlayer->closeContainer();
}

void ContainerScreen::tick()
{
    Screen::tick();
    if (!m_pMinecraft->m_pLocalPlayer->isAlive() || m_pMinecraft->m_pLocalPlayer->m_bRemoved)
        m_pMinecraft->m_pLocalPlayer->closeContainer();
}

void ContainerScreen::slotsChanged(Container* container)
{
}

bool ContainerScreen::isPauseScreen()
{
    return false;
}

ContainerScreen::SlotNavigation::SlotNavigation(ContainerScreen* screen) : m_pScreen(screen)
{
    m_area.w = screen->m_width;
    m_area.h = screen->m_height;
}

bool ContainerScreen::SlotNavigation::next(int& x, int& y, bool cycle)
{
    while (++m_index < int(m_pScreen->m_slotDisplays.size()))
    {
        const SlotDisplay& display = m_pScreen->m_slotDisplays[m_index];

        if (!display.bVisible || !(cycle || isValid(m_index))) continue;

        int off = 3 * display.size / 50;
        x = m_pScreen->m_leftPos + display.x - off + display.size / 2;
        y = m_pScreen->m_topPos + display.y - off + display.size / 2;

        return true;
    }

    return false;
}

bool ContainerScreen::SlotNavigation::isValid(ID id)
{
    Slot* hovered = m_pScreen->_findSlot();
    return !hovered || hovered->m_index != id;
}
