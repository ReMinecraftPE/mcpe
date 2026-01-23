#include "ContainerListener.hpp"
#include "world/inventory/ContainerMenu.hpp"

ContainerListener::~ContainerListener()
{
}

void ContainerListener::refreshContainerItems(ContainerMenu* menu)
{
	refreshContainer(menu, menu->getItems());
}
