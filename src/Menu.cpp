#include "Menu.hxx"

bool Menu::isActive() const {
	return active_;
}

void Menu::setActive(bool active) {
	active_ = active;
}
