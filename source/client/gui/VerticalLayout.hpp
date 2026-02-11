#pragma once

#include "Screen.hpp"
#include "components/ConsoleScrollRenderer.hpp"

//@TODO: create an abstraction for this, like GuiElementContainer
class VerticalLayout : public GuiElement
{
public:
    VerticalLayout(Screen* screen);
    ~VerticalLayout();

    GuiElement* getElement(ID) const;
    bool isTopElement(GuiElement& element) const { return element.m_yPos == m_yPos; };
    bool isBottomElement(GuiElement& element) const { return element.m_yPos == m_bottom; };
    bool selectElementByID(ID, bool sound = true);
    void selectElement(GuiElement*);

    void init(int x, int y, int w, int h, int spacing = 8, bool cyclic = true);
    void organize();
    void clear();

    bool areaNavigation(Minecraft*, AreaNavigation::Direction) override;
    void areaNavigation(AreaNavigation::Direction);
    void setSelected(bool);
    
    bool handleScroll(bool up);
    void handleScroll(float) override;
    void updateScroll(int amount);

    void pressed(Minecraft*, const MenuPointer& pointer) override;
    void released(const MenuPointer& pointer) override;
    void pressed(Minecraft*) override;

    void render(Minecraft*, const MenuPointer&) override;

public:
    class Navigation : public AreaNavigation
    {
    public:
        Navigation(VerticalLayout*);

        bool next(int& x, int& y, bool invert) override;

        bool isValid(ID) override;
    private:
        VerticalLayout* m_pLayout;
    };

    Screen* m_pScreen;
    GuiElementList m_elements;
    GuiElement* m_pSelectedElement;
    GuiElement* m_pClickedElement;
    ConsoleScrollRenderer m_scrollRenderer;
    int m_spacing;
    bool m_bCyclic;
    ID m_scrollAmount;
    ID m_elementsOnScreen;
    bool m_bCanScrollDown;
    int m_bottom;
};