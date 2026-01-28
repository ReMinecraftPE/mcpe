#include "client/gui/screens/inventory/ContainerScreen.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/Lighting.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"
#include "renderer/ShaderConstants.hpp"

ContainerScreen::ContainerScreen(ContainerMenu* menu) :
    m_pMenu(menu),
    m_imageWidth(176),
    m_imageHeight(166),
    m_leftPos(0),
    m_topPos(0)
{

}

void ContainerScreen::init()
{
    Screen::init();
    m_pMinecraft->m_pLocalPlayer->m_pContainerMenu = m_pMenu;
    m_leftPos = (m_width - m_imageWidth) / 2;
    m_topPos = (m_height - m_imageHeight) / 2;
}

void ContainerScreen::render(float partialTicks)
{
    renderBackground();
    renderBg(partialTicks);

    MatrixStack::Ref matrix = MatrixStack::World.push();
    matrix->translate(Vec3(m_leftPos, m_topPos, 0.0f));
    currentShaderColor = Color::WHITE;

    Slot* hoveredSlot = nullptr;

    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        Slot* slot = *it;
        renderSlot(slot);
        if (isHovering(slot, m_menuPointer.x, m_menuPointer.y))
        {
            hoveredSlot = slot;
            int slotX = slot->m_x;
            int slotY = slot->m_y;
            fillGradient(slotX, slotY, slotX + 16, slotY + 16, -2130706433, -2130706433);
        }
    }

    Inventory* inv = m_pMinecraft->m_pLocalPlayer->m_pInventory;
    if (!inv->getCarried().isEmpty())
    {
        matrix->translate(Vec3(0.0f, 0.0f, 200.0f));
        ItemRenderer::singleton().renderGuiItem(m_pFont, m_pMinecraft->m_pTextures, inv->getCarried(), m_menuPointer.x - m_leftPos - 8, m_menuPointer.y - m_topPos - 8, true);
        ItemRenderer::singleton().renderGuiItemOverlay(m_pFont, m_pMinecraft->m_pTextures, inv->getCarried(), m_menuPointer.x - m_leftPos - 8, m_menuPointer.y - m_topPos - 8);
    }

    Lighting::turnOff();
    renderLabels();

    if (!inv->getCarried() && hoveredSlot && hoveredSlot->hasItem())
    {
        std::string name = Language::getInstance()->get(hoveredSlot->getItem().getDescriptionId() + ".name");
        if (!name.empty())
        {
            int tx = m_menuPointer.x - m_leftPos + 12;
            int ty = m_menuPointer.y - m_topPos - 12;
            int w = m_pFont->width(name);
            fillGradient(tx - 3, ty - 3, tx + w + 3, ty + 8 + 3, -1073741824, -1073741824);
            m_pFont->drawShadow(name, tx, ty, -1);
        }
    }

    matrix.release();
    Screen::render(partialTicks);
}

void ContainerScreen::pointerPressed(int mouseX, int mouseY, MouseButtonType button)
{
    Screen::pointerPressed(mouseX, mouseY, button);
    if (m_pMinecraft->isTouchscreen()) return;
    slotClicked(mouseX, mouseY, button);
}

void ContainerScreen::pointerReleased(int mouseX, int mouseY, MouseButtonType button)
{
    Screen::pointerReleased(mouseX, mouseY, button);
    if (m_pMinecraft->isTouchscreen() && m_timeSlotDragged < 5)
        slotClicked(mouseX, mouseY, button);
    m_timeSlotDragged = 0;
}

void ContainerScreen::handlePointerPressed(bool isPressed)
{
    Screen::handlePointerPressed(isPressed);
    if (isPressed && findSlot(m_menuPointer.x, m_menuPointer.y))
        m_timeSlotDragged++;
    else m_timeSlotDragged = 0;

    if (m_pMinecraft->isTouchscreen() && m_timeSlotDragged % 5 == 0)
    {
        slotClicked(m_menuPointer.x, m_menuPointer.y, MOUSE_BUTTON_RIGHT);
    }
}

void ContainerScreen::slotClicked(int mouseX, int mouseY, MouseButtonType button)
{
    if (button == MOUSE_BUTTON_LEFT || button == MOUSE_BUTTON_RIGHT)
    {
        Slot* slot = findSlot(mouseX, mouseY);
        bool outside = mouseX < m_leftPos || mouseY < m_topPos || mouseX >= m_leftPos + m_imageWidth || mouseY >= m_topPos + m_imageHeight;
        int index = -1;
        if (slot) index = slot->m_index;
        if (outside) index = -999;
        if (index != -1)
            slotClicked(slot, index, button, index != -999 && m_pMinecraft->m_pPlatform->shiftPressed());
    }
}

void ContainerScreen::slotClicked(Slot* slot, int index, MouseButtonType button, bool quick)
{
    m_pMinecraft->m_pGameMode->handleInventoryMouseClick(m_pMenu->m_containerId, index, button, quick, m_pMinecraft->m_pLocalPlayer);
}

void ContainerScreen::keyPressed(int keyCode)
{
    Screen::keyPressed(keyCode);
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

Slot* ContainerScreen::findSlot(int mouseX, int mouseY)
{
    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        Slot* slot = *it;
        if (isHovering(slot, mouseX, mouseY)) return slot;
    }
    return nullptr;
}

bool ContainerScreen::isHovering(Slot* slot, int mouseX, int mouseY) const
{
    mouseX -= m_leftPos;
    mouseY -= m_topPos;
    return mouseX >= slot->m_x - 1 && mouseX < slot->m_x + 17 && mouseY >= slot->m_y - 1 && mouseY < slot->m_y + 17;
}

void ContainerScreen::renderSlot(Slot* slot)
{
    int x = slot->m_x;
    int y = slot->m_y;
    ItemStack& item = slot->getItem();
    if (item.isEmpty())
    {
        int icon = slot->getNoItemIcon();
        if (icon >= 0)
        {
            m_pMinecraft->m_pTextures->loadAndBindTexture("gui/items.png");
            blit(x, y, (icon % 16) * 16, (icon / 16) * 16, 16, 16, 0, 0);
            return;
        }
    }
    ItemRenderer::singleton().renderGuiItem(m_pFont, m_pMinecraft->m_pTextures, item, x, y, true);
    ItemRenderer::singleton().renderGuiItemOverlay(m_pFont, m_pMinecraft->m_pTextures, item, x, y);
}
