#include "ContainerListener.hpp"
#include "world/inventory/ContainerMenu.hpp"

void ContainerListener::refreshContainerItems(ContainerMenu* menu)
{
	refreshContainer(menu, menu->getItems());
}
