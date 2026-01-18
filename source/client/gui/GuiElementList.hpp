#pragma once

#include "GuiElement.hpp"

class GuiElementList
{
public:
	class Node
	{
	private:
		GuiElement* m_pElement;
		Node* m_pLeft;
		Node* m_pRight;

	public:
		Node(GuiElement* pElement, Node* pLeft)
		{
			m_pElement = pElement;
			m_pLeft = pLeft;
			m_pRight = nullptr;
		}

	public:
		Node* left() { return m_pLeft; }
		Node* right() { return m_pRight; }

	public:
		GuiElement* operator->() { return m_pElement; }
	};
};

