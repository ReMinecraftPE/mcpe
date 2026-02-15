#include "VerticalLayout.hpp"

VerticalLayout::VerticalLayout(Screen* screen) :
	m_pScreen(screen)
	, m_pSelectedElement(nullptr)
	, m_pClickedElement(nullptr)
	, m_spacing(0)
	, m_bCyclic(false)
	, m_scrollAmount(0)
	, m_elementsOnScreen(0)
	, m_bCanScrollDown(false)
	, m_bottom(0)
{
}

VerticalLayout::~VerticalLayout()
{
	clear();
}

GuiElement* VerticalLayout::getElement(ID index) const
{
	if (index >= 0 && index < ID(m_elements.size()))
		return m_elements[index];

	return nullptr;
}

bool VerticalLayout::selectElementById(ID id, bool sound)
{
	GuiElement* element = getElement(id);
	if (element)
	{
		selectElement(element);
		if (sound)
			m_pScreen->_playSelectSound();
		return true;
	}
	return false;
}

void VerticalLayout::selectElement(GuiElement* element)
{
	if (element != m_pSelectedElement)
	{
		if (m_pSelectedElement)
			m_pSelectedElement->setSelected(false);
		m_pSelectedElement = element;
		if (m_pSelectedElement)
			m_pSelectedElement->setSelected(true);
	}
}

void VerticalLayout::init(const IntRectangle& rect, int spacing, bool cyclic)
{
	m_xPos = rect.x;
	m_yPos = rect.y;
	m_width = rect.w;
	m_height = rect.h;
	m_spacing = spacing;
	m_bCyclic = cyclic;

	m_pScreen->_addElement(*this, !m_elements.empty());

	organize();
}

void VerticalLayout::organize()
{
	m_bottom = m_yPos;
	m_bCanScrollDown = false;

	m_elementsOnScreen = 0;

	int yDiff = 0;

	for (size_t i = m_scrollAmount; i < m_elements.size(); ++i)
	{
		GuiElement* element = m_elements[i];

		if (yDiff + element->m_height > m_height)
		{
			m_bCanScrollDown = true;
			break;
		}

		element->m_uiTheme = m_pScreen->m_uiTheme;
		element->setId(ID(i));
		element->m_width = m_width;
		element->m_xPos = m_xPos;
		element->m_yPos = m_yPos + yDiff;

		if (element->m_yPos > m_bottom)
			m_bottom = element->m_yPos;

		++m_elementsOnScreen;

		yDiff += element->m_height + m_spacing;
	}

	if (isSelected() && !m_pSelectedElement && m_pScreen->_useController())
		selectElementById(0, false);
}

void VerticalLayout::clear()
{
	if (m_elements.empty()) return;

	m_pSelectedElement = nullptr;
	m_pClickedElement = nullptr;

	for (GuiElementList::iterator it = m_elements.begin(); it != m_elements.end(); it = m_elements.erase(it))
	{
		delete (*it);
	}
}

bool VerticalLayout::areaNavigation(Minecraft* mc, AreaNavigation::Direction dir)
{
	if (m_pSelectedElement && m_pSelectedElement->areaNavigation(mc, dir)) return true;

	if (dir == AreaNavigation::DOWN)
	{
		GuiElement* element = m_pSelectedElement;
		if (element && isBottomElement(*element))
		{
			if (m_bCyclic && !m_bCanScrollDown && m_scrollAmount > 0)
			{
				int x = element->m_xPos + element->m_width / 2;
				updateScroll(0);
				AreaNavigation::ID id = Navigation(this).navigate(dir, x, 0, true);
				if (id >= 0)
					selectElementById(m_scrollAmount + id);
				return true;
			}

			handleScroll(false);
			areaNavigation(dir, element == getElement(ID(m_elements.size() - 1)));
			return true;
		}
		areaNavigation(dir);
		return true;
	}
	else if (dir == AreaNavigation::UP)
	{
		GuiElement* element = m_pSelectedElement;
		if (element && isTopElement(*element))
		{
			if (m_bCyclic && !m_scrollAmount)
			{
				int x = element->m_xPos + element->m_width / 2;
				while (m_bCanScrollDown)
				{
					updateScroll(m_scrollAmount + 1);
				}
				AreaNavigation::ID id = Navigation(this).navigate(dir, x, 0, true);
				if (id >= 0)
					selectElementById(m_scrollAmount + id);
				return true;
			}

			handleScroll(true);
		}
		areaNavigation(dir);
		return true;
	}

	return false;
}

void VerticalLayout::areaNavigation(AreaNavigation::Direction dir, bool cyclic)
{
	GuiElement* element = m_pSelectedElement;

	if (!element) return;

	AreaNavigation::ID id;
	if (m_bCyclic && cyclic)
		id = Navigation(this).navigateCyclic(dir, element->m_xPos + element->m_width / 2, element->m_yPos + element->m_height / 2);
	else
		id = Navigation(this).navigate(dir, element->m_xPos + element->m_width / 2, element->m_yPos + element->m_height / 2);

	if (id >= 0)
		selectElementById(m_scrollAmount + id);
}

void VerticalLayout::setSelected(bool b)
{
	GuiElement::setSelected(b);

	if (b && !m_pSelectedElement && m_pScreen->_useController())
		selectElementById(0, false);

	if (!b)
		selectElement(nullptr);
}

bool VerticalLayout::handleScroll(bool up)
{
	if ((m_scrollAmount > 0 && up) || (m_bCanScrollDown && !up))
	{
		updateScroll(m_scrollAmount + (up ? -1 : 1));
		return true;
	}
	return false;
}

void VerticalLayout::handleScroll(float force)
{
	handleScroll(force > 0);
}

void VerticalLayout::updateScroll(int amount)
{
	m_scrollRenderer.updateScroll(amount < m_scrollAmount ? AreaNavigation::UP : AreaNavigation::DOWN);
	m_scrollAmount = amount;
	organize();
}

void VerticalLayout::pressed(Minecraft* mc, const MenuPointer& pointer)
{
	for (int i = 0; i < m_elementsOnScreen; ++i)
	{
		GuiElement* element = m_elements[m_scrollAmount + i];
		if (element->isHovered(mc, pointer)) 
		{
			m_pClickedElement = element;
			element->pressed(mc, pointer);
			return;
		}
	}
}

void VerticalLayout::released(const MenuPointer& pointer)
{
	if (m_pClickedElement)
	{
		m_pClickedElement->released(pointer);
		m_pClickedElement = nullptr;
	}
}

void VerticalLayout::pressed(Minecraft* mc)
{
	if (m_pSelectedElement)
		m_pSelectedElement->pressed(mc);
}

void VerticalLayout::render(Minecraft* mc, const MenuPointer& pointer)
{
	for (int i = 0; i < m_elementsOnScreen; ++i)
	{
		m_elements[m_scrollAmount + i]->render(mc, pointer);
	}

	if (m_scrollAmount > 0)
		m_scrollRenderer.renderScroll(AreaNavigation::UP, *mc->m_pTextures, m_xPos + m_width - 64, m_yPos + m_height + 8);
	if (m_bCanScrollDown)
		m_scrollRenderer.renderScroll(AreaNavigation::DOWN, *mc->m_pTextures, m_xPos + m_width - 32, m_yPos + m_height + 8);
}

VerticalLayout::Navigation::Navigation(VerticalLayout* layout) : m_pLayout(layout)
{
	m_area.w = layout->m_pScreen->m_width;
	m_area.h = layout->m_pScreen->m_height;
}

bool VerticalLayout::Navigation::next(int& x, int& y, bool cycle)
{
	while (++m_index < m_pLayout->m_elementsOnScreen)
	{
		GuiElement* element = m_pLayout->m_elements[m_pLayout->m_scrollAmount + m_index];

		if (!element->isVisible() || !element->isNavigable() || !(cycle || isValid(m_index))) continue;

		x = element->m_xPos + element->m_width / 2;
		y = element->m_yPos + element->m_height / 2;

		return true;
	}

	return false;
}

bool VerticalLayout::Navigation::isValid(ID id)
{
	return m_pLayout->m_pSelectedElement->getId() != (m_pLayout->m_scrollAmount + id);
}