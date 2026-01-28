#pragma once

#include "client/gui/Screen.hpp"
#include "world/inventory/ContainerMenu.hpp"
#include "world/inventory/Slot.hpp"

class ContainerScreen : public Screen
{
public:
    ContainerScreen(ContainerMenu* menu);

    void init() override;
    void render(float partialTick) override;
    virtual void onClose() override;
    virtual void tick() override;
    bool isPauseScreen() override;

    virtual void slotsChanged(Container* container);
	
    virtual void pointerPressed(int mouseX, int mouseY, MouseButtonType button) override;
    virtual void pointerReleased(int mouseX, int mouseY, MouseButtonType button) override;
    virtual void handlePointerPressed(bool isPressed) override;
    virtual void slotClicked(int mouseX, int mouseY, MouseButtonType button);
    virtual void slotClicked(Slot* slot, int index, MouseButtonType button, bool quick);
    virtual void keyPressed(int key) override;

protected:
    virtual void renderLabels() = 0;
    virtual void renderBg(float partialTick) = 0;

private:
    void renderSlot(Slot* slot);
    Slot* findSlot(int mouseX, int mouseY);
    bool isHovering(Slot* slot, int mouseX, int mouseY) const;

public:
    ContainerMenu* m_pMenu;

protected:
    int m_imageWidth;
    int m_imageHeight;
    int m_leftPos;
    int m_topPos;

private:
    int m_timeSlotDragged;
};